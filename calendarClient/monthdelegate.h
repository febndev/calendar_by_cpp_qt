#ifndef MONTHDELEGATE_H
#define MONTHDELEGATE_H
#include "monthmodel.h"

#include <QStyledItemDelegate>   // base class for painting items
#include <QPainter>              // QPainter for custom drawing
#include <QApplication>          // QApplication::palette for colors
#include <QStyleOptionViewItem>  // QStyleOptionViewItem for metrics

class MonthDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MonthDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent){}


    // --- Painting routine ---
    void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& idx) const override {
        // 1) Fetch required data via roles
        const QDate date = idx.data(MonthRoles::DateRole).toDate();           // date in this cell
        const bool inMonth = idx.data(MonthRoles::InCurrentMonthRole).toBool(); // is part of current month
        const bool isToday = idx.data(MonthRoles::IsTodayRole).toBool();        // today?
        const int eventCount = idx.data(MonthRoles::EventsCountRole).toInt();   // summary count

        // 2) Prepare painter and style
        p->save();                                           // save state so we can restore later
        QStyleOptionViewItem o(opt);                         // local copy we can tweak
        initStyleOption(&o, idx);                            // let base fill text/alignment/etc.

        // 3) Draw default background (handles selection highlight, etc.)
        o.text.clear();                                      // we will paint text ourselves
        o.widget->style()->drawControl(QStyle::CE_ItemViewItem, &o, p, o.widget);

        // 4) Determine text color/opacity depending on in/out of month
        QColor textColor = o.palette.color(QPalette::Text);  // base text color
        if (!inMonth) {
            textColor.setAlphaF(0.45);                       // fade out days outside current month
        }

        // 5) Draw day number in top-left with some padding
        const QRect r = o.rect;                               // cell rectangle
        const int pad = 6;                                    // padding pixels
        const QString dayStr = QString::number(date.day());   // day number text

        // If today: draw a subtle circle or rounded background behind the number
        if (isToday) {
            QRect badge = QRect(r.topLeft() + QPoint(pad-2, pad-2), QSize(22, 22)); // small circle area
            p->setRenderHint(QPainter::Antialiasing, true);   // smooth circle
            p->setBrush(o.palette.highlight());               // use highlight color
            p->setPen(Qt::NoPen);                             // no border
            p->drawEllipse(badge);                            // draw circle
        }

        // Draw the day number text
        p->setPen(isToday ? o.palette.highlightedText().color() : textColor); // contrast if today badge
        QRect textRect = QRect(r.topLeft() + QPoint(pad, pad), QSize(r.width()-2*pad, 20)); // area for number
        p->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, dayStr);       // render day number

        // 6) Draw event summary dots at bottom-left (max 4 for preview aesthetics)
        int dots = qMin(eventCount, 4);                     // cap the number of dots drawn
        if (dots > 0) {
            int dotSize = 6;                                // pixel diameter
            int spacing = 4;                                // space between dots
            int startX = r.left() + pad;                    // left padding start
            int y = r.bottom() - pad - dotSize;             // vertical position near bottom

            p->setRenderHint(QPainter::Antialiasing, true); // smooth circles
            for (int i = 0; i < dots; ++i) {
                QRect dotRect(startX + i * (dotSize + spacing), y, dotSize, dotSize); // position per dot
                p->setBrush(o.palette.link());              // use a stable accent color; later map per-calendar color
                p->setPen(Qt::NoPen);                       // no border for dots
                p->drawEllipse(dotRect);                    // draw dot
            }
        }

        p->restore();                                       // restore painter state
    }

    // --- Size hint: leave to view's section size; return a reasonable minimum ---
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        Q_UNUSED(option); Q_UNUSED(index);                   // not using for now
        return QSize(80, 64);                                // a comfortable default cell size
    }
};

#endif // MONTHDELEGATE_H
