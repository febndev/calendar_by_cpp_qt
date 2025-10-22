#ifndef EVENTDTO_H
#define EVENTDTO_H
// 0819 [file] 지피티가 지금이라도 만들라고 해서 만들었음.
#pragma once
// 이벤트 데이터 DTO (UI ↔ TcpClient 내부 전달용)

#include <QString>
#include <QDateTime>

struct EventDto {
    int      calendarId = 0;      // events.calendar_id
    QString  title;               // events.title (<=255)
    QString  memo;                // events.memo (TEXT)
    QDateTime startUtc;           // events.start_time (UTC)
    QDateTime endUtc;             // events.end_time   (UTC)
    int      createdBy = 0;       // events.created_by
};

#endif // EVENTDTO_H
