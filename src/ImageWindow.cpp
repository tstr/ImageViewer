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

#include "ImageWindow.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImageWindow::ImageWindow(QWidget* parent) :
	QMainWindow(parent)
{
	setup();
}

void ImageWindow::setup()
{
	setAcceptDrops(true);

	//Create widgets
	this->setCentralWidget(createWidgets(this));
	

	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	
	//const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
	QAction* saveAction = new QAction(tr("&Save"), this);
	saveAction->setShortcuts(QKeySequence::SaveAs);
	saveAction->setStatusTip("Save image");
	connect(saveAction, &QAction::triggered, this, &ImageWindow::saveAs);
	fileMenu->addAction(saveAction);

	//const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
	QAction *openAction = new QAction(tr("&Open..."), this);
	openAction->setShortcuts(QKeySequence::Open);
	openAction->setStatusTip(tr("Open an image"));
	connect(openAction, &QAction::triggered, this, &ImageWindow::open);
	fileMenu->addAction(openAction);
	//fileToolBar->addAction(openAct);


	//Setup events
	QObject::connect(&m_img, &ImageProcessor::imageUpdated, this, &ImageWindow::setImage);

	QAbstractButton* none = new QRadioButton("none", m_filters);
	m_filters->layout()->addWidget(none);
	none->setChecked(true);
	QObject::connect(none, &QAbstractButton::toggled, [&](bool checked) { if (checked) m_img.resetImage(); });

	//Set image to greyscale
	QAbstractButton* grey = new QRadioButton("greyscale", m_filters);
	m_filters->layout()->addWidget(grey);

	QObject::connect(grey, &QAbstractButton::toggled, [&](bool checked) {
		if (checked) m_img.apply([](auto img, auto coord) {
			QColor c = qGray(img.pixel(coord));
			return qRgb(c.blue(), c.blue(), c.blue());
		});
		else
			m_img.resetImage();
	});

	//Filters
	addFilter("gaussian 3x3", filters::gaussian3);
	addFilter("gaussian 5x5", filters::gaussian5);
	addFilter("edge (H)",     filters::edgesH);
	addFilter("edge (V)",     filters::edgesV);
	addFilter("edge 2",		  filters::edges2);
	addFilter("sharpen",      filters::sharpen);
	addFilter("emboss",	      filters::emboss);

	//Nonlinear filter
	QAbstractButton* nonlinear = new QRadioButton("non-linear", m_filters);
	m_filters->layout()->addWidget(nonlinear);

	QObject::connect(nonlinear, &QAbstractButton::toggled, [&](bool checked) {
		if (checked)
			m_img.apply([&](auto img, auto coord) {

				int vs[9];
				
				for (int y = 0; y < 3; y++)
				{
					for (int x = 0; x < 3; x++)
					{
						QPoint newcoord = coord + QPoint(x - 1, y - 1);

						newcoord.setX(std::max(0, std::min(img.size().width() - 1, newcoord.x())));
						newcoord.setY(std::max(0, std::min(img.size().height() - 1, newcoord.y())));

						//Read input image
						vs[x + y*3] = img.pixel(newcoord);
					}
				}

				std::sort(std::begin(vs), std::end(vs));
				return vs[5];
			});
		else
			m_img.resetImage();
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
	m_imageView = new QLabel(parent);
	m_imageView->setText("empty");
	return m_imageView;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////

QAbstractButton* ImageWindow::addFilter(const QString& name, const Kernel<3, 3>& kernel)
{
	QAbstractButton* toggle = new QRadioButton(name, m_filters);
	m_filters->layout()->addWidget(toggle);
	QObject::connect(toggle, &QAbstractButton::toggled, [&](bool checked) { if (checked) m_img.filter(kernel); else m_img.resetImage(); });
	return toggle;
}

QAbstractButton* ImageWindow::addFilter(const QString& name, const Kernel<5, 5>& kernel)
{
	QAbstractButton* toggle = new QRadioButton(name, m_filters);
	m_filters->layout()->addWidget(toggle);
	QObject::connect(toggle, &QAbstractButton::toggled, [&](bool checked) { if (checked) m_img.filter(kernel); else m_img.resetImage(); });
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

void ImageWindow::setImage(const QImage& image)
{
	m_imageView->setPixmap(QPixmap::fromImage(image));
}

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
