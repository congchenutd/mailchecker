#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QSslSocket>

struct MailInfo
{
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

class Connection : public QObject
{
	Q_OBJECT

	typedef QList<MailInfo> MailList;
	typedef enum {
		DISCONNECTED, 
		CONNECTED, 
		LOGGED_IN,
		FOUND_BOXES,
		EXAMINE_INBOX,
		FOUND_UNSEEN,
		FETCH_UNSEEN,
		DONE
	} Status;

public:
	Connection(QObject *parent);

	void setAccount(const AccountInfo& acc);
	void setEnableSSL(bool ssl);
	void check();

protected:
	void sendData(const QString& data);

	void login();
	void findBoxes();
	void examineInbox();
	void searchUnseen();
	void fetchUnseen();
	void fetch();

	bool parseOK();
	bool parseConnection();
	bool parseLogin();
	bool parseBoxes();
	bool parseExamine();
	bool parseUnseen();
	bool parseHeader();
	
	QString findBox(const QString& string, const QString& target) const;

	MailList getUnseenMails() const;
	void setRead(int id);
	void delMail(int id);	


protected slots:
	void onError(QAbstractSocket::SocketError error);
	void onStateChanged(QAbstractSocket::SocketState state);
	void onReadSocket();

signals:
	void connectionError(const QString& msg);

protected:
	QString inboxName;
	QString trashBoxName;
	QSslSocket* socket;
	QByteArray buffer;
	Status status;
	AccountInfo account;
	QList<int> unseenMails;
};

#endif // CONNECTION_H
