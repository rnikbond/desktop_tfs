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

class SettingsDialog : public QDialog {

    Q_OBJECT

    ConfigTFS* m_Config;

public:

    explicit SettingsDialog( QWidget* parent = nullptr );
    ~SettingsDialog();

    void setConf( ConfigTFS* conf );

private:

    Ui::SettingsDialog* ui;

private:

    void selectBin();

    void save   ();
    void restore();

    void changePasswordVisibility();
};
//----------------------------------------------------------------------------------------------------------

#endif // SETTINGSDIALOG_H
