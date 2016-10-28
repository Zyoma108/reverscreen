#include "accent/selectionaccentpainter.h"

#include <QPainter>
#include <QPen>
#include <QDebug>

SelectionAccentPainter::SelectionAccentPainter(const QColor& rectColor, const QColor& shadeColor)
    : _cinemaScopePainter(shadeColor, 100)
    , _rectangleSelectedPainter(QPen(rectColor, 1, Qt::SolidLine))
    , _rectangleHighlightedPainter(QPen(rectColor, 1, Qt::DashLine))
{
}

void SelectionAccentPainter::paint(QPainter *painter, const RegionContext* context) {
    Q_ASSERT(painter != NULL);
    Q_ASSERT(context != NULL);

    if (!context->hasSelectedRegion() && !context->hasHighlightedRegion()){
        return;
    }

    const QRect& selectedRegion = context->selectedRegion();
    paint(painter, context->scopeRegion(), selectedRegion);

    const QRect& highlightedRegion = context->highlightedRegion();

    if (selectedRegion.contains(highlightedRegion, false) || selectedRegion.intersects(highlightedRegion) ) {
        QRect intersectedRegion = (selectedRegion.intersects(highlightedRegion))
                ? selectedRegion.intersected(highlightedRegion)
                : highlightedRegion;

        _rectangleHighlightedPainter.paint(painter, selectedRegion, intersectedRegion);
    }
}

void SelectionAccentPainter::paint(QPainter *painter, const QRect& scope, const QRect& region) {
    Q_ASSERT(painter != NULL);

    if (!RegionContext::isValidRegion(region)){
        return;
    }

    _cinemaScopePainter.paint(painter, scope, region);
    _rectangleSelectedPainter.paint(painter, scope, region);
    drawSizeBanner(painter, region, Qt::red);
}

void SelectionAccentPainter::drawSizeBanner(QPainter *painter, const QRect &rect, QColor baseColor)
{
    int rw = rect.width();
    int rh = rect.height();
    if (rw > 1 || rh > 1) {
        QString text = QString("%1x%2").arg(rw).arg(rh);
        QRect bannerRect(rect.bottomRight() - QPoint(80, 0), rect.bottomRight() + QPoint(0, 20));

        baseColor.setAlpha(50);
        QBrush bannerBrush(baseColor);
        painter->fillRect(bannerRect, bannerBrush);

        QPen textPen(Qt::black);
        painter->setPen(textPen);

        painter->drawText(bannerRect, text, Qt::AlignHCenter | Qt::AlignVCenter);
    }
}
