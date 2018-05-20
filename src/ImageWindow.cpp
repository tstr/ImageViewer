/*
	Image Editor main window
*/

#include <QLabel>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QMenuBar>
#include <QFileDialog>

#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include <QDebug>

#include "ImageWidget.h"
#include "ImageWindow.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImageWindow::ImageWindow(QWidget* parent) :
	QMainWindow(parent)
{
	//Setup widgets
	this->setCentralWidget(createWidgets(this));

	//Setup menu actions
	createActions();

	//Image update event
	QObject::connect(&m_img, &ImageProcessor::imageUpdated, m_imageView, &ImageWidget::setPixmap);

	/*
		Setup image operations
	*/

	QAbstractButton* none = new QRadioButton("none", m_filters);
	m_filters->layout()->addWidget(none);
	none->setChecked(true);
	QObject::connect(none, &QAbstractButton::toggled, [&](bool checked) { if (checked) m_img.resetImage(); });

	//Set image to greyscale
	QAbstractButton* grey = new QRadioButton("greyscale", m_filters);
	m_filters->layout()->addWidget(grey);

	QObject::connect(grey, &QAbstractButton::toggled, [&](bool checked) {
		if (checked) m_img.makeGrayscale(); else m_img.resetImage();
	});

	//Filters
	addFilter("gaussian 3x3", kernels::gaussian3);
	addFilter("gaussian 5x5", kernels::gaussian5);
	addFilter("edge (H)",     kernels::edgesH);
	addFilter("edge (V)",     kernels::edgesV);
	addFilter("edge 2",       kernels::edges2);
	addFilter("sharpen",      kernels::sharpen);
	addFilter("emboss",       kernels::emboss);

	//Nonlinear filter
	QAbstractButton* nonlinear = new QRadioButton("non-linear", m_filters);
	m_filters->layout()->addWidget(nonlinear);

	QObject::connect(nonlinear, &QAbstractButton::toggled, [&](bool checked) {
		if (checked) m_img.applyNonLinearFilter(); else m_img.resetImage();
	});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Widgets
////////////////////////////////////////////////////////////////////////////////////////////////////////////

QWidget* ImageWindow::createWidgets(QWidget* parent)
{
	//Create widgets
	QWidget* center = new QWidget(parent);
	center->setLayout(new QHBoxLayout(center));
	center->layout()->addWidget(createSideBar(center));
	center->layout()->addWidget(createImageView(center));

	return center;
}

QWidget* ImageWindow::createSideBar(QWidget* parent)
{
	m_filters = new QGroupBox("Filters:", parent);
	m_filters->setLayout(new QFormLayout(m_filters));
	m_filters->setMaximumWidth(200);
	m_filters->setMinimumWidth(200);
	m_filters->setAlignment(Qt::AlignTop);

	return m_filters;
}

QWidget* ImageWindow::createImageView(QWidget* parent)
{
	m_imageView = new ImageWidget(parent);
	return m_imageView;
}

void ImageWindow::createActions()
{
	setAcceptDrops(true);

	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

	QAction* saveAction = new QAction(tr("&Save"), fileMenu);
	saveAction->setShortcuts(QKeySequence::SaveAs);
	saveAction->setStatusTip("Save image");
	connect(saveAction, &QAction::triggered, this, &ImageWindow::saveAs);
	fileMenu->addAction(saveAction);

	QAction *openAction = new QAction(tr("&Open..."), fileMenu);
	openAction->setShortcuts(QKeySequence::Open);
	openAction->setStatusTip(tr("Open an image"));
	connect(openAction, &QAction::triggered, this, &ImageWindow::open);
	fileMenu->addAction(openAction);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////

QAbstractButton* ImageWindow::addFilter(const QString& name, const KernelView& kernel)
{
	QAbstractButton* toggle = new QRadioButton(name, m_filters);
	m_filters->layout()->addWidget(toggle);
	QObject::connect(toggle, &QAbstractButton::toggled, [this, kernel](bool checked) { if (checked) m_img.applyFilter(kernel); else m_img.resetImage(); });
	return toggle;
}

void ImageWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

void ImageWindow::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasUrls())
		loadImage(event->mimeData()->urls()[0].toLocalFile());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Slots
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImageWindow::loadImage(const QString& imgName)
{
	QImage i(imgName);

	if (i.isNull())
	{
		qWarning() << "Unable to read image " << imgName;
	}
	else
	{
		m_img.load(i);
		QMainWindow::setWindowTitle("Image Edit -- " + QDir(imgName).absolutePath());
	}
}

void ImageWindow::saveImage(const QString& saveName)
{
	m_img.image().save(saveName);
}

void ImageWindow::saveAs()
{
	QString name = QFileDialog::getSaveFileName(this, "Save image", "", "Image (*.png *.jpg *.jpeg)");
	this->saveImage(name);
}

void ImageWindow::open()
{
	QString open = QFileDialog::getOpenFileName(this, "Open image", "", "Image (*.png *.jpg *.jpeg)");
	this->loadImage(open);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
