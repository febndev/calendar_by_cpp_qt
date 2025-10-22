#ifndef EVENTDTO_H
#define EVENTDTO_H

#pragma once
// 이벤트 데이터 DTO (UI ↔ TcpClient 내부 전달용)

#include <QString>
#include <QDateTime>

struct EventDto {
    int      calendarId = 0;      // events.calendar_id
    QString  title;               // events.title (<=255)
    QString  memo;                // events.memo (TEXT)
    QString startUtc;           // events.start_time (UTC)
    QString endUtc;             // events.end_time   (UTC)
    int      createdBy = 0;       // events.created_by
};

#endif // EVENTDTO_H
