//----------------------------------------
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
//----------------------------------------
#include <QDialog>
//----------------------------------------
#include "conf.h"
//----------------------------------------
namespace Ui { class SettingsDialog; }
//----------------------------------------
class QListWidgetItem;
//----------------------------------------

class SettingsDialog : public QDialog {

    Q_OBJECT

    Ui::SettingsDialog* ui;

public:

    explicit SettingsDialog( QWidget* parent = nullptr );
    ~SettingsDialog();

    void setConfiguration( const Configuration& cfg );
    const Configuration& configuration() const;

private: // Config

    Configuration config;

    void save   ();
    void restore();

private: // Pages

    void initPages();

    void selectPage( QListWidgetItem* itemPage, QListWidgetItem* );
    QListWidgetItem* findPage( int page ) const;

private: // Page Azure

    void initPageConfig   ();
    void savePageConfig   ();
    void restorePageConfig();

    void changeTfPath();

    bool initAzureWorkspace();
    bool isExistsAzureWorkspace( const QString& workspace );
    bool azureWorkspaces( QStringList& workspacesAzure );
    void reconnectAzure();

    void logAzureInfo   ( const QString& text );
    void logAzureError  ( const QString& text );
    void logAzureWarning( const QString& text );
    void logAzureSuccess( const QString& text );
    void logAzureText   ( const QString& text, const QColor& color );
};
//----------------------------------------------------------------------------------------------------------

#endif // SETTINGSDIALOG_H
