/*
	Image Editor main window
*/

#pragma once

#include <QMainWindow>
#include <QImage>

class QLabel;

class ImageWindow : public QMainWindow
{
	Q_OBJECT

public:

	explicit ImageWindow(QWidget* parent = nullptr);

public slots:

	void setImage(const QImage& image);

private:

	QImage m_imageSource;
	QImage m_imageResult;
	QLabel* m_imageView;

	QWidget* createSideBar();
	QWidget* createImageView();

};
