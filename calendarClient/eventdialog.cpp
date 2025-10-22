#include "eventdialog.h"
#include "ui_eventdialog.h"
#include "EventDto.h"

EventDialog::EventDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EventDialog)
{
    ui->setupUi(this);
    // 일정추가 창 눌렀을 때 현재 날짜, 시간으로 뜨게끔 하는 코드
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    // 시간은 무시하고 날짜만 뜨게끔 하는 코드
    /*ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd");  // 시간 표시 제거
    ui->dateTimeEdit_2->setDisplayFormat("yyyy-MM-dd");*/  // 시간 표시 제거

    // 추가하기 버튼 눌렀을 때 onLoginButtonClicked() 실행
    connect(ui->submitButton, &QPushButton::clicked,
            this, &EventDialog::onSubmitButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &EventDialog::onCancelButtonClicked);

    // m_calendarId가 유효하다면 콤보박스 초기 선택 맞추기
    if (m_calendarId >= 0) {
        for (int i = 0; i < ui->cmbCalendar->count(); ++i) {
            if (ui->cmbCalendar->itemData(i).toInt() == m_calendarId) {
                ui->cmbCalendar->setCurrentIndex(i);
                break;
            }
        }
    }

}

EventDialog::~EventDialog()
{
    delete ui;
}


void EventDialog::setCalendars(const QStringList &calendars)
{
    qDebug() << "[EventDialog] setCalendars called with" << calendars
             << "this=" << this << "cmb=" << ui->cmbCalendar;
    qDebug() << "[EventDialog] setCalendars called: "<< calendars;
    ui->cmbCalendar->clear();
    ui->cmbCalendar->addItems(calendars);

    qDebug() << "[EventDialog] cmbCalendar count =" << ui->cmbCalendar->count();

}

void EventDialog::onCancelButtonClicked(){
    reject();
}


void EventDialog::onSubmitButtonClicked(){

    EventDto addEvent;
    qDebug() << "[EventDialog] submit() this=" << this
             << " m_calendarId=" << m_calendarId;

    const int calId = m_calendarId;

    addEvent.title = ui->edtTitle->text().trimmed();
    addEvent.memo = ui->txtMemo->toPlainText();
    addEvent.startUtc = (ui->dateTimeEdit->dateTime())
                            .toString("yyyy-MM-dd HH:mm:ss");
    addEvent.endUtc = (ui->dateTimeEdit_2->dateTime())
                          .toString("yyyy-MM-dd HH:mm:ss");

    // JSON 객체로 만들기
    QByteArray header = QString("type:add_event,name:null,size:").toUtf8();
    header.resize(128);

    // JSON 객체로 만들기
    QJsonObject obj;
    obj["calendarId"] = calId;
    obj["title"] = addEvent.title;
    obj["memo"] = addEvent.memo;
    obj["start_time"] = addEvent.startUtc;
    obj["end_time"] = addEvent.endUtc;
    qDebug() << "[일정추가: ]" << obj;

    QByteArray body = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    body += '\n';
    QByteArray byteArray;
    byteArray.reserve(128 + body.size());
    byteArray.append(header);
    byteArray.append(body);

    emit addEventRequested(byteArray);
}
