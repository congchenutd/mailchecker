#include "NotificationWindow.h"
#include "MailWidget.h"
#include "Setting.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDesktopWidget>
#include <QTimer>
#include <QPropertyAnimation>

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

void NotificationWindow::addMailList(const MailList& list) {
	lists << list;
}

void NotificationWindow::clear()
{
	lists.clear();
	clearWidgets();
}

void NotificationWindow::showNofitication()
{
	if(!hasNewMail())
		return;

	clearWidgets();
	foreach(MailList list, lists)
	{
		addWidget(new QLabel(list.accountName));
		foreach(MailInfo mail, list.mails)
			addWidget(new MailWidget(mail, this));
	}

	popUp();
}

void NotificationWindow::addWidget(QWidget* widget)
{
	layout->addWidget(widget);
	widgets << widget;
}

void NotificationWindow::clearWidgets()
{
	foreach(QWidget* widget, widgets)
		delete widget;
	widgets.clear();
}

bool NotificationWindow::hasNewMail() const {
	return !lists.isEmpty();
}

void NotificationWindow::popUp()
{
	show();
	setWindowOpacity(1.0);	
	
	QRect desktopRect = qApp->desktop()->availableGeometry();
	int w = width();
	int x = desktopRect.width() - w;
	int h = height();
	int y = hideGeometryAnimation->state() == QAbstractAnimation::Running ?
			geometry().y() : y = desktopRect.height();

	hideGeometryAnimation->stop();
	hideOpacityAnimation->stop();

	showGeometryAnimation->setDuration(500);
	showGeometryAnimation->setStartValue(QRect(x, y,   w, h));
	showGeometryAnimation->setEndValue  (QRect(x, desktopRect.height()-h, w, h));
	showGeometryAnimation->start();

	int seconds = UserSetting::getInstance()->value("ShowNotification").toInt();
	hideTimer.start(seconds * 1000 + 500);
}

void NotificationWindow::onHide()
{
	QRect desktopRect = qApp->desktop()->availableGeometry();
	int w = width();
	int x = desktopRect.width() - w;
	int h = height();
	int y = desktopRect.height();
	hideGeometryAnimation->setDuration(1000);
	hideGeometryAnimation->setStartValue(QRect(x, y-h,   w, h));
	hideGeometryAnimation->setEndValue  (QRect(x, y, w, h));
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
		popUp();
}
