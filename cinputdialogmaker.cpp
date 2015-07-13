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

#include "cinputdialogmaker.h"
#include "ui_cinputdialogmaker.h"

CInputDialogMaker::CInputDialogMaker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogMaker)
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

CInputDialogMaker::~CInputDialogMaker()
{
    delete ui;
}

bool CInputDialogMaker::get_data(CMaker & mk)
{
    mk.set_name(ui->lineEditName->text());
    mk.set_adress(ui->lineEditAdress->text());
    mk.set_callnumber(ui->lineEditCallnumbers->text());
    mk.set_faxnumber(ui->lineEditFaxnumbers->text());
    mk.set_email(ui->lineEditEmail->text());
    mk.set_homepage(ui->lineEditHomepage->text());
    mk.set_contectperson(ui->lineEditContect->text());
    mk.set_comment(ui->lineEditComment->text());
    mk.set_id(m_iMarkId);
    return true;
}

bool CInputDialogMaker::set_data(CMaker & mk)
{
    //for edit
    m_iMarkId=mk.get_id();
    m_sMarkName=mk.get_name();

    ui->lineEditName->setText(mk.get_name());
    ui->lineEditAdress->setText(mk.get_adress());
    ui->lineEditCallnumbers->setText(mk.get_callnumber());
    ui->lineEditFaxnumbers->setText(mk.get_faxnumber());
    ui->lineEditEmail->setText(mk.get_email());
    ui->lineEditHomepage->setText(mk.get_homepage());
    ui->lineEditContect->setText(mk.get_contectperson());
    ui->lineEditComment->setText(mk.get_comment());
    ui->pushButtonOk->setText(QString::fromUtf8("Änderung(en) anwenden"));
    ui->pushButtonOk->setEnabled(true);
    return true;
}

bool CInputDialogMaker::check_user_input(QString s)
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
                text=QString("Fehler: Herstellername fehlt");
                b=false;
            }
            else
            {
                m_pThread->set_work(WORK_MAKER_GET_COUNT,&memory);
                if(count>0 && m_sMarkName!=s)//check give maker = name?
                {
                    text=QString("Fehler: Hersteller gibt es schon");
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

void CInputDialogMaker::press_ok(void)
{
    int i,iReturn=1;
    QString s;
    CMaker mk;
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    msg.setText(s);
    msg.setInformativeText(QString::fromUtf8("Der Datensatz wurde von einer anderen Programminstanz gelöscht!\nSoll dieser Hersteller neu angelegt werden?"));
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
                    i=m_pThread->m_pDbInterface->maker_get_count(s);
                    if(i<=0)//record delete?
                    {
                        msg.exec();
                        if(msg.clickedButton()!=yesButton)
                            iReturn=0;
                        else
                        {
                            get_data(mk);
                            m_pThread->m_pDbInterface->maker_add(mk);
                            m_iMarkId=mk.get_id();//mark new id
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

void CInputDialogMaker::press_cancel(void)
{
    close();
}

