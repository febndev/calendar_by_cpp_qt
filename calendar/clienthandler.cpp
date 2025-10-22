#include "clienthandler.h"
//서버에서 각 클라이언트 소켓을 담당하는 클래스 (readyRead(), disconnected() 처리)
#include <QDebug>

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent)
    : QObject(parent) {
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::readData);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::clientDisconnected);

    qDebug() << "클라이언트 연결됨:" << socketDescriptor;
}

void ClientHandler::readData() {
    QString jsonData = QString::fromUtf8(socket->readAll());

    // 서버로 전달
    emit calendarDataReceived(jsonData);

    qDebug() << "클라이언트 요청 데이터:" << jsonData;
}

void ClientHandler::clientDisconnected() {
    qDebug() << "클라이언트 연결 종료";
    socket->deleteLater();
}
