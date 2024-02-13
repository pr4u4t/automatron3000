#ifndef MDI_WINDOW_H
#define MDI_WINDOW_H

#include <QMdiSubWindow>
#include <QCloseEvent>

class MdiWindow : public QMdiSubWindow {
	 Q_OBJECT

public:
	MdiWindow(QWidget* parent, QWidget* widget) 
		: QMdiSubWindow(parent){
		setWidget(widget);
	}

	void closeEvent(QCloseEvent* event) override {
		hide();
		event->ignore();
	}

};

#endif
