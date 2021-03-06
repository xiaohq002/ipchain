#include "ipcauthorizationtransaction.h"
#include "forms/ui_ipcauthorizationtransaction.h"
#include "walletmodel.h"
#include <QDateTime>
#include <QMessageBox>
#include "log/log.h"
#include "CDateEdit.h"
#include <QDate>
IpcAuthorizationTransaction::IpcAuthorizationTransaction(QWidget *parent) :
    QWidget(parent),walletModel(NULL),
    ui(new Ui::IpcAuthorizationTransaction)
{
    ui->setupUi(this);
    ui->comboBox_license->addItems(QStringList()<<tr("Final authority (not allowed to re authorize)")<<tr("Allow re authorization"));
    ui->comboBox_exclusive->addItems(QStringList()<<tr("General authorization"));//<<tr("Exclusive license"));
    ui->label_errmsg->setText("");

}

IpcAuthorizationTransaction::~IpcAuthorizationTransaction()
{
    delete ui;
}

void IpcAuthorizationTransaction::SetNameDisplay(QString s1)
{
    ui->namelabel->setText(s1);
}
void IpcAuthorizationTransaction::showEvent(QShowEvent * p)
{
    ui->lineEdit_address->setText("");
    ui->comboBox_license->setCurrentIndex(0);
    ui->comboBox_exclusive->setCurrentIndex(0);
    ui->lineEdit_starttime->setText("");
    ui->lineEdit_endtime->setText("");
    ui->label_errmsg->setText("");
    if(walletModel)
    {
        QDate tomorrow = walletModel->getSySQDate().addDays(1);
        CDateEdit* lineEdit_starttime = (CDateEdit*)(ui->lineEdit_starttime);
        lineEdit_starttime->setMinimumDate(tomorrow);
        CDateEdit* lineEdit_endtime = (CDateEdit*)(ui->lineEdit_endtime);
        lineEdit_endtime->setMinimumDate(tomorrow);
    }
}

void IpcAuthorizationTransaction::on_pushButton_back_pressed()
{
    Q_EMIT back();
}

void IpcAuthorizationTransaction::on_pushButton_confirm_pressed()
{   
    QString address = ui->lineEdit_address->text();
    if(address.isEmpty())
    {
        ui->label_errmsg->setText(tr("InvalidAddress"));
        return;
    }
    QString strendtime = ui->lineEdit_endtime->text();
    QString strstarttime = ui->lineEdit_starttime->text();
    if(strstarttime.isEmpty()||strendtime.isEmpty())
    {
        ui->label_errmsg->setText(tr("date error"));
        return;
    }
    int intstart = strTimeToInt(strstarttime+" 00:00:00");
    int intend = strTimeToInt(strendtime+" 23:59:59");
    if(intstart>=intend||intend<=0||intstart<=0)
    {
        ui->label_errmsg->setText(tr("date error"));
        return;
    }

    int license = ui->comboBox_license->currentIndex();
    int exclusive = ui->comboBox_exclusive->currentIndex();

    if(walletModel){
        QString msg;
        if(walletModel->prepareauthorizationIpc(address,license,exclusive,intstart,intend,msg))
        {
            ui->lineEdit_address->setText("");
           // Q_EMIT gotoSuccessfultradePage(0);

            QStringList data1;
            data1<<ui->namelabel->text();
            data1<<address;
            data1<<QString::number(license);
            data1<<QString::number(exclusive);
            data1<<QString::number(intstart);
            data1<<QString::number(intend);

            Q_EMIT gotoconfrimauthorizationIpc(data1);
        }
        else
        {
            if(msg == "AmountExceedsBalance")
                ui->label_errmsg->setText(tr("AmountExceedsBalance"));
            else if(msg == "AmountWithFeeExceedsBalance")
                ui->label_errmsg->setText(tr("AmountWithFeeExceedsBalance"));
            else if(msg == "InvalidAddress")
                ui->label_errmsg->setText(tr("InvalidAddress"));
            else if(msg == "AmountExceedsBalance")
                ui->label_errmsg->setText(tr("AmountExceedsBalance"));
            else if(msg == "Password error.")
                ui->label_errmsg->setText(tr("Password error."));
            else if(msg == "bad-IPC-timecheck-error")
                ui->label_errmsg->setText(tr("bad-IPC-timecheck-error"));
            else if(msg == "bad-IPC-uniqueAuthor-timecheck-error")
                ui->label_errmsg->setText(tr("bad-IPC-uniqueAuthor-timecheck-error"));
            else if(msg == "bad-IPC-normalAuthor-timecheck-error")
                ui->label_errmsg->setText(tr("bad-IPC-normalAuthor-timecheck-error"));
            else if(msg == "bad-IPC-reAuthor-timecheck-error")
                ui->label_errmsg->setText(tr("bad-IPC-reAuthor-timecheck-error"));
            else if(msg == "IPC-owner-starttime-is-up-yet")
                ui->label_errmsg->setText(tr("IPC-owner-starttime-is-up-yet")+ tr(" ")+m_strStringTime);
            else if(msg == "you can't uniqueauthorize,you have an authorize yet")
               ui->label_errmsg->setText(tr("you can't uniqueauthorize,you have an authorize yet"));
            else if(msg == "You can't uniqueAuthorize ,you hava an Authorize yet!")
               ui->label_errmsg->setText(tr("you can't uniqueauthorize,you have an authorize yet"));
            else if(msg == "bad-txns-inputs-spent")
                ui->label_errmsg->setText(tr("bad-txns-inputs-spent"));
            else if(msg == "IPC-Author-starttime-is-up-yet")
                ui->label_errmsg->setText(tr("IPC-Author-starttime-is-up-yet"));
            else
            {
                ui->label_errmsg->setText(msg);
            }
        }
        LOG_WRITE(LOG_INFO,"IpcAuthorizationTransaction::on_pushButton_confirm_pressed",
                  QString::number(intstart).toStdString().c_str(),
                  QString::number(intend).toStdString().c_str(),
                  QString::number(license).toStdString().c_str(),
                  QString::number(exclusive).toStdString().c_str(),
                  address.toStdString().c_str(),
                  msg.toStdString().c_str());

    }
}
int IpcAuthorizationTransaction::strTimeToInt(QString time)
{
    QDateTime datetime = QDateTime::fromString(time,"yyyy-MM-dd h:mm:ss");
    int inttime =   datetime.toTime_t();
    return inttime;
}
