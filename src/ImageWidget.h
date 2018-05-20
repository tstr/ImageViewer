/*
	Image viewer widget
*/

#pragma once

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>

class ImageWidget : public QGraphicsView
{
	Q_OBJECT

public:

	explicit ImageWidget(QWidget* parent = nullptr) :
		QGraphicsView(parent)
	{
		setScene(&m_scene);
		m_scene.addItem(&m_item);
		setDragMode(QGraphicsView::ScrollHandDrag);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setResizeAnchor(QGraphicsView::AnchorViewCenter);
	}

	void scale(qreal s) { QGraphicsView::scale(s, s); }
	QSize sizeHint() const override { return{ 400, 400 }; }

public slots:

	void setPixmap(const QPixmap& pixmap)
	{
		m_item.setPixmap(pixmap);
		auto offset = -QRectF(pixmap.rect()).center();
		m_item.setOffset(offset);
		setSceneRect(offset.x() * 4, offset.y() * 4, -offset.x() * 8, -offset.y() * 8);
		translate(1, 1);
	}

private:

	void wheelEvent(QWheelEvent* event)
	{
		qreal d = (qreal)event->delta() / 120;
		qreal magic = 8;
		scale(1.0 + (d / magic));
	}

	QGraphicsScene m_scene;
	QGraphicsPixmapItem m_item;
};