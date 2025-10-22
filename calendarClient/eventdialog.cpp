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


void EventDialog::setCalendars(const QList<CalendarInfo>& calendars)
{
    qDebug() << "EventDialog::setCalendars 실행 this=" << this
             << " ui->cmbCalendar=" << ui->cmbCalendar
             << " calendars.size=" << calendars.size();
    ui->cmbCalendar->clear();
    for (const auto& cal : calendars) {
        ui->cmbCalendar->addItem(cal.name, cal.id); // id를 itemData로 저장
    }

    // 선택 변경 시 m_calendarId 갱신
    connect(ui->cmbCalendar, &QComboBox::currentIndexChanged,
            this, [this](int idx){
                m_calendarId = ui->cmbCalendar->itemData(idx).toInt();
            });

    // 최소 한 번은 유효한 값으로 맞춰주기
    if (ui->cmbCalendar->count() > 0) {
        ui->cmbCalendar->setCurrentIndex(0); // 시그널 타면서 m_calendarId 세팅됨

    }
    // ui->cmbCalendar->addItems(calendars);

    qDebug() << "[EventDialog] cmbCalendar count =" << ui->cmbCalendar->count();

}

void EventDialog::onCancelButtonClicked(){
    reject();
}


void EventDialog::onSubmitButtonClicked(){

    qDebug() << "[EventDialog] submit() this=" << this
             << " m_calendarId=" << m_calendarId;

    int calId = m_calendarId;
    if (calId <= 0) { // -1 또는 0 같은 비정상 값이면 콤보박스에서 재시도
        if (ui->cmbCalendar && ui->cmbCalendar->count() > 0) {
            calId = ui->cmbCalendar->currentData().toInt();
        }
    }

    // 0-1) 최종 방어: 그래도 유효하지 않으면 진행 중단
    if (calId <= 0) {
        QMessageBox::warning(this, tr("캘린더"), tr("캘린더를 먼저 선택하세요."));
        qWarning() << "[EventDialog] submit blocked: invalid calId =" << calId
                   << "(m_calendarId=" << m_calendarId << ")";
        return;
    }

    // 1) 기본 입력값 검증 (제목 공백 금지)
    const QString title = ui->edtTitle->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, tr("입력 확인"), tr("제목을 입력하세요."));
        return;
    }

    // 2) 시간 검증 (종료가 시작보다 빠르면 막기)
    const QDateTime startDt = ui->dateTimeEdit->dateTime();
    const QDateTime endDt   = ui->dateTimeEdit_2->dateTime();
    if (endDt < startDt) {
        QMessageBox::warning(this, tr("시간 확인"), tr("종료 시간이 시작 시간보다 빠릅니다."));
        return;
    }

    EventDto addEvent;
    addEvent.calendarId = calId;
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
    qDebug() << "[일정추가 JSON: ]" << obj;

    QByteArray body = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    body += '\n';
    QByteArray byteArray;
    byteArray.reserve(128 + body.size());
    byteArray.append(header);
    byteArray.append(body);

    emit addEventRequested(byteArray);
}
