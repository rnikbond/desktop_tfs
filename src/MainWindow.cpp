//----------------------------------------
#include <QTimer>
#include <QSettings>
//----------------------------------------
#include "SettingsDialog.h"
//----------------------------------------
#include "MainWindow.h"
#include "ui_MainWindow.h"
//----------------------------------------

MainWindow::MainWindow( QWidget* parent ) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
//----------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow() {

    delete ui;
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::readConfig() {

    QSettings conf("tfs.conf", QSettings::IniFormat);

    conf.beginGroup( CONF_GROUP_TFS );
    config.binPath     = conf.value( CONF_BIN       , "" ).toString();
    config.collection  = conf.value( CONF_COLLECTION, "" ).toString();
    config.workspace   = conf.value( CONF_WORKSPACE , "" ).toString();
    config.workfold    = conf.value( CONF_WORKFOLD  , "" ).toString();
    conf.endGroup();

    conf.beginGroup( CONF_GROUP_CREDS );
    config.creds.login    = conf.value( CONF_LOGIN   , "" ).toString();
    config.creds.password = conf.value( CONF_PASSWORD, "" ).toString();

    if( !config.isValid() ) {
        ConfigTFS confTmp = config;

        SettingsDialog dialog;
        dialog.setConf( &confTmp );
        if( dialog.exec() == QDialog::Accepted ) {
            config = confTmp;
            saveConfig();
        }
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::saveConfig() {

    config.print();

    QSettings conf("tfs.conf", QSettings::IniFormat);

    conf.beginGroup( CONF_GROUP_TFS );
    conf.setValue( CONF_BIN       , config.binPath    );
    conf.setValue( CONF_COLLECTION, config.collection );
    conf.setValue( CONF_WORKSPACE , config.workspace  );
    conf.setValue( CONF_WORKFOLD  , config.workfold   );
    conf.endGroup();

    conf.beginGroup( CONF_GROUP_CREDS );
    conf.setValue( CONF_LOGIN   , config.creds.login    );
    conf.setValue( CONF_PASSWORD, config.creds.password );
    conf.endGroup();
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::showEvent( QShowEvent* event ) {

    QMainWindow::showEvent( event );
    QTimer::singleShot( 0, this, &MainWindow::readConfig );
}
//----------------------------------------------------------------------------------------------------------

