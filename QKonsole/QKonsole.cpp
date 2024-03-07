#include "QKonsole.h"
#include <QApplication>
#include <QTranslator>

struct QKonsoleMenu {
    QKonsoleMenu(QCoreApplication* app) 
    : m_app(app){
        m_translator = new QTranslator();
        if (m_translator->load(QLocale::system(), "QKonsole", "_", "translations")) { //set directory of ts
            m_app->installTranslator(m_translator);
        }

        m_console = new QAction(QApplication::translate("MainWindow", "Console"), nullptr);
        m_console->setData(QVariant("QKonsole"));
    }

    QAction* m_console = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QKonsole_register(Window* win, PluginsLoader* ld, QKonsoleMenu* ctx, Logger* log) {

	QObject::connect(ctx->m_console, &QAction::triggered, win, &Window::createOrActivate);
	QMenu* menu = win->findMenu(QApplication::translate("MainWindow", "&File"));
	QList<QAction*> actions = menu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
	menu->insertAction(actions.size() > 0 ? actions[1] : nullptr, ctx->m_console);

	return true;
}

static bool QKonsole_unregister(Window* win, PluginsLoader* ld, QKonsoleMenu* ctx, Logger* log) {
    return true;
}

REGISTER_PLUGIN(
	QKonsole, 
	Plugin::Type::WIDGET, 
	"0.0.1", 
	"pawel.ciejka@gmail.com", 
	"example plugin",
	QKonsole_register,
	QKonsole_unregister,
    QKonsoleMenu
)

Terminal::Terminal(QKonsole* parent)
    : QPlainTextEdit(parent) {
    document()->setMaximumBlockCount(100);
    Window* win = qobject_cast<Window*>(parent->window());
    if (win != nullptr) {
        setLocalEchoEnabled(Settings::get().value("serial/localEchoEnabled", false).toBool());
    }
 
    connect(this, &Terminal::logMessage, qobject_cast<Widget*>(parent), &Widget::message);
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