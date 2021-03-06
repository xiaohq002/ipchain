#include "ecoincreatedialog.h"
#include "forms/ui_ecoincreatedialog.h"

#include "ipchainunits.h"
#include "wallet/coincontrol.h"
#include "guiutil.h"
#include <stdint.h>
#include <string>
#include <stdio.h>
#include <QFileDialog>
#include <QFileInfo>
#include "md5thread.h"
#include "log/log.h"
#include <QValidator>
#include "CDateEdit.h"
using namespace std;
using namespace boost;
ecoincreatedialog::ecoincreatedialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ecoincreatedialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->scrollArea->setFrameShape(QFrame::NoFrame);
    connect( this,SIGNAL(showmd5(QString)),this,SLOT(showmd5Slot(QString)));
    ui->coinlabel->setMaxLength(16);
    ui->coinname->setPlaceholderText(tr("Tokens are not more than 8 characters (without IPC)"));
    ui->picEdit->setPlaceholderText(tr("Token pic"));
    ui->coinmd5->setPlaceholderText(tr("Token hash"));
    ui->coinNum->setPlaceholderText(tr("Token num"));
    ui->coinlabel->setPlaceholderText(tr("Please enter no more than 16 characters (without IPC)"));
    QRegExp regx("[a-zA-Z]+$");
    QValidator *validator = new QRegExpValidator(regx, ui->coinlabel );
    ui->coinname->setValidator( validator );

    QRegExp regxcoinNum("[0-9]+$");
    QValidator *validatorxcoinNum = new QRegExpValidator(regxcoinNum, ui->coinNum );
    ui->coinNum->setValidator( validatorxcoinNum );
    connect(ui->coinNum,SIGNAL(textChanged(const QString&)),this,SLOT(slotTextChanged(const QString&)));
    ui->chooseaddBtn->hide();
    ui->comboBox->addItems(QStringList()<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8");
}

ecoincreatedialog::~ecoincreatedialog()
{
    delete ui;
}

void ecoincreatedialog::setinfoclean()
{
    ui->coinname->setText("");
    ui->picEdit->setText("");
    ui->coinmd5->setText("");
    ui->coinNum->setText("");
    ui->coinlabel->setText("");
    ui->TimeEdit->setText("");
    ui->tiplabel->setText("");
    ui->chooseaddBtn->setText(tr("chooseaddress"));
    CDateEdit* pdataedit = (CDateEdit*)ui->TimeEdit;
    if(model)
        pdataedit->setDate(model->getSySQDate());
}

void ecoincreatedialog::setModel(WalletModel *_model)
{
    this->model = _model;
}
void ecoincreatedialog::processSendCoinsReturn(const WalletModel::SendCoinsReturn &sendCoinsReturn, const QString &msgArg)
{
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::black);

    switch(sendCoinsReturn.status)
    {
    case WalletModel::PsdErr:
        ui->tiplabel->setText(tr("password error"));
        break;
    case WalletModel::InvalidAddress:
        ui->tiplabel->setText(tr("The recipient address is not valid. Please recheck."));
        break;
    case WalletModel::InvalidAmount:
        ui->tiplabel->setText(tr("The amount to pay must be larger than 0."));
        break;
    case WalletModel::AmountExceedsBalance:
        ui->tiplabel->setText(tr("The amount exceeds your balance."));
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        ui->tiplabel->setText(tr("The total exceeds your balance when the 0.001IPC/KB transaction fee is included.").arg(msgArg));
        break;
    case WalletModel::DuplicateAddress:
        ui->tiplabel->setText(tr("Duplicate address found: addresses should only be used once each."));
        break;
    case WalletModel::TransactionCreationFailed:
        if(m_sendcoinerror!="")
            ui->tiplabel->setText(m_sendcoinerror.c_str());
        else
            ui->tiplabel->setText(tr("Transaction creation failed!"));
        break;
    case WalletModel::TransactionCommitFailed:
    {
        if("bad-Token-tokensymbol-repeat" == sendCoinsReturn.reasonCommitFailed )
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: bad-Token-tokensymbol-repeat"));

        }
        else if("bad-Token-tokenhash-repeat" == sendCoinsReturn.reasonCommitFailed )
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: bad-Token-tokenhash-repeat"));

        }
        else if("bad-Token-Multi-inType" == sendCoinsReturn.reasonCommitFailed )
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: bad-Token-Multi-inType"));

        }
        else if("bad-Token-Reg-issueDate(Regtime)" == sendCoinsReturn.reasonCommitFailed)
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: bad-Token-Reg-issueDate(Regtime)"));

        }
        else if("bad-Token-regtotoken-value-unequal" == sendCoinsReturn.reasonCommitFailed )
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: bad-Token-regtotoken-value-unequal"));
        }
        else if("bad-Token-Label-contain-errvalue"== sendCoinsReturn.reasonCommitFailed)
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: bad-Token-Label-contain-errvalue"));
        }
        else if("bad-Token-Symbol-contain-errvalue"== sendCoinsReturn.reasonCommitFailed)
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: bad-Token-Symbol-contain-errvalue"));
        }
        else if("bad-Token-value-unequal" == sendCoinsReturn.reasonCommitFailed )
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: bad-Token-value-unequal"));
        }
        else if("bad-Token-Multi-outType" == sendCoinsReturn.reasonCommitFailed )
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: bad-Token-Multi-outType"));
        }
        else
        {
            ui->tiplabel->setText(tr("The transaction was rejected with the following reason: %1").arg(sendCoinsReturn.reasonCommitFailed));
        }
    }
        break;
    case WalletModel::AbsurdFee:
        ui->tiplabel->setText(tr("A fee higher is considered an absurdly high fee."));
        break;
    case WalletModel::PaymentRequestExpired:
        ui->tiplabel->setText(tr("Payment request expired."));
        break;
    case WalletModel::OK:
    default:
        return;
    }
}
void  ecoincreatedialog::setAddress(QString address)
{
    ui->chooseaddBtn->setText(address);
    m_address = address;
}

