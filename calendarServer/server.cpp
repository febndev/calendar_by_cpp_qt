#include "server.h"



// 생성자 호출시
Server::Server(QWidget *parent): QMainWindow(parent){
    m_server = new QTcpServer();
    //listen 함수 실행
    if(m_server->listen(QHostAddress::Any, 9091))
    {
        qDebug() << "서버 시작 성공!";
        connect(m_server, &QTcpServer::newConnection,
                this,     &Server::slot_newConnection);

        /*connect(this, &Server::signal_newEvent,
                this, &Server::slot_displayedEvent)*/;

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
        q.prepare("SELECT pw FROM users WHERE id = ?");
        q.addBindValue(id);

        if (!q.exec()){
            sendErrorToClient(socket, "DB_ERROR");
            return;
        }

        if (!q.next()){
            sendErrorToClient(socket, "NO_USER");
            return;
        }

        QString dbPw = q.value(0).toString();
        if (pw == dbPw){
            sendOkToClient(socket, "LOGIN_OK");
            qDebug() << "로그인성공!";
            qDebug() << "[SERVER] THIS LINE SHOULD APPEAR!";

            // sendOkToClient 대신 간단한 코드
            socket->write("LOGIN_OK");
            socket->flush();

            qDebug() << "[SERVER] Simple response sent!";
        } else {
            sendErrorToClient(socket, "WRONG_PASSWORD");
        }
    }
    else if(type=="signup")
    {

        // 회원정보 추출
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

        // ID 검증 함수 넣어서 ID 중복 검증 통과하면 DB에 추가하는 함수, 실패하면 에러메시지 출력
        QString newId = id;
        q.prepare("SELECT COUNT(*) FROM users WHERE id = ?");
        q.addBindValue(newId);

        if (!q.exec()) {
            qDebug() << "ID check failed:" << q.lastError().text();
            return;
        }

        // 이 쿼리문이 제대로 실행될지 모르겠음..

        if (q.next()){
            if(q.value(0).toInt() > 0) {
                qDebug() << "중복된 ID입니다.";
                QMessageBox::warning(this, "중복 ID", "이미 사용 중인 ID입니다.\n다른 아이디를 입력하세요.");
                return;
            }
        }

        // INSERT 실행
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

        sendOkToClient(socket,"SIGNUP_OK");

    }
}
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
    // 디버깅 코드
    qDebug() << "[SERVER] === sendOkToClient STARTED ===";
    qDebug() << "[SERVER] Parameters check:";
    qDebug() << "[SERVER]   clientSocket pointer:" << (void*)clientSocket;
    qDebug() << "[SERVER]   okMsg:" << okMsg;
    qDebug() << "[SERVER]   okMsg length:" << okMsg.length();

    if (!clientSocket || !clientSocket->isOpen()) {
        qDebug() << "[SERVER] Cannot send OK - socket invalid";
        return;
    }

    qDebug() << "[SERVER] Sending OK to client:" << okMsg;

    // 헤더 생성: type을 login_resp로 설정
    QByteArray header;
    if (okMsg == "LOGIN_OK"){
        header.prepend(QString("type:login_resp,name:null,size:%1")
                           .arg(okMsg.size()).toUtf8());

    } else if (okMsg == "SIGNUP_OK") {
        header.prepend(QString("type:signup_resp,name:null,size:%1").arg(okMsg.size()).toUtf8());
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

