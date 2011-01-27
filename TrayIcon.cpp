#include "TrayIcon.h"
#include "MailCheckerDlg.h"
#include "Setting.h"
#include "Connection.h"
#include <QMenu>
#include <QSqlQuery>

TrayIcon::TrayIcon(QObject *parent)
	: QSystemTrayIcon(parent)
{
	dlg = new MailCheckerDlg;

	QMenu* trayMenu = new QMenu;

	actionCheck       = new QAction(tr("Check All"), this);
	actionTellMeAgain = new QAction(tr("Tell me again"), this);
	actionApplication = new QAction(tr("Open Mail Client"), this);
	actionSettings    = new QAction(tr("Settings"), this);
	actionExit        = new QAction(tr("Exit"), this);
	actionCheck      ->setIcon(QIcon(":/MailChecker/Images/CheckMail.png"));
	actionTellMeAgain->setIcon(QIcon(":/MailChecker/Images/TellMeAgain.png"));
	actionApplication->setIcon(QIcon(":/MailChecker/Images/Application.png"));
	actionSettings   ->setIcon(QIcon(":/MailChecker/Images/Settings.png"));
	actionExit       ->setIcon(QIcon(":/MailChecker/Images/Exit.png"));
	trayMenu->addAction(actionCheck);
	trayMenu->addAction(actionTellMeAgain);
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
	connect(&timer,            SIGNAL(timeout()),   this, SLOT(onTimeout()));
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));

	connection = new Connection(this);
	timer.start(UserSetting::getInstance()->value("Interval").toInt() * 60 * 1000);
	onCheckAll();
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

		connection->setEnableSSL(true);
		connection->setAccount(AccountInfo(accountName, protocol, host, user, pass, port, ssl));
		if(connection->check())
		{
			QString message;
			MailList mails = connection->getUnseenMails();
			foreach(MailInfo mail, mails)
				message += mail.from + "\n" + mail.subject + "\n" + mail.date + "\n\n";
			showMessage(tr("You've got mail!"), message);
		}
	}
	alert();
}

void TrayIcon::alert()
{
	QString soundFile = UserSetting::getInstance()->value("Sound").toString();
//	if()
}

void TrayIcon::onTimeout() {
	onCheckAll();
}