void ecoincreatedialog::on_CreateeCoinButton_pressed()
{
    m_sendcoinerror = "";
    ui->tiplabel->setText(tr(""));
    QString coinNumText = ui->coinNum->text();
    coinNumText = coinNumText.replace(",","");
    Q_EMIT GetOldAddress();
    if(!model || !model->getOptionsModel())
        return;
    if("" == ui->coinname->text() ||
            "" == ui->coinname->text() ||
            "" == ui->picEdit->text() ||
            "" == ui->coinmd5->text() ||
            "" == coinNumText ||
            "" ==ui->chooseaddBtn->text() ||
            "" == ui->coinlabel->text() ||
            "" == ui->TimeEdit->text())
    {
        ui->tiplabel->setText(tr("input info"));
        return;
    }
    if(0 == coinNumText.toDouble())
    {
        ui->tiplabel->setText(tr("num error"));
        return;
    }
    QString stdlabel = ui->coinlabel->text();
    std::string tempstr =  stdlabel.toStdString();
    if(tempstr.length()>16)
    {
        ui->tiplabel->setText(tr("Token label size too large."));
        return;
    }
    int accuracy = ui->comboBox->currentIndex();


    QString add1 = ui->chooseaddBtn->text() ;
    uint64_t amount1 = (uint64_t)coinNumText.toULongLong();

    if(amount1<=0||amount1>999999999999999)
    {
        ui->tiplabel->setText(tr("coinNum (0,999999999999999]"));
        return ;
    }
    int iaccuracy = accuracy;
    uint64_t iamount1= amount1;
    while(iaccuracy--){
        if(iamount1>999999999999999)
        {
            ui->tiplabel->setText(tr("coinNum*10^accuracy must no more than 999999999999999"));
            return ;
        }
        iamount1=iamount1*10;
    }
    if(iamount1<=0||iamount1>999999999999999)
    {
        ui->tiplabel->setText(tr("coinNum*10^accuracy must no more than 999999999999999"));
        return ;
    }

    QString timestr = ui->TimeEdit->text();
    int inttimr = strTimeToInt(timestr);
    if(inttimr<=0)
    {
        ui->tiplabel->setText(tr("time error"));
        return ;
    }

    QString coin_name = ui->coinname->text();
    QString coin_md5 =ui->coinmd5->text();
    QString coin_label =  ui->coinlabel->text();

    QString msg;

    WalletModel::SendCoinsReturn status =model->prepareeCoinsCreate(coin_name,coin_md5,coin_label,timestr,add1,msg,inttimr, accuracy, amount1);

    if(WalletModel::OK== status.status )
    {
        QStringList data1;
        data1<<coin_name;
        data1<<coin_md5;
        data1<<QString::number(amount1);
        data1<<QString::number(accuracy);
        data1<<coin_label;
        data1<<timestr;
        Q_EMIT jumptoecoinconfrimcreatedialog(data1);
    }
    else
    {
        m_sendcoinerror =  model->m_sendcoinerror;//20180201
        processSendCoinsReturn(status);
    }

    /*
    WalletModel::SendCoinsReturn sendStatus = model->sendCoins(currentTransaction);

    if (sendStatus.status == WalletModel::OK)
    {
        Q_EMIT CreateeCoinSuccess();
    }
    else
    {
        m_sendcoinerror =  model->m_sendcoinerror;
    }

    fNewRecipientAllowed = true;
*/
}

void ecoincreatedialog::on_chooseaddBtn_pressed()
{
    Q_EMIT selectaddress(3);
}
void ecoincreatedialog::fileToMd5(std::string strmd5)
{
    Q_EMIT showmd5(QString::fromStdString(strmd5));
}
void ecoincreatedialog::showmd5Slot(QString strmd5)
{
    ui->coinmd5->setText(strmd5);
}

void ecoincreatedialog::on_openPicBtn_pressed()
{

    QString file_full, file_name, file_path;
    QFileInfo fi;
    file_full = QFileDialog::getOpenFileName(this,"","/","image Files(*.bmp *.jpg *.png *.jpeg)");
    if(file_full == "")
    {
        return;
    }
    fi = QFileInfo(file_full);
    file_name = fi.fileName();
    file_path = fi.absolutePath();
    ui->picEdit->setText(file_name);

    boost::thread m_ptr(fileToMd5Thread,file_full.toStdString(),string("eCoinCreate"),(void*)this);
    m_ptr.detach();

}
int ecoincreatedialog::strTimeToInt(QString time)
{
    QDate date = QDate::fromString(time,"yyyy-MM-dd");
    QDateTime datetime(date);
    int inttime =   datetime.toTime_t();
    return inttime;
}
uint64_t ecoincreatedialog::getpow10(uint64_t oldnum,int n)
{
    while (n--) {
        oldnum = oldnum*10;
    }
    return oldnum;
}
void ecoincreatedialog::slotTextChanged(const QString & text)
{
    QString textContent = text;
    QString numtexttemp = textContent;
    QString textContenttemp = numtexttemp.replace(",","");
    uint64_t maxLength = 999999999999999;
    uint64_t amount1 = (uint64_t)textContenttemp.toULongLong();
    if(amount1 > maxLength)
    {
        int position = ui->coinNum->cursorPosition();
        textContent.chop(1);
        ui->coinNum->setText(textContent);
        ui->coinNum->setCursorPosition(position-1);
    }
}
