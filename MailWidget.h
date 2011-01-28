#ifndef MAILWIDGET_H
#define MAILWIDGET_H

#include <QWidget>
#include "ui_MailWidget.h"
#include "Connection.h"

class MailWidget : public QWidget
{
	Q_OBJECT

public:
	MailWidget(const MailInfo& info, QWidget *parent = 0);
	void setMail(const MailInfo& info);
	QString getAccountName() const;
	int     getMailID()      const;

private slots:
	void onSetRead();
	void onDel();

signals:
	void mailDeleted(MailWidget*);

private:
	Ui::MailWidget ui;
	MailInfo mailInfo;
};

#endif // MAILWIDGET_H