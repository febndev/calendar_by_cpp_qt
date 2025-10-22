#include "server.h"

#define PKT_REQ_CAL_LIST   "cal_total_list_req"   // 클라 → 서버: 캘린더 목록 주세요
#define PKT_RES_CAL_LIST   "cal_total_list"       // 서버 → 클라: 목록 내려감

// 생성자 호출시
Server::Server(QWidget *parent): QMainWindow(parent){
    m_server = new QTcpServer();
    //listen 함수 실행
    if(m_server->listen(QHostAddress::Any, 9091))
    {
        qDebug() << "서버 시작 성공!";
        connect(m_server, &QTcpServer::newConnection,
                this,     &Server::slot_newConnection);

    }
    else
    {
        QMessageBox::critical(this,"QTCPServer",QString("Unable to start the server: %1.").arg(m_server->errorString()));
        exit(EXIT_FAILURE);
    }
}
// 소멸자 지정
Server::~Server()
{
    // 서버에 연결된 모든 연결 소켓 해제
    foreach (QTcpSocket* socket, qset_connectedClients)
    {
        socket->close();
        socket->deleteLater();
    }

    // 서버 소켓 해제
    m_server->close();
    m_server->deleteLater();

    //delete ui;
}
void Server::slot_newConnection()
{
    qDebug() << "[SERVER] newConnection!";
    // appendToSocketList 함수로 연결 객체 관리 처리
    while (m_server->hasPendingConnections())
        appendToSocketList(m_server->nextPendingConnection());

}

void Server::appendToSocketList(QTcpSocket* socket)
{
    qDebug() << "클라이언트 연결됨! sockd=" << socket->socketDescriptor()
        << " total=" << (qset_connectedClients.size() + 1);

    // [ex.02.4.1]
    // 연결된 소켓을 qset으로 관리
    qset_connectedClients.insert(socket);

    // [ex.02.4.2]
    // 연결된 소켓에 read 할 데이터가 들어오면,
    // 이 객체의(Server) slot_readSocket 실행하여 처리
    connect(socket, &QTcpSocket::readyRead,
            this,   &Server::slot_readSocket);

    // [ex.02.4.3]
    // 연결된 소켓과 연결이 해제되면,
    // 이 객체의(Server) slot_discardSocket 슬롯 함수 실행하여 처리
    connect(socket, &QTcpSocket::disconnected,
            this,   &Server::slot_discardSocket);

    // [ex.02.4.4]
    // 연결된 소켓에 문제가 발생하면,
    // 이 객체의(MainWindow) slot_displayError 슬롯 함수 실행하여 처리
    connect(socket, &QAbstractSocket::errorOccurred,
            this,   &Server::slot_displayError);


}

// 연결된 소켓에서 연결이 끊어지면 동작
void Server::slot_discardSocket()
{
    // 연결된 socket 의 disconnected 시그널을 발생시킨 socket 객체를 찾아서
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    // 해당 소켓을 qset_connectedSKT 에서 제거하고, 메시지 출력
    QSet<QTcpSocket*>::iterator it = qset_connectedClients.find(socket);
    if (it != qset_connectedClients.end()){
        //slot_displayMessage(QString("INFO :: A client has just left the room").arg(socket->socketDescriptor()));
        qset_connectedClients.remove(*it);
    }
    // 0819 [DB] 세션 매핑도 제거
    m_userBySocket.remove(socket);

    // ui 콤보박스 재설정
    //refreshComboBox();
    socket->deleteLater();
}

// 연결된 소켓에서 에러 발생하면 출력
void Server::slot_displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, "QTCPServer", "The host was not found. Please check the host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, "QTCPServer", "The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.");
        break;
    default:
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        QMessageBox::information(this, "QTCPServer", QString("The following error occurred: %1.").arg(socket->errorString()));
        break;
    }
}

