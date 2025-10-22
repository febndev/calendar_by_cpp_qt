#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QDate>
#include <QMap>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextCharFormat>
#include <QToolTip>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui {
class CalendarWidget;
}
QT_END_NAMESPACE

class QCalendarWidget;
class QLabel;
class TcpClient; // 서버와 통신할 TcpClient 전방 선언


class CalendarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarWidget(TcpClient* tcp, QWidget *parent = nullptr);
    ~CalendarWidget();


    // 이벤트 개수(요약) 전달: 날짜별로 점(도트) 표시
    void setEventCounts(const QMap<QDate, int>& counts);
public slots:
    // 오늘로 이동
    void goToday();
    // 활성 캘린더 Id 리스트 설정 (필터 체크박스와 연결)
    void setActiveCalendars(const QStringList& ids);

private slots:

    // 이전달 이동
    void goPrevMonth();
    // 다음달 이동
    void goNextMonth();
    // 월 이동
    void onMonthChanged(int year, int month);
    // 날짜 더블클릭했을 때 실행할 것. (정의 끝나지 않았음)
    void onDateDoubleClicked(const QDate& d);
    // TcpClient로부터 월별 카운트 응답을 받았을 때 처리 -> 이게 뭔지 모르것어 시부럴
    void onMonthCountsReceived(quint32 reqId, QDate from, QDate to, QMap<QDate,int> counts);

private:
    // Designer UI 객체
    Ui::CalendarWidget *ui;

    // 내부 위젯들(디자이너 없이 코드로 생성)
    QCalendarWidget* m_cal; // 달력 본체
    QLabel* m_title; // 상단 "YYYY년 M월"제목
    // 서버와 통신할 TcpClient 주입 받은거기 때문에 중복 아님
    TcpClient* m_tcp = nullptr;

    // 날짜별 이벤트 개수(0~N). 1~4개까지 점으로 표시
    QMap<QDate, int> m_counts;
    // 필터된 캘린더 ID들 예) my, TeamA
    QStringList m_activeCals;
    // 마지막으로 보낸 요청 ID (응답 매칭용이라는데 이거 뭔지 하나도 모르것다 시부
    quint32 m_lastReqId = 0;
    // 제목 초기세팅으로 다시 표시
    void rebuildTitle();
    // 날짜에 이벤트 수만큼 점 표시
    void repaintDots();
    // 현재 보이는 월 범위로 서버에 "월별 카운트" 요청 보내기
    void requestCountsForCurrentMonth();

};

#endif // CALENDARWIDGET_H
