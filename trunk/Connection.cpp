#include "Connection.h"
#include "Setting.h"
#include <QStringList>

Connection::Connection(QObject *parent)	: QObject(parent), socket(0)
{
	timeout = UserSetting::getInstance()->value("Timeout").toInt() * 1000;  // ms
	setEnableSSL(true);
}

void Connection::setEnableSSL(bool ssl)
{
	if(socket != 0)
		socket->deleteLater();
	socket = new QSslSocket(this);
	qDebug() << "\nnew socket";
	//QObject::connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
	//				this,   SLOT(onStateChanged(QAbstractSocket::SocketState)));
	//QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), 
	//				 this, SLOT(onError(QAbstractSocket::SocketError)));
}

void Connection::onError(QAbstractSocket::SocketError error)
{
	//qDebug() << "Error: " << socket->errorString();
	//emit connectionError(socket->errorString());
}

void Connection::onStateChanged(QAbstractSocket::SocketState state)
{
	//if(state == QAbstractSocket::UnconnectedState)
	//{
	//	qDebug() << "Connection lost";
	//	socket->deleteLater();
	//	socket = 0;
	//}
}

bool Connection::check() 
{
	unseenMails.clear();
	unseenMailsIDs.clear();
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

	close();
	qDebug() << "\nLogout\n";
	return true;

fail:
	close();
	qDebug() << "\nMission failed\n";
	return false;
}

void Connection::close()
{
	socket->deleteLater();
	socket = 0;
}

bool Connection::connect()
{
	socket->connectToHostEncrypted(account.host, account.port);
	qDebug() << "\nConnect to host";
	readResponse();
	return parseConnection();
}

bool Connection::login()
{
	sendCommand(tr(". login %1 %2").arg(account.user).arg(account.pass));
	return parseLogin();
}
bool Connection::findBoxes() {
	sendCommand(". list \"\" \"*\"");
	return parseBoxes();
}
bool Connection::searchUnseen()
{
	sendCommand(tr(". examine %1").arg(inboxName));
	if(!parseExamine())
		return false;
	sendCommand(". search unseen");
	return parseUnseen();
}
bool Connection::fetchUnseen()
{
	foreach(int id, unseenMailsIDs)
	{
		sendCommand(tr(". fetch %1 body[header.fields (subject from date)]").arg(id));
		MailInfo info;
		bool result = parseHeader(info);
		if(!result)
			return false;
		info.id = id;
		unseenMails << info;
	}
	return true;
}
bool Connection::logout()
{
	sendCommand(". logout");
	return parseLogout();
}


bool Connection::parseOK() {
	return response.indexOf(". OK") >= 0;
}
bool Connection::parseConnection() {
	return response.indexOf("* OK") > 0 || response.indexOf("ready") > 0;
}
bool Connection::parseLogin() {
	return parseOK();
}
bool Connection::parseBoxes()
{
	inboxName    = findBox(response, "inbox");
	trashBoxName = findBox(response, "trash");
	return !inboxName.isEmpty() && !trashBoxName.isEmpty();
}
bool Connection::parseExamine() {
	return parseOK();
}
bool Connection::parseUnseen()
{
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
			unseenMailsIDs << i;
	}
	return true;
}

bool Connection::parseHeader(MailInfo& info)
{
	int fromStart = response.indexOf("From: ");
	if(fromStart == -1)
		return false;
	int fromEnd = response.indexOf("\r", fromStart);
	info.from = response.mid(fromStart, fromEnd - fromStart + 1);

	int subjectStart = response.indexOf("Subject: ");
	if(subjectStart == -1)
		return false;
	int subjectEnd = response.indexOf("\r", subjectStart);
	info.subject = response.mid(subjectStart, subjectEnd - subjectStart + 1);

	int dateStart = response.indexOf("Date: ");
	if(dateStart == -1)
		return false;
	int dateEnd = response.indexOf("\r", dateStart);
	info.date = response.mid(dateStart, dateEnd - dateStart + 1);

	return true;
}

bool Connection::parseLogout() {
	return parseOK();
}

void Connection::setAccount(const AccountInfo& acc) {
	account = acc;
}

QString Connection::findBox(const QString& string, const QString& target) const
{
	QStringList lines = string.split("\r\n");
	foreach(QString line, lines)
	{
		QStringList sections = line.split(" ");
		foreach(QString section, sections)
		{
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
	if(socket == 0 || !socket->isOpen())
		return;
	qint64 bytesWritten = socket->write(command.toUtf8() + "\r\n");
	if(bytesWritten != command.size() + 2)
		qDebug("Could not write all bytes");
	if(bytesWritten > 0 && !socket->waitForBytesWritten(timeout))
		qDebug("Could not write bytes from buffer");
	qDebug() << "\n ----------- Send ------------\n" << command;
	readResponse();
}

void Connection::readResponse()
{
	if(!socket->waitForReadyRead(timeout))
	{
		emit connectionError("Could not receive data:");
		qDebug("Could not receive data:");
		return;
	}
	response = socket->readAll();

	qDebug() << "\n ------------- Response -------------\n" << response;
}

MailList Connection::getUnseenMails() const {
	return unseenMails;
}
