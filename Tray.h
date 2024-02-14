#ifndef TRAY_H
#define TRAY_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>

#include "mainwindow.h"

class Tray : public QSystemTrayIcon {
	Q_OBJECT

public:

	Tray(MainWindow *win)
	: m_window(win){
		setIcon(QIcon(":/res/arduino.png"));
		m_menu.addAction(tr("Window"), this, SLOT(windowTriggered()));
		//m_menu.addAction(tr("Log"), this, SLOT(logTriggered()));
		m_menu.addSeparator();
		m_menu.addAction(tr("Quit"), this, SLOT(quit()));
		setContextMenu(&m_menu);
	}

protected slots:

	void windowTriggered() {
		if (m_window->isVisible()) {
			m_window->hide();
		} else {
			m_window->show();
		}
	}

	void logTriggered() {
		
	}

	void quit() {
		QApplication::quit();
	}

private:
	QMenu m_menu;
	MainWindow* m_window = nullptr;
};

#endif