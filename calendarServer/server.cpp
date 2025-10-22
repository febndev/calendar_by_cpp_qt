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
    // 0819 [DB] 이거 때문에 오류가 났었던 것 같은데 또 넣네.
    if (!socketStream.commitTransaction()) {
        qDebug() << "[SERVER] waiting for more data...";
        return;
    }
    // 0819 찝찝한 코드 넣기 끝

    // '\0' 패딩과 공백 제거 유틸
    auto stripPad = [](QString s) {
        s.replace(QChar('\0'), QChar(' '));
        return s.trimmed();
    };

    // client 에서 보낸 payload(순수한 데이터, 전달 메시지)를
    // buffer에서 처음 128 byte 부분만 읽어들여서 header 에 담고 type을 찾는다.
    QString header = stripPad(buffer.mid(0,128));
    QString type = header.split(",")[0].split(":")[1].trimmed();

    // buffer의 128 byte 이후 부분을
    // QString body = stripPad(QString::fromUtf8(buffer.mid(128)));
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
        q.prepare("SELECT code, pw FROM users WHERE id = ?");
        q.addBindValue(id);

        if (!q.exec()){
            sendErrorToClient(socket, "DB_ERROR");
            return;
        }

        if (!q.next()){
            sendErrorToClient(socket, "NO_USER");
            return;
        }

        QString dbPw = q.value("pw").toString();
        const int userCode = q.value("code").toInt();
        if (pw == dbPw){

            m_userBySocket.insert(socket, userCode);
            qDebug() << "로그인성공!";

            qDebug() << "[LOGIN] map set"
                     << "socket=" << socket
                     << "userCode=" << userCode;

            sendOkToClient(socket, "LOGIN_OK");
            sendCalendarList(socket, userCode);
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
}


// 0821 JSON으로 수정 / 0820 유저가 접근 가능한 캘린더 목록(탭, 개행 텍스트)
// (캘린더 id, 이름, 컬러, 접근 권한)을 응답으로 내려보냄
// 시간 남으면 이 함수랑 위에 함수랑 합치고 싶다
void Server::sendCalendarTotalList(QTcpSocket* m_socket, int userCode)
{
    // stream으로 보내기
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
    q.addBindValue(userCode); // DB상 고유한 user tbl 의 code 컬럼

    // 실패 실행했을 경우 err에 에러메시지 전달
    if (!q.exec()) {
        qDebug() << "[SERVER] cal_list query error:" << q.lastError();
        QByteArray err = QByteArray("cal_total_list\t0\n"); // 비어있다고 응답
        m_socket->write(err);
        m_socket->flush();
        return;
    }

    // 값들을 반복해서 가져옴.
    QJsonArray calLists;
    while (q.next()) {
        // 하나의 달력의 값들을 가져와서 각 변수에 저장
        const int id = q.value(0).toInt();  //calID
        const QString name    = q.value(1).toString();  // calName
        const QString color   = q.value(2).toString();  // "#RRGGBB"
        const int     canEdit = q.value(3).toInt();     // 0/1
        // 하나의 Json객체를 만들어서
        QJsonObject calListValue;
        calListValue["id"] = id;
        calListValue["name"] = name;
        calListValue["color"] = color;
        calListValue["can_edit"] = (canEdit != 0);
        // 배열에 추가
        calLists.append(calListValue);
    }

    //ByteArray 형태로 클라이언트 한테 전송할 준비
    // header 만들기
    QString header;

    // 여기부터 수정해야함.
    // headerstr.prepend(QString("type:cal_total_list,name:null,size:%1")
    //                       .arg(size(calLists)).toUtf8());
    // calLists 는 QJonObject기 때문에 calLists.size()로 적어야함.
    header.prepend(QString("type:cal_total_list,name:null,size:%1")
                          .arg(calLists.size()).toUtf8());
    header.resize(128);

    // 캘린더 리스트에 헤더 합치기
    QJsonObject addHeader;
    addHeader["type"] = header;
    addHeader["body"] = calLists;

    //QJsonDocument로 저장, toJson으로 변환 후 네트워크 통신용으로 개행문자 추가
    QByteArray byteArray = QJsonDocument(addHeader).toJson(QJsonDocument::Compact);
    byteArray += '\n';

    // 소켓으로 전송
    socketStream << byteArray;
    m_socket->flush();
    m_socket->waitForBytesWritten(3000);

}

// userCode 꺼내 쓰기 위해서 만든 함수
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
        header.prepend(QString("type:cal_resp,name:null,size%1").arg(okMsg.size()).toUtf8());
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



