//----------------------------------------
#include <QProcess>
#include <QTextCodec>
//----------------------------------------
#include "ManagerTFS.h"
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

void ManagerTFS::entriesDir( const QString& dir ) {

    clear();

#ifdef WIN32
    QString separator = "\r\n";
#else
    QString separator = "\n";
#endif

    QStringList args = { "dir",
                        QString("-login:%1,%2").arg(config->creds.login).arg(config->creds.password),
                        QString("-collection:%1").arg(config->collection),
                        dir };

    QProcess tfs_process;
    tfs_process.setProcessChannelMode( QProcess::MergedChannels );

    tfs_process.start( config->binPath, args );
    tfs_process.waitForFinished();

    QByteArray tf_output = tfs_process.readAllStandardOutput();
    m_result = QTextCodec::codecForName("cp1251")->toUnicode(tf_output).split( separator, Qt::SkipEmptyParts );

    m_error_code = tfs_process.exitCode();
    if( m_error_code != 0 ) {
        m_error_text = m_result.join(',');
        m_result.clear();
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

void ManagerTFS::clear() {

    m_error_code = 0;
    m_error_text.clear();
    m_result    .clear();
}
//----------------------------------------------------------------------------------------------------------

