#ifndef LOADING_DIALOG_H
#define LOADING_DIALOG_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMovie>
#include <QLabel>

#include "../api/api.h"
#include "../core/core.h"

class LoadingDialog : public QWidget{

	Q_OBJECT

public:
	LoadingDialog(QWidget* parent = nullptr, Logger* logger = nullptr) 
		: QWidget(parent)
		, m_logger(logger){
		Qt::WindowFlags flags = windowFlags();
		flags |= Qt::WindowStaysOnTopHint;
		flags |= Qt::FramelessWindowHint;
		flags |= Qt::Dialog;
		setWindowFlags(flags);
		setWindowModality(Qt::ApplicationModal);
		QLayout* l = new QVBoxLayout();
		setLayout(l);
		QMovie* movie = new QMovie(":/images/kOnzy.gif");
		if (movie->isValid() == false) {
			m_logger->message("LoadingDialog: invalid resource: " + movie->lastErrorString());
		}
		movie->setScaledSize(QSize(64, 64));
		QLabel* label = new QLabel();
		label->setMovie(movie);
		//label->setMaximumSize(128, 128);
		l->addWidget(label);
		QLabel* txt = new QLabel(tr("Please wait"));
		txt->setAlignment(Qt::AlignCenter);
		txt->setStyleSheet("QLabel{ font-weight:bold;}");
		l->addWidget(txt);
		movie->start();
	}
private:
	Logger* m_logger = nullptr;
};

#endif
