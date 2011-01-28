#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>
#include <QTimer>

class MailCheckerDlg;
class QAction;
class Connection;
class NotificationWindow;

class TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	TrayIcon(QObject* parent = 0);

private slots:
	void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
	void onCheckAll();
	void onTimeout();
	void onTellMeAgain();

private:
	void alert();

private:
	MailCheckerDlg* dlg;
	QAction* actionCheck;
	QAction* actionTellMeAgain;
	QAction* actionApplication;
	QAction* actionSettings;
	QAction* actionExit;
	Connection* connection;
	QTimer timer;
	NotificationWindow* notification;
};

#endif // TRAYICON_H
