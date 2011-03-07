#include "NotificationWindow.h"
#include "MailWidget.h"
#include "Setting.h"
#include "AccountFrame.h"
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
	showSeconds = UserSetting::getInstance()->value("ShowNotification").toInt();
}

void NotificationWindow::addAccountMails(const AccountMails& list)
{
	if(list.mails.isEmpty())
		return;

	AccountFrame* frame = new AccountFrame(list.accountName);
	connect(frame, SIGNAL(frameDeleted(QWidget*)), this, SLOT(onAdjustGeometry(QWidget*)));
	foreach(MailInfo mail, list.mails)
	{
		MailWidget* mailWidget = new MailWidget(mail, frame);
		connect(mailWidget, SIGNAL(mailDeleted(QWidget*)),    this, SLOT(onAdjustGeometry(QWidget*)));
		connect(mailWidget, SIGNAL(newMailCountChanged(int)), this, SIGNAL(newMailCountChanged(int)));
		frame->addMailWidget(mailWidget);
	}
	addFrame(frame);
}

void NotificationWindow::clear()
{
	foreach(QWidget* frame, frames)
		delete frame;
	frames.clear();
	MailWidget::newMailCount = 0;
}

void NotificationWindow::showNofitication()
{
	if(!hasNewMail())
		return;

	show();
	shrink();
	setWindowOpacity(1.0);	

	showGeometryAnimation->setDuration(500);
	showGeometryAnimation->setStartValue(getStartRect());
	showGeometryAnimation->setEndValue  (getFinalRect());
	showGeometryAnimation->start();
	hideGeometryAnimation->stop();
	hideOpacityAnimation->stop();

	hideTimer.start(showSeconds * 1000 + 500);
}

void NotificationWindow::addFrame(AccountFrame* frame)
{
	layout->addWidget(frame);
	frames << frame;
}

bool NotificationWindow::hasNewMail() const {
	return !frames.isEmpty();
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

void NotificationWindow::enterEvent(QEvent*)
{
	if(hideGeometryAnimation->state() == QAbstractAnimation::Running && hasNewMail())
		showNofitication();      // move back
	hideTimer.stop();            // do not hide until mouse leave
}

void NotificationWindow::leaveEvent(QEvent*) {
	hideTimer.start(showSeconds * 1000);
}

void NotificationWindow::onAdjustGeometry(QWidget* widget)
{
	frames.removeAt(frames.indexOf(widget));  // no effect if widget is not AccountFrame
	layout->removeWidget(widget);             // same
	shrink();
	setGeometry(getFinalRect());              // move to bottom right
	if(frames.isEmpty())
		onHide();
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

void NotificationWindow::shrink()
{
	adjustSize();
	resize(10, 10);
}