#ifndef DBMANAGER_H
#define DBMANAGER_H

#pragma once
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QMap> // 날짜별 개수 맵
#include <QDate>
#include <QStringList> // 다중캘린더 필터 등


class DBManager : public QObject
{
    Q_OBJECT
public:
    DBManager();
    ~DBManager();

    bool connect(const QString& host = "127.0.0.1",
                 const QString& dbName = "calendarApp",
                 const QString& user = "root",
                 const QString& password = "12345",
                 int port = 3306);
/* 이벤트 갯수 세는 함수
    QMap<QDate,int> fetchEventCounts(const QDate& fromDate,
                                      const QDate& toDate,
                                      const QStringList& calendarIds = {}) const;
*/
    void disconnect();
    QString lastError() const;

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
