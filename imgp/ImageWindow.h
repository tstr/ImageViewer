/*
	Image Editor main window
*/

#pragma once

#include <QMainWindow>
#include <QImage>
#include <QAbstractButton>

#include "ImagePipeline.h"

class QLabel;
class QSlider;
class QGroupBox;
class QMenu;
class ImageWidget;

class ImageWindow : public QMainWindow
{
	Q_OBJECT

public:

	explicit ImageWindow(QWidget* parent = nullptr);

	const ImagePipeline& imgproc() const { return m_img; }

public slots:

	void loadImage(const QString& imgName);
	void saveImage(const QString& saveName);

private slots:

	void saveAs();
	void open();

private:

	ImagePipeline m_img;

	ImageWidget* m_imageView;
	QGroupBox* m_filters;
	QSlider* m_gammaSlider;

	QMenu* m_fileMenu;
	QMenu* m_viewMenu;

	// Events
	void dropEvent(QDropEvent* event);
	void dragEnterEvent(QDragEnterEvent *event);


	QAbstractButton* addFilter(const QString& name, const KernelView& kernel);
	QAbstractButton* addHalftoneFilter(const QString& name, Dithering mode);

	// Setup
	void createActions();
	QWidget* createTools(QWidget* parent = nullptr);
	QWidget* createImageView(QWidget* parent = nullptr);
	QWidget* createWidgets(QWidget* parent = nullptr);
};
