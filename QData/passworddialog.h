#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QCryptographicHash>

namespace Ui {
class PasswordDialog;
}

class PasswordDialog : public QDialog {
    Q_OBJECT

public:
    explicit PasswordDialog(QWidget* parent = 0, const QByteArray& password = QByteArray());
    
    ~PasswordDialog();

private slots:

    void okPressed();

    void cancelPressed();

private:
    const QByteArray m_hash;
    Ui::PasswordDialog* m_ui = nullptr;

    bool checkPassword(const QString& inputPassword);

protected:
    bool event(QEvent* e) override;
};

#endif // PASSWORDDIALOG_H
