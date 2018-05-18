/*
	Image Editor main window
*/

#pragma once

#include <QMainWindow>
#include <QImage>
#include <QAbstractButton>

class QLabel;
class QGroupBox;

class ImageWindow : public QMainWindow
{
	Q_OBJECT

public:

	explicit ImageWindow(QWidget* parent = nullptr);

	QAbstractButton* addFilter(const QString& name);

public slots:

	void setImage(const QImage& image);

private:

	QLabel* m_imageView;
	QGroupBox* m_filters;

	QWidget* createSideBar();
	QWidget* createImageView();
};
