#include "MailWidget.h"
#include <QSqlQuery>

MailWidget::MailWidget(const MailInfo& info, QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.btSetRead, SIGNAL(clicked()), this, SLOT(onSetRead()));
	connect(ui.btDel,     SIGNAL(clicked()), this, SLOT(onDel()));
	connect(ui.labelSubject, SIGNAL(linkActivated()), this, SLOT(onSubjectClicked()));

	setMail(info);
	newMailCount ++;
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
	Connection* connection = new Connection(getAccountInfo(mailInfo.accountName), this);
	connection->setMission(Connection::READ);
	connection->setTargetID(mailInfo.id);
	connection->start();
	setSubjectBold(false);
	emit newMailCountChanged(--newMailCount);
}

void MailWidget::onDel()
{
	Connection* connection = new Connection(getAccountInfo(mailInfo.accountName), 0);
	connection->setMission(Connection::DELETE);
	connection->setTargetID(mailInfo.id);
	connection->start();
	connect(connection, SIGNAL(finished()), connection, SLOT(deleteLater()));
	emit mailDeleted(this);
	emit newMailCountChanged(--newMailCount);
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

void MailWidget::onSubjectClicked()
{

}

int MailWidget::newMailCount = 0;
