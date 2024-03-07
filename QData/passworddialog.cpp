#include "passworddialog.h"
#include "ui_passworddialog.h"
#include <QMessageBox>
#include <QPushButton>

PasswordDialog::PasswordDialog(QWidget* parent, const QByteArray& hash)
    : QDialog(parent)
    , m_ui(new Ui::PasswordDialog)
    , m_hash(hash){
    m_ui->setupUi(this);
    
    if (m_hash.isEmpty()) {
        m_ui->wrongPasswordLabel->setText(tr("<b>The password is unset, so access is prevented</b><br>"
                                           "Go to application settings to set the password."));
        m_ui->lineEdit->setEnabled(false);
    }

    m_ui->lineEdit->setEchoMode(QLineEdit::Password);
    connect(m_ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::pressed, this, &PasswordDialog::okPressed);
    connect(m_ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::pressed, this, &PasswordDialog::cancelPressed);
}

bool PasswordDialog::event(QEvent* e){
    return QDialog::event(e);
}

PasswordDialog::~PasswordDialog(){
    delete m_ui;
}
bool PasswordDialog::checkPassword(const QString& inputPassword){
    if (m_hash.isEmpty()) {
        return false;
    }
    
    return inputPassword.toLocal8Bit() == m_hash;
}

void PasswordDialog::okPressed() {
    if (checkPassword(m_ui->lineEdit->text())) {
        m_ui->wrongPasswordLabel->setText(tr("<b>Password correct, press Ok to enter.</b>"));
        done(QDialog::Accepted);
    }
    else {
        m_ui->wrongPasswordLabel->setEnabled(false);
        m_ui->wrongPasswordLabel->setText(tr("Password incorrect"));
    }
}

void PasswordDialog::cancelPressed(){
    done(QDialog::Rejected);
}
