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

#include "cinputdialogarticle.h"
#include "ui_cinputdialogarticle.h"


CInputDialogArticle::CInputDialogArticle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogArticle)
{
    ui->setupUi(this);
    m_pThread=NULL;
    m_iNewWaregroupId=m_iNewMakerId=-1;
    ui->lineEditName->setFocus();
    ui->buttonBox->setEnabled(false);
    m_sMarkArticleName=QString("");
    m_sMarkArticleNumber1=QString("");
    m_sMarkArticleNumber2=QString("");

    ui->lineEditInventory->setMaxLength(9);
    ui->lineEditMaxInventory->setMaxLength(9);

    //conects
    connect(ui->pushButtonWaregroup,SIGNAL(clicked()),this,SLOT(browse_waregroups()));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(pressed_ok()));
    connect(ui->comboBox,SIGNAL(editTextChanged(QString)),this,SLOT(edit_maker_combobox(QString)));
    connect(ui->lineEditWaregroup,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->checkBoxWarning,SIGNAL(clicked()),this,SLOT(checkbox_warning()));
    connect(ui->lineEditWarningLimit,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->lineEditInventory,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->lineEditMaxInventory,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->lineEditBaseprice,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->lineEditSalesprice,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->lineEditName,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->lineEditArticlenumber,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->lineEditArticlenumber_2,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    //-
    setMinimumSize(width(),height());
    setMaximumSize(width(),height());
}

CInputDialogArticle::~CInputDialogArticle()
{
    delete ui;
}

void CInputDialogArticle::changeEvent(QEvent *e)
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

bool CInputDialogArticle::init(int iSelectedWaregroupId)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    //fill maker combobox
    CPointerMemory memory;
    memory.set_combobox(ui->comboBox);
    if(!m_pThread->set_work(WORK_MAKER_FILL_COMBOBOX,&memory))
    {}
    //fill waregroup
    m_dlgWaregroup.updateWaregroup(iSelectedWaregroupId);
    return true;
}

 bool CInputDialogArticle::set_thread(CWorkThread * pThread)
 {
     m_pThread=pThread;
     m_dlgWaregroup.set_thread(pThread);
     return true;
 }

bool CInputDialogArticle::browse_waregroups(void)
{
    if(m_dlgWaregroup.exec())
    {
        QString s;
        int i;
        m_dlgWaregroup.get_selected_waregroup(s,i);
        ui->lineEditWaregroup->setText(s);
    }
    return true;
}

bool CInputDialogArticle::edit_maker_combobox(QString s)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    int i;
    QString sCondition=QString("name='%1'").arg(s);
    CPointerMemory memory;
    memory.set_string(&sCondition);
    memory.set_int(&i);
    m_pThread->set_work(WORK_MAKER_GET_COUNT,&memory);
    if(i<=0)
    {//maker not exis.?
        ui->comboBox->setItemText(0,s);
        ui->comboBox->setCurrentIndex(0);
    }
    else
    {
        i=ui->comboBox->findText(s);
        if(i>=0)
            ui->comboBox->setCurrentIndex(i);
    }
    check_user_input();
    return true;
}

