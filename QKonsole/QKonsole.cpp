#include "QKonsole.h"
#include <QApplication>
#include <QTranslator>

bool QKonsole_register(Window* win, PluginsLoader* ld) {
    QCoreApplication* app = QApplication::instance();

    QTranslator* translator = new QTranslator();
    if (translator->load(QLocale::system(), "QKonsole", "_", "translations")) { //set directory of ts
        app->installTranslator(translator);
    }
    
    QAction* console = new QAction(QApplication::translate("MainWindow", "Console"), win);
	console->setData(QVariant("QKonsole"));
	QObject::connect(console, &QAction::triggered, win, &Window::createOrActivate);
	QMenu* menu = win->findMenu(QApplication::translate("MainWindow", "&File"));
	if (menu == nullptr) {
		if ((menu = win->menuBar()->addMenu(QApplication::translate("MainWindow","&File"))) == nullptr) {
			return false;
		}
	}

	QList<QAction*> actions = menu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
	menu->insertAction(actions[1], console);
	return true;
}

bool QKonsole_unregister(Window* win, PluginsLoader* ld) {
	return true;
}

REGISTER_PLUGIN(
	QKonsole, 
	Plugin::Type::WIDGET, 
	"0.0.1", 
	"pawel.ciejka@gmail.com", 
	"example plugin",
	QKonsole_register,
	QKonsole_unregister
)

Terminal::Terminal(QKonsole* parent)
    : QPlainTextEdit(parent) {
    document()->setMaximumBlockCount(100);
    Window* win = qobject_cast<Window*>(parent->window());
    if (win != nullptr) {
        setLocalEchoEnabled(win->settings().value("serial/localEchoEnabled", false).toBool());
    }
    /*
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
    */
 
    connect(this, &Terminal::logMessage, qobject_cast<MdiChild*>(parent), &MdiChild::logMessage);
}

void Terminal::setLocalEchoEnabled(bool set) {
    m_localEchoEnabled = set;
}

void Terminal::keyPressEvent(QKeyEvent* e) {
    emit logMessage("Terminal::keyPressEvent");

    switch (e->key()) {
        case Qt::Key_Backspace:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            break;
        case Qt::Key_Return:
            QPlainTextEdit::keyPressEvent(e);
            emit enterPressed();
            break;
        default:
            if (m_localEchoEnabled) {
                emit logMessage("localEcho");
                QPlainTextEdit::keyPressEvent(e);
            }
            emit getData(e->text().toLocal8Bit());
    }
}

void Terminal::mousePressEvent(QMouseEvent* e) {
    Q_UNUSED(e);
    setFocus();
}

void Terminal::mouseDoubleClickEvent(QMouseEvent* e) {
    Q_UNUSED(e);
}

void Terminal::contextMenuEvent(QContextMenuEvent* e) {
    Q_UNUSED(e);
}

QKonsole::QKonsole(const Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
    : Widget(ld, plugins, parent, settingsPath), m_terminal(new Terminal(this)) {
    QBoxLayout* l = new QVBoxLayout();
    l->addWidget(m_terminal);
    setLayout(l);

    putPrompt(prompt());

    QObject::connect(m_terminal, &Terminal::enterPressed, this, &QKonsole::enterPressed);
}

void QKonsole::putData(const QByteArray& data) {
    m_terminal->insertPlainText(data);
    QScrollBar* bar = m_terminal->verticalScrollBar();
    if (bar != nullptr) {
        bar->setValue(bar->maximum());
    }
}

void QKonsole::enterPressed() {
    putPrompt(prompt());
}

void QKonsole::putPrompt(const QString& prompt) {
    QTextCursor cur = m_terminal->textCursor();
    cur.movePosition(QTextCursor::End);
    cur.insertText(prompt);
    m_terminal->setTextCursor(cur);
}

void QKonsole::settingsChanged() {}