//----------------------------------------
#include <QDebug>
//----------------------------------------
#include "ManagerTFS.h"
//----------------------------------------
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
//----------------------------------------

enum Pages {
    PageTFS      , ///< Конфигурация TFS
};
//----------------------------------------
enum {
    PageRole = Qt::UserRole + 1,
};
//----------------------------------------

SettingsDialog::SettingsDialog( QWidget* parent ) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->splitter->setSizes( {150, 500} );
    setWindowTitle( tr("Настройка") );

    initPages();

    connect( ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::save );
}
//----------------------------------------------------------------------------------------------------------

SettingsDialog::~SettingsDialog() {

    delete ui;
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::save() {

    savePageConfig();
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::restore() {

    restorePageConfig();
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::setConfiguration( const Configuration& cfg ) {

    config = cfg;
    restore();

    ui->pagesList->setCurrentItem( findPage(PageTFS) );
}
//----------------------------------------------------------------------------------------------------------

const Configuration& SettingsDialog::configuration() const {
    return config;
}
//----------------------------------------------------------------------------------------------------------

QListWidgetItem* SettingsDialog::findPage( int page ) const {

    for( int row = 0; row < ui->pagesList->count(); row++ ) {
        QListWidgetItem* itemPage = ui->pagesList->item( row );
        if( itemPage->data(PageRole).toInt() == page ) {
            return itemPage;
        }
    }

    return nullptr;
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::selectPage( QListWidgetItem* itemPage, QListWidgetItem* ) {

    if( itemPage == nullptr ) {
        return;
    }

    int page = itemPage->data(PageRole).toInt();
    switch( page) {
        case PageTFS: {
            ui->pagesStack->setCurrentWidget( ui->pageConfigTFS );
            break;
        }
        default: {
            qDebug() << "SettingsDialog::selectPage()" << QString("unknown page role: %1").arg(page);
            break;
        }
    }
}
//----------------------------------------------------------------------------------------------------------

void SettingsDialog::initPages() {

    QListWidgetItem* itemPageTFS = new QListWidgetItem;

    itemPageTFS->setText( tr("Конфигурация TFS") );
    itemPageTFS->setData( PageRole, PageTFS      );

    ui->pagesList->addItem( itemPageTFS       );

    initPageConfig();

    connect( ui->pagesList, &QListWidget::currentItemChanged, this, &SettingsDialog::selectPage );
}
//----------------------------------------------------------------------------------------------------------