bool CInputDialogArticle::pressed_ok(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    bool b=true;
    QString s;
    QString name;
    //-check maker-
    name=ui->comboBox->itemText(ui->comboBox->currentIndex());
    int maker_id=-1;
    CMaker mk;
    if(name.length()>0)
    {
        maker_id=m_pThread->m_pDbInterface->maker_get_id(name);//search
        if(maker_id==-1)
        {//maker not found?
            s=QString("Der Hersteller '%1' existiert nicht.").arg(name);
            QMessageBox msg(QMessageBox::Question,"","");
            QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
            msg.addButton(QString("Nein"),QMessageBox::NoRole);
            msg.setWindowTitle("?");
            msg.setText(s);
            msg.setInformativeText("Soll dieser Hersteller angelegt werden?");
            msg.exec();
            if(msg.clickedButton()==yesButton)
            {
                //insert new maker in db
                mk.set_name(name);
                b=m_pThread->m_pDbInterface->maker_add(mk);
                if(!b)//error
                {}
                else
                    m_iNewMakerId=mk.get_id();
            }
        }
    }
    if(b)
    {
        //check waregroup
        name=ui->lineEditWaregroup->text();
        int waregroup_id=-1;
        CWaregroup wg;
        m_dlgWaregroup.get_selected_waregroup(s,waregroup_id);
        if(name!=s && name.length()>0)
        {
            waregroup_id=m_pThread->m_pDbInterface->waregroup_get_id(name,-1);
            if(waregroup_id==-1)
            {//waregroup not exist.
                s=QString("Die Warengruppe '%1' existiert nicht im Hauptverzeichnis.").arg(name);
                QMessageBox msg2(QMessageBox::Question,"","");
                QPushButton * yesButton2=msg2.addButton(QString("Ja"),QMessageBox::YesRole);
                msg2.addButton(QString("Nein"),QMessageBox::NoRole);
                msg2.setWindowTitle("?");
                msg2.setText(s);
                msg2.setInformativeText("Soll diese Warengruppe angelegt werden?");
                msg2.exec();
                if(msg2.clickedButton()==yesButton2)
                {
                    //insert new waregroup in db
                    wg.set_name(name);
                    wg.set_parent_id(-1);//insert intro root dir
                    b=m_pThread->m_pDbInterface->waregroup_add(wg);
                    if(!b)//error
                    {}
                    else
                        m_iNewWaregroupId=wg.get_id();
                }
            }
        }
    }
    return b;
}

bool CInputDialogArticle::get_data(CArticle & ar)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    bool b;
    int i,maker_id=-1,waregroup_id=-1;
    QString name,s;
    unsigned int uiI;
    float fSalesprice,fBaseprice;
    ar.set_name(ui->lineEditName->text());
    ar.set_articlenumber(ui->lineEditArticlenumber->text());
    ar.set_articlenumber2(ui->lineEditArticlenumber_2->text());
    ar.set_valuta(ui->lineEditValuta->text());
    ar.set_location(ui->lineEditLocation->text());
    ar.set_comment(ui->lineEditComment->text());
    ar.set_unit(ui->lineEditUnit->text());
    //inv
    uiI=ui->lineEditInventory->text().toUInt(&b,10);
    if(!b)
        uiI=0;
    ar.set_inventory(uiI);
    //max.inv
    uiI=ui->lineEditMaxInventory->text().toUInt(&b,10);
    if(!b)
        uiI=0;
    ar.set_max_inventory(uiI);
    //-maker data-
    name=ui->comboBox->itemText(ui->comboBox->currentIndex());
    maker_id=m_pThread->m_pDbInterface->maker_get_id(name);
    ar.set_maker_id(maker_id);
    //waregroup
    if(ui->lineEditWaregroup->text().length()<=0)//warergroup not set?
        ar.set_waregroup_id(-1);
    else
    {
        if(m_iNewWaregroupId<0)
            m_dlgWaregroup.get_selected_waregroup(name,waregroup_id);
        else
        {
            name=ui->lineEditWaregroup->text();
            waregroup_id=m_pThread->m_pDbInterface->waregroup_get_id(name,-1);
        }
        ar.set_waregroup_id(waregroup_id);
    }
    //baseprice
    fBaseprice=ui->lineEditBaseprice->text().toFloat(&b);
    if(!b)
        fBaseprice=0.0;
    ar.set_base_price(fBaseprice);
    //salesprice
    fSalesprice=ui->lineEditSalesprice->text().toFloat(&b);
    if(!b)
        fSalesprice=0.0;
    ar.set_sales_price(fSalesprice);
    //warning limit
    if(!ui->checkBoxWarning->isChecked())//not enable?
        ar.set_warning_limit(-1);
    else
    {
        s=ui->lineEditWarningLimit->text();
        i=s.toInt(&b,10);
        if(!b)//error by cast
            ar.set_warning_limit(-1);
        else
            ar.set_warning_limit(i);
    }
    //-
    return true;
}

