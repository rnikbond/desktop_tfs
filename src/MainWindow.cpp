//----------------------------------------
#include <QTimer>
#include <QSettings>
#include <QProcess>
#include <QTextCodec>
#include <QTemporaryFile>
#include <QFileIconProvider>
//----------------------------------------
#include "SettingsDialog.h"
//----------------------------------------
#include "MainWindow.h"
#include "ui_MainWindow.h"
//----------------------------------------
enum CustomRoles {
    LoadRole = Qt::UserRole + 1,
};
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

    QStringList rootDirs = whatsInFolder("$/");
    createTreeItems( nullptr, rootDirs );
}
//----------------------------------------------------------------------------------------------------------

QStringList MainWindow::whatsInFolder( const QString& folder ) {

    QStringList args = { "dir", folder };

    QProcess tfs_process;
    tfs_process.setReadChannelMode(QProcess::MergedChannels);
    tfs_process.start( config.binPath, args );
    tfs_process.waitForFinished();

    QByteArray tf_output = tfs_process.readAllStandardOutput();
    QStringList subdirsList =QTextCodec::codecForName("cp1251")->toUnicode(tf_output).split( "\r\n", Qt::SkipEmptyParts );
    subdirsList.removeLast();

    for( int i = 0; i < subdirsList.count(); i++ ) {
        QString dirName = subdirsList.at(i);
        if( dirName.contains(":") ) {
            subdirsList.removeAt( i );
            i--;
        } else {
             subdirsList[i] = subdirsList[i].remove("$");
        }
    }

    return subdirsList;
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::createTreeItems( QTreeWidgetItem* item, const QStringList& names ) {

    QList<QTreeWidgetItem*> items;

    for( const QString& name : names ) {
        QTreeWidgetItem* newItem = new QTreeWidgetItem;
        newItem->setText( 0, name       );
        newItem->setIcon( 0, icon(name) );

        items.append( newItem );
    }

    if( item == nullptr ) {
        ui->treeWidget->addTopLevelItems( items );
    } else {
        item->addChildren( items );
    }
}
//----------------------------------------------------------------------------------------------------------

QString MainWindow::fullPathTo( QTreeWidgetItem* item ) {

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

    return path;
}
//----------------------------------------------------------------------------------------------------------

QPixmap MainWindow::icon( const QString& name ) {

    bool isFolder = name.indexOf('.') < 1;
    if( isFolder ) {
        return QPixmap(":/folder.png");
    }

    QString ext;
    int lastPoint = name.lastIndexOf(".");
    if( lastPoint >= 0 && lastPoint < name.length() ) {
        ext = name.right( name.length() - lastPoint );
    }

    QTemporaryFile* tmpFile = new QTemporaryFile( this );
    tmpFile->setFileTemplate( QString("XXXXXX%1").arg(ext) );
    tmpFile->open();
    tmpFile->close();

    const QSize imageSize( 64, 64 );

    QFileIconProvider* IconProvider = new QFileIconProvider();
    QPixmap image = IconProvider->icon(tmpFile->fileName()).pixmap(imageSize);
    delete IconProvider;

    if( image.isNull() ) {
        image = style()->standardIcon(QStyle::SP_FileIcon).pixmap(imageSize);
    }

    return image;
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::expantNode( QTreeWidgetItem* item, int ) {

    if( item->data(0, LoadRole).isValid() ) {
        return;
    }

    QString path = fullPathTo( item );
    QStringList names = whatsInFolder( path );
    createTreeItems( item, names );

    item->setData( 0, LoadRole, true );
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

