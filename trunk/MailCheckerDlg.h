#ifndef MAILCHECKERDLG_H
#define MAILCHECKERDLG_H

#include <QtGui/QDialog>
#include "ui_MailCheckerDlg.h"
#include <QSystemTrayIcon>

class UserSetting;
class QSqlTableModel;
class QDataWidgetMapper;

class MailCheckerDlg : public QDialog
{
	Q_OBJECT

public:
	MailCheckerDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MailCheckerDlg();

public slots:
	void onOpenApp();

protected:
	void closeEvent(QCloseEvent* event);

private:
	void loadSettings();

private slots:
	void onOK();
	void onCancel();
	void onAdd();
	void onDel();
	void onSelectAccount(const QModelIndex& idx);
	void onProtocolChanged(const QString& protocol);

private:
	Ui::MailCheckerDlgClass ui;
	enum {NAME, PROTOCOL, HOST, USER, PASSWORD, PORT, SSL};

	UserSetting* setting;
	QSqlTableModel* model;
	QDataWidgetMapper* mapper;
	int currentRow;
};

#endif // MAILCHECKERDLG_H
