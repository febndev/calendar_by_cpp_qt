#include "calendarwidget.h"
#include "ui_calendarwidget.h"

#include <QHeaderView>
#include <QTableView>

#include "tcpclient.h"

CalendarWidget::CalendarWidget(TcpClient* tcp, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalendarWidget)
    , m_cal(nullptr)        // 달력 위젯 포인터 null로 초기화
    , m_title(nullptr)      // 제목 라벨 포인터 null로 초기화
    , m_tcp(tcp)            // tcp 주입 저장
{
    ui->setupUi(this);      // Designer UI 연결 (비어있어도 호출해두면 안전하다는데)

    // 최상위 레이아웃: 세로 배치 (위: 버튼/제목, 아래: 달력)
    auto* root = new QVBoxLayout(this);
    // 상단 레이아웃: 좌우 버튼 + 가운데 제목
    auto* top = new QHBoxLayout();
    auto* prevBtn = new QPushButton("◀", this); // 이전 달 이동 버튼
    auto* nextBtn = new QPushButton("▶", this); // 다음 달 이동 버튼
    m_title = new QLabel(this); // "2025년 8월" 같은 제목 표시
    m_title->setStyleSheet("font-weight:600; font-size:16px; color:#0b2242;"); // 제목 굵고 크게

    // 상단 좌우 이동 버튼 스타일
    prevBtn->setStyleSheet(R"(
    QPushButton {
        background:#cfe2ff; color:#0b2242; border:0; border-radius:8px; padding:4px 10px;
    }
    QPushButton:hover { background:#9ec5fe; })");

    nextBtn->setStyleSheet(prevBtn->styleSheet());

    // 상단 레이아웃에 버튼과 라벨 배치
    top->addWidget(prevBtn);
    top->addStretch(); // 가운데로 밀어주기용 공간
    top->addWidget(m_title);
    top->addStretch();
    top->addWidget(nextBtn);

    // 달력 위젯 생성
    m_cal = new QCalendarWidget(this);
    m_cal->setStyleSheet(R"(
    /* 달력 전체 박스 */
    QCalendarWidget {
        background-color:#eaf3ff;         /* 전체 배경 옅은 파랑 */
        border:1px solid #9cc3ff;         /* 외곽선 연한 파랑 */
    }
    /* 상단 네비게이션 바 (년/월 영역) */
    QCalendarWidget QWidget#qt_calendar_navigationbar {
        background:#eaf3ff;               /* 네비바 배경 */
    }
    /* 네비게이션 버튼(<<, <, >, >>, 월 표시 스핀박스 버튼 등) */
    QCalendarWidget QToolButton {
        color:#0b2242;                     /* 글자색 남색 */
        background:#cfe2ff;                /* 버튼 배경 */
        border:0; border-radius:8px; padding:4px 8px;
    }
    QCalendarWidget QToolButton:hover { background:#9ec5fe; }

    /* 날짜 그리드 뷰 */
    QCalendarWidget QAbstractItemView:enabled {
        background:#ffffff;                /* 그리드 배경 */
        color:#0b2242;                     /* 날짜 글자색 */
        selection-background-color:#3a86ff;/* 선택 날짜 배경 */
        selection-color:white;             /* 선택 날짜 글자 */
        outline:0;
    }
    QCalendarWidget QAbstractItemView:disabled { color:#9aa4b2; }
)");
    m_cal->setGridVisible(true);            // 그리드 표시
    m_cal->setFirstDayOfWeek(Qt::Monday);   // 월요일부터 시작
    m_cal->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader); // 주차 헤더 숨김

    //루트 레이아웃에 추가
    root->addLayout(top);
    root->addWidget(m_cal);

    // 시그널 연결
    connect(prevBtn, &QPushButton::clicked,
            this, &CalendarWidget::goPrevMonth); // 클릭시 이전달로 이동
    connect(nextBtn, &QPushButton::clicked,
            this, &CalendarWidget::goNextMonth); // 클릭시 다음 달로 이동
    connect(m_cal, &QCalendarWidget::currentPageChanged,
            this, &CalendarWidget::onMonthChanged); // 달력페이지 이동, 제목/ 포맷갱신

    // 날짜 더블클릭 처리
    // QCalendarWidget은 기본 doubleClicked 시그널이 없어서 셀 클릭 2번을 이용 (간단 처리)
    connect(m_cal, &QCalendarWidget::clicked, this, [this](const QDate& d){
        static QDate last; // 마지막 클릭 날짜
        static qint64 lastMs=0; //마지막 클릭 시각(ms)
        const qint64 now = QDateTime::currentMSecsSinceEpoch(); // 현재시간
        // 같은 날짜를 350ms 이내 두 번 클릭시 더블클릭으로 간주
        if (d == last && (now - lastMs) < 350) {
            onDateDoubleClicked(d); // 더블클릭 슬롯 호출
        }
        last = d; lastMs = now; // 상태갱신
    });

    // TcpClient 응답 신호 연결(월별 카운트 수신)
    if (m_tcp) {
        connect(m_tcp, &TcpClient::monthCountsReceived,
                this, &CalendarWidget::onMonthCountsReceived);
    }

    // ---- [중요] 도트 오버레이 생성 ----
    m_overlay = new QWidget(m_cal);
    m_overlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_overlay->setAttribute(Qt::WA_TranslucentBackground, true);
    m_overlay->setGeometry(m_cal->rect());
    m_overlay->raise();
    // viewport가 리사이즈/리페인트 될 때 오버레이도 따라가도록 이벤트 필터
    m_cal->installEventFilter(this);
    m_overlay->installEventFilter(this);
    // 초기 제목과 날짜 강조 적용
    rebuildTitle();
    repaintDots();

    // 시작 시 현재 월 기준으로 서버에 카운트 요청
    requestCountsForCurrentMonth();

    // 데모용 데이터(원하면 삭제)
    // QMap<QDate,int> demo;
    // QDate today = QDate::currentDate();
    // demo[today] = 3;
    // demo[today.addDays(2)] = 1;
    // demo[today.addDays(5)] = 5; // 4개까지만 점 표시
    // setEventCounts(demo);
    // 근데 왜 점이 표시가 안되냐고

}

CalendarWidget::~CalendarWidget()
{
    delete ui;
}
// [API] 날짜별 카운트 외부 설정(보통 서버 응답 시 호출)
void CalendarWidget::setEventCounts(const QMap<QDate, int>& counts)
{
    m_counts = counts; // 복사해서 내부 저장
    repaintDots(); // 날짜별 툴팁, 포맷 갱신
}

// 0818 [슬롯] 오늘로 이동
void CalendarWidget::goToday()
{
    const QDate today = QDate::currentDate();
    m_cal->setSelectedDate(today);
    m_cal->showSelectedDate();
    // 오늘과 다른 달이면 페이지 전환 -> onMonthChanged 에서 자동 재요청
    if (m_cal->yearShown() != today.year() || m_cal->monthShown() != today.month()) {
        m_cal->setCurrentPage(today.year(), today.month());
    }
}
// 0818 [슬롯] 활성 캘린더 ID 설정 -> 즉시 재요청
void CalendarWidget::setActiveCalendars(const QStringList& ids)
{
    m_activeCals = ids;
    requestCountsForCurrentMonth();
}

// [슬롯] 이전 달 이동
void CalendarWidget::goPrevMonth()
{
    int y = m_cal->yearShown(); // 현재 연도
    int m = m_cal->monthShown(); // 현재 월
    if (--m == 0) { m = 12; --y; } // 1월이면 12월로 이동
    m_cal->setCurrentPage(y, m); // 달력 페이지 변경
}

// [슬롯] 다음 달 이동
void CalendarWidget::goNextMonth()
{
    int y = m_cal->yearShown();
    int m = m_cal->monthShown();
    if (++m == 13) { m = 1; ++y; } // 12월이면 다음해 1월로 이동
    m_cal->setCurrentPage(y, m);
}

// [슬롯] 달력 페이지 변경시
void CalendarWidget::onMonthChanged(int year, int month)
{
    Q_UNUSED(year); // 지금은 사용 안함
    Q_UNUSED(month);
    rebuildTitle(); // 제목갱신
    requestCountsForCurrentMonth(); // ← 페이지 바뀌면 즉시 재요청
    repaintDots(); // 강조, 툴팁 갱신
}

// [슬롯] 날짜 더블클릭 유사 동작
void CalendarWidget::onDateDoubleClicked(const QDate& d)
{
    // 여기서 이벤트 다이얼로그 띄우기/신규 등록 등으로 연결
    // 데모: 툴팁
    QToolTip::showText(QCursor::pos(),
                       QString("더블클릭: %1").arg(d.toString("yyyy-MM-dd")), this);
    // TODO: 여기서 “해당 날짜의 상세 이벤트 목록 요청” 패킷을 TcpClient로 보내도 됨

}

// [헬퍼] 제목 라벨 갱신
void CalendarWidget::rebuildTitle()
{
    m_title->setText(QString("%1년 %2월")
                         .arg(m_cal->yearShown()).arg(m_cal->monthShown()));
}

// [헬퍼] 날짜 포맷/ 이벤트 표시 갱신
void CalendarWidget::repaintDots()
{
    // 요일 포맷 초기화(중요)
    QTextCharFormat base;
    QBrush weekdayBrush(QColor("#0b2242")); // 남색
    base.setForeground(weekdayBrush);
    m_cal->setWeekdayTextFormat(Qt::Monday, base);
    m_cal->setWeekdayTextFormat(Qt::Tuesday, base);
    m_cal->setWeekdayTextFormat(Qt::Wednesday, base);
    m_cal->setWeekdayTextFormat(Qt::Thursday, base);
    m_cal->setWeekdayTextFormat(Qt::Friday, base);
    QTextCharFormat weekend = base;
    weekend.setForeground(QColor("#24579b"));
    m_cal->setWeekdayTextFormat(Qt::Saturday, weekend);
    m_cal->setWeekdayTextFormat(Qt::Sunday, weekend);

    // 오늘 강조 포맷
    QTextCharFormat todayFmt;
    todayFmt.setBackground(QBrush(QColor(58,134,255,60))); // #3a86ff 투명
    todayFmt.setForeground(QBrush(QColor("#0b2242")));
    m_cal->setDateTextFormat(QDate::currentDate(), todayFmt);

    // 현 월 범위 구하기
    int y = m_cal->yearShown();
    int m = m_cal->monthShown();
    QDate first(y, m, 1);
    int startDow = first.dayOfWeek();         // 1=Mon..7=Sun
    QDate gridStart = first.addDays(-(startDow - 1)); // 달력 첫칸(월요일 기준)

    // 6x7 셀 모두 갱신
    for (int i=0; i<42; ++i) {
        QDate d = gridStart.addDays(i);

        // 기본 포맷부터 시작
        QTextCharFormat f = m_cal->dateTextFormat(d);
        // 이번 달 외의 날짜는 반투명
        if (d.month() != m) {
            QColor c("#4b5563");
            c.setAlphaF(0.45);
            f.setForeground(c);
        }
        //0823 [일정표시]
        const int cnt = m_counts.value(d, 0);
        if (cnt > 0) {
            f.setToolTip(QString("이벤트 %1개").arg(cnt)); // 표시용
        }
        if (d != QDate::currentDate()) {
            m_cal->setDateTextFormat(d, f);
        }
    }

    // ---- 오버레이 다시 그리기 ----
    if (m_overlay) m_overlay->update();

/* 기존 코드
        // 이벤트 점(문자 기반 간단 표시: 아래 첨자 점들)
        int cnt = m_counts.value(d, 0);
        if (cnt > 0) {
            int dots = qMin(cnt, 4);
            // 문자열 뒤에 작은 점 유니코드 추가 (●), 보여지는 글씨 크기에 따라 보임
            QString mark(dots, QChar(0x2022)); // •• (중점). 진한 점은 25CF(●)
            // 날짜 텍스트를 건드릴 수 없으니, 배경색으로 살짝 띄우기 + 툴팁 제공
            f.setToolTip(QString("이벤트 %1개").arg(cnt));
            // 강조 배경을 아주 옅게
            f.setBackground(QBrush(QColor(58,134,255,18)));
            // 글꼴 아래첨자 느낌은 어려우니, 툴팁으로 대체
        }

        // 오늘은 이미 위에서 별도 포맷 주었지만, out-of-month면 오늘 강조를 덮지 않게 조건
        if (d != QDate::currentDate()) {
            m_cal->setDateTextFormat(d, f);
        }

    }
*/
}

//0818 [헬퍼] 현재 월 범위로 서버에 "월별 카운트"요청
void CalendarWidget::requestCountsForCurrentMonth()
{
    if(!m_tcp) return; // TCpClient가 없으면 요청 불가

    // [from, to) 반개구간: 이번 달 1일 부터 다음 달 1일 전까지
    const int y = m_cal->yearShown();
    const int m = m_cal->monthShown();
    const QDate from(y, m, 1);
    const QDate to = from.addMonths(1);

    //TcpClient에 "월별 카운트 요청" API 호출 -> 요청 ID를 받아 저장
    // 응답이 올 때 reqId로 매칭해서 구분 가능
    m_lastReqId = m_tcp->requestMonthCounts(from, to, m_activeCals);
}

//[슬롯] 서버에서 월별 카운트 응답을 받았을 때 처리
void CalendarWidget::onMonthCountsReceived(quint32 reqId, QDate from, QDate to, QMap<QDate,int> counts)
{
    // 내가 마지막으로 보낸 요청과만 매칭
    if (reqId != m_lastReqId) return;

    // 현재 보이는 월과 응답 범위가 다르면 (사용자가 빨리 넘긴 경우) 무시해도 됨.
    // 여긴 단순화 : 그대로 반영
    setEventCounts(counts);
}

//0823 [일정표시] 이 필터로 오버레이 위치/리페인트 동기화 시키는거
bool CalendarWidget::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == m_cal) {
        switch (ev->type()) {
        case QEvent::Resize:
            if (m_overlay) m_overlay->setGeometry(m_cal->rect());
            break;
        case QEvent::UpdateRequest:
        case QEvent::Paint:
            if (m_overlay) m_overlay->update(); // 뷰가 다시 그려질 때 오버레이도 갱신
            break;
        default:
            break;
        }
    }
    else if (obj == m_overlay && ev->type() == QEvent::Paint) {
        QPainter p(m_overlay);
        p.setRenderHint(QPainter::Antialiasing, true);
        paintDotsOnOverlay(p, m_overlay->rect());
        return true; // 우리가 그렸음
    }
    return QWidget::eventFilter(obj, ev);
}

