#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QDate>
#include <QLineEdit>
#include <QStringList>

namespace Ui {
class EventDialog;
}

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = nullptr);
    ~EventDialog();

public slots:
    // 캘린더 목록을 받아서 콤보박스 채우는 슬롯
    void setCalendars(const QStringList& calendars);

private slots:
    // 일정 추가/ 수정 버튼 눌렀을 때 할 일
    // void onSubmitButtonClicked();

signals:
    // TcpClient 한테 입력된 정보 전달 시그널, 대충 썼음 수정해야함.
    // void addEventRequested(const quint32 calId, const QString & title, const QDate& starttime, const QDate& endtime, const QString& memo);

private:
    Ui::EventDialog *ui;
};

#endif // EVENTDIALOG_H
