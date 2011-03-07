#include "SubjectLabel.h"

SubjectLabel::SubjectLabel(QWidget *parent)
	: QLabel(parent) {}

void SubjectLabel::mousePressEvent(QMouseEvent*) {
	emit clicked();
}
