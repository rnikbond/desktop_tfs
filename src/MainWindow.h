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
    void expantNode( QTreeWidgetItem* item, int ) ;

    void createTreeItems( QTreeWidgetItem* item, const QStringList& childTextList );
    QString fullPathTo( QTreeWidgetItem* item );
private:

    QStringList whatsInFolder( const QString& folder );

protected:

    void showEvent( QShowEvent* event );
};
//----------------------------------------------------------------------------------------------------------

#endif // MAINWINDOW_H
