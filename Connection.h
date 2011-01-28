#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QSslSocket>
#include <QList>

struct MailInfo
{
	QString accountName;
	int     id;
	QString subject;
	QString from;
	QString date;
};

typedef QList<MailInfo> Mails;

struct MailList
{
	QString accountName;
	Mails   mails;
};

struct AccountInfo
{
	AccountInfo() {}
	AccountInfo(const QString& name, const QString& pro, const QString& h, 
				const QString& u, const QString& p, int pt, bool s)
		: accountName(name), protocol(pro), host(h), user(u), pass(p), port(pt), ssl(s) {}

	QString accountName;
	QString protocol;
	QString host;
	QString user;
	QString pass;
	int     port;
	bool    ssl;
};

class QTcpSocket;

class Connection : public QObject
{
	Q_OBJECT

public:
	Connection(QObject* parent = 0);

	void setAccount(const AccountInfo& acc);
	void setEnableSSL(bool ssl);
	bool check();
	MailList getUnseenMails() const;
	bool setRead(int id);
	bool delMail(int id);	

protected:
	bool connect();
	bool login();
	bool findBoxes();
	bool searchUnseen();
	bool fetchUnseen();
	bool logout();
	void close();
	bool doSetRead(int id);
	bool doDelMail(int id);

	bool parseOK();
	bool parseBoxes();
	bool parseUnseen();
	bool parseHeader(MailInfo& info);
	
	QString findBox(const QString& string, const QString& target) const;


	void sendCommand(const QString& command);
	void readResponse();
	bool responseDone();
	QString elide(const QString& string, int length = 50);

protected slots:
	void onError(QAbstractSocket::SocketError error);
	void onStateChanged(QAbstractSocket::SocketState state);

signals:
	void connectionError(const QString& msg);

protected:
	int     timeout;
	QString inboxName;
	QString trashBoxName;
	QSslSocket* socket;
	QByteArray response;
	AccountInfo account;
	QList<int> unseenMailsIDs;
	MailList   unseenMails;
};

#endif // CONNECTION_H