bool CInputDialogArticle::set_data(CArticle & ar)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    //give it trades from article?
    CPointerMemory memory;
    int id=ar.get_id();
    memory.set_int(&id);
    bool bInvEnable=m_pThread->set_work(WORK_TRADE_GIVE_IT_WITH_ARTICLE,&memory);
    memory.clear();
    if(bInvEnable)//mirror
        bInvEnable=false;
    else
        bInvEnable=true;

    QString s;
    ui->lineEditValuta->setText(ar.get_valuta());
    ui->lineEditName->setText(ar.get_name());
    ui->lineEditArticlenumber->setText(ar.get_articlenumber());
    ui->lineEditArticlenumber_2->setText(ar.get_articlenumber2());
    ui->lineEditComment->setText(ar.get_comment());
    ui->lineEditLocation->setText(ar.get_location());
    ui->lineEditUnit->setText(ar.get_unit());

    //inv.
    s=QString("%1").arg(ar.get_inventory());
    ui->lineEditInventory->setText(s);
    ui->lineEditInventory->setEnabled(bInvEnable);
    //max.inv.
    if(ar.get_max_inventory()>0)
    {
        s=QString("%1").arg(ar.get_max_inventory());
        ui->lineEditMaxInventory->setText(s);
    }
    //baseprice
    if(ar.get_base_price()>0.0)
    {
        s=QString("%1").arg(ar.get_base_price());
        ui->lineEditBaseprice->setText(s);
    }
    //salesprice
    if(ar.get_sales_price()>0.0)
    {
        s=QString("%1").arg(ar.get_sales_price());
        ui->lineEditSalesprice->setText(s);
    }
    //maker
    s=m_pThread->m_pDbInterface->maker_get_name(ar.get_maker_id());
    for(int i=0;i<ui->comboBox->count();i++)
    {
        if(s==ui->comboBox->itemText(i))
        {//found
            ui->comboBox->setCurrentIndex(i);
            break;
        }
    }
    //waregroup
    m_dlgWaregroup.get_selected_waregroup(s,id);
    ui->lineEditWaregroup->setText(s);
    //warning limit
    if(ar.get_warning_limit()>=0)
    {//set?
        s=QString("%1").arg(ar.get_warning_limit());
        ui->lineEditWarningLimit->setText(s);
        ui->lineEditWarningLimit->setEnabled(true);
        ui->checkBoxWarning->setChecked(true);
    }
    else
    {
        ui->lineEditWarningLimit->setEnabled(false);
        ui->checkBoxWarning->setChecked(false);
    }
    //-
    ui->buttonBox->setEnabled(true);
    return true;
}

bool CInputDialogArticle::checkbox_warning(void)
{
    if(ui->checkBoxWarning->isChecked())
        ui->lineEditWarningLimit->setEnabled(true);
    else
        ui->lineEditWarningLimit->setEnabled(false);
    check_user_input();
    return true;
}

