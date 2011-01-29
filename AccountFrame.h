#ifndef ACCOUNTFRAME_H
#define ACCOUNTFRAME_H

#include <QFrame>
#include "ui_AccountFrame.h"

class MailWidget;

class AccountFrame : public QFrame
{
	Q_OBJECT

public:
	AccountFrame(const QString& name, QWidget *parent = 0);
	void addMailWidget(MailWidget* widget);
	bool isEmpty();

private slots:
	void onDel(QWidget* widget);

signals:
	void frameDeleted(QWidget*);

private:
	Ui::AccountFrame ui;
};

#endif // ACCOUNTFRAME_H
