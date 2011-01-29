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
	setSubjectBold(true);
}

void MailWidget::onSetRead()
{
	Connection* connection = new Connection(this);
	connection->setAccount(getAccountInfo(mailInfo.accountName));
	connection->setMission(Connection::READ);
	connection->setTargetID(mailInfo.id);
	connection->start();
	setSubjectBold(false);
}

void MailWidget::onDel()
{
	//Connection* connection = new Connection;
	//connection->setAccount(getAccountInfo(mailInfo.accountName));
	//connection->setMission(Connection::DELETE);
	//connection->setTargetID(mailInfo.id);
	//connection->start();
	emit mailDeleted(this);
	deleteLater();
}

QString MailWidget::getAccountName() const {
	return mailInfo.accountName;
}

int MailWidget::getMailID() const {
	return mailInfo.id;
}

AccountInfo MailWidget::getAccountInfo(const QString& name) const
{
	AccountInfo result;
	QSqlQuery query;
	query.exec(tr("select * from Accounts where Name = \"%1\"").arg(name));
	if(query.next())
	{
		result.accountName = query.value(0).toString();
		result.protocol    = query.value(1).toString();
		result.host        = query.value(2).toString();
		result.user        = query.value(3).toString();
		result.pass        = query.value(4).toString();
		result.port        = query.value(5).toInt();
		result.ssl         = query.value(6).toBool();
		result.enable      = query.value(7).toBool();
	}
	return result;
}

void MailWidget::setSubjectBold(bool bold)
{
	QFont font;
	font.setBold(bold);
	ui.labelSubject->setFont(font);
}
