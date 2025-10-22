#include "dbmanager.h"
#include "mainwindow.h"
#include "server.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QLibraryInfo>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // DB 연결
    DBManager db;
    db.connect("127.0.0.1", "calendarApp","root","12345",3306);

    // 서버 연결
    Server server;

    //화면 띄우기
    MainWindow w;
    w.show();
    return a.exec();
}

/* DB 연결 테스트용 간단 쿼리문
    if (db.connect("127.0.0.1", "calendarApp","root","12345",3306)){
        db.insertTestUser("testid", "12345", "홍길동", "hong@test.com", "A");
    } else {
        qDebug() << "DB 연결 테스트 실패:"<< db.lastError();
    }
*/
