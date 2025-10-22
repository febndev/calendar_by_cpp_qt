#include "tcpclient.h"
//#include "logindialog.h"
#include "calendarinfo.h"

#include <QDataStream>
#include <QDebug>
#include <QThread>

#define PKT_RES_CAL_LIST   "cal_total_list"   // 서버와 동일하게!

enum : quint16 {
    PKT_LOGIN_REQ = 1001, // 예시. 프로젝트의 protocol.h 값으로 교체 가능
};

// QTcpSocket 기반 클라이언트 기능 (서버 접속, 메시지 송수신)
TcpClient::TcpClient(QWidget *parent) : QMainWindow(parent)
{
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost("127.0.0.1",9091);

    if(m_socket->waitForConnected())
    {
    }
    else
    {
        QMessageBox::critical(this,"QTCPClient", QString("The following error occurred: %1.").arg(m_socket->errorString()));
        exit(EXIT_FAILURE);
    }

    // 연결된 socket에 read 할 데이터가 들어오면,
    // 이 객체의(MainWindow) slot_readSocket 실행하여 처리
    connect(m_socket, &QTcpSocket::readyRead,
            this,     &TcpClient::slot_readSocket);

    // signal_newMessage 시그널이 발생하면 (socket read 가 아닌, MainWindow 시그널)
    // slot_displayMessage 실행하여 UI에 출력
    //connect(this, &TcpClient::signal_newEvent,
    //        this, &TcpClient::slot_displayMessage);

    // 연결된 소켓과 연결이 해제되면,
    // 이 객체의(MainWindow) slot_discardSocket 슬롯 함수 실행하여 처리
    connect(m_socket, &QTcpSocket::disconnected,
            this,     &TcpClient::slot_discardSocket);


    // 연결된 소켓에 문제가 발생하면,
    // 이 객체의(MainWindow) slot_displayError 슬롯 함수 실행하여 처리
    connect(m_socket, &QAbstractSocket::errorOccurred,
            this,     &TcpClient::slot_displayError);

}

// 소멸자
TcpClient::~TcpClient()
{
    // socket 해제
    if(m_socket->isOpen())
        m_socket->close();
    // delete ui;
}

// 서버에서 연결이 끊어지면 동작
void TcpClient::slot_discardSocket()
{
    m_socket->deleteLater();
    m_socket=nullptr;

    //ui->statusBar->showMessage("Disconnected!");
}

// 연결된 소켓에서 에러 발생하면 출력
void TcpClient::slot_displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, "QTCPClient", "The host was not found. Please check the host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, "QTCPClient", "The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.");
        break;
    default:
        QMessageBox::information(this, "QTCPClient", QString("The following error occurred: %1.").arg(m_socket->errorString()));
        break;
    }
}


