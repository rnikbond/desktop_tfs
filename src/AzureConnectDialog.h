//----------------------------------------
#ifndef AZURECONNECTDIALOG_H
#define AZURECONNECTDIALOG_H
//----------------------------------------
#include <QDialog>
//----------------------------------------
#include "conf.h"
//----------------------------------------
namespace Ui { class AzureConnectDialog; }
//----------------------------------------

class AzureConnectDialog : public QDialog {

    Q_OBJECT

    Ui::AzureConnectDialog *ui;

    ConfigTFS m_Config;

public:

    explicit AzureConnectDialog( QWidget* parent = nullptr );
    ~AzureConnectDialog();

    void setConfig( const ConfigTFS& config );
    const ConfigTFS& config() const;

private:

    void reloadFromConfig();
    void reconnect();

    void changePasswordVisibility();

    bool initAzureWorkspace ();
    bool isExistsWorkspace( const QString& workspace );
    bool azureWorkspaces( QStringList& workspacesAzure );

    void logInfo   ( const QString& text );
    void logError  ( const QString& text );
    void logWarning( const QString& text );
    void logSuccess( const QString& text );
    void logText   ( const QString& text, const QColor& color );

protected:

    void showEvent( QShowEvent* event );
};
//----------------------------------------------------------------------------------------------------------

#endif // AZURECONNECTDIALOG_H
