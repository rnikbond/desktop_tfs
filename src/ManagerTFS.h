//----------------------------------------
#ifndef MANAGERTFS_H
#define MANAGERTFS_H
//----------------------------------------
#include <QObject>
//----------------------------------------
#include "conf.h"
//----------------------------------------

class ManagerTFS : public QObject {

    Q_OBJECT

    Configuration config;

public:

    QStringList m_result;

    int     m_error_code;
    QString m_error_text;

public:

    ManagerTFS( QObject* parent = nullptr );

    void setConfiguration( const Configuration& cfg  );
    void clear();

public:

    void checkConnection();
    void entriesDir( const QString& dir );
    void cloneDir  ( const QString& dir );
    void workspaces();
    void workfolds();
    void createWorkspace( const QString& name );
    void status( const QString& dir );

private:

    void execute( const QStringList& args );
};
//----------------------------------------

#endif // MANAGERTFS_H
