#include "TrayIcon.h"
#include <QtGui/QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

bool openDB(const QString& name)
{
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
	database.setDatabaseName(name);
	if(!database.open())
	{
		QMessageBox::critical(0, "Error", "Can not open database");
		return false;
	}
	return true;
}

void createTables()
{
	QSqlQuery query;
	query.exec("create table Accounts( \
			   Name varchar primary key, \
			   Protocol varchar, \
			   Host varchar, \
			   User varchar, \
			   Password varchar, \
			   Port int, \
			   SSL bool \
   )");
}

int main(int argc, char *argv[])
{
	if(!openDB("MailChecker.db"))
		return 1;
	createTables();

	QApplication app(argc, argv);
	TrayIcon tray;
	return app.exec();
}
