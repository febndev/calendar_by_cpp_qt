#include "tcpclient.h"

// QTcpSocket 기반 클라이언트 기능 (서버 접속, 메시지 송수신)
TcpClient::TcpClient(QWidget *parent) : QMainWindow(parent)
{
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost(QHostAddress::LocalHost,9090);

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
    socketStream.setVersion(QDataStream::Qt_5_15);

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

    // client 에서 보낸 payload(순수한 데이터, 전달 메시지)를
    // buffer에서 처음 128 byte 부분만 읽어들여서 header 에 담고 fileType을 찾는다.
    QString header = buffer.mid(0,128);
    QString type = header.split(",")[0].split(":")[1];

    // buffer의 128 byte 이후 부분을
    buffer = buffer.mid(128);

    // fileType이 attachment 라면 파일 수신 로직을 실행하고
    // fileType이 message 라면 문장 수신 로직을 실핸한다.
    // 여기 밑은 다 수정해야됨. login, signup 등등으로
    if(type=="attachment")
    {
        // 파일 전송은, 1)저장될 파일 이름, 2) 파일 확장자 3) 파일 크기 정보가 필요하다.
        QString fileName = header.split(",")[1].split(":")[1];
        QString ext = fileName.split(".")[1];
        QString size = header.split(",")[2].split(":")[1].split(";")[0];

        // 파일 전송 메시지를 받으면, 메시지 박스를 띄워서 전송 받을 것인지 확인한다.
        // 메시지 박스에서 yes를 선택하면 파일을 읽는다.
        if (QMessageBox::Yes == QMessageBox::question(this, "QTCPServer", QString("You are receiving an attachment from sd:%1 of size: %2 bytes, called %3. Do you want to accept it?").arg(m_socket->socketDescriptor()).arg(size).arg(fileName)))
        {
            // 저장될 파일의 경로를 설정하고, 파일 이름과, 확장자를 설정한다.
            QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/"+fileName, QString("File (*.%1)").arg(ext));

            // file 객체를 위에서 설정한 경로를 기반으로 연결하고
            QFile file(filePath);

            // file 객체를 열고, buffer에 들어있는 byte를 쓴다(내보낸다. 통신이랑 같다).
            if(file.open(QIODevice::WriteOnly))
            {
                file.write(buffer);

                // 파일이 저장되는 것에 대한 메시지를 ui에 출력한다.
                QString message = QString("INFO :: Attachment from sd:%1 successfully stored on disk under the path %2").arg(m_socket->socketDescriptor()).arg(QString(filePath));
                // emit signal_newMessage(message);
            }
            else
                QMessageBox::critical(this,"QTCPServer", "An error occurred while trying to write the attachment.");
        }
        else
        {
            // 메시지 박스에서 No 전송 거부시 메시지를 출력한다.
            QString message = QString("INFO :: Attachment from sd:%1 discarded").arg(m_socket->socketDescriptor());
            // emit signal_newMessage(message);
        }
    }
    else if(type=="signup")
    {
        // 전송된 메시지를 출력한다.
        QString message = QString("%1 :: %2").arg(m_socket->socketDescriptor()).arg(QString::fromStdString(buffer.toStdString()));
        // emit signal_newMessage(message);
    }
}
