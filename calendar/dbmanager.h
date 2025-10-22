#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

class DBManager
{
public:
    DBManager();
    ~DBManager();

    bool connect(const QString& host = "127.0.0.1",
                 const QString& dbName = "calendarApp",
                 const QString& user = "root",
                 const QString& password = "12345",
                 int port = 3306);

    void disconnect();
    QString lastError() const;

    // --- 테스트용 INSERT 함수 ---
    bool insertTestUser(int id,
                        const QString& userid,
                        const QString& pw,
                        const QString& username,
                        const QString& useremail,
                        const QString& role);
private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
