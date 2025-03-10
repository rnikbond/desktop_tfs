//----------------------------------------
#include <QFIle>
#include <QSettings>
#include <QMessageBox>
//----------------------------------------
#include "MainWindow.h"
#include "ui_MainWindow.h"
//----------------------------------------

void MainWindow::readConfig() {

    if( !QFile(CONF_PATH).exists() ) {
        changeSettings();
        return;
    }

    QSettings conf(CONF_PATH, QSettings::IniFormat);

    int version = conf.value( CONF_VERSION, 0 ).toInt();
    switch( version ) {
        case CURRENT_CONF_VERSION: {
            break;
        }
        default: {
            QMessageBox::critical( this, tr("Ошибка"), tr("Некорретный файл конфигурации") );
            changeSettings();
            return;
        }
    }

    conf.beginGroup( CONF_GROUP_AZURE );
    config.Azure.tfPath    = conf.value( CONF_TF_PATH         , "" ).toString();
    config.Azure.url       = conf.value( CONF_AZURE_URL       , "" ).toString();
    config.Azure.workspace = conf.value( CONF_AZURE_WORKSPACE , "" ).toString();
    config.Azure.login     = conf.value( CONF_AZURE_LOGIN     , "" ).toString();
    config.Azure.password  = conf.value( CONF_AZURE_PASSWORD  , "" ).toString();
    conf.endGroup();

    if( !config.Azure.isIncomplete() ) {
        changeSettings();
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::saveConfig() {

    QSettings conf(CONF_PATH, QSettings::IniFormat);

    conf.setValue( CONF_VERSION, CURRENT_CONF_VERSION );

    conf.beginGroup( CONF_GROUP_AZURE );
    conf.setValue( CONF_TF_PATH        , config.Azure.tfPath    );
    conf.setValue( CONF_AZURE_URL      , config.Azure.url       );
    conf.setValue( CONF_AZURE_WORKSPACE, config.Azure.workspace );
    conf.setValue( CONF_AZURE_LOGIN    , config.Azure.login     );
    conf.setValue( CONF_AZURE_PASSWORD , config.Azure.password  );
    conf.endGroup();
}
//----------------------------------------------------------------------------------------------------------
