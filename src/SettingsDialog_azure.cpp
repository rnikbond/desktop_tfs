//----------------------------------------
#include <QTime>
#include <QDebug>
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

void SettingsDialog::changeTfPath() {

    QString dir = config.Azure.tfPath.isEmpty() ? QDir::homePath() : config.Azure.tfPath;
    QString path = QFileDialog::getOpenFileName( this, tr("Программа TF"), dir, BIN_FILTER );
    if( path.isEmpty() ) {
        return;
    }

    ui->tfPathEdit->setText( QDir::fromNativeSeparators(path) );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::reconnectAzure() {

    ui->infoEdit->clear();

    savePageConfig();

    if( config.Azure.tfPath.isEmpty() ) {
        logAzureError( "Не указан путь к программе TF" );
        ui->tfPathEdit->setFocus();
        return;
    }

    if( !QFile(config.Azure.tfPath).exists() ) {
        logAzureError( "Не найдена программа TF" );
        ui->tfPathEdit->setFocus();
        return;
    }

    if( config.Azure.url.isEmpty() ) {
        logAzureError( "Не указан URL" );
        ui->asureUrlEdit->setFocus();
        return;
    }

    if( config.Azure.login.isEmpty() ) {
        logAzureError( "Не указан Логин" );
        ui->loginEdit->setFocus();
        return;
    }

    if( config.Azure.password.isEmpty() ) {
        logAzureError( "Не указан Пароль" );
        ui->passwordEdit->setFocus();
        return;
    }

    if( !config.Azure.workspace.isEmpty() ) {
        if( !isExistsAzureWorkspace(config.Azure.workspace) ) {
            logAzureWarning( tr("Рабочее пространство %1 не найдено").arg(config.Azure.workspace) );
            config.Azure.workspace.clear();
        }
    }

    if( config.Azure.workspace.isEmpty() ) {
        logAzureInfo( tr("Подключение к рабочему пространству...") );
        if( !initAzureWorkspace() ) {
            return;
        }
    }

    if( config.Azure.workspace.isEmpty() ) {
        logAzureError( tr("Не удалось подключиться к рабочему пространству") );
        return;
    }

    ManagerTFS tfs;
    tfs.setConfiguration( config );
    tfs.checkConnection();

    if( tfs.m_error_code != 0 ) {
        logAzureError( tfs.m_error_text );
        return;
    }

    logAzureSuccess( "Подключение успешно установлено" );
}
//----------------------------------------------------------------------------------------------------------

bool SettingsDialog::isExistsAzureWorkspace( const QString& workspace ) {

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

bool SettingsDialog::azureWorkspaces( QStringList& workspacesAzure ) {

    workspacesAzure.clear();

    ManagerTFS tfs;
    tfs.setConfiguration( config );
    tfs.workspaces();

    if( tfs.m_error_code != 0 ) {
        logAzureError( tfs.m_error_text );
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

bool SettingsDialog::initAzureWorkspace() {

    ManagerTFS tfs;
    tfs.setConfiguration( config );
    tfs.workspaces();

    if( tfs.m_error_code != 0 ) {
        logAzureError( tfs.m_error_text );
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
        logAzureInfo( tr("Создается рабочее пространство: %1").arg(machineHostName) );
        tfs.createWorkspace( machineHostName );
        if( tfs.m_error_code != 0 ) {
            logAzureError( tfs.m_error_text );
            return false;
        }
    }

    config.Azure.workspace = machineHostName;
    logAzureSuccess( tr("Подключено к рабочему пространсву: %1").arg(config.Azure.workspace) );

    return true;
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::savePageConfig() {

    config.Azure.tfPath   = ui->tfPathEdit  ->text();
    config.Azure.url      = ui->asureUrlEdit->text();
    config.Azure.login    = ui->loginEdit   ->text();
    config.Azure.password = ui->passwordEdit->text();
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::restorePageConfig() {

    ui->tfPathEdit  ->setText( config.Azure.tfPath   );
    ui->asureUrlEdit->setText( config.Azure.url      );
    ui->loginEdit   ->setText( config.Azure.login    );
    ui->passwordEdit->setText( config.Azure.password );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::initPageConfig() {

    ui->tfPathEdit->setPlaceholderText( EXAMPLE_TF_BIN );
    ui->infoEdit ->setPlaceholderText( tr("Здесь будет отображаться информация о подключении...") );

    ui->azureChechButton->setIcon( QIcon(":/settings_update.png") );

    connect( ui->tfPathButton    , &QToolButton::clicked, this, &SettingsDialog::changeTfPath   );
    connect( ui->azureChechButton, &QPushButton::clicked, this, &SettingsDialog::reconnectAzure );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::logAzureInfo( const QString& text ) {
    logAzureText( text, Qt::darkGray );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::logAzureError( const QString& text ) {
    logAzureText( text, Qt::darkRed );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::logAzureWarning( const QString& text ) {
    logAzureText( text, Qt::darkYellow );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::logAzureSuccess( const QString& text ) {
    logAzureText( text, Qt::darkGreen );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::logAzureText( const QString& text, const QColor& color ) {

    QColor textColorSave = ui->infoEdit->textColor();
    ui->infoEdit->setTextColor( color );
    ui->infoEdit->append( QString("%1: %2").arg(QTime::currentTime().toString("hh:mm:ss.zzz"), text) );
    ui->infoEdit->setTextColor( textColorSave );
}
//----------------------------------------------------------------------------------------------------------