bool CInputDialogArticle::check_user_input(void)
{
    int i,count=-1,id;
    unsigned int uiInput=0,uiMaxInventory=0;
    bool bWarning=false,b=true;
    QString s3,s2,s,sError;
    QPalette paRed=ui->labelError->palette();
    QPalette paYellow=paRed;
    paRed.setColor(QPalette::WindowText,QColor(170,0,0));
    paYellow.setColor(QPalette::WindowText,QColor(200,200,0));
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);

    //article-name
    if(m_pThread!=NULL)
    {
        s2=ui->lineEditName->text();
        if(s2.length()<=0)
        {
            b=false;
            sError=QString("Fehler: Artikelbezeichnung fehlt");
        }
        else
        {
            s=QString("name='%1'").arg(s2);
            m_pThread->set_work(WORK_ARTICLE_GET_COUNT,&memory);
            if(count>0 && s2!=m_sMarkArticleName)
            {
                b=false;
                sError=QString("Fehler: Artikel mit dieser Bezeichnung existiert schon");
            }
        }
    }

    //limit
    if(ui->checkBoxWarning->isChecked() && b==true)
    {
        s=ui->lineEditWarningLimit->text();
        if(s.length()>0)
            s.toInt(&b);
        if(!b)
            sError=QString("Fehler: Warnlimit muss eine Zahl sein");
    }

    //saleprice
    if(b)
    {
        s=ui->lineEditSalesprice->text();
        if(s.length()>0)
            s.toFloat(&b);
        if(!b)
            sError=QString("Fehler: Verkaufspreis muss eine Zahl sein");
    }

    //baseprice
    if(b)
    {
        s=ui->lineEditBaseprice->text();
        if(s.length()>0)
            s.toFloat(&b);
        if(!b)
            sError=QString("Fehler: Einkaufspreis muss eine Zahl sein");
    }

    //max inv.
    if(b)
    {
        s=ui->lineEditMaxInventory->text();
        if(s.length()>0)
            uiMaxInventory=s.toUInt(&b);
        if(!b)
            sError=QString("Fehler: max.Lagerkapazität muss eine Zahl sein");
    }

    //inventory
    if(b)
    {
        s=ui->lineEditInventory->text();
        if(s.length()>0)//input?
        {
            uiInput=s.toUInt(&b,10);
            if(!b)
                sError=QString("Fehler: Bestand muss eine Zahl sein");
            else
            {
                s=ui->lineEditMaxInventory->text();
                if(s.length()>0)
                    uiMaxInventory=s.toUInt(&b);
                if(b)
                {
                    if(uiInput>uiMaxInventory)
                    {
                        sError=QString("Fehler: Bestand muss kleiner sein als die Lagerkapazität");
                        b=false;
                    }
                }
            }
        }
    }

    //warnings
    if(b && m_pThread!=NULL)
    {
        //maker
        s2=ui->comboBox->itemText(ui->comboBox->currentIndex());
        s=QString("name='%1'").arg(s2);
        m_pThread->set_work(WORK_MAKER_GET_COUNT,&memory);
        if(!s2.isEmpty())
        {
            if(count<=0)//not found
            {
                sError=QString("Warnung: Hersteller existiert nicht");
                bWarning=true;
            }
        }

        //waregroup
        if(!bWarning)
        {
            s2=ui->lineEditWaregroup->text();
            m_dlgWaregroup.get_selected_waregroup(s3,id);
            if(s2!=s3 && s2.length()>0)
            {
                s=QString("name='%1' AND parent_id=-1").arg(s2);
                m_pThread->set_work(WORK_WAREGROUP_GET_COUNT,&memory);
                if(count<=0)
                {
                    sError=QString("Warnung: Warengruppe existiert nicht im Hauptverzeichnis");
                    bWarning=true;
                }
            }
        }

        //articlenumbers
        if(!bWarning)
        {
            s2=ui->lineEditArticlenumber->text();
            if(s2.length()>0)
            {
                if(m_sMarkArticleNumber1!=s2)//edit?
                    i=0;
                else
                    i=1;
                s=QString("articlenumber='%1'").arg(s2);
                m_pThread->set_work(WORK_ARTICLE_GET_COUNT,&memory);
                if(count>i)//articlenumber found? (edit ? >1)
                {
                    sError=QString("Warnung: 1.Artikelnummer ist bereits vergeben");
                    bWarning=true;
                }
            }
        }
        if(!bWarning)
        {
            s2=ui->lineEditArticlenumber_2->text();
            if(s2.length()>0)
            {
                if(m_sMarkArticleNumber2!=s2)//edit?
                    i=0;
                else
                    i=1;
                s=QString("articlenumber2='%1'").arg(s2);
                m_pThread->set_work(WORK_ARTICLE_GET_COUNT,&memory);
                if(count>i)//articlenumber2 found? (edit ? >1)
                {
                    sError=QString("Warnung: 2.Artikelnummer ist bereits vergeben");
                    bWarning=true;
                }
            }
        }
    }

    //-
    if(b==true && bWarning==false)
        ui->labelError->setText("");
    else if(sError.length()>0)
    {
        if(!bWarning)
        {//error
            ui->labelError->setPalette(paRed);//text color red
            ui->labelError->setText(sError);
        }
        else
        {//warning
            ui->labelError->setPalette(paYellow);//text color yellow
            ui->labelError->setText(sError);
        }
    }
    ui->buttonBox->setEnabled(b);
    return true;
}
