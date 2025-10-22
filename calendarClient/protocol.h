#ifndef PROTOCOL_H
#define PROTOCOL_H

#pragma once
// 클라이언트/서버 공통 프로토콜 정의

#include <QtGlobal>
#include <QString>

// 패킷 식별용 매직(고정값). 'EVT1' (0x45 0x56 0x54 0x31)
static constexpr quint32 PROTOCOL_MAGIC = 0x45565431;

// 오퍼레이션 코드(확장 가능)
enum class OpCode : quint16 {
    // Event
    ADD_EVENT_REQ = 1001,
    ADD_EVENT_ACK = 1002,

    // (추가 예정)
    // UPDATE_EVENT_REQ = 1011,
    // UPDATE_EVENT_ACK = 1012,
    // DELETE_EVENT_REQ = 1021,
    // DELETE_EVENT_ACK = 1022,
};

// 참고용 구조체 (직렬화는 PacketBuilder/Parser에서 수행)
struct AddEventReq {
    qint32  calendarId;
    QString title;
    QString memo;
    qint64  startEpochSec; // UTC epoch seconds
    qint64  endEpochSec;   // UTC epoch seconds
    qint32  createdBy;
};

struct AddEventAck {
    qint32  result;    // 0=OK, else=error code
    qint32  eventId;   // PK of events.id (성공 시)
    QString message;   // 오류 상세(선택)
};


#endif // PROTOCOL_H
