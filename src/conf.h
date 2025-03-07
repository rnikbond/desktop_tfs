//----------------------------------------
#ifndef CONF_H
#define CONF_H
//----------------------------------------
#include <QDebug>
#include <QString>
//----------------------------------------
#define CONF_GROUP_TFS   "tfs"
#define CONF_BIN         "bin"
#define CONF_COLLECTION  "collection"
#define CONF_WORKSPACE   "workspace"
#define CONF_WORKFOLD    "workfold"
#define CONF_GROUP_CREDS "credentials"
#define CONF_LOGIN       "login"
#define CONF_PASSWORD    "password"
//----------------------------------------

struct Credentials {
    QString login;
    QString password;
};
//----------------------------------------

struct ConfigTFS {
    QString     binPath   ;
    QString     azureUrl  ;
    QString     workspace ;
    QString     workfold  ;
    Credentials creds     ;

    ConfigTFS() {}
    ConfigTFS( const ConfigTFS& other ) {
        binPath        = other.binPath       ;
        azureUrl       = other.azureUrl      ;
        workspace      = other.workspace     ;
        workfold       = other.workfold      ;
        creds.login    = other.creds.login   ;
        creds.password = other.creds.password;
    }

    bool isValid() {
        return !binPath       .isEmpty() &&
               !azureUrl      .isEmpty() &&
               !workspace     .isEmpty() &&
               !workfold      .isEmpty() &&
               !creds.login   .isEmpty() &&
               !creds.password.isEmpty();
    }

    void print() {
        qDebug() << "-------------------";
        qDebug() << "binPath       : " << binPath       ;
        qDebug() << "collection    : " << azureUrl      ;
        qDebug() << "workspace     : " << workspace     ;
        qDebug() << "workfold      : " << workfold      ;
        qDebug() << "creds.login   : " << creds.login   ;
        qDebug() << "creds.password: " << creds.password;
        qDebug() << "-------------------";
    }

    void operator = (const ConfigTFS& other) {
        binPath        = other.binPath       ;
        azureUrl       = other.azureUrl      ;
        workspace      = other.workspace     ;
        workfold       = other.workfold      ;
        creds.login    = other.creds.login   ;
        creds.password = other.creds.password;
    }
};
//----------------------------------------

#endif // CONF_H
