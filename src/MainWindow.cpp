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

    ui->splitter->setSizes( {300, 100} );

    ui->treeWidget->setColumnCount( 1 );
    ui->treeWidget->header()->hide();

    m_TFS = new ManagerTFS( this );
    m_TFS->init( &config );

    connect( ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::expantNode );
}
//----------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow() {

    delete ui;
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::reloadTree() {

    ui->treeWidget->blockSignals( true );
    ui->logEdit   ->clear();
    ui->treeWidget->clear();
    ui->treeWidget->blockSignals( false );

    m_TFS->entriesDir( "$/" );
    appendOutput();

    if( m_TFS->m_error_code != 0 ) {
        ui->logEdit->append( QString("%1: %2").arg(m_TFS->m_error_code).arg(m_TFS->m_error_text ) );
        QMessageBox::critical( this, tr("Ошибка"), m_TFS->m_error_text );
        return;
    }

    ui->treeWidget->blockSignals( true );
    createTreeItems( nullptr, m_TFS->m_result );
    ui->treeWidget->blockSignals( false );
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
    appendOutput();

    if( m_TFS->m_error_code != 0 ) {
        QMessageBox::critical( this, tr("Ошибка"), m_TFS->m_error_text );
        return;
    }

    createTreeItems( item, m_TFS->m_result );

    item->setData( 0, LoadRole, true );
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

    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if( item == nullptr ) {
        return;
    }

    m_TFS->cloneDir( item->data(0, TFSPathRole).toString() );
    appendOutput();
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::changeSettings() {

    ConfigTFS confTmp = config;

    SettingsDialog dialog;
    dialog.setConf( &confTmp );
    if( dialog.exec() != QDialog::Accepted ) {
        return;
    }

    config = confTmp;
    saveConfig();
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
        changeSettings();
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::saveConfig() {

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

void MainWindow::appendOutput() {

    if( m_TFS->m_error_code != 0 ) {
        QColor textColorSave = ui->logEdit->textColor();

        ui->logEdit->setTextColor( Qt::red );

        ui->logEdit->append( QString("Ошибка %1").arg(m_TFS->m_error_code));
        ui->logEdit->append( m_TFS->m_error_text );

        ui->logEdit->setTextColor( textColorSave );
        return;
    }

    if( !m_TFS->m_result.isEmpty() ) {
        ui->logEdit->append( m_TFS->m_result.join('\n') );
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::init() {

    readConfig();
    reloadTree();
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::createToolBar() {

    QAction* reloadAction   = new QAction( QIcon(":/update.png"     ), tr("Обновить" ) );
    QAction* cloneAction    = new QAction( QIcon(":/save.png"       ), tr("Получить" ) );
    QAction* settingsAction = new QAction( QIcon(":/customizing.png"), tr("Настройки") );

    QToolBar* toolBar = new QToolBar;
    toolBar->addAction( reloadAction );
    toolBar->addAction( cloneAction  );
    toolBar->addSeparator();
    toolBar->addAction( settingsAction );

    connect( reloadAction  , &QAction::triggered, this, &MainWindow::reloadTree     );
    connect( cloneAction   , &QAction::triggered, this, &MainWindow::cloneCurrent   );
    connect( settingsAction, &QAction::triggered, this, &MainWindow::changeSettings );

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

