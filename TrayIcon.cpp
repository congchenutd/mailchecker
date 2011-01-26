#include "TrayIcon.h"
#include "MailCheckerDlg.h"
#include <QMenu>

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

	connect(actionExit,        SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actionSettings,    SIGNAL(triggered()), dlg,  SLOT(show()));
	connect(actionApplication, SIGNAL(triggered()), dlg,  SLOT(onOpenApp()));
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));
}

void TrayIcon::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::DoubleClick)
		dlg->onOpenApp();
}

