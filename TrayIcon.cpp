#include "TrayIcon.h"
#include "MailCheckerDlg.h"
#include "Connection.h"
#include <QMenu>
#include <QSqlQuery>

TrayIcon::TrayIcon(QObject *parent)
	: QSystemTrayIcon(parent)
{
	dlg = new MailCheckerDlg;

	QMenu* trayMenu = new QMenu;

	actionCheck       = new QAction(tr("Check All"), this);
	actionApplication = new QAction(tr("Open Mail Client"), this);
	actionSettings    = new QAction(tr("Settings"), this);
	actionExit        = new QAction(tr("Exit"), this);
	actionCheck      ->setIcon(QIcon(":/MailChecker/Images/CheckMail.png"));
	actionApplication->setIcon(QIcon(":/MailChecker/Images/Application.png"));
	actionSettings   ->setIcon(QIcon(":/MailChecker/Images/Settings.png"));
	actionExit       ->setIcon(QIcon(":/MailChecker/Images/Exit.png"));
	trayMenu->addAction(actionCheck);
	trayMenu->addAction(actionApplication);
	trayMenu->addAction(actionSettings);
	trayMenu->addAction(actionExit);

	setIcon(QIcon(":/MailChecker/Images/Mail.png"));
	setContextMenu(trayMenu);
	show();

	connect(actionCheck,       SIGNAL(triggered()), this, SLOT(onCheckAll()));
	connect(actionExit,        SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actionSettings,    SIGNAL(triggered()), dlg,  SLOT(show()));
	connect(actionApplication, SIGNAL(triggered()), dlg,  SLOT(onOpenApp()));
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));

	connection = new Connection(this);
}

void TrayIcon::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::DoubleClick)
		dlg->onOpenApp();
}

void TrayIcon::onCheckAll()
{
	QSqlQuery query;
	query.exec(tr("select * from Accounts"));
	while(query.next())
	{
		QString accountName = query.value(0).toString();
		QString protocol    = query.value(1).toString();
		QString host        = query.value(2).toString();
		QString user        = query.value(3).toString();
		QString pass        = query.value(4).toString();
		int     port        = query.value(5).toInt();
		bool    ssl         = query.value(6).toBool();

		connection->setAccount(AccountInfo(accountName, protocol, host, user, pass, port, ssl));
		connection->check();
	}
}
