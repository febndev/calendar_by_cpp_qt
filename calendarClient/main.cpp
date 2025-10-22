#include "mainwindow.h"
#include "tcpclient.h"
#include "logindialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 0820 [MetaType] 런타임 등록
    qRegisterMetaType<CalendarInfo>("CalendarInfo");
    qRegisterMetaType<QList<CalendarInfo>>("QList<CalendarInfo>");

    // 0818 [Dock Widget 관련] QSettings 기본 키 공간 지정 (회사/ 앱명은 프로젝트에 맞춰 수정)
    QCoreApplication::setOrganizationName("MyCompany");
    QCoreApplication::setApplicationName("CalendarApp");

    MainWindow w;
    TcpClient * tcp = new TcpClient(&w);

    w.setTcpClient(tcp);
    w.show();
    return a.exec();
}
