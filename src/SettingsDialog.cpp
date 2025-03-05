//----------------------------------------
#include <QDebug>
#include <QFileDialog>
//----------------------------------------
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
//----------------------------------------

SettingsDialog::SettingsDialog( QWidget* parent ) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    m_Config = nullptr;

    connect( ui->buttonBox    , &QDialogButtonBox::accepted, this, &SettingsDialog::save      );
    connect( ui->binPathButton, &QToolButton     ::clicked , this, &SettingsDialog::selectBin );
}
//----------------------------------------------------------------------------------------------------------

SettingsDialog::~SettingsDialog() {

    delete ui;
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::selectBin() {

    QString filter;
    QString dir = m_Config->binPath;

#ifdef WIN32
    filter = "*.exe";
#endif

    if( dir.isEmpty() ) {
#ifdef WIN32
        dir = "C:/";
#else
        dir = QDir::homePath();
#endif
    }

    QString path = QFileDialog::getOpenFileName( this, tr("Программа TF"), dir, filter );
    if( path.isEmpty() ) {
        return;
    }

    ui->binPathEdit->setText( QDir::fromNativeSeparators(path) );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::setConf( ConfigTFS* conf ) {

    m_Config = conf;
    restore();
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::save() {

    m_Config->binPath        = ui->binPathEdit   ->text();
    m_Config->collection     = ui->collectionEdit->text();
    m_Config->creds.login    = ui->loginEdit     ->text();
    m_Config->creds.password = ui->passwordEdit  ->text();
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::restore() {

    if( m_Config == nullptr ) {
        qWarning() << "SettingsDialog::restore()" << "config is nullptr";
        return;
    }

    // Путь к программе TF
    ui->binPathEdit->setText( m_Config->binPath );

    // Asure DevOps Server
    ui->collectionEdit->setText( m_Config->collection );

    // Аутентификация
    ui->loginEdit   ->setText( m_Config->creds.login    );
    ui->passwordEdit->setText( m_Config->creds.password );
}
//----------------------------------------------------------------------------------------------------------
