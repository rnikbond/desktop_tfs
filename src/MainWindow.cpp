//----------------------------------------
#include <QTimer>
#include <QScreen>
#include <QToolBar>
#include <QProcess>
#include <QSettings>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QGuiApplication>
#include <QFileIconProvider>
//----------------------------------------
#include "SettingsDialog.h"
//----------------------------------------
#include "MainWindow.h"
#include "ui_MainWindow.h"
//----------------------------------------
enum CustomRoles {
    LoadRole    = Qt::UserRole + 1, ///< Признак закгрузки содержимого
    ItemTypeRole,                   ///< Тип элемента
    TFSPathRole ,                   ///< Путь к элементу в tfs
};
//----------------------------------------
enum FileTypes {
    File  , ///< Файл
    Folder, ///< Директория
};
//----------------------------------------

MainWindow::MainWindow( QWidget* parent ) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle( tr("Клиент TFS") );
    createToolBar();

    m_TFS = new ManagerTFS( this );
    m_TFS->init( &config );

    ui->treeWidget->setColumnCount( 1 );
    ui->treeWidget->header()->hide();

    connect( ui->treeWidget, &QTreeWidget::itemDoubleClicked , this, &MainWindow::expantNode );
    connect( ui->treeWidget, &QTreeWidget::currentItemChanged, this, &MainWindow::selectItem );
}
//----------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow() {

    delete ui;
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::reloadTree() {

    ui->treeWidget->blockSignals( true );
    ui->treeWidget->clear();
    ui->folderList->clear();
    ui->treeWidget->blockSignals( false );

    m_TFS->entriesDir( "$/" );
    if( m_TFS->m_error_code != 0 ) {
        QMessageBox::critical( this, tr("Ошибка"), m_TFS->m_error_text );
        return;
    }

    ui->treeWidget->blockSignals( true );
    createTreeItems( nullptr, m_TFS->m_result );
    ui->treeWidget->blockSignals( false );
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::selectItem( QTreeWidgetItem* item, QTreeWidgetItem* ) {

    if( item->data(0, ItemTypeRole).toInt() != Folder ) {
        return;
    }

    if( !item->data(0, LoadRole).isValid() ) {
        expantNode( item, 0 );
    }

    ui->folderList->clear();

    for( int i = 0; i < item->childCount(); i++ ) {

        QTreeWidgetItem* childItem = item->child( i );

        QListWidgetItem* listItem = new QListWidgetItem( ui->folderList );
        listItem->setData( Qt::DisplayRole   , childItem->data(0, Qt::DisplayRole   ) );
        listItem->setData( Qt::DecorationRole, childItem->data(0, Qt::DecorationRole) );
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::expantNode( QTreeWidgetItem* item, int ) {

    if( item->data(0, ItemTypeRole).toInt() != Folder ) {
        return;
    }

    if( item->data(0, LoadRole).isValid() ) {
        return;
    }

    QString path = item->data(0, TFSPathRole).toString();

    m_TFS->entriesDir( path );
    if( m_TFS->m_error_code != 0 ) {
        QMessageBox::critical( this, tr("Ошибка"), m_TFS->m_error_text );
        return;
    }

    createTreeItems( item, m_TFS->m_result );

    item->setData( 0, LoadRole, true );
    ui->treeWidget->expandItem( item );
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::createTreeItems( QTreeWidgetItem* item, const QStringList& names ) {

    QString parentPath = "$";
    if( item != nullptr ) {
        parentPath = item->data(0, TFSPathRole).toString();
    }

    QList<QTreeWidgetItem*> items;

    for( const QString& name : names ) {

        QString path = parentPath + "/" + name;

        QTreeWidgetItem* newItem = new QTreeWidgetItem;
        newItem->setText( 0, name       );
        newItem->setIcon( 0, icon(name) );
        newItem->setData( 0, ItemTypeRole, fileType(name) );
        newItem->setData( 0, TFSPathRole, path );
        newItem->setData( 0, Qt::ToolTipRole, path );

        items.append( newItem );
    }

    if( item == nullptr ) {
        ui->treeWidget->addTopLevelItems( items );
    } else {
        item->addChildren( items );
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::cloneCurrent() {


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

    reloadTree();
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

QPixmap MainWindow::icon( const QString& name ) {

    if( fileType(name) == Folder ) {
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

int MainWindow::fileType( const QString& name ) const {

    return (name.indexOf('.') < 1) ? Folder : File;
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::init() {

    readConfig();
    reloadTree();
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::createToolBar() {

    QAction* reloadAction = new QAction( QIcon(":/update.png"), tr("Обновить") );
    QAction* cloneAction  = new QAction( QIcon(":/save.png"  ), tr("Получить") );

    QToolBar* toolBar = new QToolBar;
    toolBar->addAction( reloadAction );
    toolBar->addAction( cloneAction  );

    connect( reloadAction, &QAction::triggered, this, &MainWindow::reloadTree   );
    connect( cloneAction , &QAction::triggered, this, &MainWindow::cloneCurrent );

    addToolBar( toolBar );
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::showEvent( QShowEvent* event ) {

    QRect rect  = QGuiApplication::primaryScreen()->geometry();
    QPoint center = rect.center();
    center.setX( center.x() - (width ()/2) );
    center.setY( center.y() - (height()/2) );
    move(center);

    QMainWindow::showEvent( event );
    QTimer::singleShot( 0, this, &MainWindow::init );
}
//----------------------------------------------------------------------------------------------------------

