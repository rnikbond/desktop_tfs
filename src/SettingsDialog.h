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

    void setConf( ConfigTFS* conf );

private: // Config

    ConfigTFS* m_Config;

    void save   ();
    void restore();

private: // Pages

    void initPages();

    void selectPage( QListWidgetItem* itemPage, QListWidgetItem* );
    QListWidgetItem* findPage( int page ) const;

private: // Page config

    void initPageConfig   ();
    void savePageConfig   ();
    void restorePageConfig();
    void updatePageConfig ();

    void selectBin();
    void showAzureSettings();
};
//----------------------------------------------------------------------------------------------------------

#endif // SETTINGSDIALOG_H
