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

    const ConfigTFS* config;

public:

    QStringList m_result;

    int     m_error_code;
    QString m_error_text;

public:

    ManagerTFS( QObject* parent = nullptr );

    void init( const ConfigTFS* cfg );
    void clear();

public:

    void checkConnection();
    void entriesDir( const QString& dir );
    void cloneDir  ( const QString& dir );
    void workspaces();
    void createWorkspace( const QString& name );

private:

    void execute( const QStringList& args );
};
//----------------------------------------

#endif // MANAGERTFS_H
