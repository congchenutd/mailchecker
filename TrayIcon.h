#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>
#include <QTimer>
#include <QSet>

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
	void onTellMeAgain();
	void onCheckDone();
	void onNewMailCountChanged(int count);
	void onUpdateAnimation();

private:
	void alert();
	void setHasNewMail(bool has);

private:
	MailCheckerDlg* dlg;
	QAction* actionCheck;
	QAction* actionTellMeAgain;
	QAction* actionApplication;
	QAction* actionSettings;
	QAction* actionExit;
	QTimer timer;
	QTimer animationTimer;
	NotificationWindow* notification;
	QSet<Connection*> threads;
};

#endif // TRAYICON_H
