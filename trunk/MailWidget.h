#ifndef MAILWIDGET_H
#define MAILWIDGET_H

#include <QWidget>
#include "ui_MailWidget.h"

struct MailInfo;

class MailWidget : public QWidget
{
	Q_OBJECT

public:
	MailWidget(const MailInfo& info, QWidget *parent = 0);
	void setMail(const MailInfo& info);

private slots:
	void onSetRead();
	void onDel();

private:
	Ui::MailWidget ui;
	int id;
};

#endif // MAILWIDGET_H
