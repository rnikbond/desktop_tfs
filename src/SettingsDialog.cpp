//----------------------------------------
#include <QDebug>
#include <QAction>
#include <QFileDialog>
//----------------------------------------
#include "ManagerTFS.h"
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
enum Pages {
    PageTFS      , ///< Конфигурация TFS
    PageWorkspace, ///< Рабочая область
};
//----------------------------------------
enum {
    PageRole = Qt::UserRole + 1,
};
//----------------------------------------

SettingsDialog::SettingsDialog( QWidget* parent ) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    m_Config = nullptr;

    ui->setupUi(this);
    ui->splitter->setSizes( {150, 500} );
    setWindowTitle( tr("Настройка") );

    initPages();
    ui->pagesList->setCurrentItem( findPage(PageTFS) );

    connect( ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::save  );
}
//----------------------------------------------------------------------------------------------------------

SettingsDialog::~SettingsDialog() {

    delete ui;
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

void SettingsDialog::changePasswordVisibility() {

    QAction* viewPasswordAction = ui->passwordEdit->actions().last();

    if( ui->passwordEdit->echoMode() == QLineEdit::PasswordEchoOnEdit) {
        viewPasswordAction->setIcon( QIcon(":/eye_close.png") );
        viewPasswordAction->setToolTip( tr("Скрыть пароль") );
        ui->passwordEdit->setEchoMode( QLineEdit::Normal );
    } else {
        viewPasswordAction->setIcon( QIcon(":/eye_open.png") );
        viewPasswordAction->setToolTip( tr("Показать пароль") );
        ui->passwordEdit->setEchoMode( QLineEdit::PasswordEchoOnEdit );
    }
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

    { // Страница "Конфигурация TFS"

        // Путь к программе TF
        ui->binPathEdit->setText( m_Config->binPath );

        // Asure DevOps Server
        ui->collectionEdit->setText( m_Config->collection );

        // Аутентификация
        ui->loginEdit   ->setText( m_Config->creds.login    );
        ui->passwordEdit->setText( m_Config->creds.password );
    }

    { // Страница "Рабочая область"

        ManagerTFS tfs;
        tfs.init( m_Config );

        tfs.workspaces();

        for( int i = 0; i < 3; i++ ) {
            tfs.m_result.removeFirst();
        }

        foreach( QString workspaceInfo, tfs.m_result ) {

            QString workspace = workspaceInfo.split("  ", Qt::SkipEmptyParts)[0];

            QListWidgetItem* item = new QListWidgetItem( ui->workspacesList );
            item->setText   ( workspace     );
            item->setToolTip( workspaceInfo );
        }
    }
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::setConf( ConfigTFS* conf ) {

    m_Config = conf;
    restore();
}
//----------------------------------------------------------------------------------------------------------

QListWidgetItem* SettingsDialog::findPage( int page ) const {

    for( int row = 0; row < ui->pagesList->count(); row++ ) {
        QListWidgetItem* itemPage = ui->pagesList->item( row );
        if( itemPage->data(PageRole).toInt() == page ) {
            return itemPage;
        }
    }

    return nullptr;
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::selectPage( QListWidgetItem* itemPage, QListWidgetItem* ) {

    if( itemPage == nullptr ) {
        return;
    }

    int page = itemPage->data(PageRole).toInt();
    switch( page) {
        case PageTFS      : ui->pagesStack->setCurrentWidget( ui->pageConfigTFS ); break;
        case PageWorkspace: ui->pagesStack->setCurrentWidget( ui->pageWorkspace ); break;
        default: {
            qDebug() << "SettingsDialog::selectPage()" << QString("unknown page role: %1").arg(page);
            break;
        }
    }
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::initPages() {

    QListWidgetItem* itemPageTFS       = new QListWidgetItem;
    QListWidgetItem* itemPageWorkspace = new QListWidgetItem;

    itemPageTFS      ->setText( tr("Конфигурация TFS") );
    itemPageWorkspace->setText( tr("Рабочая область" ) );

    itemPageTFS      ->setData( PageRole, PageTFS       );
    itemPageWorkspace->setData( PageRole, PageWorkspace );

    ui->pagesList->addItem( itemPageTFS       );
    ui->pagesList->addItem( itemPageWorkspace );

    initPageTFS      ();
    initPageWorkspace();

    connect( ui->pagesList, &QListWidget::currentItemChanged, this, &SettingsDialog::selectPage );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::initPageTFS() {

    ui->binPathEdit   ->setPlaceholderText( EXAMPLE_TF_BIN );
    ui->collectionEdit->setPlaceholderText("http://<hostname>:<port>/<collection>");

    QAction* viewPasswordAction = new QAction( QIcon(":/eye_open.png"), "");
    ui->passwordEdit->addAction( viewPasswordAction, QLineEdit::TrailingPosition );

    changePasswordVisibility();

    connect( ui->binPathButton , &QToolButton     ::clicked  , this, &SettingsDialog::selectBin                );
    connect( viewPasswordAction, &QAction         ::triggered, this, &SettingsDialog::changePasswordVisibility );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::initPageWorkspace() {

}
//----------------------------------------------------------------------------------------------------------
