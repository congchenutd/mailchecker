#include "TrayIcon.h"
#include "MailCheckerDlg.h"
#include "Setting.h"
#include "Connection.h"
#include "NotificationWindow.h"
#include <QMenu>
#include <QSqlQuery>
#include <QSound>
#include <QProcess>

TrayIcon::TrayIcon(QObject *parent)	: QSystemTrayIcon(parent)
{
	dlg = new MailCheckerDlg;
	notification = new NotificationWindow;
	connect(notification, SIGNAL(newMailCountChanged(int)), this, SLOT(onNewMailCountChanged(int)));

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

	setHasNewMail(false);
	setContextMenu(trayMenu);
	show();

	connect(actionCheck,       SIGNAL(triggered()), this, SLOT(onCheckAll()));
	connect(actionTellMeAgain, SIGNAL(triggered()), this, SLOT(onTellMeAgain()));
	connect(actionExit,        SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actionSettings,    SIGNAL(triggered()), dlg,  SLOT(show()));
	connect(actionApplication, SIGNAL(triggered()), dlg,  SLOT(onOpenApp()));
	connect(&timer,            SIGNAL(timeout()),   this, SLOT(onCheckAll()));
	connect(&animationTimer,   SIGNAL(timeout()),   this, SLOT(onUpdateAnimation()));
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));

	timer.start(UserSetting::getInstance()->value("Interval").toInt() * 60 * 1000);
	onCheckAll();
}

void TrayIcon::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::DoubleClick)
	{
		QProcess* process = new QProcess;
		process->start(tr("\"%1\"").arg(UserSetting::getInstance()->value("Application").toString()));
	}
}

void TrayIcon::onCheckAll()
{
	notification->clear();
	QSqlQuery query;
	query.exec(tr("select * from Accounts"));
	while(query.next())
	{
		AccountInfo account;
		account.accountName = query.value(0).toString();
		account.protocol    = query.value(1).toString();
		account.host        = query.value(2).toString();
		account.user        = query.value(3).toString();
		account.pass        = query.value(4).toString();
		account.port        = query.value(5).toInt();
		account.ssl         = query.value(6).toBool();
		account.enable      = query.value(7).toBool();

		if(account.enable)
		{
			Connection* connection = new Connection(this);
			threads << connection;
			connect(connection, SIGNAL(finished()), this, SLOT(onCheckDone()));

			connection->setAccount(account);
			connection->setMission(Connection::CHECK);
			connection->start();
		}
	}
}

void TrayIcon::onCheckDone()
{
	Connection* connection = qobject_cast<Connection*>(sender());
	if(connection->missionSuccessful())
		notification->addAccountMails(connection->getNewMails());
	threads.remove(connection);
	if(threads.isEmpty())
		alert();
}

void TrayIcon::alert()
{
	if(!notification->hasNewMail())
	{
		setHasNewMail(false);
		return;
	}

	QString soundFile = UserSetting::getInstance()->value("Sound").toString();
	if(QFile::exists(soundFile))
		QSound::play(soundFile);

	if(UserSetting::getInstance()->value("Popup").toBool())
		onTellMeAgain();

	setHasNewMail(true);
}

void TrayIcon::onTellMeAgain() {
	notification->showNofitication();
}

void TrayIcon::onNewMailCountChanged(int count) {
	setHasNewMail(count > 0);
}

void TrayIcon::setHasNewMail(bool has)
{
	if(has)		
		animationTimer.start(500);
	else	
	{
		animationTimer.stop();
		setIcon(QIcon(":/MailChecker/Images/Mail.png"));
	}
}

void TrayIcon::onUpdateAnimation()
{
	static bool on = true;
	setIcon(on ? QIcon(":/MailChecker/Images/NewMail.png") 
			   : QIcon(":/MailChecker/Images/Mail.png"));
	on = !on;
}
