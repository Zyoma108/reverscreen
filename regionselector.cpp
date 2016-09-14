// Qt
#include <QPainter>
#include <QDebug>

// app
#include <regionselector.h>
#include <accent/accentpainter.h>


static const QColor RegionColor = Qt::red;
static const QColor ShaderColor = QColor::fromRgba(0x50a0a0a0);

static const int CheckerSize = 8;

RegionSelector::RegionSelector(QWidget *parent, const QImage& image)
    : QWidget(parent)
    , _image(image)
    , _accentPainter(NULL)
{
    setAutoFillBackground(false);
    resize(image.size());
}

void RegionSelector::setSelectionStrategy(QSharedPointer<SelectionStrategy> strategy, QCursor cursor)
{
    _strategy = strategy;

    setCursor(cursor);
    update();
}

void RegionSelector::setAccentPainter(AccentPainter *accentPainter)
{
    if (_accentPainter) {
        delete _accentPainter;
    }

    _accentPainter = accentPainter;

    update();
}

QRect RegionSelector::selectedRegion() const
{
    return QRect(_startPoint, _endPoint).normalized();
}

void RegionSelector::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.drawImage(0, 0, _image);

    if (_startPoint != _endPoint && _accentPainter != NULL) {
        QRect scope(0, 0, _image.width(), _image.height());
        QRect region(_startPoint, _endPoint);
        _accentPainter->paint(&painter, scope, region.normalized());
    }
}

void RegionSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        _endPoint = _startPoint = _strategy->begin(event->pos());
        update();
        event->accept();

        emit signalSelectionStarted();
    }
}

void RegionSelector::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
        emit signalSelectionFinished();
    }
}

void RegionSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        event->accept();

        _endPoint = _strategy->end(QRect(_startPoint, event->pos()));

        update();
    }
}

void RegionSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        emit signalSelectionCancelled();
    }
}

void RegionSelector::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}

void RegionSelector::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();

    QRect hint = _strategy->hint(event->pos());
    _startPoint = hint.topLeft();
    _endPoint = hint.bottomRight();

    update();
}

void RegionSelector::drawRegionRect(QPainter &painter)
{
    QPen pen(RegionColor);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(_startPoint.x(), _startPoint.y(),
                     _endPoint.x()-_startPoint.x(), _endPoint.y()-_startPoint.y());
}

void RegionSelector::drawRegionShaders(QPainter &painter)
{
    QBrush brush(ShaderColor);

    int min_x = qMin(_startPoint.x(), _endPoint.x());
    int min_y = qMin(_startPoint.y(), _endPoint.y());
    int max_x = qMax(_startPoint.x(), _endPoint.x());
    int max_y = qMax(_startPoint.y(), _endPoint.y());

    // top shader
    painter.fillRect(0, 0, _image.width(), min_y, brush);

    // bottom shader
    painter.fillRect(0, max_y + 1, _image.width(), _image.height() - max_y, brush);

    // left shader
    painter.fillRect(0, min_y, min_x, max_y - min_y + 1, brush);

    // right shader
    painter.fillRect(max_x + 1, min_y, _image.width() - max_x, max_y - min_y + 1, brush);
}
