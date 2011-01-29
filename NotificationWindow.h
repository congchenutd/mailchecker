#ifndef NOTIFICATIONWINDOW_H
#define NOTIFICATIONWINDOW_H

#include <QWidget>
#include <QList>
#include <QTimer>
#include "ui_NotificationWindow.h"
#include "Connection.h"

class QVBoxLayout;
class QPropertyAnimation;
class AccountFrame;

class NotificationWindow : public QWidget
{
	Q_OBJECT

public:
	NotificationWindow(QWidget *parent = 0);

	bool hasNewMail() const;
	void clear();
	void showNofitication();
	void addAccountMails(const AccountMails& list);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void enterEvent     (QEvent*);
	virtual void leaveEvent     (QEvent*);

private slots:
	void onHide();
	void onAdjustGeometry(QWidget* widget);

signals:
	void newMailCountChanged(int);

private:
	void addFrame(AccountFrame* widget);
	QRect getStartRect() const;
	QRect getFinalRect() const;
	void shrink();

private:
	Ui::NotificationWindow ui;
	QVBoxLayout* layout;
	QList<QWidget*> frames;
	QPropertyAnimation* showGeometryAnimation;
	QPropertyAnimation* hideGeometryAnimation;
	QPropertyAnimation* hideOpacityAnimation;
	QTimer hideTimer;
	int showSeconds;
};

#endif // NOTIFICATIONWINDOW_H