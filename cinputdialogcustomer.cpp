/*  LaVa 2, a inventory managment tool
    Copyright (C) 2013 - Robert Ewert - robert.ewert@gmail.com - www.robert.ewert.de.vu
    created with QtCreator(Qt 4.8.0)

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

#include "cinputdialogcustomer.h"
#include "ui_cinputdialogcustomer.h"

CInputDialogCustomer::CInputDialogCustomer(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CInputDialogCustomer)
{
    m_ui->setupUi(this);
    m_ui->lineEditCustomerNumber->setFocus();
    m_ui->pushButtonOk->setEnabled(false);
    m_pThread=NULL;
    m_iMarkId=-1;
    m_sCustomerNumber=QString("");
    //-
    connect(m_ui->pushButtonNomination,SIGNAL(clicked()),this,SLOT(customer_number_nomination()));
    connect(m_ui->lineEditCustomerNumber,SIGNAL(textChanged(QString)),this,SLOT(customer_number_changed(QString)));
    connect(m_ui->pushButtonOk,SIGNAL(clicked()),this,SLOT(press_ok()));
    connect(m_ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(press_cancel()));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDialogCustomer::~CInputDialogCustomer()
{
    delete m_ui;
}

bool CInputDialogCustomer::get_data(CCustomer & cu)
{
    cu.set_city(m_ui->lineEditCity->text());
    cu.set_comment(m_ui->lineEditComment->text());
    cu.set_customernumber(m_ui->lineEditCustomerNumber->text());
    cu.set_first_name(m_ui->lineEditFirstName->text());
    cu.set_name(m_ui->lineEditName->text());
    cu.set_postcode(m_ui->lineEditPostcode->text());
    cu.set_street(m_ui->lineEditStreet->text());
    cu.set_callnumber(m_ui->lineEditCallnumber->text());
    cu.set_faxnumber(m_ui->lineEditFaxnumber->text());
    cu.set_email(m_ui->lineEditEmail->text());
    cu.set_id(m_iMarkId);
    return true;
}

bool CInputDialogCustomer::set_data(CCustomer & cu)
{
    //for edit
    m_iMarkId=cu.get_id();
    m_sMarkName=cu.get_name();

    m_ui->pushButtonOk->setText(QString("Änderung(en) anwenden"));
    m_ui->pushButtonOk->setEnabled(true);
    m_ui->lineEditCity->setText(cu.get_city());
    m_ui->lineEditComment->setText(cu.get_comment());
    m_ui->lineEditCustomerNumber->setText(cu.get_customernumber());
    m_ui->lineEditFirstName->setText(cu.get_first_name());
    m_ui->lineEditName->setText(cu.get_name());
    m_ui->lineEditPostcode->setText(cu.get_postcode());
    m_ui->lineEditStreet->setText(cu.get_street());
    m_ui->lineEditCallnumber->setText(cu.get_callnumber());
    m_ui->lineEditFaxnumber->setText(cu.get_faxnumber());
    m_ui->lineEditEmail->setText(cu.get_email());
    m_sCustomerNumber=cu.get_customernumber();
    return true;
}

void CInputDialogCustomer::customer_number_nomination(void)
{
    QString s;
    CPointerMemory memory;
    memory.set_string(&s);
    //-
    if(m_pThread!=NULL)//db connection?
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            m_pThread->set_work(WORK_GET_CUSTOMER_NUMBER_NOMINATION,&memory);//get nomination
            if(s.length()>0)//found?
            {
                m_ui->lineEditCustomerNumber->setText(s);
                m_ui->lineEditCustomerNumber->setFocus();
            }
        }
    }
}

bool CInputDialogCustomer::customer_number_changed(QString s)
{
    QString text,s2;
    int count=-1;
    CPointerMemory memory;
    memory.set_string(&s2);
    memory.set_int(&count);
    bool b=true;
    //-
    if(s.length()<=0)//no customer number?
    {
        text=QString("Fehler: Kundennummer fehlt");
        b=false;
    }
    else if(s==m_sCustomerNumber)//edit mode and = mark customer number?
        m_ui->labelError->setText("");
    else if(m_pThread!=NULL)
    {
        s2=QString("customer_number='%1'").arg(s);
        m_pThread->set_work(WORK_CUSTOMER_GET_COUNT,&memory);
        if(count>0)//dopple?
        {
            text=QString("Fehler: Kundennummer gibt es schon");
            b=false;
        }
    }
    //-
    m_ui->pushButtonOk->setEnabled(b);
    m_ui->labelError->setText(text);//set comment
    //-
    return b;
}

void CInputDialogCustomer::press_ok(void)
{
    int i,iReturn=1;
    QString s;
    CCustomer cu;
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    msg.setText(s);
    msg.setInformativeText("Der Datensatz wurde von einer anderen Programminstanz gelöscht!\nSoll dieser Kunde neu angelegt werden?");
    //-
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            //another client create a record with this number?
            if(!customer_number_changed(m_ui->lineEditCustomerNumber->text()))
                iReturn=-1;//error
            else
            {
                //check by edit-mode, another client delete record?
                if(m_iMarkId>0)//edit-mode?
                {
                    s=QString("id = %1").arg(m_iMarkId);
                    s+=QString(" AND name = '%1'").arg(m_sMarkName);
                    i=m_pThread->m_pDbInterface->customer_get_count(s);
                    if(i<=0)//record delete?
                    {
                        msg.exec();
                        if(msg.clickedButton()!=yesButton)
                            iReturn=0;
                        else
                        {
                            get_data(cu);
                            m_sCustomerNumber=m_pThread->m_pDbInterface->customer_get_customer_number_nomination();
                            m_ui->lineEditCustomerNumber->setText(m_sCustomerNumber);
                            cu.set_customernumber(m_sCustomerNumber);
                            m_pThread->m_pDbInterface->customer_add(cu);
                            m_iMarkId=cu.get_id();//mark new id
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

void CInputDialogCustomer::press_cancel(void)
{
    close();
}