// slot_readSocket() 안에 type을 구별해서 login 등 다 하나의 함수로 실행
void Server::slot_readSocket()
{
    // [ex.02.5.1]
    // 슬롯 함수가 실행되면, sender()를 통해 signal을 발생시킨 객체를 찾아 return
    // 이 슬롯(slot_readSocket)은 서버에 연결된 socket의 readyread 시그널에 대한 슬롯으로
    // 연결 메시지를 보낸 socket을 찾을 수 있다.
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    qDebug() << "[SERVER] slot_readSocket called, available bytes:" << socket->bytesAvailable();

    // QByteArray 타입의 buffer를 만들고
    QByteArray buffer;

    // 서버에 연결된 socket을 stream으로 연결한다.
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_6_0);

    // stream으로 데이터를 읽어들이고, buffer로 넘기면
    socketStream.startTransaction();
    socketStream >> buffer;
    if (!socketStream.commitTransaction()) {
        qDebug() << "[SERVER] waiting for more data...";
        return;
    }

    // '\0' 패딩과 공백 제거 유틸
    auto stripPad = [](QString s) {
        s.replace(QChar('\0'), QChar(' '));
        return s.trimmed();
    };
    /* 챗지피티 코드
    QString header = stripPad(QString::fromUtf8(buffer.mid(0, 128)));
    // "type:xxx,name:...,size:n" → type만 안전하게 추출
    QString typePart = header.section(',', 0, 0);          // "type:xxx"
    QString type     = typePart.section(':', 1).trimmed(); // "xxx"
    // 128B~: 본문(요청별 포맷 상이: 기존 login/signup/addCal 등은 '|' 구분,
    // add_event는 JSON 한 줄)
    QByteArray bodyBytes = buffer.mid(128);
    QString body   = QString::fromUtf8(bodyBytes);
*/
    // client 에서 보낸 payload(순수한 데이터, 전달 메시지)를
    // buffer에서 처음 128 byte 부분만 읽어들여서 header 에 담고 type을 찾는다.
    QString header = stripPad(buffer.mid(0,128));
    QString type = header.split(",")[0].split(":")[1].trimmed();

    // buffer의 128 byte 이후 부분을
    // QString body = stripPad(QString::fromUtf8(buffer.mid(128)));
    QByteArray bodyBytes = buffer.mid(128);

    QString body = QString::fromUtf8(buffer.mid(128));
    qDebug() << "body =" << body;
    qDebug() << "body after stripPad =" << stripPad(body);

    QString id;
    QString pw;
    QString username;
    QString useremail;
    QString role;

    // login 이면 DB에 검증 요청 후 결과를 송신한다.
    if(type=="login")
    {
        qDebug() << "login body =" << body;

        // id, pw 추출
        QStringList parts = body.split("|");
        if (parts.size() == 2) {
            id = parts.at(0).trimmed();
            pw = parts.at(1).trimmed();
        }

        // DB 에서 비교 후 클라이언트에게 전송
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery q(db);
        // 안전하게 값을 추출, 비교하기 위해서 쿼리문 준비
        q.prepare("SELECT code, pw FROM users WHERE id = ?");
        //사용자가 입력한 id 값을 바인딩 해서 쿼리문 물음표 자리에 넣는다.
        q.addBindValue(id);

        if (!q.exec()){
            sendErrorToClient(socket, "DB_ERROR");
            return;
        }

        if (!q.next()){
            sendErrorToClient(socket, "NO_USER");
            return;
        }
        //DB에서 패스워드 가져와서 문자열로 변환, code 값은 숫자로 변환
        QString dbPw = q.value("pw").toString();
        const int userCode = q.value("code").toInt();
        if (pw == dbPw){
            // 로그인에 성공하면 m_userBySocket에 소켓과 user테이블의 code값을 넣어놓음.
            m_userBySocket.insert(socket, userCode);
            qDebug() << "로그인성공!";
            qDebug() << "[LOGIN] map set"
                     << "socket=" << socket
                     << "userCode=" << userCode;
            //로그인 성공 메시지 보낸 후(시그널 포함)
            sendOkToClient(socket, "LOGIN_OK");
            //달력 리스트도 보낸다.
            sendCalendarTotalList(socket, userCode);
        } else {
            sendErrorToClient(socket, "WRONG_PASSWORD");
        }
    }
    else if(type=="signup")
    {
        // 사용자가 입력한 회원정보 추출
        QStringList parts = body.split("|");
        if (parts.size() >= 5) {
            id = parts.at(0).trimmed();
            pw = parts.at(1).trimmed();
            username = parts.at(2).trimmed();
            useremail = parts.at(3).trimmed();
            role = parts.at(4).trimmed();
        }

        // DB에 추가
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery q(db);
        //0819 [DB 캘린더추가]
        if (!db.transaction()) { sendErrorToClient(socket, "TX_BEGIN_FAIL"); return; }

        // ID 검증 함수 넣어서 ID 중복 검증 통과하면 DB에 추가하는 함수, 실패하면 에러메시지 출력
        QString newId = id; // 비교용으로 newId 생성
        q.prepare("SELECT COUNT(*) FROM users WHERE id = ?");
        q.addBindValue(newId);

        if (!q.exec()) {
            qDebug() << "ID check failed:" << q.lastError().text();
            return;
        }

        if (q.next()){
            if(q.value(0).toInt() > 0) {
                qDebug() << "중복된 ID입니다.";
                QMessageBox::warning(this, "중복 ID", "이미 사용 중인 ID입니다.\n다른 아이디를 입력하세요.");
                return;
            }
        }

        // 검증 후 값을 꺼내서 INSERT 실행
        q.prepare("INSERT INTO users (id, pw, username, useremail, role) VALUES (?, ?, ?, ?, ?)");
        q.addBindValue(id);
        q.addBindValue(pw);
        q.addBindValue(username);
        q.addBindValue(useremail);
        q.addBindValue(role);
        qDebug() << id << pw << username << useremail << role;

        if (!q.exec()) {
            qDebug() << "Insert failed:" << q.lastError().text();
            return;
        }

        const int userCode = q.lastInsertId().toInt();

        // 비공유용 개인 캘린더 생성 (is_public=0)
        QSqlQuery qc(db);
        qc.prepare("INSERT INTO calendars (name, owner_code, is_public) VALUES (?, ?, 0)");
        qc.addBindValue(QString("%1의 캘린더").arg(username));
        qc.addBindValue(userCode);
        if (!qc.exec()) { db.rollback(); sendErrorToClient(socket, "INSERT_CAL_FAIL"); return; }
        const int calId = qc.lastInsertId().toInt();

        // 4) calendar_members 본인 등록(can_edit=1)
        QSqlQuery qm(db);
        qm.prepare("INSERT INTO calendar_members (calendar_id, user_id, can_edit) VALUES (?, ?, 1)");
        qm.addBindValue(calId);
        qm.addBindValue(userCode);
        if (!qm.exec()) { db.rollback(); sendErrorToClient(socket, "INSERT_MEMBER_FAIL"); return; }

        if (!db.commit()) { db.rollback(); sendErrorToClient(socket, "TX_COMMIT_FAIL"); return; }

        // 회원가입 완료
        sendOkToClient(socket,"SIGNUP_OK");
    }
    else if (type == "addCal") {
        // body : "name|isPublic" (isPublic 생략시 0)
        QStringList parts = stripPad(body).split("|");
        const QString calName = parts.value(0).trimmed();
        const bool isPublic = parts.value(1).trimmed() == "1";

        if (calName.isEmpty()) {
            sendErrorToClient(socket, "ERROR:EMPTY_NAME"); return; }

        // 로그인한 사용자 코드 꺼내기
        const int ownerCode = m_userBySocket.value(socket, -1);
        qDebug() << "ownerCode:"<<ownerCode;
        if (ownerCode <= 0) { sendErrorToClient(socket, "ERROR:NOT_LOGGED_IN"); return; }

        // 여기서부터는 DB INSERT
        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen() && !db.open()) { sendErrorToClient(socket, "ERROR:DB_OPEN_FAIL"); return; }
        if (!db.transaction()) { sendErrorToClient(socket, "ERROR:TX_BEGIN_FAIL"); return; }

        // calendar tbl에 추가하는 쿼리
        QSqlQuery q1(db);
        q1.prepare("INSERT INTO calendars (name, owner_code, is_public) VALUES (?, ?, ?)");
        q1.addBindValue(calName);
        q1.addBindValue(ownerCode);
        q1.addBindValue(isPublic ? 1 : 0);
        if (!q1.exec()) { db.rollback(); sendErrorToClient(socket, "ERROR:CAL_INSERT"); return; }
        const int calId = q1.lastInsertId().toInt();

        // calendar_members tbl에 추가하는 쿼리
        QSqlQuery q2(db);
        q2.prepare("INSERT INTO calendar_members (calendar_id, user_id, can_edit) VALUES (?, ?, 1)");
        q2.addBindValue(calId);
        q2.addBindValue(ownerCode);
        if (!q2.exec()) { db.rollback(); sendErrorToClient(socket, "ERROR:MEMBER_INSERT"); return; }

        if (!db.commit()) { db.rollback(); sendErrorToClient(socket, "ERROR:TX_COMMIT_FAIL"); return; }

        sendOkToClient(socket, QString("CREATE_OK|%1|%2|%3")
                                          .arg(calId)
                                          .arg(calName)
                                          .arg(isPublic ? 1 : 0));
    }
    else if (type == "invite"){
        // body : "inviteeId|calId" (isPublic 생략시 0)
        QStringList parts = stripPad(body).split("|");
        const QString inviteeLoginId = parts.value(0).trimmed();
        const int calId = parts.value(1).trimmed().toInt();

        // DB 접근
        /* 이 함수는 inviteUserToCalendar 함수 안에 있으니까 주석 처리함.
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery q(db);
        */
        // 이 전에 inviterCode를 가져와서 밑의 inviteUserToCalendar 매개 변수로 넣어야함.
        const int inviterCode = userCodeOf(socket);
        QString err; // 일반변수로 선언 후 밑의 inviteUserToCalendar의 매개변수로 넣을때는 주소값 넣으면 됨.
        // 캘린더 초대 함수 실행
        inviteUserToCalendar(calId, inviteeLoginId, inviterCode, &err);
    }
    else if (type == "cal_list_req"){
        int userid = m_userBySocket.value(socket, 0);
        sendCalendarList(socket, userid);
    }
    else if (type == "add_event"){
        qDebug() << "일정 추가 시작";
        // 한 줄만 추출
        const int eol = bodyBytes.indexOf('\n');
        if (eol < 0) { sendAddEventResp(socket, /*ok=*/false, QVariant()); return; }
        const QByteArray line = bodyBytes.left(eol);

        QJsonParseError jerr;
        const QJsonDocument doc = QJsonDocument::fromJson(line, &jerr);
        if (jerr.error != QJsonParseError::NoError || !doc.isObject()) {
            qWarning() << "[SERVER] add_event JSON parse error:" << jerr.errorString() << "line:" << line;
            sendAddEventResp(socket, /*ok=*/false, QVariant());
            return;
        }
        const QJsonObject obj = doc.object();

        // 필수 필드 확인
        if (!obj.contains("calendarId") || !obj.contains("title")
            || !obj.contains("start_time") || !obj.contains("end_time"))
        {
            sendAddEventResp(socket, /*ok=*/false, QVariant());
            return;
        }

        const int calendarId     = obj.value("calendarId").toInt();
        const QString title      = obj.value("title").toString();
        const QString memo       = obj.value("memo").toString();
        const QString start_time = obj.value("start_time").toString();
        const QString end_time   = obj.value("end_time").toString();

        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isValid() || !db.isOpen()) {
            sendAddEventResp(socket, /*ok=*/false, QVariant());
            return;
        }

        QSqlQuery q(db);
        q.prepare(R"(
            INSERT INTO events (calendar_id, title, memo, start_time, end_time)
            VALUES (?, ?, ?, ?, ?)
        )");
        q.addBindValue(calendarId);
        q.addBindValue(title);
        q.addBindValue(memo);
        q.addBindValue(start_time);
        q.addBindValue(end_time);

        if (!q.exec()) {
            qWarning() << "[SERVER] add_event insert error:" << q.lastError();
            sendAddEventResp(socket, /*ok=*/false, QVariant());
            return;
        }

        const QVariant newId = q.lastInsertId();
        sendAddEventResp(socket, /*ok=*/true, newId);
    }
}

