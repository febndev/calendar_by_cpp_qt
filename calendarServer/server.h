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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QByteArray>

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
    void sendErrorToClient(QTcpSocket *clientSocket, const QString &errorMsg);
    void sendOkToClient(QTcpSocket *clientSocket, const QString &okMsg);


signals:
    // 클라이언트로부터 캘린더 데이터(요청/응답) 수신 시 알림
    void signal_newEvent(const QString &jsonData);
    // 아직 무슨 함수가 필요한지 제대로 안정해져서 막 적었음.
    //0814 코드 시작
    // 로그인 검증 결과 전송 시그널 (이 신호를 TcpClient에서 받아서 달력창을 띄우라고 시그널을 보내야함)
    void signal_loginResult();
    // 회원가입 결과 전송 시그널 (이 신호를 받아서 또 Tcpclient에서 로그인 창을 띄우라고 시그널을 보내야함)
    void signal_signupResult();

private slots:
    void slot_newConnection();
    void slot_discardSocket();
    void slot_readSocket();
    void slot_displayError(QAbstractSocket::SocketError socketError);
    //여기서부터는 그냥 일단 적었음.
    //void slot_displayedEvent();

    //void on_pushButtonLogin();
    //void on_pushButtonSignup();

    // 0814 코드
    // 로그인 신호가 오면 (TcpClient 에서 시그널을 보내야함) 처리하는 slot 함수
    //void slot_checkLogin();
    // 회원가입 신호가 오면 (TcpClient에서 시그널을 보내야함) 저장하고 저장 완료했다고 시그널을 쏴야함
    //void slot_saveSignup();
    // 아니 잠깐만 이거 하면 안된다는거잖아. slot_readSocket()에서 분기해야된다는 거잖아.

};
#endif // SERVER_H
