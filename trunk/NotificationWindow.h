#ifndef NOTIFICATIONWINDOW_H
#define NOTIFICATIONWINDOW_H

#include <QWidget>
#include <QList>
#include <QTimer>
#include "ui_NotificationWindow.h"
#include "Connection.h"

class QVBoxLayout;
class QPropertyAnimation;

class NotificationWindow : public QWidget
{
	Q_OBJECT

public:
	NotificationWindow(QWidget *parent = 0);

	bool hasNewMail() const;
	void clear();
	void showNofitication();
	void addMailList(const MailList& list);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseMoveEvent (QMouseEvent*);

private:
	void popUp();

private slots:
	void onHide();

private:
	void addWidget(QWidget* widget);
	void clearWidgets();

private:
	Ui::NotificationWindow ui;
	QList<MailList> lists;
	QVBoxLayout* layout;
	QList<QWidget*> widgets;
	QPropertyAnimation* showGeometryAnimation;
	QPropertyAnimation* hideGeometryAnimation;
	QPropertyAnimation* hideOpacityAnimation;
	QTimer hideTimer;
};

#endif // NOTIFICATIONWINDOW_H