void Server::sendAddEventResp(QTcpSocket* socket, bool ok, const QVariant& insertId)
{
    QByteArray header = QString("type:add_event_resp,name:null,size:1").toUtf8();
    header.resize(128);

    QJsonObject obj;
    obj["result"]   = ok ? QStringLiteral("CREATE_OK") : QStringLiteral("ERROR");
    if (ok && insertId.isValid()) obj["event_id"] = insertId.toLongLong();
    else                          obj["event_id"] = QJsonValue::Null;

    QByteArray line = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    line += '\n';

    QByteArray out;
    out.reserve(128 + line.size());
    out.append(header);
    out.append(line);

    QDataStream ds(socket);
    ds.setVersion(QDataStream::Qt_6_0);
    ds << out;

    socket->flush();
    socket->waitForBytesWritten(3000);
}


// 0821 JSON으로 수정 / 0820 유저가 접근 가능한 캘린더 목록(탭, 개행 텍스트)
// (캘린더 id, 이름, 컬러, 접근 권한)을 응답으로 내려보냄
// 시간 남으면 이 함수랑 위에 함수랑 합치고 싶다
void Server::sendCalendarTotalList(QTcpSocket* m_socket, int userCode)
{
    QDataStream socketStream(m_socket);
    socketStream.setVersion(QDataStream::Qt_6_0);

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);

    q.prepare(R"(
        SELECT c.id, c.name, IFNULL(c.color, '#787878'), IFNULL(cm.can_edit, 0)
        FROM calendars c
        JOIN calendar_members cm ON cm.calendar_id = c.id
        WHERE cm.user_id = ?
        ORDER BY c.id ASC
    )");
    q.addBindValue(userCode);

    if (!q.exec()) {
        qWarning() << "[SERVER] cal_total_list query error:" << q.lastError();
    }
    // 결과 → 본문 줄(JSON 객체)
    QList<QJsonObject> rows;
    while (q.next()) {
        QJsonObject o;
        o["id"]       = q.value(0).toInt();
        o["name"]     = q.value(1).toString();
        o["color"]    = q.value(2).toString();
        o["can_edit"] = (q.value(3).toInt() != 0);
        rows.append(o);
    }

    // 1) 헤더 128바이트
    QByteArray out;
    QByteArray header = QString("type:cal_total_list,name:null,size:%1")
                            .arg(rows.size()).toUtf8();
    header.resize(128);                     // 널/공백 패딩 유지 (클라에서 stripPad로 처리)
    out.append(header);

    // 2) 본문: 객체 한 줄씩
    for (const auto& o : rows) {
        QByteArray line = QJsonDocument(o).toJson(QJsonDocument::Compact);
        line += '\n';
        out.append(line);
    }

    // 3) 전송 (QDataStream 프레이밍 유지)

    socketStream << out;
    m_socket->flush();
    m_socket->waitForBytesWritten(3000);
}

