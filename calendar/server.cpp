#include "server.h"


// 생성자 호출시
Server::Server(QWidget *parent): QMainWindow(parent){
    m_server = new QTcpServer();
    //listen 함수 실행
    if(m_server->listen(QHostAddress::Any, 9090))
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
    // appendToSocketList 함수로 연결 객체 관리 처리
    while (m_server->hasPendingConnections())
        appendToSocketList(m_server->nextPendingConnection());

}

void Server::appendToSocketList(QTcpSocket* socket)
{
    qDebug() << "클라이언트 연결됨!";

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

    // [ex.02.4.5]
    // 소켓 디스크립터로 대상 선택 가능하도록 ui 표시
    // ui->comboBox_receiver->addItem(QString::number(socket->socketDescriptor()));

    // [ex.02.4.6]
    // 연결된 클라이언트 정보와, 소켓 디스크립터(정수 식별자) 출력
   // slot_displayMessage(QString("INFO :: Client with sockd:%1 has just entered the room").arg(socket->socketDescriptor()));
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

    // QByteArray 타입의 buffer를 만들고
    QByteArray buffer;

    // 서버에 연결된 socket을 stream으로 연결한다.
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);

    // stream으로 데이터를 읽어들이고, buffer로 넘기면
    socketStream.startTransaction();
    socketStream >> buffer;

    // stream startTransaction 실행 문제시 에러 표시 후 함수 종료
    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Waiting for more data to come..").arg(socket->socketDescriptor());
        // emit singal_newMessage(message);
        return;
    }

    // client 에서 보낸 payload(순수한 데이터, 전달 메시지)를
    // buffer에서 처음 128 byte 부분만 읽어들여서 header 에 담고 fileType을 찾는다.
    QString header = buffer.mid(0,128);
    QString type = header.split(",")[0].split(":")[1];

    // buffer의 128 byte 이후 부분을
    buffer = buffer.mid(128);


    if(type=="login")
    {
    }
    else if(type=="signup")
    {

    }
}
