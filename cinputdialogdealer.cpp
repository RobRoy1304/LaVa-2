/*  LaVa 2, a inventory managment tool
    Copyright (C) 2015 - Robert Ewert - robert.ewert@gmail.com - www.robert.ewert.de.vu
    created with QtCreator(Qt 4.8)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cinputdialogdealer.h"
#include "ui_cinputdialogdealer.h"

CInputDialogDealer::CInputDialogDealer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogDealer)
{
    ui->setupUi(this);
    ui->lineEditName->setFocus();
    ui->pushButtonOk->setEnabled(false);
    m_pThread=NULL;
    m_iMarkId=-1;

    //disable auto default buttons
    ui->pushButtonCancel->setAutoDefault(false);
    ui->pushButtonOk->setAutoDefault(false);
    //-
    connect(ui->lineEditName,SIGNAL(textChanged(QString)),this,SLOT(check_user_input(QString)));
    connect(ui->pushButtonOk,SIGNAL(clicked()),this,SLOT(press_ok()));
    connect(ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(press_cancel()));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDialogDealer::~CInputDialogDealer()
{
    delete ui;
}

bool CInputDialogDealer::get_data(CDealer & de)
{
    de.set_name(ui->lineEditName->text());
    de.set_adress(ui->lineEditAdress->text());
    de.set_callnumber(ui->lineEditCallnumbers->text());
    de.set_faxnumber(ui->lineEditFaxnumbers->text());
    de.set_email(ui->lineEditEmail->text());
    de.set_homepage(ui->lineEditHomepage->text());
    de.set_contectperson(ui->lineEditContect->text());
    de.set_comment(ui->lineEditComment->text());
    de.set_customernumber(ui->lineEditCustomernumber->text());
    de.set_id(m_iMarkId);
    return true;
}

bool CInputDialogDealer::set_data(CDealer & de)
{
    //for edit
    m_iMarkId=de.get_id();
    m_sMarkName=de.get_name();

    ui->pushButtonOk->setText(QString::fromUtf8("Änderung(en) anwenden"));
    ui->pushButtonOk->setEnabled(true);
    ui->lineEditName->setText(de.get_name());
    ui->lineEditAdress->setText(de.get_adress());
    ui->lineEditCallnumbers->setText(de.get_callnumber());
    ui->lineEditFaxnumbers->setText(de.get_faxnumber());
    ui->lineEditEmail->setText(de.get_email());
    ui->lineEditHomepage->setText(de.get_homepage());
    ui->lineEditContect->setText(de.get_contectperson());
    ui->lineEditComment->setText(de.get_comment());
    ui->lineEditCustomernumber->setText(de.get_customernumber());
    return true;
}

bool CInputDialogDealer::check_user_input(QString s)
{
    bool b=true;
    QString text,condition=QString("name='%1'").arg(s);
    int count=-1;
    CPointerMemory memory;
    memory.set_string(&condition);
    memory.set_int(&count);

    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            if(s.length()<=0)
            {
                text=QString::fromUtf8("Fehler: Händlername fehlt");
                b=false;
            }
            else
            {
                m_pThread->set_work(WORK_DEALER_GET_COUNT,&memory);
                if(count>0 && m_sMarkName!=s)//check give maker = name?
                {
                    text=QString::fromUtf8("Fehler: Händler gibt es schon");
                    b=false;
                }
            }
            //-
            ui->pushButtonOk->setEnabled(b);
            ui->labelNameError->setText(text);//set comment
        }
    }
    return b;
}

void CInputDialogDealer::press_ok(void)
{
    int i,iReturn=1;
    QString s;
    CDealer de;
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    msg.setText(s);
    msg.setInformativeText(QString::fromUtf8("Der Datensatz wurde von einer anderen Programminstanz gelöscht!\nSoll dieser Händler neu angelegt werden?"));
    //-
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            //another client create a record with this name?
            if(!check_user_input(ui->lineEditName->text()))
                iReturn=-1;//error
            else
            {
                //check by edit-mode, another client delete record?
                if(m_iMarkId>0)//edit-mode?
                {
                    s=QString("id = %1").arg(m_iMarkId);
                    s+=QString(" AND name = '%1'").arg(m_sMarkName);
                    i=m_pThread->m_pDbInterface->dealer_get_count(s);
                    if(i<=0)//record delete?
                    {
                        msg.exec();
                        if(msg.clickedButton()!=yesButton)
                            iReturn=0;
                        else
                        {
                            get_data(de);
                            m_pThread->m_pDbInterface->dealer_add(de);
                            m_iMarkId=de.get_id();//mark new id
                            iReturn=1;
                        }
                    }
                }
            }
        }
    }
    //-
    if(iReturn>=0)
        done(iReturn);
}

void CInputDialogDealer::press_cancel(void)
{
    close();
}

