#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QTextStream>
#include <QFile>
#include <QMap>

class Logger : public QObject
{
	typedef QMap<QString, Logger*> LoggerManager;

public:
	static Logger& logger(const QString& fileName = "Log.txt");
	Logger& operator<< (const QString& str);

private:
	Logger(const QString& fileName, QObject* parent = 0);
	~Logger() {}	
	Logger(const Logger& other);
	Logger& operator= (const Logger& other);

private:
	QTextStream stream;
	QFile file;
	static LoggerManager manager;
};

#endif // LOGGER_H
