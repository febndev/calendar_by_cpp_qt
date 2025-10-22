#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QDate>
#include <QLineEdit>

namespace Ui {
class dlgEvent;
}

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = nullptr);
    ~EventDialog();
// 0819 내가 쓴 코드 내가 쓴다 지피티새끼야
private slots:
    // 일정 추가/ 수정 버튼 눌렀을 때 할 일
    void onSummitButtonClicked();

signals:
    // TcpClient 한테 입력된 정보 전달 시그널, 대충 썼음 수정해야함.
    void addEventRequested(const quint32 calId, const QString & title, const QDate& starttime, const QDate& endtime, const QString& memo);

private:
    Ui::dlgEvent *ui;
};

#endif // EVENTDIALOG_H