// 첨부파일 또는 메시지 수신 처리
void TcpClient::slot_readSocket()
{
    // QByteArray 타입의 buffer를 만들고
    QByteArray buffer;

    // 서버에 연결된 socket을 stream으로 연결한다.
    QDataStream socketStream(m_socket);
    socketStream.setVersion(QDataStream::Qt_6_0);

    // stream으로 데이터를 읽어들이고, buffer로 넘기면
    socketStream.startTransaction();
    socketStream >> buffer;

    // stream startTransaction 실행 문제시 에러 표시 후 함수 종료
    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Waiting for more data to come..").arg(m_socket->socketDescriptor());
        // emit signal_newEvent(message);
        return;
    }

    qDebug() << "buffer =" << buffer;

    // '\0' 패딩과 공백 제거 유틸
    auto stripPad = [](QString s) {
        s.replace(QChar('\0'), QChar(' '));
        return s.trimmed();
    };
    // client 에서 보낸 payload(순수한 데이터, 전달 메시지)를
    // buffer에서 처음 128 byte 부분만 읽어들여서 header 에 담고 fileType을 찾는다.
    QString header = stripPad(buffer.mid(0,128));
    QString type = header.split(",")[0].split(":")[1].trimmed();

    // JSON 데이터는 개행 제거 하면 안되서 이 문장 if문 안으로 이동.
    // QString body = stripPad(QString::fromUtf8(buffer.mid(128)));

    qDebug() << "[클라이언트] Type:" << type;
    qDebug() << "[TcpClient emit] this=" << this;

    // type에 따른 분기시작
    // 로그인 성공했을 때 시그널 보내기
    if(type == "login_resp")
    {
        QString body = stripPad(QString::fromUtf8(buffer.mid(128)));
        qDebug() << "[클라이언트] Body:" << body;

        if (body == "LOGIN_OK"){
            qDebug() << "[클라이언트] Login successful!";
            emit loginSuccess();
        } else if (body == "NO_USER") {
            qDebug() << "[클라이언트] User not found";
            emit loginFailed("사용자를 찾을 수 없습니다.");
        } else if (body == "WRONG_PASSWORD") {
            qDebug() << "[클라이언트] Wrong password";
            emit loginFailed("비밀번호가 틀렸습니다.");
        } else if (body == "DB_ERROR") {
            qDebug() << "[클라이언트] Database error";
            emit loginFailed("서버 오류가 발생했습니다.");
        } else {
            qDebug() << "[클라이언트] Unknown login response:" << body;
            emit loginFailed("알 수 없는 오류가 발생했습니다.");
        }
    }
    // 회원가입 성공했을 때 시그널 전송
    else if(type=="signup_resp")
    {
        QString body = stripPad(QString::fromUtf8(buffer.mid(128)));
        qDebug() << "[클라이언트] Body:" << body;

        if(body =="SIGNUP_OK"){
            qDebug() << "[클라이언트] Signup successful!";
            emit signupSuccess();
        }
        else {emit signupFailed("회원가입 실패!");}
    }
    // 아 이거 필요하네 addCalsuccess(); 신호 보내야 되니까 필요하네
    // sendOkToClient 수정 해야되네
    else if (type == "cal_resp")
    {
        QString body = stripPad(QString::fromUtf8(buffer.mid(128)));
        qDebug() << "[클라이언트] Body:" << body;

        if(body == "CREATE_OK"){
            qDebug() << "[클라이언트] 달력만들기 성공";
            emit addCalSuccess();
        }
        else {emit addCalFailed("달력 추가 실패!");}
    }

    // 캘린더 목록, 값 가져오기
    // JSon 으로 받아서 다시 lst 만들어야되나.. ?
    else if (type == "cal_total_list")
    {
        QByteArray body = buffer.mid(128);

        qDebug() << "[클라이언트] Body:" << body;
        QJsonDocument doc;
        QList<CalendarInfo> calList;
        QStringList calNameList;
        while (true) {
            qDebug() <<"JSON 파싱 시작";
            int eol = body.indexOf('\n');
            if (eol < 0) {
                // 더 이상 완전한 한 줄이 없으면 루프 종료
                break;
            }
            QByteArray line = body.left(eol);
            qDebug() << "JSon 한줄: "<< line;
            body.remove(0, eol + 1);

            // 빈 라인은 건너 뛰기
            if (line.trimmed().isEmpty()){
                continue;
            }

            QJsonParseError err;
            doc = QJsonDocument::fromJson(line, &err);
            if (err.error != QJsonParseError::NoError) {
                qWarning() << "[JSON] parse error:" << err.errorString()
                << "line:" << line;
                continue; // 파싱 실패한 라인은 스킵
            }
            if (!doc.isObject()) {
                qWarning() << "[JSON] not an object" << line;
                continue;
            }

            QJsonObject obj = doc.object();

            CalendarInfo ci{};
            ci.id   = obj.value("id").toInt();
            ci.name = obj.value("name").toString();

            QString colorHex = obj.value("color").toString("#787878");
            QColor color(colorHex);
            ci.color = color.isValid() ? color : QColor(120,120,120);
            // 이부분 다시 봐야함. role은 직급이고 can_Edit은 캘린더 멤버 테이블의 값임.-_-?
            if (obj.contains("role"))
                ci.role = obj.value("role").toString();
            else
                ci.role = obj.value("can_edit").toInt() ? "editor" : "viewer";

            const QJsonValue vis = obj.value("visible");
            ci.can_Edit = vis.isBool() ? vis.toBool()
                                      : (vis.isDouble() ? (vis.toInt()!=0) : true);

            calList.append(ci);
            calNameList.append(ci.name);

            // 제대로 파싱되고 있는지 확인하는 코드
            qDebug() << "[PARSED]"
                     << "id:"        << ci.id
                     << "name:"      << ci.name
                     << "color:"     << ci.color.name(QColor::HexRgb)
                     << "role:"      << ci.role
                     << "can_Edit:"  << ci.can_Edit;
        }

        qDebug() << "[EMIT] calendarListUpdated count =" << calList.size();
        emit calendarTotalListUpdated(calList);
    } else if(type == "cal_list") {

        QString body = stripPad(QString::fromUtf8(buffer.mid(128)));

        const QStringList list = body.split("|", Qt::SkipEmptyParts);
        qDebug() << list;
        emit calendarListUpdated(list);

        qDebug() << "[클라이언트] Body:" << list;

    }
}


