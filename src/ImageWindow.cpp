/*
	Image Editor main window
*/

#include <QLabel>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QGroupBox>

#include "ImageWindow.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImageWindow::ImageWindow(QWidget* parent) :
	QMainWindow(parent)
{
	QWidget* center = new QWidget(this);
	center->setLayout(new QHBoxLayout(center));
	center->layout()->addWidget(createSideBar());
	center->layout()->addWidget(createImageView());
	this->setCentralWidget(center);
}

QWidget* ImageWindow::createSideBar()
{
	QGroupBox* ctrlArea = new QGroupBox("Options:", this);
	//ctrlArea->setLayout(ctrlLayout);
	ctrlArea->setMaximumWidth(200);
	ctrlArea->setMinimumWidth(100);

	return ctrlArea;
}

QWidget* ImageWindow::createImageView()
{
	m_imageView = new QLabel(this);
	m_imageView->setText("empty");
	return m_imageView;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImageWindow::setImage(const QImage& image)
{
	m_imageView->setPixmap(QPixmap::fromImage(image));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
