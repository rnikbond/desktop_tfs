//----------------------------------------
#include <QTime>
#include <QAction>
#include <QSysInfo>
//----------------------------------------
#include "ManagerTFS.h"
//----------------------------------------
#include "AzureConnectDialog.h"
#include "ui_AzureConnectDialog.h"
//----------------------------------------

AzureConnectDialog::AzureConnectDialog( QWidget* parent ) : QDialog(parent), ui(new Ui::AzureConnectDialog)
{
    ui->setupUi( this );

    ui->asureUrlEdit->setPlaceholderText("http://<hostname>:<port>/<collection>");

    QAction* viewPasswordAction = new QAction( QIcon(":/eye_open.png"), "");
    ui->passwordEdit->addAction( viewPasswordAction, QLineEdit::TrailingPosition );

    //connect( viewPasswordAction  , &QAction    ::triggered, this, &AzureConnectDialog::changePasswordVisibility );
    connect( ui->azureChechButton, &QPushButton::clicked  , this, &AzureConnectDialog::reconnect                );
}
//----------------------------------------------------------------------------------------------------------

AzureConnectDialog::~AzureConnectDialog() {

    delete ui;
}
//----------------------------------------------------------------------------------------------------------

void AzureConnectDialog::reconnect() {

    ui->infoEdit->clear();

    m_Config.azureUrl       = ui->asureUrlEdit->text();
    m_Config.creds.login    = ui->loginEdit   ->text();
    m_Config.creds.password = ui->passwordEdit->text();

    if( m_Config.azureUrl.isEmpty() ) {
        logError( "Не указан URL" );
        ui->asureUrlEdit->setFocus();
        return;
    }

    if( m_Config.creds.login.isEmpty() ) {
        logError( "Не указан Логин" );
        ui->loginEdit->setFocus();
        return;
    }

    if( m_Config.creds.password.isEmpty() ) {
        logError( "Не указан Пароль" );
        ui->passwordEdit->setFocus();
        return;
    }

    if( !m_Config.workspace.isEmpty() ) {
        if( !isExistsWorkspace(m_Config.workspace) ) {
            logWarning( tr("Рабочее пространство %1 не найдено").arg(m_Config.workspace) );
            m_Config.workspace.clear();
        }
    }

    if( m_Config.workspace.isEmpty() ) {
        logInfo( tr("Подключение к рабочему пространству...") );
        if( !initAzureWorkspace() ) {
            return;
        }
    }

    if( m_Config.workspace.isEmpty() ) {
        logError( tr("Не удалось подключиться к рабочему пространству") );
        return;
    }

    ManagerTFS tfs;
    tfs.init( &m_Config );
    tfs.checkConnection();

    if( tfs.m_error_code != 0 ) {
        logError( tfs.m_error_text );
        return;
    }

    logSuccess( "Подключение успешно установлено" );
}
//----------------------------------------------------------------------------------------------------------

bool AzureConnectDialog::initAzureWorkspace() {

    ManagerTFS tfs;
    tfs.init( &m_Config );
    tfs.workspaces();

    if( tfs.m_error_code != 0 ) {
        logError( tfs.m_error_text );
        return false;
    }

    QStringList workspacesAzure;
    if( !azureWorkspaces(workspacesAzure) ) {
        return false;
    }

    bool    isWorkspaceFound = false;
    QString machineHostName  = QSysInfo::machineHostName();

    foreach( const QString& workspace, workspacesAzure ) {
        if( QString::compare(workspace, machineHostName, Qt::CaseInsensitive) == 0 ) {
            isWorkspaceFound = true;
            break;
        }
    }

    if( !isWorkspaceFound ) {
        logInfo( tr("Создается рабочее пространство: %1").arg(machineHostName) );
        tfs.createWorkspace( machineHostName );
        if( tfs.m_error_code != 0 ) {
            logError( tfs.m_error_text );
            return false;
        }
    }

    m_Config.workspace = machineHostName;
    logSuccess( tr("Подключено к рабочему пространсву: %1").arg(m_Config.workspace) );

    return true;
}
//----------------------------------------------------------------------------------------------------------

bool AzureConnectDialog::isExistsWorkspace( const QString& workspace ) {

    QStringList workspacesAzure;
    if( !azureWorkspaces(workspacesAzure) ) {
        return false;
    }

    foreach( const QString& workspaceAzure, workspacesAzure ) {
        if( QString::compare(workspaceAzure, workspace, Qt::CaseInsensitive) == 0 ) {
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------------------------------------

bool AzureConnectDialog::azureWorkspaces( QStringList& workspacesAzure ) {

    workspacesAzure.clear();

    ManagerTFS tfs;
    tfs.init( &m_Config );
    tfs.workspaces();

    if( tfs.m_error_code != 0 ) {
        logError( tfs.m_error_text );
        return false;
    }

    foreach( const QString& workspaceInfo, tfs.m_result ) {
        QStringList infoParts = workspaceInfo.split("  ", Qt::SkipEmptyParts);
        if( infoParts.count() > 0 ) {
            workspacesAzure.append( infoParts[0] );
        }
    }

    return true;
}
//----------------------------------------------------------------------------------------------------------

void AzureConnectDialog::changePasswordVisibility() {

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

void AzureConnectDialog::reloadFromConfig() {

    ui->asureUrlEdit->setText( m_Config.azureUrl       );
    ui->loginEdit   ->setText( m_Config.creds.login    );
    ui->passwordEdit->setText( m_Config.creds.password );
}
//----------------------------------------------------------------------------------------------------------

void AzureConnectDialog::setConfig( const ConfigTFS& config ) {

    m_Config = config;
}
//----------------------------------------------------------------------------------------------------------

const ConfigTFS& AzureConnectDialog::config() const {

    return m_Config;
}
//----------------------------------------------------------------------------------------------------------

void AzureConnectDialog::showEvent( QShowEvent* event ) {

    reloadFromConfig();

    QDialog::showEvent( event );
}
//----------------------------------------------------------------------------------------------------------

void AzureConnectDialog::logInfo( const QString& text ) {
    logText( text, Qt::darkGray );
}
//----------------------------------------------------------------------------------------------------------

void AzureConnectDialog::logError( const QString& text ) {
    logText( text, Qt::darkRed );
}
//----------------------------------------------------------------------------------------------------------

void AzureConnectDialog::logWarning( const QString& text ) {
    logText( text, Qt::darkYellow );
}
//----------------------------------------------------------------------------------------------------------

void AzureConnectDialog::logSuccess( const QString& text ) {
    logText( text, Qt::darkGreen );
}
//----------------------------------------------------------------------------------------------------------

void AzureConnectDialog::logText( const QString& text, const QColor& color ) {

    QColor textColorSave = ui->infoEdit->textColor();
    ui->infoEdit->setTextColor( color );
    ui->infoEdit->append( QString("%1: %2").arg(QTime::currentTime().toString("hh:mm:ss.zzz"), text) );
    ui->infoEdit->setTextColor( textColorSave );
}
//----------------------------------------------------------------------------------------------------------

