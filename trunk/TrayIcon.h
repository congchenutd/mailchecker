#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>

class MailCheckerDlg;
class QAction;

class TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	TrayIcon(QObject* parent = 0);

private slots:
	void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

private:
	MailCheckerDlg* dlg;
	QAction* actionCheck;
	QAction* actionApplication;
	QAction* actionSettings;
	QAction* actionExit;
};

#endif // TRAYICON_H
