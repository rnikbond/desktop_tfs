//----------------------------------------
#include <QDebug>
#include <QAction>
#include <QFileDialog>
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

SettingsDialog::SettingsDialog( QWidget* parent ) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle( tr("Настройка") );

    m_Config = nullptr;

    ui->binPathEdit   ->setPlaceholderText( EXAMPLE_TF_BIN );
    ui->collectionEdit->setPlaceholderText("http://<hostname>:<port>/<collection>");

    QAction* viewPasswordAction = new QAction( QIcon(":/eye_open.png"), "");
    ui->passwordEdit->addAction( viewPasswordAction, QLineEdit::TrailingPosition );

    changePasswordVisibility();

    connect( ui->buttonBox     , &QDialogButtonBox::accepted , this, &SettingsDialog::save                     );
    connect( ui->binPathButton , &QToolButton     ::clicked  , this, &SettingsDialog::selectBin                );
    connect( viewPasswordAction, &QAction         ::triggered, this, &SettingsDialog::changePasswordVisibility );
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

    // Путь к программе TF
    ui->binPathEdit->setText( m_Config->binPath );

    // Asure DevOps Server
    ui->collectionEdit->setText( m_Config->collection );

    // Аутентификация
    ui->loginEdit   ->setText( m_Config->creds.login    );
    ui->passwordEdit->setText( m_Config->creds.password );
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::setConf( ConfigTFS* conf ) {

    m_Config = conf;
    restore();
}
//----------------------------------------------------------------------------------------------------------
