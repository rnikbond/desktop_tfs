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

    ConfigTFS* m_Config;

public:

    explicit SettingsDialog( QWidget* parent = nullptr );
    ~SettingsDialog();

    void setConf( ConfigTFS* conf );

private:

    void selectBin();

    void save   ();
    void restore();

    void changePasswordVisibility();

private: // UI

    Ui::SettingsDialog* ui;

    void selectPage( QListWidgetItem* itemPage, QListWidgetItem* );
    QListWidgetItem* findPage( int page ) const;

    void initPages();
    void initPageTFS();
    void initPageWorkspace();
};
//----------------------------------------------------------------------------------------------------------

#endif // SETTINGSDIALOG_H