// 중요하구만 userCode 꺼내 쓰기 위해서 만든 함수
int Server::userCodeOf(QTcpSocket* sock) const
{
    return m_userBySocket.value(sock, -1); // -1: 비로그인/없음
}

// 공유 캘린더 실제 초대 처
bool Server::inviteUserToCalendar(int calId,
                                  const QString& inviteeLoginId,
                                  int inviterCode,
                                  QString* err)// err빼고 싶다.
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);

    // 1) 권한 확인 (여기선 멤버 + 편집 권한 보유자만 초대 가능)
    q.prepare(R"(
        SELECT 1
        FROM calendar_members
        WHERE calendar_id = ? AND user_id = ? AND can_edit = 1
    )");
    q.addBindValue(calId);
    q.addBindValue(inviterCode);
    if (!q.exec() || !q.next()) {
        if (err) *err = "NO_PERMISSION";
        return false;
    }

    // 2) 초대 대상 code 조회
    int inviteeCode = -1;
    q.prepare("SELECT code FROM users WHERE id = ?");
    q.addBindValue(inviteeLoginId);
    if (!q.exec() || !q.next()) {
        if (err) *err = "NO_SUCH_USER";
        return false;
    }
    inviteeCode = q.value(0).toInt();

    // 3) 멤버십 추가 (중복이면 can_edit만 갱신)
    q.prepare(R"(
        INSERT INTO calendar_members (calendar_id, user_id, can_edit)
        VALUES (?, ?, 1)
        ON DUPLICATE KEY UPDATE can_edit = VALUES(can_edit)
    // )");
    q.addBindValue(calId);
    q.addBindValue(inviteeCode);
    if (!q.exec()) {
        if (err) *err = "DB_ERROR";
        return false;
    }

    // 4) 온라인이면 사용자2에게 "캘린더 목록 새로고침" 알림 보내기(프로토콜에 맞게)
    //notifyUserCalendarsDirty(inviteeCode); // ⇒ 클라가 requestCalendarList() 호출하도록

    return true;
}

