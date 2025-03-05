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
class QTreeWidgetItem;
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

    void checkTfsConnection();
    void requestTree();
    void expantNode( QTreeWidgetItem* item, int ) ;

protected:

    void showEvent( QShowEvent* event );
};
//----------------------------------------------------------------------------------------------------------

#endif // MAINWINDOW_H
