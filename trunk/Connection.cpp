#include "Connection.h"
#include <QStringList>

Connection::Connection(QObject *parent)	: QObject(parent), socket(0), status(DISCONNECTED)
{
	setEnableSSL(true);
}

void Connection::setEnableSSL(bool ssl)
{
	if(socket != 0)
		socket->deleteLater();
	socket = new QSslSocket(this);
	connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
			this,   SLOT(onStateChanged(QAbstractSocket::SocketState)));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), 
			this,   SLOT(onError(QAbstractSocket::SocketError)));
	connect(socket, SIGNAL(readyRead()), this, SLOT(onReadSocket()));
}

void Connection::check() 
{
//	socket->connectToHostEncrypted(account.host, account.port);


	if(!login())
		return;
	if(!findUnseen())
		return;
	if(!fetchUnseen())
		return;
}

void Connection::login() {
	sendData(tr(". login %1 %2").arg(account.user).arg(account.pass));
}
void Connection::findBoxes() {
	sendData(". list \"\" \"*\"");
}
void Connection::examineInbox() {
	sendData(tr(". examine %1").arg(inboxName));
}
void Connection::searchUnseen() {
	sendData(". search unseen");
}
void Connection::fetchUnseen()
{
	foreach()
	{
		if(!fetch())
			return;
	}
	return;
}
void Connection::fetch()
{

}

void Connection::onError(QAbstractSocket::SocketError error)
{
	qFatal("socket error");
	emit connectionError(socket->errorString());
}

void Connection::sendData(const QString& data) {
	if(socket)
		socket->write(data.toUtf8() + "\r\n");
}

void Connection::onStateChanged(QAbstractSocket::SocketState state)
{
	if(state == QAbstractSocket::UnconnectedState)
	{
		qFatal("Connection lost");
		socket->deleteLater();
		socket = 0;
	}
}

void Connection::onReadSocket()
{
	buffer = socket->readAll();
	switch(status)
	{
	case DISCONNECTED:
		if(parseConnection())
		{
			status = CONNECTED;
			login();
		}
		break;
	case CONNECTED:
		if(parseLogin())
		{
			status = LOGGED_IN;
			findBoxes();
		}
		break;
	case LOGGED_IN:
		if(parseBoxes())
		{
			status = FOUND_BOXES;
			examineInbox();
		}
		break;
	case FOUND_BOXES:
		if(parseExamine())
		{
			status = EXAMINE_INBOX;
			searchUnseen();
		}
		break;
	case EXAMINE_INBOX:
		if(parseUnseen())
		{
			status = FETCH_UNSEEN;
			fetchUnseen();
		}
		break;
	case FETCH_UNSEEN:
		if(parseHeader())
		{
			status = FETCH_UNSEEN;
			fetchUnseen();
		}
		else {
			status = DONE;
		}
		break;
	}
}


bool Connection::parseOK() {
	return buffer.indexOf(". OK") > 0;
}
bool Connection::parseConnection() {
	return buffer.indexOf("* OK") > 0 || buffer.indexOf("ready") > 0;
}
bool Connection::parseLogin() {
	return parseOK();
}
bool Connection::parseBoxes()
{
	inboxName    = findBox(buffer, "inbox");
	trashBoxName = findBox(buffer, "trash");
	return !inboxName.isEmpty() && !trashBoxName.isEmpty();
}
bool Connection::parseExamine() {
	return parseOK();
}
bool Connection::parseUnseen()
{
	if(buffer.indexOf(". OK") == -1)
		return false;

	unseenMails.clear();
	int start = buffer.indexOf("* SEARCH ") + 9;
	int end   = buffer.indexOf(". OK ") - 1;
	QString line = buffer.mid(start, end - start + 1);
	QStringList ids = line.split(" ");
	foreach(QString id, ids)
		unseenMails << id.toInt();
	return true;
}

bool Connection::parseHeader()
{
	return true;
}

void Connection::setAccount(const AccountInfo& acc) {
	account = acc;
}

QString Connection::findBox(const QString& string, const QString& target) const
{
	QStringList lines = string.split("\n");
	foreach(QString line, lines)
	{
		QStringList sections = line.split(" ");
		foreach(QString section, sections)
		{
			if(section.indexOf(target, 0, Qt::CaseInsensitive) > 0)
			{
				QString name = section.remove('\"');
				return name.left(name.length() - 1);   // remove last \0
			}
		}
	}
	return QString();
}