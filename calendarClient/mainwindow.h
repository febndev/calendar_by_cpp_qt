#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// 로고 및 앱 초기진입 창
#include "calendarinfo.h"

#include <QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <QCalendarWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QWidget>
#include <QMessageBox>
#include <QCheckBox>
#include <QListWidget> // [Dock] 왼쪽 : 캘린더 선택 패널
#include <QGroupBox>
#include <QMainWindow> // [Dock] 메인 프레임

// 무거워서 cpp파일에서 include 하는게 좋다그래서 옮김. 대신 밑에 class로 간단하게.
//#include <QListWidget>
//#include <QAction> // [Dock] toggleViewAction() 반환
//#include <QSettings> // [Dock] saveState/restoreState 저장소 뭔데 이거 시부
//#include <QCloseEvent> // [Dock] closeEvent 오버라이드
//#include <QDockWidget> // [Dock] 도크 본체
//#include <QMenuBar> // [Dock] View 메뉴
//#include <QTextEdit> // [Dock] 오른쪽 : 이벤트 상세 패널

class QPushButton;
class Logo;
class LoginDialog;
class TcpClient;
class QDockWidget;
class QTextEdit;
//0819 [Dock]
class QTreeView;
class QStandardItemModel;
class QPushButton;
//0819 [Right Dock]
class EventDialog;
class AddCalendarDialog;
class QLineEdit;
// 0820 공유할 캘린더 선택용 콤보박스
class QComboBox;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// 0819[Dock - QtreeView]
// struct CalendarInfo {
//     int id; // 캘린더 고유 ID (DB Key)
//     QString name; // 표시 이름
//     QColor color; // 표시 색상
//     QString role; // 권한 (owner, editor, viewer 등)
//     bool visible; // 기본 표시 여부
// };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void setTcpClient(TcpClient* t); // 서버 - 클라이언트 연결 한번만 하게끔 끌어다쓰는거
// 0818 [Dock] 프로텍티드로 closeEvent 선언
protected:
    void closeEvent(QCloseEvent* e) override; // 도크 레이아웃 저장

private slots:
    void openLoginDialog(); // 버튼 클릭시 실행되는 슬롯
    // void showCalendarWidget(); // 캘린더 화면 보여주기
    void onLoginSuccess();
    void onLoginFailed(const QString& ErrorMsg);
    void openSignupDialog();
    //0819 [우 Dock] 버튼 클릭시 실행할 slot
    void openAddEventDialog(); // 일정추가
    void openAddCalDialog(); // 캘린더추가
    void onClickShare(); // 달력 공유하기 버튼

private:
    Ui::MainWindow *ui;
    LoginDialog* loginDlg = nullptr;
    TcpClient* tcp = nullptr;
    LoginDialog *currentLoginDialog;  // 현재 로그인 다이얼로그 참조용
    //0818 [Dock] 도크용 객체 4개 선언
    // 왼쪽 : 캘린더 토글 도크
    QDockWidget* m_calDock = nullptr;
    // QListWidget* m_calList = nullptr;
    // 0819[Dock] 왼쪽 리스트 변경
    QTreeView* m_calTree = nullptr;
    QStandardItemModel* m_calModel = nullptr;

    // 0819 [우 Dock] 0818 작성 내용 전부 수정
    // 오른쪽 : 이벤트 상세 도크
    QDockWidget* m_rightDock = nullptr;
    QLineEdit* m_inviteEdit = nullptr;
    QPushButton *m_btnAddEvent = nullptr;
    QPushButton *m_btnAddCalendar = nullptr;
    QPushButton *m_btnShare = nullptr;
    //0820 공유할 달력 선택용 콤보박스 객체 선언
    QComboBox* m_cmbCalendars = nullptr;
    // 달력과 Dock 세팅하는 함수
    void showCalendarWidget();
    void setupSideDocks();     // ← 최소 도킹 패널 생성
    //0818[Dock] 함수 세개 선언
    void setupDocks(); // 도크 생성, 부착
    void restoreUiState(); // 도크 상태 복원 (생성 후 호출)
    void applyCalendarFilter(); // 캘린더 체크 변경시 필터
    //0819[Dock] TreeView 와 Standarditemmodel 관련 함수 추가
    void setupCalendarTreeModel(); // 트리 모델 구성
    void loadCalendars(const QList<CalendarInfo>& list); // 서버/DB목록 로드
    //0819 [우 Dock] 오른쪽 Dock 구성하는 함수
    void setupRightDock();
    // 0819 달력 왼쪽 도크 새로고침
    QStringList m_calNameCache; // 최신 캘린더 이름 목록 캐시
    QList<CalendarInfo> m_calCache; // 최신목록 캐시
    void refreshCalendars(); //
    // 0820 캘린더 id 를 가져오는 함수
    int currentSelectedCalId() const;


};

#endif // MAINWINDOW_H
