#include "Logger.h"
#include <QDateTime>

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
	return *this;
}

Logger& Logger::logger(const QString& fileName)
{
	LoggerManager::iterator it = manager.find(fileName);
	if(it != manager.end())
		return *it.value();

	Logger* logger = new Logger(fileName);
	manager.insert(fileName, logger);
	return *logger;
}

Logger::LoggerManager Logger::manager;
