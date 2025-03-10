//----------------------------------------
#include <QDir>
#include <QTime>
#include <QTimer>
#include <QScreen>
#include <QToolBar>
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
    ItemTypeRole ,                  ///< Тип элемента
    TFSPathRole  ,                  ///< Путь к элементу в tfs
    LocalPathRole,                  ///< Путь к элементу на диске
};
//----------------------------------------
enum FileTypes {
    File  , ///< Файл
    Folder, ///< Директория
};
//----------------------------------------
enum TfsFileFlags {
    EditFlag  ,
    CreateFlag,
    DeleteFlag,
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

    initChanges();

    connect( ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::expantNode );
}
//----------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow() {

    delete ui;
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::initChanges() {

    QTreeWidgetItem* includeItem = new QTreeWidgetItem;
    QTreeWidgetItem* excludeItem = new QTreeWidgetItem;

    includeItem->setText( 0, tr("Включенные изменения") );
    excludeItem->setText( 0, tr("Исключенные изменения") );

    ui->statusTree->addTopLevelItem( includeItem );
    ui->statusTree->addTopLevelItem( excludeItem );
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::reloadStatus() {

    statusDirs.clear();

    for( int rootRow = 0; rootRow < ui->statusTree->topLevelItemCount(); rootRow++ ) {
        QTreeWidgetItem* rootItem = ui->statusTree->topLevelItem( rootRow );
        for( int childRow = 0; childRow < ui->statusTree->topLevelItemCount(); childRow++ ) {
            QTreeWidgetItem* childItem = rootItem->child( childRow );
            delete childItem;
        }
    }

    foreach( const QString& dirLocal, config.Azure.workfoldes ) {

        ManagerTFS tfs;
        tfs.setConfiguration( config );
        tfs.status( dirLocal );
        appendOutput( tfs );

        if( tfs.m_error_code != 0 ) {
            continue;
        }

        // "Имя файла                                   Изменение  Локальный путь"
        // "------------------------------------------- ---------- ------------------------"
        // "$/KOTMI/mainline/develop/Workstation/ScdRetro/src"
        // "$/KOTMI/mainline/develop/Workstation/ScdRetro/src"
        // "modules.cpp                                 изменить   E:\\mainline\\develop\\Workstation\\ScdRetro\\src\\modules.cpp"
        // "$/KOTMI/mainline/srv/development/DBdll"
        // "DBdll.vcxproj                               изменить   E:\\mainline\\srv\\development\\DBdll\\DBdll.vcxproj"
        // "DBdll_x64.vcxproj                           изменить   E:\\mainline\\srv\\development\\DBdll\\DBdll_x64.vcxproj"

        foreach( QString item, tfs.m_result ) {
            if( item.contains("build") || item.contains(".vs") || item.contains("добавление") ) {
                continue;
            }

            if( !item.contains(dirLocal) ) {
                continue;
            }

            QStringList parts;
            TfsFileFlags tfsFlag = EditFlag;

            if( item.contains("добавление", Qt::CaseInsensitive) ) {
                parts = item.split("добавление");
                tfsFlag = CreateFlag;
            } else if( item.contains("изменить", Qt::CaseInsensitive) ) {
                parts = item.split("изменить");
                tfsFlag = EditFlag;
            }

            if( parts.count() != 2 ) {
                continue;
            }

            parts[0] = parts[0].trimmed();
            parts[1] = parts[1].trimmed();
            parts[0] = QDir::toNativeSeparators(parts[0]);
            addEdit( parts[0], parts[1], tfsFlag );
        }

        break;
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::addEdit( const QString& file, const QString& path, int tfsFlag ) {

    QString dir = QString(path).remove(file);
    dir = QDir::toNativeSeparators(dir);
    if( dir.endsWith("/") || dir.endsWith("\\") ) {
        dir = dir.remove( dir.length() - 1, 1 );
    }

    QTreeWidgetItem* dirItem = statusDirs[dir];

    if( dirItem == nullptr ) {

        dirItem = new QTreeWidgetItem;
        dirItem->setText( 0, dir );
        dirItem->setIcon( 0, QPixmap(":/folder.png") );

        QTreeWidgetItem* includeItem = ui->statusTree->topLevelItem( 0 );
        includeItem->addChild( dirItem );
        statusDirs[dir] = dirItem;
    }

    QString display = file;
    switch( tfsFlag ) {
        case EditFlag  : { display += tr(" [изменение]" ); break; }
        case CreateFlag: { display += tr(" [создание]"  ); break; }
        case DeleteFlag: { display += tr(" [удаление]"  ); break; }
        default        : { display += tr(" [неизвестно]"); break; }
    }

    QTreeWidgetItem* fileItem = new QTreeWidgetItem;
    fileItem->setData( 0, LocalPathRole, path );
    fileItem->setText( 0, display );

    dirItem->addChild( fileItem );
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::reloadTree() {

    ui->treeWidget->blockSignals( true );
    ui->treeWidget->clear();
    ui->treeWidget->blockSignals( false );

    ManagerTFS tfs;
    tfs.setConfiguration( config );
    tfs.entriesDir( "$/" );
    appendOutput( tfs );

    if( tfs.m_error_code != 0 ) {
        QMessageBox::critical( this, tr("Ошибка"), tfs.m_error_text );
        return;
    }

    ui->treeWidget->blockSignals( true );
    createTreeItems( nullptr, tfs.m_result );
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

    ManagerTFS tfs;
    tfs.setConfiguration( config );
    tfs.entriesDir( path );
    appendOutput( tfs );

    if( tfs.m_error_code != 0 ) {
        QMessageBox::critical( this, tr("Ошибка"), tfs.m_error_text );
        return;
    }

    createTreeItems( item, tfs.m_result );

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

void MainWindow::reloadWorkfolds() {

    config.Azure.workfoldes.clear();

    ManagerTFS tfs;
    tfs.setConfiguration( config );
    tfs.workfolds();
    appendOutput( tfs );

    if( tfs.m_error_code != 0 ) {
        return;
    }

    for( int i = 2; i < tfs.m_result.count(); i++ ) {

        // [0]: $/KOTMI/mainline
        // [1]: E:\\mainline
        QStringList bundles = tfs.m_result[i].split(": ", Qt::SkipEmptyParts);
        if( bundles.count() != 2 ) {
            continue;
        }

        if( !bundles[0].contains("$") ) {
            continue;
        }

        config.Azure.workfoldes[bundles[0]] = QDir::toNativeSeparators(bundles[1]);
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::cloneCurrent() {

    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if( item == nullptr ) {
        return;
    }

    ManagerTFS tfs;
    tfs.setConfiguration( config );
    tfs.cloneDir( item->data(0, TFSPathRole).toString() );
    appendOutput( tfs );
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::changeSettings() {

    SettingsDialog dialog;
    dialog.setConfiguration( config );
    if( dialog.exec() != QDialog::Accepted ) {
        return;
    }

    config = dialog.configuration();
    saveConfig();
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

void MainWindow::appendOutput( const ManagerTFS& tfs ) {

    ui->logEdit->append("");
    ui->logEdit->append( QTime::currentTime().toString("hh:mm:ss.zzz") );

    if( tfs.m_error_code != 0 ) {
        QColor textColorSave = ui->logEdit->textColor();

        ui->logEdit->setTextColor( Qt::red );

        ui->logEdit->append( QString("Ошибка %1").arg(tfs.m_error_code));
        ui->logEdit->append( tfs.m_error_text );

        ui->logEdit->setTextColor( textColorSave );
        return;
    }

    if( !tfs.m_result.isEmpty() ) {
        ui->logEdit->append( tfs.m_result.join('\n') );
    }
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::init() {

    readConfig();

    if( !config.Azure.isIncomplete() ) {
        return;
    }

    reloadWorkfolds();
    reloadTree();
}
//----------------------------------------------------------------------------------------------------------

void MainWindow::createToolBar() {

    QAction* reloadAction   = new QAction( QIcon(":/refresh.png"    ), tr("Обновить" ) );
    QAction* cloneAction    = new QAction( QIcon(":/save.png"       ), tr("Получить" ) );
    QAction* statusAction   = new QAction( QIcon(":/eye_open.png"   ), tr("Настройки") );
    QAction* settingsAction = new QAction( QIcon(":/customizing.png"), tr("Настройки") );

    QToolBar* toolBar = new QToolBar;
    toolBar->addAction( reloadAction );
    toolBar->addAction( cloneAction  );
    toolBar->addSeparator();
    toolBar->addAction( statusAction );
    toolBar->addSeparator();
    toolBar->addAction( settingsAction );

    connect( reloadAction  , &QAction::triggered, this, &MainWindow::reloadTree     );
    connect( cloneAction   , &QAction::triggered, this, &MainWindow::cloneCurrent   );
    connect( statusAction  , &QAction::triggered, this, &MainWindow::reloadStatus   );
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

