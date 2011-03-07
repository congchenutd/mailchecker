#ifndef SUBJECTLABEL_H
#define SUBJECTLABEL_H

#include <QLabel>

class SubjectLabel : public QLabel
{
	Q_OBJECT

public:
	SubjectLabel(QWidget *parent);

protected:
	virtual void mousePressEvent(QMouseEvent*);

signals:
	void clicked();
};

#endif // SUBJECTLABEL_H
