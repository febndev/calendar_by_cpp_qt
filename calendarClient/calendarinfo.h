#ifndef CALENDARINFO_H
#define CALENDARINFO_H
#include <QString>
#include <QColor>
#include <QJsonObject>


struct CalendarInfo {
    int id;
    QString name;
    QColor color;
    QString role;
    bool can_Edit;
};


#endif // CALENDARINFO_H
