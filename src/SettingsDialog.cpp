//----------------------------------------
#include <QDebug>
//----------------------------------------
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
//----------------------------------------

SettingsDialog::SettingsDialog( QWidget* parent ) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    m_Config = nullptr;

    connect( ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::save );
}
//----------------------------------------------------------------------------------------------------------

SettingsDialog::~SettingsDialog() {

    delete ui;
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::setConf( ConfigTFS* conf ) {

    m_Config = conf;
    restore();
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::save() {

    m_Config->binPath        = ui->binPathEdit->text();
    m_Config->creds.login    = ui->loginEdit  ->text();
    m_Config->creds.password = ui->passwordEdit->text();
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::restore() {

    if( m_Config == nullptr ) {
        qWarning() << "SettingsDialog::restore()" << "config is nullptr";
        return;
    }

    // Путь к программе TF
    ui->binPathEdit->setText( m_Config->binPath );

    // Аутентификация
    ui->loginEdit   ->setText( m_Config->creds.login    );
    ui->passwordEdit->setText( m_Config->creds.password );
}
//----------------------------------------------------------------------------------------------------------
