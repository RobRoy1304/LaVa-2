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

#include "cinputdialogmaker.h"
#include "ui_cinputdialogmaker.h"

CInputDialogMaker::CInputDialogMaker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogMaker)
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

CInputDialogMaker::~CInputDialogMaker()
{
    delete ui;
}


void CInputDialogMaker::changeEvent(QEvent *e)
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
    return true;
}

bool CInputDialogMaker::set_data(CMaker & mk)
{
    ui->lineEditName->setText(mk.get_name());
    ui->lineEditAdress->setText(mk.get_adress());
    ui->lineEditCallnumbers->setText(mk.get_callnumber());
    ui->lineEditFaxnumbers->setText(mk.get_faxnumber());
    ui->lineEditEmail->setText(mk.get_email());
    ui->lineEditHomepage->setText(mk.get_homepage());
    ui->lineEditContect->setText(mk.get_contectperson());
    ui->lineEditComment->setText(mk.get_comment());
    m_sMarkName=mk.get_name();
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
    if(s.length()<=0)
    {
        text=QString("Fehler: Herstellername fehlt");
        b=false;
    }
    else
    {
        if(m_pThread!=NULL)
        {
            m_pThread->set_work(WORK_MAKER_GET_COUNT,&memory);
            if(count>0 && m_sMarkName!=s)//check give maker = name?
            {
                text=QString("Fehler: Hersteller gibt es schon");
                b=false;
            }
        }
    }
    //-
    ui->buttonBox->setEnabled(b); // button on/off
    ui->labelNameError->setText(text);//set comment
    return true;
}

