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

    void reloadWorkfolds();
    void cloneCurrent();
    void reloadStatus();
    void initChanges();

    void addEdit( const QString& file, const QString& path, int tfsFlag );

    QMap<QString, QTreeWidgetItem*> statusDirs;

private: // Config

    Configuration config;

    void readConfig();
    void saveConfig();

    void changeSettings();

private: // Tree

    void reloadTree();

    void expantNode( QTreeWidgetItem* item, int );
    void createTreeItems( QTreeWidgetItem* item, const QStringList& childTextList );

private: // Tools

    QPixmap icon( const QString& name );
    int fileType( const QString& name ) const;

private: // UI

    Ui::MainWindow* ui;

    void createToolBar();
    void appendOutput ( const ManagerTFS& tfs );

protected:

    void showEvent( QShowEvent* event );
};
//----------------------------------------------------------------------------------------------------------

#endif // MAINWINDOW_H
