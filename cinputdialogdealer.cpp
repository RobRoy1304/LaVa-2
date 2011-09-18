/*  LaVa 2, a inventory managment tool
    Copyright (C) 2011 - Robert Ewert - robert.ewert@gmail.com - www.robert.ewert.de.vu
    created with QtCreator(Qt 4.7.0)

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
    m_pThread=NULL;
    ui->buttonBox->setEnabled(false);
    connect(ui->lineEditName,SIGNAL(textChanged(QString)),this,SLOT(check_user_input(QString)));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDialogDealer::~CInputDialogDealer()
{
    delete ui;
}

void CInputDialogDealer::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
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
    return true;
}

bool CInputDialogDealer::set_data(CDealer & de)
{
    ui->lineEditName->setText(de.get_name());
    ui->lineEditAdress->setText(de.get_adress());
    ui->lineEditCallnumbers->setText(de.get_callnumber());
    ui->lineEditFaxnumbers->setText(de.get_faxnumber());
    ui->lineEditEmail->setText(de.get_email());
    ui->lineEditHomepage->setText(de.get_homepage());
    ui->lineEditContect->setText(de.get_contectperson());
    ui->lineEditComment->setText(de.get_comment());
    ui->lineEditCustomernumber->setText(de.get_customernumber());
    m_sMarkName=de.get_name();
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
    if(s.length()<=0)
    {
        text=QString("Fehler: Händlername fehlt");
        b=false;
    }
    else
    {
        if(m_pThread!=NULL)
        {
            m_pThread->set_work(WORK_DEALER_GET_COUNT,&memory);
            if(count>0 && m_sMarkName!=s)//check give dealer = name?
            {
                text=QString("Fehler: Händler gibt es schon");
                b=false;
            }
        }
    }
    //-
    ui->buttonBox->setEnabled(b); // button on/off
    ui->labelNameError->setText(text);//set comment
    return true;
}

