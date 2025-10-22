#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QDate>
#include <QLineEdit>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QDateTime>

namespace Ui {
class EventDialog;
}

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = nullptr);
    ~EventDialog();
    void setCalendarId(int id) { m_calendarId = id;
        qDebug() << "[EventDialog] setCalendarId =" << id << "this=" << this;
    }


public slots:
    // 캘린더 목록을 받아서 콤보박스 채우는 슬롯
    void setCalendars(const QStringList& calendars);

private slots:
    // 일정 추가/ 수정 버튼 눌렀을 때 할 일
    void onSubmitButtonClicked();
    void onCancelButtonClicked();

signals:
    // TcpClient 한테 입력된 정보 전달 시그널
    void addEventRequested(const QByteArray& byteArray);

private:
    Ui::EventDialog *ui;
    int m_calendarId; //캘린더 고유값 (calId 값 저장할 변수)
};

#endif // EVENTDIALOG_H
