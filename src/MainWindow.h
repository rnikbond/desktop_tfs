//----------------------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//----------------------------------------
#include <QMainWindow>
//----------------------------------------
#include "conf.h"
//----------------------------------------
namespace Ui { class MainWindow; }
//----------------------------------------

class MainWindow : public QMainWindow {

    Q_OBJECT

public:

    MainWindow( QWidget* parent = nullptr );
    ~MainWindow();

private:

    Ui::MainWindow* ui;

private:

    ConfigTFS config;

    void readConfig();
    void saveConfig();

protected:

    void showEvent( QShowEvent* event );
};
//----------------------------------------------------------------------------------------------------------

#endif // MAINWINDOW_H
