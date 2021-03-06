#include "unionaccountjoin.h"
#include "ui_unionaccountjoin.h"
#include "log/log.h"
#include <QFileDialog>
#include <QTextStream>
#ifdef ENABLE_WALLET
#include "walletmodel.h"
#endif
unionaccountjoin::unionaccountjoin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::unionaccountjoin)
{
    ui->setupUi(this);
    QRegExp regx("^\\S+$");
    QValidator *validator = new QRegExpValidator(regx, ui->lineEdit_name );
    ui->lineEdit_name->setValidator( validator );
}

unionaccountjoin::~unionaccountjoin()
{
    delete ui;
}
void unionaccountjoin::setModel(WalletModel *_model)
{
    this->model = _model;
}
void unionaccountjoin::on_joinBtn_pressed()
{
    if(ui->lineEdit_name->text() == "" || ui->textEdit_add->document()->toPlainText().toStdString() == "")
    {
        ui->label_err->setText(tr("input info"));
        return;
    }
    std::string m_failreason;
    if(model->joinunionaccount(ui->lineEdit_name->text().toStdString(),m_failreason,ui->textEdit_add->document()->toPlainText().toStdString()))
    {
        ui->label_err->setText(tr("Join success. For your asset safety, please backup your wallet again!"));
        Q_EMIT refreshunionaccount();
        //  Q_EMIT opensuccessjoinPage();
    }
    else
    {
        if("script is not p2sh script!" ==m_failreason )
        {
            ui->label_err->setText(tr("script is not p2sh script!"));
        }
        else if("AddMultiAddress failed!" ==m_failreason)
        {
            ui->label_err->setText(tr("AddMultiAddress failed!"));
        }
        else if("MultiAdd is not yours" == m_failreason)
        {
            ui->label_err->setText(tr("MultiAdd is not yours"));
        }
        else if("Address duplication!" == m_failreason)
        {
            ui->label_err->setText(tr("Address duplication!"));
        }
        else if("Password error."== m_failreason)
        {
            ui->label_err->setText(tr("Password error."));
        }
        else
        {
            LOG_WRITE(LOG_INFO,"JOIN FAIL",m_failreason.c_str());
            ui->label_err->setText(tr("join error"));
        }
    }
}
void unionaccountjoin::showEvent(QShowEvent *event)
{
    ui->lineEdit_name->setText("");
    ui->textEdit_add->clear();
    ui->label_err->setText("");

}


void unionaccountjoin::on_btn_import_pressed()
{
    ui->textEdit_add->clear();
    ui->label_err->setText("");
    QString path = QFileDialog::getOpenFileName(this,tr("Open File"),".",tr("Text Files(*.txt)"));

    if(!path.isEmpty()) {

        QFile file(path);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

            // QMessageBox::warning(this, tr("Read File"),tr("Cannot open file:\n%1").arg(path));
            LOG_WRITE(LOG_INFO,"file open fail");
            return;

        }

        QTextStream in(&file);


        QString str = in.readAll();
        int pos1 ;

        QLocale locale;
        QString timestr;
        if( locale.language() == QLocale::Chinese )
        {
            pos1 = str.indexOf("邀请码:") ;
        }else{
           pos1 = str.indexOf("script:");
        }

        if(-1 == pos1)
        {
            ui->label_err->setText(tr("Please reselect the file"));
        }
        else
        {

             QString m_str;
            if( locale.language() == QLocale::Chinese )
            {
                m_str =str.mid(pos1+5 ,-1);
            }else{
               m_str =str.mid(pos1+7 ,-1);
            }



            m_str.replace(" ", "");
            ui->textEdit_add->setText(m_str);
        }
        file.close();

    } else {

        // QMessageBox::warning(this, tr("Path"),tr("You did not select any file."));

    }

}
