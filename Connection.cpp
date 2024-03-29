#include "Connection.h"
#include "Setting.h"
#include "Logger.h"
#include <QStringList>
#include <QDebug>

Connection::Connection(const AccountInfo& account, QObject *parent)
	: QThread(parent), socket(0)
{
	timeout = UserSetting::getInstance()->value("Timeout").toInt() * 1000;  // ms
	socket = new QSslSocket(this);
	setAccount(account);
	logger = &(Logger::logger(account.accountName));
	*logger << "New socket";

	QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

bool Connection::check() 
{
	if(!connect())
		goto fail;
	if(!login())
		goto fail;
	if(!findBoxes())
		goto fail;
	if(!searchUnseen())
		goto fail;
	if(!fetchUnseen())
		goto fail;
	if(!logout())
		goto fail;

	*logger << "Logout";
	return true;

fail:
	*logger << "Check failed";
	return false;
}

bool Connection::connect()
{
	socket->connectToHostEncrypted(account.host, account.port);
	*logger << "Connect to host";
	readResponse();
	return parseOK();
}
bool Connection::login()
{
	sendCommand(tr(". login %1 %2").arg(account.user).arg(account.pass));
	return parseOK();
}
bool Connection::findBoxes() {
	sendCommand(". list \"\" \"*\"");
	return parseBoxes();
}
bool Connection::searchUnseen()
{
	sendCommand(tr(". examine %1").arg(inboxName));
	if(!parseOK())
		return false;
	sendCommand(". search unseen");
	return parseUnseen();
}
bool Connection::fetchUnseen()
{
	newMails.mails.clear();
	foreach(int id, newIDs)
	{
		sendCommand(tr(". fetch %1 body[header.fields (subject from date)]").arg(id));
		MailInfo info;
		bool result = parseHeader(info);
		if(!result)
			return false;
		info.id = id;
		newMails.mails << info;
	}
	return true;
}
bool Connection::logout()
{
	sendCommand(". logout");
	return parseOK();
}

bool Connection::parseOK() {
	return response.indexOf(". OK") >= 0 || response.indexOf("* OK") >= 0;
}
bool Connection::parseBoxes()
{
	inboxName    = findBox(response, "inbox");
	trashBoxName = findBox(response, "trash");
	return !inboxName.isEmpty() && !trashBoxName.isEmpty();
}
bool Connection::parseUnseen()
{
	newIDs.clear();
	int start = response.indexOf("* SEARCH ") + 9;
	if(start == -1)
		return false;
	int end = response.indexOf("\r\n");
	QString line = response.mid(start, end - start + 1);
	QStringList ids = line.split(" ");
	foreach(QString id, ids)
	{
		int i = id.toInt();
		if(i > 0)
			newIDs << i;
	}
	qSort(newIDs.begin(), newIDs.end(), qGreater<int>());
	return true;
}

bool Connection::parseHeader(MailInfo& info)
{
	int fromStart = response.indexOf("From: ") + 6;
	if(fromStart == -1)
		return false;
	int fromEnd = response.indexOf("\r", fromStart);
	info.from = elide(response.mid(fromStart, fromEnd - fromStart + 1));

	int subjectStart = response.indexOf("Subject: ") + 9;
	if(subjectStart == -1)
		return false;
	int subjectEnd = response.indexOf("\r", subjectStart);
	info.subject = elide(response.mid(subjectStart, subjectEnd - subjectStart + 1));

	int dateStart = response.indexOf("Date: ") + 6;
	if(dateStart == -1)
		return false;
	int dateEnd = response.indexOf("\r", dateStart);
	info.date = elide(response.mid(dateStart, dateEnd - dateStart + 1));

	info.accountName = account.accountName;
	return true;
}

void Connection::setAccount(const AccountInfo& acc)
{
	account = acc;
	newMails.accountName = acc.accountName;
}

QString Connection::findBox(const QString& string, const QString& target) const
{
	QStringList lines = string.split("\r\n");
	foreach(QString line, lines)
	{
		QStringList sections = line.split(" ");
		foreach(QString section, sections) {
			if(section.indexOf(target, 0, Qt::CaseInsensitive) > 0)
			{
				QString name = section.remove('\"');
				return name.left(name.length());
			}
		}
	}
	return QString();
}

void Connection::sendCommand(const QString& command)
{
	if(!socket->isOpen())
		return;
	qint64 bytesWritten = socket->write(command.toUtf8() + "\r\n");
	if(bytesWritten != command.size() + 2)
		*logger << "Could not write all bytes";
	if(bytesWritten > 0 && !socket->waitForBytesWritten(timeout))
		*logger << "Could not write bytes from buffer";
	*logger << "------------ Send ------------" << command;
	readResponse();
}

void Connection::readResponse()
{
	if(!socket->waitForReadyRead(timeout))
	{
		*logger << "Could not receive data";
		return;
	}
	response.clear();
	while(!parseOK())  // done
	{
		QString data = socket->readAll();
		if(data.isEmpty())
		{
			if(!socket->waitForReadyRead(timeout))
			{
				*logger << "Could not receive data";
				return;
			}
		}
		else
			response += data;
	}

	*logger << "------------- Response -------------" << response;
}

AccountMails Connection::getNewMails() const {
	return newMails;
}

QString Connection::elide(const QString& string, int length /*= 20*/) const {
	return string.length() <= length ? string : string.left(length) + " ...";
}

bool Connection::setRead(int id)
{
	if(!connect())
		goto fail;
	if(!login())
		goto fail;
	if(!findBoxes())
		goto fail;
	if(!doSetRead(id))
		goto fail;
	if(!logout())
		goto fail;

fail:
	*logger << "Logout";
	return true;
}

bool Connection::doSetRead(int id)
{
	sendCommand(tr(". select %1").arg(inboxName));
	if(!parseOK())
		return false;
	sendCommand(tr(". store %1 +flags \\seen").arg(id));
	return parseOK();
}

bool Connection::delMail(int id)
{
	if(!connect())
		goto fail;
	if(!login())
		goto fail;
	if(!findBoxes())
		goto fail;
	if(!doDelMail(id))
		goto fail;
	if(!logout())
		goto fail;

fail:
	*logger << "Logout";
	return true;
}

bool Connection::doDelMail(int id)
{
	sendCommand(tr(". select %1").arg(inboxName));
	if(!parseOK())
		return false;
	sendCommand(tr(". copy %1 %2").arg(id).arg(trashBoxName));
	if(!parseOK())
		return false;
	sendCommand(tr(". store %1 +flags \\deleted").arg(id));
	if(!parseOK())
		return false;
	sendCommand(". expunge");
	return parseOK();
}

void Connection::run()
{
	successful = false;
	switch(mission)
	{
	case CHECK:
		successful = check();
		break;
	case DELETE:
		successful = delMail(targetID);
		break;
	case READ:
		successful = setRead(targetID);
		break;
	default:
		break;
	}
}