// 로그인 시 캘린더 리스트 보내는거
void Server::sendCalendarList(QTcpSocket* socket, int userId)
{
    // 1) DB에서 캘린더 이름 조회
    QSqlQuery q;
    q.prepare("SELECT name FROM calendars WHERE owner_code = ?");
    q.addBindValue(userId);

    if (!q.exec()) {
        qDebug() << "캘린더 목록 조회 실패:" << q.lastError();
        return;
    }

    QStringList calendars;
    while (q.next()) {
        calendars << q.value(0).toString(); // name
    }

    // 2) 바디(payload) 만들기: "이름1|이름2|이름3"
    //   * 이름 안에 '|'가 들어갈 일 없다는 가정(있다면 escape 필요)
    const QString body = calendars.join("|");
    const QByteArray bodyBytes = body.toUtf8();

    // 3) 헤더(128바이트) 만들기: "type:cal_list"
    //    - 클라 코드: header.split(",")[0].split(":")[1] 로 type 파싱
    //    - 콤마(,)가 없어도 문제 없음(그냥 전체가 첫 토큰)
    QByteArray header;
    header.prepend(QString("type:cal_list,name:null,size:%1").arg(body.size()).toUtf8());
    header.resize(128);                  // 128바이트로 맞추며 남는 부분은 '\0'로 패딩됨

    // 4) 최종 buffer = [128바이트 헤더] + [바디]
    QByteArray byteArray = body.toUtf8();
    byteArray.prepend(header);

    // 5) QDataStream으로 전송 (클라가 startTransaction/>> buffer 로 읽음)
    QDataStream out(socket);
    out.setVersion(QDataStream::Qt_6_0);
    out << byteArray;

    socket->flush();
    socket->waitForBytesWritten(3000);

}








