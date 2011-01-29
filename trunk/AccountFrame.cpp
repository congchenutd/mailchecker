#include "AccountFrame.h"
#include "MailWidget.h"

AccountFrame::AccountFrame(const QString& name, QWidget *parent) : QFrame(parent)
{
	ui.setupUi(this);
	ui.labelAccountName->setText(name);
}

void AccountFrame::addMailWidget(MailWidget* widget)
{
	ui.layout->addWidget(widget);
	connect(widget, SIGNAL(mailDeleted(QWidget*)), this, SLOT(onDel(QWidget*)));
}

bool AccountFrame::isEmpty() {
	return ui.layout->count() <= 2;
}

void AccountFrame::onDel(QWidget* widget)
{
	ui.layout->removeWidget(widget);
	if(isEmpty())
	{
		emit frameDeleted(this);
		deleteLater();
	}
}
