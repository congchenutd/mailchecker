#include "MailWidget.h"
#include <QSqlQuery>

MailWidget::MailWidget(const MailInfo& info, QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.btSetRead, SIGNAL(clicked()), this, SLOT(onSetRead()));
	connect(ui.btDel,     SIGNAL(clicked()), this, SLOT(onDel()));

	setMail(info);
}

void MailWidget::setMail(const MailInfo& info)
{
	mailInfo = info;
	ui.labelSubject->setText(info.subject);
	ui.labelFrom->setText(info.from);
	ui.labelDate->setText(info.date);
}

void MailWidget::onSetRead()
{
	QSqlQuery query;
	query.exec(tr("select * from Accounts where Name = \"%1\"").arg(mailInfo.accountName));
	if(query.next())
	{
		QString accountName = query.value(0).toString();
		QString protocol    = query.value(1).toString();
		QString host        = query.value(2).toString();
		QString user        = query.value(3).toString();
		QString pass        = query.value(4).toString();
		int     port        = query.value(5).toInt();
		bool    ssl         = query.value(6).toBool();
		bool    enable      = query.value(7).toBool();

		QFont font;
		font.setBold(false);
		ui.labelSubject->setFont(font);

		Connection connection;
		connection.setEnableSSL(ssl);
		connection.setAccount(AccountInfo(accountName, protocol, host, user, pass, port, ssl));
		connection.setRead(mailInfo.id);
	}
}

void MailWidget::onDel()
{
	QSqlQuery query;
	query.exec(tr("select * from Accounts where Name = \"%1\"").arg(mailInfo.accountName));
	if(query.next())
	{
		QString accountName = query.value(0).toString();
		QString protocol    = query.value(1).toString();
		QString host        = query.value(2).toString();
		QString user        = query.value(3).toString();
		QString pass        = query.value(4).toString();
		int     port        = query.value(5).toInt();
		bool    ssl         = query.value(6).toBool();
		bool    enable      = query.value(7).toBool();

		emit mailDeleted(this);
		deleteLater();		
		
		Connection connection;
		connection.setEnableSSL(ssl);
		connection.setAccount(AccountInfo(accountName, protocol, host, user, pass, port, ssl));
		connection.delMail(mailInfo.id);
	}
}

QString MailWidget::getAccountName() const {
	return mailInfo.accountName;
}

int MailWidget::getMailID() const {
	return mailInfo.id;
}