// loginDialog에서 id, pw 받아서 이제 서버로 보냄

void TcpClient::sendLoginRequest(const QString& id, const QString& pw)
{
    if(m_socket)
    {
        if(m_socket->isOpen())
        {
            QString str = QString("%1|%2").arg(id).arg(pw);
            qDebug() << "[CLIENT] Sending login data:" << str;

            // stream으로 보내기
            QDataStream socketStream(m_socket);
            socketStream.setVersion(QDataStream::Qt_6_0);

            // header 부분에 type을 login으로 설정
            QByteArray header;
            header.prepend(QString("type:login,name:null,size:%1")
                               .arg(str.size()).toUtf8());
            header.resize(128);

            // 헤더 생성 코드 개선했더니 안됐음. 지워야하는데 참고로 남겨둠.
            // QString headerStr = QString("type:login,name:null,size:%1").arg(str.size());
            // QByteArray header = headerStr.toUtf8();

            // login 인코딩 설정하고 QByteArray에 할당
            QByteArray byteArray = str.toUtf8();
            byteArray.prepend(header);

            // 디버깅 코드
            qDebug() << "[CLIENT] Sending packet size:" << byteArray.size();
            qDebug() << "[CLIENT] Header:" << header;
            qDebug() << "[CLIENT] Body:" << str;

            // 소켓으로 전송
            socketStream << byteArray;
            m_socket->flush();
            m_socket->waitForBytesWritten(3000);

            qDebug() << "[CLIENT] Data sent successfully";

        }
        else
            QMessageBox::critical(this, "QTCPClient", "Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this, "QTCPClient", "Not connected");
}

void TcpClient::sendSignupRequest(const QString &id,
                                const QString &pw,
                                const QString &username,
                                const QString &useremail,
                                const QString &level)
{
    if(m_socket){
        if(m_socket->isOpen()){
            QString str = QString("%1|%2|%3|%4|%5")
                              .arg(id)
                              .arg(pw)
                              .arg(username)
                              .arg(useremail)
                              .arg(level);

            QDataStream socketStream(m_socket);
            socketStream.setVersion(QDataStream::Qt_6_0);

            // header 부분에 type을 login으로 설정
            QByteArray header;
            header.prepend(QString("type:signup,name:null,size:%1")
                               .arg(str.size()).toUtf8());
            header.resize(128);

            // login 인코딩 설정하고 QByteArray에 할당
            QByteArray byteArray = str.toUtf8();
            byteArray.prepend(header);

            // 디버깅 코드
            qDebug() << "[CLIENT] Sending packet size:" << byteArray.size();
            qDebug() << "[CLIENT] Header:" << header;
            qDebug() << "[CLIENT] Body:" << str;

            // 소켓으로 전송
            socketStream << byteArray;
            m_socket->flush();
            m_socket->waitForBytesWritten(3000);

            qDebug() << "[CLIENT] Data sent successfully";
        }
        else
            QMessageBox::critical(this, "QTCPClient", "Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this, "QTCPClient", "Not connected");
}

// 0819 [캘린더추가] 서버로 정보 보내는 함수
void TcpClient::sendAddCalRequest(const QString& calName, bool is_Public){
    if(m_socket){
        if(m_socket->isOpen()){
            QString str = QString("%1|%2")
                .arg(calName)
                .arg(is_Public);

            QDataStream socketStream(m_socket);
            socketStream.setVersion(QDataStream::Qt_6_0);

            // header 부분에 type을 addCal으로 설정
            QByteArray header;
            header.prepend(QString("type:addCal,name:null,size:%1")
                               .arg(str.size()).toUtf8());
            header.resize(128);

            // 인코딩 설정하고 QByteArray에 할당
            QByteArray byteArray = str.toUtf8();
            byteArray.prepend(header);

            // 디버깅 코드
            qDebug() << "[CLIENT] Sending packet size:" << byteArray.size();
            qDebug() << "[CLIENT] Header:" << header;
            qDebug() << "[CLIENT] Body:" << str;

            // 소켓으로 전송
            socketStream << byteArray;
            m_socket->flush();
            m_socket->waitForBytesWritten(3000);

            qDebug() << "[CLIENT] Data sent successfully";
        }
        else
            QMessageBox::critical(this, "QTCPClient", "Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this, "QTCPClient", "Not connected");
}

//0820 캘린더 공유 서버에 요청하는 함수
void TcpClient::sendInviteRequest(const QString& inviteeId, int calId)
{
    if(m_socket)
    {
        if(m_socket->isOpen())
        {
            QString str = QString("%1|%2").arg(inviteeId).arg(calId);
            qDebug() << "[CLIENT] Sending login data:" << str;

            // stream으로 보내기
            QDataStream socketStream(m_socket);
            socketStream.setVersion(QDataStream::Qt_6_0);

            // header 부분에 type을 invite으로 설정
            QByteArray header;
            header.prepend(QString("type:invite,name:null,size:%1")
                               .arg(str.size()).toUtf8());
            header.resize(128);

            // login 인코딩 설정하고 QByteArray에 할당
            QByteArray byteArray = str.toUtf8();
            byteArray.prepend(header);

            // 디버깅 코드
            qDebug() << "[CLIENT] Sending packet size:" << byteArray.size();
            qDebug() << "[CLIENT] Header:" << header;
            qDebug() << "[CLIENT] Body:" << str;

            // 소켓으로 전송
            socketStream << byteArray;
            m_socket->flush();
            m_socket->waitForBytesWritten(3000);

            qDebug() << "[CLIENT] Data sent successfully";
        }

        else
            QMessageBox::critical(this, "QTCPClient", "Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this, "QTCPClient", "Not connected");
}

// 0818 [API] "월별 이벤트 카운트" 요청 보내기 정의
quint32 TcpClient::requestMonthCounts(const QDate& from, const QDate& to, const QStringList& calendarIds)
{
    const quint32 reqId = m_nextReqId++; // 요청Id 발급 , 아니 DB에서 code 값 같은거 가져오면 되는게 아닌가봉가
    // 요청을 구분하기 위한 id . 서버 - 클라이언트가 동시에 여러 요청을 처리할 때
    // 어떤 응답이 어떤 요청에 대한 것인지 식별하는 용도

    // 이 달의 이벤트 갯수를 알기 위해서 날짜 범위를 서버로 보냄.
    //패킷 빌드 -> 이 부분 수정해야함.
    // const QByteArray bytes = PacketBuilder::buildMonthCountsRequest(reqId, from, to, calendarIds);

    // 윗코드 수정하면 주석 풀면 됨. 아니면 sendToServer함수 있으면 그걸 넣도록
    // m_socket->write(bytes);
    // m_socket->flush();

    // 호출하는 쪽에서 reqId 값을 사용하지 않는다면 자료형을 void로 바꾸고 이 코드 지워도 됨.
    return reqId;
}

// 0822 캘린더 목록 요청하는 함수
void TcpClient::sendCalendarListRequest(){
    if(m_socket){
        if(m_socket->isOpen()){
            qDebug() << "[TcpClient] requestCalendarList() ENTER";

            QString body = QString("달력요청");
            QDataStream socketStream(m_socket);
            socketStream.setVersion(QDataStream::Qt_6_0);

            QByteArray header;
            header.prepend(QString("type:cal_list_req,name:null,size:%1")
                               .arg(body.size())
                               .toUtf8());
            header.resize(128); // 고정 길이 패딩('\0')

            QByteArray byteArray = body.toUtf8();
            byteArray.prepend(header);

            socketStream << byteArray;

            m_socket->flush();
            m_socket->waitForBytesWritten(3000);

            qDebug() << "[TcpClient] requestCalendarList sent"
                     << " bytes=" << byteArray.size()
                     << " sockState=" << m_socket->state();
        }
        else
            QMessageBox::critical(this, "QTCPClient", "Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this, "QTCPClient", "Not connected");
}

void TcpClient::sendAddEventRequest(const QByteArray& byteArray){
    if(m_socket){
        if(m_socket->isOpen()){
            qDebug() << "[TcpClient] sendAddEventRequest() ENTER";

            QDataStream socketStream(m_socket);
            socketStream.setVersion(QDataStream::Qt_6_0);


            socketStream << byteArray;

            m_socket->flush();
            m_socket->waitForBytesWritten(3000);

            qDebug() << "[TcpClient] requestCalendarList sent"
                     << " bytes=" << byteArray.size()
                     << " sockState=" << m_socket->state();
        }
        else
            QMessageBox::critical(this, "QTCPClient", "Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this, "QTCPClient", "Not connected");
}


// 0822 이거 지우고 싶은데... 간단하게 리스트만 불러온다고 만들었던 건데..
// void TcpClient::slot_calLstReadyRead(){

//     QByteArray data = m_socket->readAll();
//     QString msg = QString::fromUtf8(data);
//     qDebug() << "slot_calLstReadyRead() 실행: "<< msg;
//     // 서버에서 "캘린더1|캘린더2|캘린더3" 형식으로 보내주는 걸 리스트에 저장
//     QStringList calendars = msg.split("|", Qt::SkipEmptyParts);

//     emit calendarListUpdated(calendars); // MainWindow에 알려줌
// }
