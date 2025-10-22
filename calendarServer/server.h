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
//0819 [DB] 회원가입시 개인 캘린더 자동 추가하기 위해서 헤더파일 삽입
#include <QDataStream>
#include <QHash>
#include <QVariant>
// 0821 [JSON]
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class Server : public QMainWindow { //UI 로 서버 상태를 확인할 수도 있으니까 QTcpServer 가 아닌 QMainWindow 상속
    Q_OBJECT
public:
    // 서버 시작 , 중지
    explicit Server(QWidget *parent = nullptr);
    ~Server();

private:
    QTcpServer* m_server;
    QSet<QTcpSocket*> qset_connectedClients;
    // 사용자 코드 usercode
    QHash<QTcpSocket*, int> m_userBySocket;

    void appendToSocketList(QTcpSocket* socket); // 소켓 리스트에 추가
    void sendErrorToClient(QTcpSocket *clientSocket, const QString &errorMsg);
    void sendOkToClient(QTcpSocket *clientSocket, const QString &okMsg);
    //0819 앞에 Type 추가 하는 함수
    // void sendTyped(QTcpSocket *clientSocket, const QString& respType, const QString& body);
    void sendCalendarList(QTcpSocket* socket, int userId);
    //0820 [캘린더 초대]
    // 로그인 사용자 찾기 등 사용자 구분 함수
    int userCodeOf(QTcpSocket* sock) const;
    // 초대처리
    bool inviteUserToCalendar(int calId, const QString& inviteeLoginId,
                              int inviterCode, QString* err);
    void notifyUserCalendarsDirty(int userCode); // 알림 브로드캐스트
    void sendCalendarTotalList(QTcpSocket* socket, int userCode);


signals:
    // 클라이언트로부터 캘린더 데이터(요청/응답) 수신 시 알림
    void signal_newEvent(const QString &jsonData);

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

};
#endif // SERVER_H
