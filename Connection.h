#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QSslSocket>
#include <QList>
#include <QThread>

struct MailInfo
{
	QString accountName;
	int     id;
	QString subject;
	QString from;
	QString date;
};

typedef QList<MailInfo> Mails;

struct AccountMails
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
	bool    enable;
};

class QTcpSocket;

class Connection : public QThread
{
	Q_OBJECT

public:
	Connection(QObject* parent = 0);

	typedef enum {CHECK, DELETE, READ} Mission;
	void run();
	void setMission(Mission m) { mission = m; }
	void setTargetID(int id)   { targetID = id; }
	bool missionSuccessful() const { return successful; }
	void setAccount(const AccountInfo& acc);
	AccountMails getNewMails() const;

protected:
	bool check();
	bool setRead(int id);
	bool delMail(int id);

	bool connect();
	bool login();
	bool findBoxes();
	bool searchUnseen();
	bool fetchUnseen();
	bool logout();
	bool doSetRead(int id);
	bool doDelMail(int id);
	QString findBox(const QString& string, const QString& target) const;
	QString elide(const QString& string, int length = 50) const;

	bool parseOK();
	bool parseBoxes();
	bool parseUnseen();
	bool parseHeader(MailInfo& info);

	void sendCommand(const QString& command);
	void readResponse();

protected:
	int     timeout;
	QString inboxName;
	QString trashBoxName;
	QSslSocket* socket;
	QByteArray response;
	AccountInfo account;
	QList<int> newIDs;
	AccountMails   newMails;
	Mission mission;
	int targetID;
	bool successful;
};

#endif // CONNECTION_H
