#include "dbmanager.h"
#include <QDebug>
#include <QSqlDatabase>
// 0819 [캘린더추가] 캘린더 추가를 위한 헤더파일
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

DBManager::DBManager()
{
    // QMYSQL 드라이버 사용
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}

DBManager::~DBManager()
{
    disconnect();
}

bool DBManager::connect(const QString& host, const QString& dbName,
                        const QString& user, const QString& password, int port)
{
    m_db.setHostName(host);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(password);
    m_db.setPort(port);

    if (!m_db.open()) {
        qDebug() << "DB 연결 실패:" << m_db.lastError().text();
        return false;
    }

    qDebug() << "DB 연결 성공!";
    return true;
}

void DBManager::disconnect()
{
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "DB 연결 종료";
    }
}

QString DBManager::lastError() const
{
    return m_db.lastError().text();
}




/* 테스트 코드용 함수 정의 시작
bool DBManager::insertTestUser(
                               const QString& id,
                               const QString& pw,
                               const QString& username,
                               const QString& useremail,
                               const QString& role)
{
    // DB 연결 확인
    if (!m_db.isOpen()) {
        qDebug() << "DB가 열려있지 않습니다.";
        return false;
    }
    // 쿼리 객체 생성(기본 DB 연결을 사용)
    QSqlQuery query;
    //SQL 준비문 : named placeholder(:id 등)을 사용한 prepared statement
    query.prepare(R"(
    INSERT INTO users (id, pw, username, useremail, role, created_at)
    VALUES (:id, :pw, :username, :useremail, :role, NOW())
)");
    // 각각의 named placeholder에 실제 값을 바인딩
    query.bindValue(":id", id);
    query.bindValue(":pw", pw);
    query.bindValue(":username", username);
    query.bindValue(":useremail", useremail);
    query.bindValue(":role", role);

    // 준비된 쿼리를 실행하고, 실패하면 에러로그를 출력하고 false 리턴
    if (!query.exec()) {
        qDebug() << "INSERT 실패:" << query.lastError().text();
        return false;
    }
    // 성공로그
    qDebug() << "INSERT 성공!";
    // 테스트 코드 끝
    return true;
}
*/
