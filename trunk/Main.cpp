#include "TrayIcon.h"
#include "Logger.h"
#include <QtGui/QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>

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
			   SSL bool, \
			   Enable bool \
   )");
}

int main(int argc, char *argv[])
{
	if(!openDB("MailChecker.db"))
		return 1;
	createTables();
	Logger::logger().setFileName("Log.txt");
	Logger::logger() << "============= Program started ===============";

	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);
	TrayIcon tray;
	return app.exec();
}
