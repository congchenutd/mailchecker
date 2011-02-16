#include "Logger.h"
#include <QDateTime>
#include <QDebug>

Logger::Logger(const QString& fileName, QObject* parent /*= 0*/) 
	: QObject(parent)
{
	file.setFileName(fileName);
	file.open(QFile::WriteOnly | QFile::Append);
	stream.setDevice(&file);
}

Logger& Logger::operator<<(const QString& str)
{
	stream << QDateTime::currentDateTime().toString() << ": " << str << "\r\n";
	stream.flush();
	return *this;
}

Logger& Logger::logger(const QString& name)
{
	QString fileName = name.endsWith(".txt", Qt::CaseInsensitive) ? name : name + ".txt";
	LoggerManager::iterator it = manager.find(fileName);
	if(it != manager.end())
		return *it.value();

	Logger* logger = new Logger(fileName);
	manager.insert(fileName, logger);
	return *logger;
}

void Logger::destroyLoggers()
{
	foreach(Logger* logger, manager)
		delete logger;
	manager.clear();
}

Logger::LoggerManager Logger::manager;
