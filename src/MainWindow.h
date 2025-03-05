//----------------------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//----------------------------------------
#include <QMainWindow>
//----------------------------------------
#include "ManagerTFS.h"
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
    ManagerTFS* m_TFS;

    void readConfig();
    void saveConfig();

    void checkTfsConnection();

    void selectItem( QTreeWidgetItem* item, QTreeWidgetItem*  );
    void expantNode( QTreeWidgetItem* item, int );

    void createTreeItems( QTreeWidgetItem* item, const QStringList& childTextList );
    QPixmap icon( const QString& name );
    int fileType( const QString& name ) const;

protected:

    void showEvent( QShowEvent* event );
};
//----------------------------------------------------------------------------------------------------------

#endif // MAINWINDOW_H