void CalendarWidget::paintDotsOnOverlay(QPainter& p, const QRect& vpRect)
{
    // 뷰포트 크기/셀이미 맞추기
    const int cols = 7;
    const int y = m_cal->yearShown();
    const int m = m_cal->monthShown();
    const QDate first(y, m, 1);
    const int startDow = first.dayOfWeek();      // 1=Mon..7=Sun
    const int daysInMonth = first.daysInMonth();

    // 대략적인 행 수 계산 (6행 기준으로 그리면 가장 안전)
    const int rows = 6;
    const int cellW = vpRect.width() / cols;
    const int cellH = vpRect.height() / rows;

    // 첫 칸의 셀 인덱스 (월요일 시작)
    int c = (startDow - 1); // 0..6
    int r = 0;

    QDate d = first;
    for (int i=0; i<daysInMonth; ++i, d = d.addDays(1)) {
        const QRect cell(vpRect.left() + c*cellW,
                         vpRect.top()  + r*cellH,
                         cellW, cellH);

        const int cnt = m_counts.value(d, 0);
        if (cnt > 0) {
            const int maxDots = 4;
            const int n = qMin(cnt, maxDots);

            // 점 지름/배치
            const int dotD = qMin(10, cellH/8);
            const int gap  = dotD/2;
            const int totalW = n*dotD + (n-1)*gap;
            const int yPos = cell.bottom() - dotD - 4;
            int xPos = cell.center().x() - totalW/2;

            p.setRenderHint(QPainter::Antialiasing, true);
            p.setPen(Qt::NoPen);
            for (int k=0; k<n; ++k) {
                p.setBrush(QColor("#3a86ff")); // 기본 테마 블루 (캘린더 색 적용은 이후 확장 포인트)
                p.drawEllipse(QRect(xPos, yPos, dotD, dotD));
                xPos += dotD + gap;
            }

            // 개수 초과시 +N
            if (cnt > maxDots) {
                p.setPen(QColor("#394a6a"));
                p.setBrush(Qt::NoBrush);
                p.drawText(QRect(cell.right()-22, yPos-2, 22, dotD+4),
                           Qt::AlignCenter, QString("+%1").arg(cnt-maxDots));
            }
        }

        // 다음 셀
        if (++c >= cols) { c = 0; ++r; }
    }
}
