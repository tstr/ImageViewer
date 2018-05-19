/*
	Image Editor main window
*/

#pragma once

#include <QMainWindow>
#include <QImage>
#include <QAbstractButton>

#include "ImageProcessor.h"

class QLabel;
class QGroupBox;

class ImageWindow : public QMainWindow
{
	Q_OBJECT

public:

	explicit ImageWindow(QWidget* parent = nullptr);

	QAbstractButton* addFilter(const QString& name, const Kernel<3,3>& kernel);
	QAbstractButton* addFilter(const QString& name, const Kernel<5,5>& kernel);

	const ImageProcessor& imgproc() const { return m_img; }

public slots:

	void loadImage(const QString& imgName);
	void saveImage(const QString& saveName);

private slots:

	void setImage(const QImage& image);
	void saveAs();
	void open();

private:

	ImageProcessor m_img;

	QLabel* m_imageView;
	QGroupBox* m_filters;

	void setup();

	void dropEvent(QDropEvent* event);
	void dragEnterEvent(QDragEnterEvent *event);

	QWidget* createSideBar(QWidget* parent = nullptr);
	QWidget* createImageView(QWidget* parent = nullptr);
	QWidget* createWidgets(QWidget* parent = nullptr);
};
