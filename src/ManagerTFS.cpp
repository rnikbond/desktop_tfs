//----------------------------------------
#include <QProcess>
#include <QTextCodec>
//----------------------------------------
#include "ManagerTFS.h"
//----------------------------------------
#ifdef WIN32
# define SEP "\r\n"
#else
# define SEP "\n"
#endif
//----------------------------------------

ManagerTFS::ManagerTFS( QObject* parent  ) : QObject(parent) { }
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::setConfiguration( const Configuration& cfg ) {
    config = cfg;
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::status( const QString& dir ) {

    clear();

    QStringList args = {
                        "status",
                        "-recursive",
                        //"-format:Detailed", // Детальная информация по изменениям
                        "-nodetect", // Отключает отображение новых файлов
                        dir
                       };

    execute( args );
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::checkConnection() {

    clear();

    QStringList args = {
                        "workfold",
                        QString("-login:%1,%2"  ).arg(config.Azure.login, config.Azure.password),
                        QString("-collection:%1").arg(config.Azure.url ),
                        QString("-workspace:%1" ).arg(config.Azure.workspace),
                       };

    execute( args );
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::cloneDir( const QString& dir ) {

    clear();

    QStringList args = { "get",
                         dir,
                        "-recursive",
                         QString("-collection:%1").arg(config.Azure.url),
                         QString("-login:%1,%2"  ).arg(config.Azure.login, config.Azure.password)
                       };

    execute( args );
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::entriesDir( const QString& dir ) {

    clear();

    QStringList args = { "dir",
                        QString("-login:%1,%2"  ).arg(config.Azure.login, config.Azure.password),
                        QString("-collection:%1").arg(config.Azure.url),
                        dir };

    execute( args );

    if( m_error_code != 0 ) {
        return;
    }

    if( m_result.isEmpty() ) {
        return;
    }

    // Последнеим элементом будет количество элементов - удаляем
    m_result.removeLast();

    for( int i = 0; i < m_result.count(); i++ ) {
        QString dirName = m_result.at(i);
        if( dirName.contains(":") ) {
            m_result.removeAt( i );
            i--;
        } else {
            m_result[i] = m_result[i].remove("$");
        }
    }
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::workspaces() {

    clear();

    QStringList args = {
                        "workspaces",
                        QString("-login:%1,%2"  ).arg(config.Azure.login, config.Azure.password),
                        QString("-collection:%1").arg(config.Azure.url),
                       };

    execute( args );
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::workfolds() {

    clear();

    QStringList args = {
                        "workfold",
                        QString("-workspace:%1" ).arg(config.Azure.workspace),
                        QString("-collection:%1").arg(config.Azure.url),
                        QString("-login:%1,%2"  ).arg(config.Azure.login, config.Azure.password),
                       };

    execute( args );
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::createWorkspace( const QString& name ) {

    clear();

    QStringList args = {
                        "workspace",
                        "-new",
                        name,
                        QString("-login:%1,%2"  ).arg(config.Azure.login, config.Azure.password),
                        QString("-collection:%1").arg(config.Azure.url),
                       };

    execute( args );
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::execute( const QStringList& args ) {

    QProcess tfs_process;
    tfs_process.setProcessChannelMode( QProcess::MergedChannels );
#ifndef WIN32
    tfs_process.setWorkingDirectory( "/home/rnb/work/mainline" );
#endif
    tfs_process.start( config.Azure.tfPath, args );
    tfs_process.waitForFinished();

    switch( tfs_process.error() ) {
        case QProcess::UnknownError: {
            break;
        }
        case QProcess::FailedToStart: {
            m_error_code = -1;
            m_error_text = tr("Неверно указан путь к программе TF: %1").arg(config.Azure.tfPath);
            return;
        }
        default: {
            m_error_code = tfs_process.error() * (-1);
            m_error_text = tfs_process.errorString();
            return;
        }
    }

    QByteArray tf_output = tfs_process.readAllStandardOutput();
    m_result = QTextCodec::codecForName("cp1251")->toUnicode(tf_output).split(SEP, Qt::SkipEmptyParts);

    m_error_code = tfs_process.exitCode();
    if( m_error_code != 0 ) {
        m_error_text = m_result.join('\n');
        m_result.clear();
    }
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::clear() {

    m_error_code = 0;
    m_error_text.clear();
    m_result    .clear();
}
//----------------------------------------------------------------------------------------------------------

