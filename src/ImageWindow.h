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
class ImageWidget;

class ImageWindow : public QMainWindow
{
	Q_OBJECT

public:

	explicit ImageWindow(QWidget* parent = nullptr);

	QAbstractButton* addFilter(const QString& name, const KernelView& kernel);

	const ImageProcessor& imgproc() const { return m_img; }

public slots:

	void loadImage(const QString& imgName);
	void saveImage(const QString& saveName);

private slots:

	void saveAs();
	void open();

private:

	ImageProcessor m_img;

	ImageWidget* m_imageView;
	QGroupBox* m_filters;

	// Events
	void dropEvent(QDropEvent* event);
	void dragEnterEvent(QDragEnterEvent *event);

	// Setup
	void createActions();
	QWidget* createSideBar(QWidget* parent = nullptr);
	QWidget* createImageView(QWidget* parent = nullptr);
	QWidget* createWidgets(QWidget* parent = nullptr);
};
