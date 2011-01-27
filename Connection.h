#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QSslSocket>

struct MailInfo
{
	int     id;
	QString subject;
	QString from;
	QString date;
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

typedef QList<MailInfo> MailList;

class Connection : public QObject
{
	Q_OBJECT

public:
	Connection(QObject *parent);

	void setAccount(const AccountInfo& acc);
	void setEnableSSL(bool ssl);
	bool check();
	MailList getUnseenMails() const;

protected:
	bool connect();
	bool login();
	bool findBoxes();
	bool searchUnseen();
	bool fetchUnseen();
	bool logout();
	void close();

	bool parseOK();
	bool parseConnection();
	bool parseLogin();
	bool parseBoxes();
	bool parseExamine();
	bool parseUnseen();
	bool parseHeader(MailInfo& info);
	bool parseLogout();
	
	QString findBox(const QString& string, const QString& target) const;

	void setRead(int id);
	void delMail(int id);	

	void sendCommand(const QString& command);
	void readResponse();

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
