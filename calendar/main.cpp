#include "dbmanager.h"
#include "mainwindow.h"
#include "server.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DBManager db;
    if (db.connect("127.0.0.1", "calendarApp","root","12345",3306)){
        db.insertTestUser(1, "testid", "1234", "홍길동", "hong@test.com", "A");
    } else {
        qDebug() << "DB 연결 테스트 실패:"<< db.lastError();
    }

    Server server;

    MainWindow w;
    w.show();
    return a.exec();
}
