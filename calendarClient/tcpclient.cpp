#include "tcpclient.h"
#include "logindialog.h"
#include <QDataStream>
#include <QDebug>
#include <QThread>


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

    // buffer의 128 byte 이후 부분을
    QString body = stripPad(QString::fromUtf8(buffer.mid(128)));

    qDebug() << "[클라이언트] Type:" << type;
    qDebug() << "[클라이언트] Body:" << body;

    // type에 따른 분기
    if(type == "login_resp")
    {
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
    else if(type=="signup_resp")
    {
        if(body =="SIGNUP_OK"){
            qDebug() << "[클라이언트] Signup successful!";
            emit signupSuccess();
        }
        else {emit signupFailed("회원가입 실패!");}
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
