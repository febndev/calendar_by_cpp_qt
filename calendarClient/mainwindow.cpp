#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "logo.h"
#include "signupdialog.h"
#include "tcpclient.h"
#include "calendarwidget.h"
//0819 [우Dock]
#include "eventdialog.h"
#include "addcalendardialog.h"

// 무거운 헤더 파일은 여기에 삽입하라는데 두고 보자고
// 무거운 헤더 목록
#include <QDockWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QMenuBar>
#include <QAction>
#include <QSettings>
#include <QCloseEvent>
// 0819[Dock] QTreeView 관련
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QColor>
#include <QHeaderView>
#include <QPainter>
#include <QSignalBlocker>
// 0819 끝
// 0819 [Dock - 정수 상수] COL 은 QStandardItemModel 열인덱스 ROLE 은 데이터 저장/조회 할때 역할 키
namespace {
static constexpr int COL_NAME  = 0;
static constexpr int COL_ROLE  = 1;
static constexpr int COL_ID    = 2;

static constexpr int ROLE_CAL_ID    = Qt::UserRole + 1;
static constexpr int ROLE_CAL_COLOR = Qt::UserRole + 2;
static constexpr int ROLE_CAL_ROLE  = Qt::UserRole + 3;
}

namespace {
// 작은 색상 점 아이콘 생성 유틸 , 다른 클래스나 위젯에서도 재사용할거면 클래스 안으로 옮겨야함
static QIcon makeColorDot(const QColor& c, int size = 12)
{
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setBrush(c);
    p.setPen(Qt::NoPen);
    p.drawEllipse(0, 0, size, size);
    return QIcon(pm);
}
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 로고 위젯 생성
    Logo *logo = new Logo(this);
    // 중앙 위젯 교체
    setCentralWidget(logo);

    // 시작하기 버튼 누르기
    auto *startButton = logo->findChild<QPushButton*>("startButton");
    connect(startButton, &QPushButton::clicked,
            this, &MainWindow::openLoginDialog);

    loginDlg = new LoginDialog(this);
    loginDlg->setAttribute(Qt::WA_DeleteOnClose, false); // 메인과 함께 살도록


    // 회원가입 버튼 → 회원가입 다이얼로그 열기
    connect(loginDlg, &LoginDialog::signupRequested, this, [this](){
        openSignupDialog();  // 더 이상 매개변수로 login 넘기지 않음
    });

