#include "NotificationWindow.h"
#include "MailWidget.h"
#include "Setting.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDesktopWidget>
#include <QTimer>
#include <QPropertyAnimation>
#include <QBrush>
#include <QPalette>
#include <QColor>

NotificationWindow::NotificationWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::ToolTip | Qt::WindowStaysOnTopHint);
	setMouseTracking(true);

	showGeometryAnimation = new QPropertyAnimation(this, "geometry");
	hideGeometryAnimation = new QPropertyAnimation(this, "geometry");
	hideOpacityAnimation  = new QPropertyAnimation(this, "windowOpacity");
	connect(hideGeometryAnimation, SIGNAL(finished()), this, SLOT(close()));

	layout = new QVBoxLayout(this);
	setLayout(layout);

	connect(&hideTimer, SIGNAL(timeout()), this, SLOT(onHide()));
}

void NotificationWindow::addMailList(const MailList& list)
{
	if(list.mails.isEmpty())
		return;

	addAccountLine(list.accountName);
	foreach(MailInfo mail, list.mails)
	{
		MailWidget* mailWidget = new MailWidget(mail, this);
		addWidget(mailWidget);
		connect(mailWidget, SIGNAL(mailDeleted(MailWidget*)), this, SLOT(onDelMail(MailWidget*)));
	}
}

void NotificationWindow::clear()
{
	foreach(QWidget* widget, widgets)
		delete widget;
	widgets.clear();
}

void NotificationWindow::showNofitication()
{
	if(!hasNewMail())
		return;

	show();
	setWindowOpacity(1.0);	

	showGeometryAnimation->setDuration(500);
	showGeometryAnimation->setStartValue(getStartRect());
	showGeometryAnimation->setEndValue  (getFinalRect());
	showGeometryAnimation->start();
	hideGeometryAnimation->stop();
	hideOpacityAnimation->stop();

	int seconds = UserSetting::getInstance()->value("ShowNotification").toInt();
	hideTimer.start(seconds * 1000 + 500);
}

void NotificationWindow::addWidget(QWidget* widget)
{
	layout->addWidget(widget);
	widgets << widget;
}

bool NotificationWindow::hasNewMail() const {
	return layout->count() > 0;
}

void NotificationWindow::onHide()
{
	hideTimer.stop();
	hideGeometryAnimation->setDuration(1000);
	QRect rect = getFinalRect();
	hideGeometryAnimation->setStartValue(getFinalRect());
	hideGeometryAnimation->setEndValue  (getStartRect());
	hideGeometryAnimation->start();

	hideOpacityAnimation->setDuration(1000);
	hideOpacityAnimation->setStartValue(1.0);
	hideOpacityAnimation->setEndValue  (0.0);
	hideOpacityAnimation->start();
}

void NotificationWindow::mousePressEvent(QMouseEvent*)
{
	hideTimer.stop();
	onHide();
}

void NotificationWindow::mouseMoveEvent(QMouseEvent*) {
	if(hideGeometryAnimation->state() == QAbstractAnimation::Running)
		showNofitication();
}

void NotificationWindow::addAccountLine(const QString& accountName)
{
	QLabel* accountLabel = new QLabel(accountName);

	QFont font;
	font.setBold(true);
	accountLabel->setFont(font);

	QPalette palette;
	QColor color(Qt::red);
	QBrush brush(color);
	palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
	accountLabel->setPalette(palette);

	accountLabel->setAlignment(Qt::AlignHCenter);
	addWidget(accountLabel);

	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	addWidget(line);
}

void NotificationWindow::onDelMail(MailWidget* widget)
{
	widgets.removeAt(widgets.indexOf(widget));
	layout->removeWidget(widget);
	adjustSize();
	setGeometry(getFinalRect());
}

QRect NotificationWindow::getFinalRect() const
{
	QRect desktopRect = qApp->desktop()->availableGeometry();
	return QRect(desktopRect.width()-width(), desktopRect.height()-height(), 
				 width(), height());
}

QRect NotificationWindow::getStartRect() const
{
	QRect desktopRect = qApp->desktop()->availableGeometry();
	int y = hideGeometryAnimation->state() == QAbstractAnimation::Running ?
			geometry().y() : desktopRect.height();
	return QRect(desktopRect.width()-width(), y, width(), height());
}