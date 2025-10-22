#ifndef TCPCLIENT_H
#define TCPCLIENT_H


// QMainWindow 기반 클라이언트 기능 (서버 접속, 메시지 송수신)
#include <QMainWindow>
#include <QAbstractSocket>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QMetaType>
#include <QString>
#include <QStandardPaths>
#include <QTcpSocket>

class TcpClient : public QMainWindow {
    Q_OBJECT
public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    // void connectToServer(const QString &host, quint16 port);
    // 캘린더 요청 전송 (JSON 포맷)
//    void sendCalendarRequest(const QString &jsonData);

private:
    QTcpSocket *m_socket;

signals:
    // 서버로부터 받은 캘린더 데이터
    void signal_newEvent(const QString &jsonData);

private slots:
    void slot_discardSocket();
    void slot_displayError(QAbstractSocket::SocketError socketError);
    void slot_readSocket();

    //void on_pushButtonLoginClicked();
    //void on_pushButtonSignupClicked();
    //void slot_displayMessage(const QString& str);


};

#endif // TCPCLIENT_H