    // 로그인 요청 → TCP 전송/응답 연결 (tcp가 준비된 뒤에도 한 번 더 안전 연결)
    if (tcp) {
        connect(loginDlg, &LoginDialog::loginRequested,
                tcp, &TcpClient::sendLoginRequest, Qt::UniqueConnection);
        connect(tcp, &TcpClient::loginSuccess,
                this, &MainWindow::onLoginSuccess, Qt::UniqueConnection);
        connect(tcp, &TcpClient::loginFailed,
                this, &MainWindow::onLoginFailed, Qt::UniqueConnection);
    }
    // 0819 [우Dock] 우측 Dock 구성
    //setupRightDock();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 회원가입 요청이 들어왔을 때 실행되는 함수 TcpClient와 SignupDialog 연결
void MainWindow::openSignupDialog()
{
    // login->hide();
    // 로그인 다이얼로그가 살아있을 때, 그 "자식"으로 모달 실행 -> 이게 뭔말이여
    if(!loginDlg) return;

    SignUpDialog signup(loginDlg);

    if (tcp) {
        // 1) 회원가입 요청 보내기: SignUpDialog → TcpClient
        connect(&signup, &SignUpDialog::sendSignupRequested,
                tcp,    &TcpClient::sendSignupRequest);

        // 2) 서버 응답 처리: TcpClient → (여기서 다이얼로그 제어)
        connect(tcp, &TcpClient::signupSuccess, &signup, [&](){
            QMessageBox::information(&signup,"회원가입 완료", "가입이 완료되었습니다. 로그인해 주세요.");
            signup.accept();
        });

        connect(tcp, &TcpClient::signupFailed, &signup, [&](const QString& reason){
            QMessageBox::warning(&signup, "회원가입 실패",
                                 reason.isEmpty() ? "회원가입에 실패했습니다." : reason);
            // 실패해도 다이얼로그는 열어둠(사용자가 수정 후 재시도 가능)
        });

        signup.exec();
        loginDlg->raise();
        loginDlg->activateWindow();
    }
}

void MainWindow::openLoginDialog()
{
    // 스택으로 새로 만들지 말고 멤버 dlg만 사용
    if (!loginDlg)
        loginDlg = new LoginDialog(this);

    // 모달이면 exec(), 비모달이면 show()로 해야함.
    // 둘 중 하나만 선택
    const int ret = loginDlg->exec();
    if (ret == QDialog::Accepted) {
        QMessageBox::information(this, "로그인 성공", "환영합니다!");
    } else {
        QMessageBox::information(this, "로그인 취소", "로그인 창이 닫혔습니다.");
    }
}

// TCP와 LoginDialog 연결 함수
void MainWindow::setTcpClient(TcpClient* t)
{
    tcp = t;
    if (loginDlg && tcp) {
        connect(loginDlg, &LoginDialog::loginRequested,
                tcp, &TcpClient::sendLoginRequest, Qt::UniqueConnection);
        connect(tcp, &TcpClient::loginSuccess,
                this, &MainWindow::onLoginSuccess, Qt::UniqueConnection);
        connect(tcp, &TcpClient::loginFailed,
                this, &MainWindow::onLoginFailed, Qt::UniqueConnection);
    }
}

// 달력 보여주기 함수
void MainWindow::showCalendarWidget()
{
    qDebug() << "[MAINWINDOW] Showing calendar widget";
    // Q_ASSERT(tcp); // 네트워크 통신 오류 방어하는 코드라는데 글쎄다
    auto *calendarWidget = new CalendarWidget(tcp,this);
    setCentralWidget(calendarWidget);

    // 달력 크기 최소폭/사이즈 확장
    calendarWidget->setMinimumWidth(800);
    calendarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    qDebug() << "[MAINWINDOW] Calendar widget set as central widget";
    setWindowTitle("일정 관리 - 캘린더");

    // 달력 중앙뷰가 자리 잡은 '다음'에 도크 세팅 + 상태 복원
    setupDocks();
    restoreUiState();
}

// 로그인 성공했을때 달력화면으로 변경하는 함수
void MainWindow::onLoginSuccess()
{
    qDebug() << "[MAINWINDOW] Login success received!";

    // 멤버 다이얼로그에 직접 접근
    if (loginDlg)
        loginDlg->accept(); // exec()를 끝내고 메인 캘린더로 진입

    showCalendarWidget();
}

// 로그인 실패했을때 메시지 띄우는 함수 -> 이거 다시 로그인 시도하게끔 해야되나?
void MainWindow::onLoginFailed(const QString& errorMessage)
{
    qDebug() << "[MAINWINDOW] Login failed:" << errorMessage;
    QMessageBox::warning(this, "로그인 실패", errorMessage);
}


// 0818[Dock] 도크 구현 함수 정의
void MainWindow::setupDocks()
{
    // 0819 [Dock - TreeView] calList 부분을 TreeView에 맞춰서 수정
    // ===== [왼쪽] 캘린더 토글 도크 =====
    if (!m_calDock){
        m_calDock = new QDockWidget(tr("Calendars"), this);
        m_calDock->setObjectName("dock_calendars"); // 상태 복원 식별자(필수)
        m_calDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        m_calDock->setFeatures(QDockWidget::DockWidgetClosable
                               |QDockWidget::DockWidgetMovable
                               |QDockWidget::DockWidgetFloatable);
        // 트리 + 모델 생성
        m_calTree  = new QTreeView(m_calDock);
        m_calModel = new QStandardItemModel(m_calTree);
        m_calTree->setModel(m_calModel);
        m_calTree->setRootIsDecorated(false); // 트리 화살표 제거(테이블처럼 보이게)
        m_calTree->setAlternatingRowColors(true);
        m_calTree->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_calTree->setSelectionMode(QAbstractItemView::SingleSelection);
        // 0819 좌Dock과 우Dock합치는 과정에서 추가된 코드
        m_calDock->setWidget(m_calTree);
        addDockWidget(Qt::LeftDockWidgetArea, m_calDock);
    }

    // 트리 헤더 / 데이터 로딩 (불필요한 신호 방지)
    {
        QSignalBlocker blocker(m_calModel); // 로딩 중 신호 차단(안전) - 뭐시가 안전하단거여
        setupCalendarTreeModel(); // 헤더/컬럼 구성

        // (예시) 서버/DB에서 받아왔다고 가정한 목록
        QList<CalendarInfo> sample = {
            { "cal_personal", "Personal", QColor(60,179,113), "owner",  true  },
            { "cal_team_a",   "Team A",   QColor(65,105,225),  "editor", true  },
            { "cal_proj_x",   "Project X",QColor(220, 20, 60), "viewer", false }
        };
        loadCalendars(sample);
    }

    // 체크 변경 감지: 모델의 itemChanged 신호 사용
    connect(m_calModel, &QStandardItemModel::itemChanged, this, [this](QStandardItem*){
        applyCalendarFilter();
    });
    // 좌Dock 중복으로 실행돼서 주석 처리
    // m_calDock->setWidget(m_calTree);
    // addDockWidget(Qt::LeftDockWidgetArea, m_calDock);
    //0819 수정 끝

    // ===== [오른쪽] 메뉴 도크 =====

    // Dock이 아직 없으면 생성, 있으면 내부 위젯만 교체
    if (!m_rightDock) {
        m_rightDock = new QDockWidget(tr("메뉴"), this);
        m_rightDock->setObjectName("RightDock");
        // 필요 시 아래 옵션으로 고정/이동 등 제어 가능
        // m_rightDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        addDockWidget(Qt::RightDockWidgetArea, m_rightDock);
    }

    // 패널(컨테이너) 준비
    QWidget *panel = new QWidget(m_rightDock);
    auto *vbox = new QVBoxLayout(panel);
    vbox->setContentsMargins(8,8,8,8);
    vbox->setSpacing(8);

    // 1) 초대 라벨 + LineEdit
    auto *lbl = new QLabel(tr("캘린더 공유할 ID(또는 이메일)"), panel);
    if (!m_inviteEdit) {
        m_inviteEdit = new QLineEdit(panel);
        m_inviteEdit->setObjectName("inviteLineEdit");
        m_inviteEdit->setPlaceholderText(tr("캘린더를 공유할 id를 입력해주세요."));
        m_inviteEdit->setClearButtonEnabled(true);
    } else {
        m_inviteEdit->setParent(panel); // 부모 재지정
    }

    // 2) 버튼 3개(중복 생성 방지)
    if (!m_btnAddEvent) {
        m_btnAddEvent = new QPushButton(tr("일정 추가"), panel);
        m_btnAddEvent->setObjectName("btnAddEvent");
        connect(m_btnAddEvent, &QPushButton::clicked,
                this, &MainWindow::onClickAddEvent, Qt::UniqueConnection);
    } else {
        m_btnAddEvent->setParent(panel);
    }

    if (!m_btnAddCalendar) {
        m_btnAddCalendar = new QPushButton(tr("캘린더 추가"), panel);
        m_btnAddCalendar->setObjectName("btnAddCalendar");
        connect(m_btnAddCalendar, &QPushButton::clicked,
                this, &MainWindow::onClickAddCalendar, Qt::UniqueConnection);
    } else {
        m_btnAddCalendar->setParent(panel);
    }

    if (!m_btnShare) {
        m_btnShare = new QPushButton(tr("공유하기"), panel);
        m_btnShare->setObjectName("btnShare");
        connect(m_btnShare, &QPushButton::clicked,
                this, &MainWindow::onClickShare, Qt::UniqueConnection);
    } else {
        m_btnShare->setParent(panel);
    }

    // 배치
    vbox->addWidget(lbl);
    vbox->addWidget(m_inviteEdit);
    vbox->addSpacing(4);
    vbox->addWidget(m_btnAddEvent);
    vbox->addWidget(m_btnAddCalendar);
    vbox->addWidget(m_btnShare);
    vbox->addStretch();

    // Dock에 패널 장착 (이전 위젯 정리)
    if (QWidget *old = m_rightDock->widget()) old->deleteLater();
    m_rightDock->setWidget(panel);

    // ===== [View 메뉴] 토글 액션 추가 (중복 방지)
    QMenu *viewMenu = nullptr;
    // 1) 이미 만든 메뉴가 있나 찾아보고
    for (auto *m : menuBar()->findChildren<QMenu*>()) {
        if (m->title() == tr("View")) { viewMenu = m; break; }
    }
    // 2) 없으면 새로 생성
    if (!viewMenu) {
        viewMenu = menuBar()->addMenu(tr("View"));
        viewMenu->setObjectName("menuView");
    }
    // 3) 액션이 중복으로 들어가지 않게 체크
    auto addToggleOnce = [viewMenu](QDockWidget *dock) {
        QAction *act = dock->toggleViewAction();
        // 이미 존재하는지 확인
        bool exists = false;
        for (QAction *a : viewMenu->actions())
            if (a == act) { exists = true; break; }
        if (!exists) viewMenu->addAction(act);
    };
    addToggleOnce(m_calDock);
    addToggleOnce(m_rightDock);

    // 0819 [Dock] 좌, 우 사이즈 조절
    const int W = width();
    int leftWidth = W * 20 / 100;
    int rightWidth = W * 15 / 100;

    // resizeDocks({m_calDock, m_rightDock}, {leftWidth, rightWidth}, Qt::Horizontal);
    // 좌우는 서로 다른 DockArea이므로 각각 한 번씩 호출하는 게 확실함
    resizeDocks({m_calDock}, {leftWidth}, Qt::Horizontal); // 왼쪽 영역 폭 보정
    resizeDocks({m_rightDock}, {rightWidth}, Qt::Horizontal); // 오른쪽 영역 폭 보정

    // 과도한 확장 방지 가드(원하면 값 조정)
    m_calDock ->setMinimumWidth(100);
    m_calDock ->setMaximumWidth(W/3);
    m_rightDock->setMinimumWidth(100);
    m_rightDock->setMaximumWidth(W/3);


    // 기본 가시성 확보(복원 상태가 없거나 잘못 저장된 경우 대비)
    m_calDock->show();
    m_rightDock->show();

    /* 0819 한번 주석처리 해보자..
    // Dock에 패널 장착 (이전 QTextEdit 교체)
    QWidget *old = m_rightDock->widget();
    m_rightDock->setWidget(panel);
    if (old) old->deleteLater();   // 기존 QTextEdit 메모리 정리

    // 시그널 연결
    connect(m_btnAddEvent,     &QPushButton::clicked, this, &MainWindow::onClickAddEvent);
    connect(m_btnAddCalendar,  &QPushButton::clicked, this, &MainWindow::onClickAddCalendar);
    connect(m_btnShare,        &QPushButton::clicked, this, &MainWindow::onClickShare);

    // 상단 메뉴 → View에 토글 추가(사용자가 껐다 켤 수 있게)
    QMenu* viewMenu = menuBar()->addMenu(tr("View"));
    viewMenu->addAction(m_calDock->toggleViewAction());
    viewMenu->addAction(m_rightDock->toggleViewAction());
    */
}

void MainWindow::restoreUiState()
{
    // 도크/툴바 레이아웃 복원 (생성 완료 후 호출해야 위치가 맞음)
    QSettings s; // main.cpp에서 org/app 지정했다고 가정
    const QByteArray state = s.value("mainwindow/state").toByteArray();
    if (!state.isEmpty())
        restoreState(state);

    // (선택) 창 크기/위치까지 쓰고 싶다면:
    // restoreGeometry(s.value("mainwindow/geometry").toByteArray());
}
// 0819 수정 TreeView에 맞춰서 수정하였음.
void MainWindow::applyCalendarFilter()
{
    QStringList activeIds;

    // 모델의 모든 행을 돌며 체크된 캘린더만 모음
    const int rows = m_calModel->rowCount();
    for (int r = 0; r < rows; ++r)
    {
        QStandardItem* nameItem = m_calModel->item(r, COL_NAME);
        if (!nameItem) continue;

        const bool checked = (nameItem->checkState() == Qt::Checked);
        if (!checked) continue;

        const QString calId = nameItem->data(ROLE_CAL_ID).toString();
        activeIds << calId;
    }

    // TODO: 여기서 네트워크/모델 반영
    // 예:
    // QDate from = ...; QDate to = ...;  // 현재 월 범위
    // tcp->requestMonthCounts(from, to, activeIds);
    // calendarModel->setActiveCalendars(activeIds);
    // calendarView->repaint();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    // 현재 도크 레이아웃 저장
    QSettings s;
    s.setValue("mainwindow/state", saveState());
    // s.setValue("mainwindow/geometry", saveGeometry()); // 선택

    QMainWindow::closeEvent(e);
}
// 도크 함수 정의 끝

//0819 [Dock - TreeView] 세팅
void MainWindow::setupCalendarTreeModel()
{
    // 헤더 타이틀
    m_calModel->setColumnCount(3);
    m_calModel->setHeaderData(COL_NAME, Qt::Horizontal, tr("Calendar"));
    m_calModel->setHeaderData(COL_ROLE, Qt::Horizontal, tr("Permission"));
    m_calModel->setHeaderData(COL_ID,   Qt::Horizontal, tr("ID"));

    // 헤더 리사이즈 정책
    auto* hdr = m_calTree->header();
    hdr->setStretchLastSection(false);
    hdr->setSectionResizeMode(COL_NAME, QHeaderView::Stretch);
    hdr->setSectionResizeMode(COL_ROLE, QHeaderView::ResizeToContents);
    hdr->setSectionResizeMode(COL_ID,   QHeaderView::ResizeToContents);

    // ID 컬럼은 사용자에게 안 보여줘도 되면 숨김
    m_calTree->setColumnHidden(COL_ID, true);
}
// 서버에서 캘린더 목록 받아왔을 때 이 함수에 그대로 넣어주면 된다.
// QList<CalendarInfo> List 만들어서 loadCalendars(list);
void MainWindow::loadCalendars(const QList<CalendarInfo>& list)
{
    m_calModel->removeRows(0, m_calModel->rowCount());

    for (const auto& cal : list)
    {
        // 0: Name(+체크박스 + 색상 아이콘)
        auto* nameItem = new QStandardItem(cal.name);
        nameItem->setEditable(false);
        nameItem->setCheckable(true);
        nameItem->setCheckState(cal.enabled ? Qt::Checked : Qt::Unchecked);
        nameItem->setIcon(makeColorDot(cal.color));
        // 사용자 데이터로 숨김 정보 저장 (행의 대표 아이템에 몰아넣음)
        nameItem->setData(cal.id,    ROLE_CAL_ID);
        nameItem->setData(cal.color, ROLE_CAL_COLOR);
        nameItem->setData(cal.role,  ROLE_CAL_ROLE);

        // 1: Role
        auto* roleItem = new QStandardItem(cal.role);
        roleItem->setEditable(false);

        // 2: ID (숨김 예정)
        auto* idItem = new QStandardItem(cal.id);
        idItem->setEditable(false);

        // 한 행으로 추가
        QList<QStandardItem*> row { nameItem, roleItem, idItem };
        m_calModel->appendRow(row);
    }
}

// 0819 [우Dock] 버튼 클릭시 실행되는 slots 추가
void MainWindow::onClickAddEvent()
{
    // “일정 추가” 다이얼로그 열기 (선택한 날짜/캘린더 등은 이후 개선)
    EventDialog dlg(this);
    // TODO: dlg.setDefaultDate(selectedDate);  // 달력 선택과 연동하면 베스트
    if (dlg.exec() == QDialog::Accepted) {
        // 다이얼로그에서 입력받은 정보를 서버로 등록 요청
        // 예) m_tcp->requestAddEvent(dlg.toEventPayload());
        //     repaint/refresh (월별 카운트, 일정 목록 갱신)
    }
}

void MainWindow::onClickAddCalendar()
{
    AddCalendarDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        // 예) m_tcp->requestAddCalendar(dlg.calendarName(), dlg.color());
        //     캘린더 목록/토글 UI 갱신 (우리가 정의한 Menu2 영역)
    }
}

void MainWindow::onClickShare()
{
    const QString invitee = m_inviteEdit ? m_inviteEdit->text().trimmed() : QString();
    if (invitee.isEmpty()) {
        QMessageBox::warning(this, tr("공유하기"), tr("초대할 ID 또는 이메일을 입력하세요."));
        return;
    }
    if (!tcp) {
        QMessageBox::critical(this, tr("공유하기"), tr("네트워크 연결이 없습니다."));
        return;
    }

    // 서버로 “캘린더 공유” 요청 전송
    // ---- 패킷 예시 (우리 프로젝트 프로토콜에 맞게 구현) ----
    // m_tcp->requestShareCalendar(/*calendarId*/ currentCalendarId, invitee);

    // 우선은 스텁 호출 형태로 남겨둘게:
    // TODO: 구현 후 성공/실패 시 서버 응답 받아서 메시지 표시
    //       ex) statusBar()->showMessage("공유 요청을 보냈습니다.", 3000);

    QMessageBox::information(this, tr("공유하기"),
                             tr("'%1'에게 공유 요청을 전송했습니다(가정).").arg(invitee));
}
