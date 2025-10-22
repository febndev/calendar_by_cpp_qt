#ifndef SERVER_H
#define SERVER_H
// 서버 시작, 새 클라이언트 연결 감지, 연결관리

#include <QMainWindow>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaType>
#include <QSet>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>
// #include "clienthandler.h"

class Server : public QMainWindow { //UI 로 서버 상태를 확인할 수도 있으니까 QTcpServer 가 아닌 QMainWindow 상속
    Q_OBJECT
public:
    // 서버 시작 , 중지
    explicit Server(QWidget *parent = nullptr);
    ~Server();

private:
    QTcpServer* m_server;
    QSet<QTcpSocket*> qset_connectedClients;

    void appendToSocketList(QTcpSocket* socket); // 소켓 리스트에 추가

signals:
    // 클라이언트로부터 캘린더 데이터(요청/응답) 수신 시 알림
    void signal_newEvent(const QString &jsonData);
// 아직 무슨 함수가 필요한지 제대로 안정해져서 막 적었음.
private slots:
    void slot_newConnection();
    void slot_discardSocket();
    void slot_readSocket();
    void slot_displayError(QAbstractSocket::SocketError socketError);
//여기서부터는 그냥 일단 적었음.
    //void slot_displayedEvent();

    //void on_pushButtonLogin();
    //void on_pushButtonSignup();



};
#endif // SERVER_H
