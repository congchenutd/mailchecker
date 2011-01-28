#include "MailWidget.h"
#include "Connection.h"

MailWidget::MailWidget(const MailInfo& info, QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.btSetRead, SIGNAL(clicked()), this, SLOT(onSetRead()));
	connect(ui.btDel,     SIGNAL(clicked()), this, SLOT(onDel()));

	setMail(info);
}

void MailWidget::setMail(const MailInfo& info)
{
	id = info.id;
	ui.labelSubject->setText(info.subject);
	ui.labelFrom->setText(info.from);
	ui.labelDate->setText(info.date);
//	ui.labelContent->setText(content);
}

void MailWidget::onSetRead()
{

}

void MailWidget::onDel()
{

}
