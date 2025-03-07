//----------------------------------------
#include <QDebug>
#include <QFileDialog>
//----------------------------------------
#include "ManagerTFS.h"
#include "AzureConnectDialog.h"
//----------------------------------------
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
//----------------------------------------
#ifdef WIN32
 #define BIN_FILTER     "*.exe"
 #define EXAMPLE_TF_BIN "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/IDE/CommonExtensions/Microsoft/TeamFoundation/Team Explorer/TF.exe"
#else
 #define BIN_FILTER ""
 #define EXAMPLE_TF_BIN "/home/user/TEE-CLC-xx.xxx.x/tf"
#endif
//----------------------------------------

void SettingsDialog::showAzureSettings() {

    AzureConnectDialog dialog;
    dialog.setConfig( *m_Config );
    if( dialog.exec() != QDialog::Accepted ) {
        return;
    }

    const ConfigTFS& cfg = dialog.config();
    m_Config->azureUrl       = cfg.azureUrl      ;
    m_Config->workspace      = cfg.workspace     ;
    m_Config->creds.login    = cfg.creds.login   ;
    m_Config->creds.password = cfg.creds.password;
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::selectBin() {

    QString dir = m_Config->binPath.isEmpty() ? QDir::homePath() : m_Config->binPath;
    QString path = QFileDialog::getOpenFileName( this, tr("Программа TF"), dir, BIN_FILTER );
    if( path.isEmpty() ) {
        return;
    }

    ui->binPathEdit->setText( QDir::fromNativeSeparators(path) );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::updatePageConfig() {
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::savePageConfig() {

    m_Config->binPath = ui->binPathEdit->text();
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::restorePageConfig() {

    ui->binPathEdit ->setText( m_Config->binPath );
    updatePageConfig();
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::initPageConfig() {

    ui->binPathEdit->setPlaceholderText( EXAMPLE_TF_BIN );

    connect( ui->binPathButton    , &QToolButton::clicked, this, &SettingsDialog::selectBin         );
    connect( ui->azureServerButton, &QPushButton::clicked, this, &SettingsDialog::showAzureSettings );
}
//----------------------------------------------------------------------------------------------------------

