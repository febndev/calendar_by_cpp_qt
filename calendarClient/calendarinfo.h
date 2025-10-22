#ifndef CALENDARINFO_H
#define CALENDARINFO_H
#include <QString>
#include <QColor>
#include <QJsonObject>
#include <QMetaType>

struct CalendarInfo {
    int id;
    QString name;
    QColor color;
    QString role;
    bool can_Edit;
};
Q_DECLARE_METATYPE(CalendarInfo)
Q_DECLARE_METATYPE(QList<CalendarInfo>)

#endif // CALENDARINFO_H

