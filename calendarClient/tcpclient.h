#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#pragma once

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
#include <QObject>
#include <QDate>
#include <QMap>
#include <QStringList>

class TcpClient : public QMainWindow {
    Q_OBJECT
public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    // void connectToServer(const QString &host, quint16 port);
    // 캘린더 요청 전송 (JSON 포맷)
    //    void sendCalendarRequest(const QString &jsonData);
    void sendLoginRequest(const QString& id, const QString& pw);
    void sendSignupRequest(const QString &id,
                            const QString &pw,
                            const QString &username,
                            const QString &useremail,
                            const QString &level);

    // 0818 [API] 현재 연결 여부 (중복인지 검토 필요)
    bool isConnected() const;

    // 0818 [API] "월별 이벤트 카운트" 요청 보내기
quint32 requestMonthCounts(const QDate& from, const QDate& to, const QStringList& calendarIds);

signals:
    // 서버로부터 받은 캘린더 데이터
    void signal_newEvent(const QString &jsonData);
    // 로그인 결과 시그널
    void loginSuccess();
    void loginFailed(const QString& errorMessage);
    void signupSuccess();
    void signupFailed(const QString& errorMessage);
    //0818
    void connected(); //내가 이런 함수를 아직도 안썼다고..?
    void disconnected(); // 밑에 있는 slot_discardSocket(); 과 다른 것인가
    void errorOccurred(const QString& msg); // 이것도 slot_displayError 와 다른 것인가..?

    void monthCountsReceived(quint32 reqId, QDate from, QDate to, QMap<QDate,int> counts);

private:
    QTcpSocket *m_socket;
    //0818
    // 내부 패킷 송수신 처리 등
    quint32 m_nextReqId = 1; // 요청 Id 증가용
private slots:
    void slot_discardSocket();
    void slot_displayError(QAbstractSocket::SocketError socketError);
    void slot_readSocket();

    // void sendLoginRequest(const QString &id, const QString &pw);

    //void on_pushButtonSignupClicked();
    //void slot_displayMessage(const QString& str);


};

#endif // TCPCLIENT_H

