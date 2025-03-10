//----------------------------------------
#ifndef CONF_H
#define CONF_H
//----------------------------------------
#include <QFile>
#include <QDebug>
#include <QString>
//----------------------------------------
#define CONF_PATH            "tfs.cfg"
#define CURRENT_CONF_VERSION 1

#define CONF_VERSION "version"
#define CONF_GROUP_AZURE "tfs"
    #define CONF_TF_PATH     "tf_path"
    #define CONF_AZURE_URL   "url"
    #define CONF_AZURE_WORKSPACE   "workspace"
    #define CONF_AZURE_LOGIN       "login"
    #define CONF_AZURE_PASSWORD    "password"
//----------------------------------------

struct AzureDevOps {

    QString tfPath   ;
    QString url      ;
    QString workspace;
    QString login    ;
    QString password ;

    QMap<QString,QString> workfoldes;

    AzureDevOps() {}
    AzureDevOps( const AzureDevOps& other ) {
        tfPath    = other.tfPath   ;
        url       = other.url      ;
        workspace = other.workspace;
        login     = other.login    ;
        password  = other.password ;
    }

    void operator = ( const AzureDevOps& other ) {
        tfPath    = other.tfPath   ;
        url       = other.url      ;
        workspace = other.workspace;
        login     = other.login    ;
        password  = other.password ;
    }

    bool isIncomplete() const {

        if( tfPath.isEmpty() || !QFile(tfPath).exists() ) {
            return false;
        }

        return ( !url      .isEmpty() &&
                 !workspace.isEmpty() &&
                 !login    .isEmpty() &&
                 !password .isEmpty() );
    }
};
//----------------------------------------

struct Configuration {

    AzureDevOps Azure;

    Configuration() {}
    Configuration( const Configuration& other ) {
        Azure = other.Azure;
    }

    void operator = ( const Configuration& other ) {
        Azure = other.Azure;
    }
};
//----------------------------------------

#endif // CONF_H
