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

private: // init

    void init();

private: // TFS

    ManagerTFS* m_TFS;

    void cloneCurrent();

private: // Config

    ConfigTFS config;

    void readConfig();
    void saveConfig();

private: // Tree

    void reloadTree();

    void selectItem( QTreeWidgetItem* item, QTreeWidgetItem*  );
    void expantNode( QTreeWidgetItem* item, int );
    void createTreeItems( QTreeWidgetItem* item, const QStringList& childTextList );

private: // Tools

    QPixmap icon( const QString& name );
    int fileType( const QString& name ) const;

private: // UI

    Ui::MainWindow* ui;

    void createToolBar();

protected:

    void showEvent( QShowEvent* event );
};
//----------------------------------------------------------------------------------------------------------

#endif // MAINWINDOW_H
