#include "QKonsole.h"

bool QKonsole_register(Window* win) {
	QAction* console = new QAction(Window::tr("Console"), win);
	console->setData(QVariant("QKonsole"));
	QObject::connect(console, &QAction::triggered, win, &Window::createOrActivate);
	QMenu* menu = win->findMenu("&File");
	if (menu == nullptr) {
		if ((menu = win->menuBar()->addMenu("&File")) == nullptr) {
			return false;
		}
	}

	QList<QAction*> actions = menu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
	menu->insertAction(actions[1], console);
	return true;
}

bool QKonsole_unregister(Window* win) {
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

Terminal::Terminal(QWidget* parent)
    : QPlainTextEdit(parent) {
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);

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
    default:
        if (m_localEchoEnabled) {
            emit logMessage("localEcho");
            keyPressEvent(e);
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
}

void QKonsole::putData(const QByteArray& data) {
    m_terminal->insertPlainText(data);
    QScrollBar* bar = m_terminal->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void QKonsole::settingsChanged() {}