#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>

class MailCheckerDlg;
class QAction;
class Connection;

class TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	TrayIcon(QObject* parent = 0);

private slots:
	void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
	void onCheckAll();

private:
	MailCheckerDlg* dlg;
	QAction* actionCheck;
	QAction* actionApplication;
	QAction* actionSettings;
	QAction* actionExit;
	Connection* connection;
};

#endif // TRAYICON_H
