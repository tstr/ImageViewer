/*
	Image Editor main window
*/

#include <QLabel>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>

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
	m_filters = new QGroupBox("Filters:", this);
	m_filters->setLayout(new QFormLayout(m_filters));
	m_filters->setMaximumWidth(200);
	m_filters->setMinimumWidth(100);
	m_filters->setAlignment(Qt::AlignTop);

	return m_filters;
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

QAbstractButton* ImageWindow::addFilter(const QString& name)
{
	QAbstractButton* toggle = new QRadioButton(name, m_filters);
	m_filters->layout()->addWidget(toggle);
	return toggle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
