//----------------------------------------
#include <QTimer>
#include <QSettings>
#include <QProcess>
#include <QTextCodec>
//----------------------------------------
#include "SettingsDialog.h"
//----------------------------------------
#include "MainWindow.h"
#include "ui_MainWindow.h"
//----------------------------------------

MainWindow::MainWindow( QWidget* parent ) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->treeWidget->setColumnCount( 1 );

    connect( ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::expantNode );
}
//----------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow() {

    delete ui;
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::checkTfsConnection() {

    qDebug() << "config.binPath: " << config.binPath;

   // QStringList args = { "workspaces", "-collection:http://it-tfs.decima.local:8080/tfs/KOTMI" };
   // QStringList args = { "workfold", "-workspace:RNB", "-collection:http://it-tfs.decima.local:8080/tfs/KOTMI" };

//    QStringList args = { "dir", "$/" };

//    QProcess tfs_process;
//    tfs_process.setReadChannelMode(QProcess::MergedChannels);
//    tfs_process.start( config.binPath, args );

//    //qDebug() << "error: " << tfs_process.errorString();
//    tfs_process.waitForFinished();

//    QByteArray line = tfs_process.readAllStandardOutput();
//    QString output = QTextCodec::codecForName("cp1251")->toUnicode( line );
//    QStringList lines = output.split( "\r\n" );

//    for( QString str : lines ) {
//        qDebug() << str;
//    }

    requestTree();

}
//----------------------------------------------------------------------------------------------------------

void MainWindow::requestTree() {

    auto dir_info = [&]( const QString& path ) {

        QStringList args = { "dir", path };

        QProcess tfs_process;
        tfs_process.setReadChannelMode(QProcess::MergedChannels);
        tfs_process.start( config.binPath, args );
        tfs_process.waitForFinished();

        QByteArray line = tfs_process.readAllStandardOutput();
        QString output = QTextCodec::codecForName("cp1251")->toUnicode( line );
        QStringList lines = output.split( "\r\n" );

        return lines;
    };



    QStringList rootDirs = dir_info( "$/" );
    for( int i = 0; i < rootDirs.count(); i++ ) {

        QString dirName = rootDirs.at(i);
        if( !dirName.startsWith("$") || dirName == "$/:" ) {
            continue;
        }

        dirName = dirName.remove("$");

        qDebug() << dirName;

        QTreeWidgetItem* dirItem = new QTreeWidgetItem;
        dirItem->setText( 0, dirName );

        ui->treeWidget->addTopLevelItem( dirItem );
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::expantNode( QTreeWidgetItem* item, int ) {

    auto dir_info = [&]( const QString& path ) {

        QStringList args = { "dir", path };

        QProcess tfs_process;
        tfs_process.setReadChannelMode(QProcess::MergedChannels);
        tfs_process.start( config.binPath, args );
        tfs_process.waitForFinished();

        QByteArray line = tfs_process.readAllStandardOutput();
        QString output = QTextCodec::codecForName("cp1251")->toUnicode( line );
        QStringList lines = output.split( "\r\n" );

        return lines;
    };

    QString path = item->text(0);

    QTreeWidgetItem* item_parent = item->parent();
    while( item_parent != nullptr ) {
        if( !path.isEmpty() ) {
            path.prepend( "/" );
        }
        path.prepend( item_parent->text(0) );
        item_parent = item_parent->parent();
    }

    path.prepend( "$/" );

    qDebug() << "path: " << path;

    QStringList rootDirs = dir_info( path );
    for( int i = 0; i < rootDirs.count(); i++ ) {

        QString dirName = rootDirs.at(i);
        if( !dirName.startsWith("$") || dirName.startsWith("$/") ) {
            continue;
        }

        dirName = dirName.remove("$");

        qDebug() << dirName;

        QTreeWidgetItem* dirItem = new QTreeWidgetItem;
        dirItem->setText( 0, dirName );

        item->addChild(dirItem  );
    }
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
        if( dialog.exec() != QDialog::Accepted ) {
            return;
        }

        config = confTmp;
        saveConfig();
    }

    checkTfsConnection();
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