// 클라이언트 콘솔 창에 문구 보내는 코드들
// 에러가 나면 로그인 실패 문구를 헤더 붙여서 클라이언트한테 보낸다.
void Server::sendErrorToClient(QTcpSocket *clientSocket, const QString &errorMsg) {
    if (!clientSocket || !clientSocket->isOpen()) {
        qDebug() << "[SERVER] Cannot send error - socket invalid";
        return;
    }

    qDebug() << "[SERVER] Sending error to client:" << errorMsg;

    // 헤더 생성: type을 login_resp로 설정
    QByteArray header;
    header.prepend(QString("type:login_resp,name:null,size:%1")
                       .arg(errorMsg.size()).toUtf8());
    header.resize(128);

    // body 부분 생성
    QByteArray responseData = errorMsg.toUtf8();
    responseData.prepend(header);

    // QDataStream으로 전송 (클라이언트와 동일한 방식)
    QDataStream socketStream(clientSocket);
    socketStream.setVersion(QDataStream::Qt_6_0);
    socketStream << responseData;

    clientSocket->flush();
    qDebug() << "[SERVER] Error response sent, total size:" << responseData.size();
}
// 로그인 검증결과를 Client에게 헤더 붙여서 보낸다.
void Server::sendOkToClient(QTcpSocket *clientSocket, const QString &okMsg) {

    if (!clientSocket || !clientSocket->isOpen()) {
        qDebug() << "[SERVER] Cannot send OK - socket invalid";
        return;
    }

    qDebug() << "[SERVER] Sending OK to client:" << okMsg;

    // 헤더 생성: type들로 분기할 수 있게 추가
    QByteArray header;
    if (okMsg == "LOGIN_OK"){
        header.prepend(QString("type:login_resp,name:null,size:%1")
                           .arg(okMsg.size()).toUtf8());

    } else if (okMsg == "SIGNUP_OK") {
        header.prepend(QString("type:signup_resp,name:null,size:%1").arg(okMsg.size()).toUtf8());
    } else if (okMsg == "CREATE_OK") {
        header.prepend(QString("type:cal_resp,name:null,size:%1").arg(okMsg.size()).toUtf8());
    }

    header.resize(128);

    // body 부분에 okMsg 설정
    QByteArray responseData = okMsg.toUtf8();

    // header와 body 결합 (그냥 responseData로 바로 선언하였음)
    responseData.prepend(header);

    // QDataStream으로 전송 (클라이언트와 동일한 방식)
    QDataStream socketStream(clientSocket);
    socketStream.setVersion(QDataStream::Qt_6_0);
    socketStream << responseData;

    clientSocket->flush();
    qDebug() << "[SERVER] OK response sent, total size:" << responseData.size();
}

// // ★ 임의 응답 타입을 128바이트 헤더 + 바디로 보내는 헬퍼
// void Server::sendTyped(QTcpSocket* clientSocket, const QString& respType, const QString& body) {
//     if (!clientSocket || !clientSocket->isOpen()) return;

//     QByteArray header;
//     header.prepend(QString("type:%1,name:null,size:%2")
//                        .arg(respType)
//                        .arg(body.toUtf8().size()).toUtf8());
//     header.resize(128);

//     QByteArray responseData = body.toUtf8();
//     responseData.prepend(header);

//     QDataStream socketStream(clientSocket);
//     socketStream.setVersion(QDataStream::Qt_6_0);
//     socketStream << responseData;
//     clientSocket->flush();
// }



