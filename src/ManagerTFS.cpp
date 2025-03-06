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

ManagerTFS::ManagerTFS( QObject* parent  ) : QObject(parent)
{
    config = nullptr;
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::init( const ConfigTFS* cfg ) {

    config = cfg;
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::cloneDir( const QString& dir ) {

    clear();

    QStringList args = { "get",
                         dir,
                        "-recursive",
                         QString("-collection:%1").arg(config->collection),
                         QString("-login:%1,%2"  ).arg(config->creds.login).arg(config->creds.password)
                       };

    execute( args );
    if( m_error_code != 0 ) {
        return;
    }

    qDebug() << m_result;
}
//----------------------------------------------------------------------------------------------------------

void ManagerTFS::entriesDir( const QString& dir ) {

    clear();

    QStringList args = { "dir",
                        QString("-login:%1,%2").arg(config->creds.login).arg(config->creds.password),
                        QString("-collection:%1").arg(config->collection),
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

void ManagerTFS::execute( const QStringList& args ) {

    QProcess tfs_process;
    tfs_process.setProcessChannelMode( QProcess::MergedChannels );
#ifndef WIN32
    tfs_process.setWorkingDirectory( "/home/rnb/work/mainline" );
#endif
    tfs_process.start( config->binPath, args );
    tfs_process.waitForFinished();

    switch( tfs_process.error() ) {
        case QProcess::UnknownError: {
            break;
        }
        case QProcess::FailedToStart: {
            m_error_code = -1;
            m_error_text = tr("Неверно указан путь к программе TF: %1").arg(config->binPath);
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

