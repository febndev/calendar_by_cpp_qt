#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

//서버에서 각 클라이언트 소켓을 담당하는 클래스 (readyRead(), disconnected() 처리)
#include <QObject>
#include <QTcpSocket>

class ClientHandler : public QObject {
    Q_OBJECT
public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);

signals:
    // 클라이언트에서 받은 캘린더 데이터
    void calendarDataReceived(const QString &jsonData);

private slots:
    void readData();
    void clientDisconnected();

private:
    QTcpSocket *socket;
};
#endif // CLIENTHANDLER_H
