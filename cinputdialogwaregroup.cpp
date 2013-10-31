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

#include "cinputdialogwaregroup.h"
#include "ui_cinputdialogwaregroup.h"

CInputDialogWaregroup::CInputDialogWaregroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogWaregroup)
{
    ui->setupUi(this);
    m_iMarkId=m_iMarkParentId=-1;
    m_pThread=NULL;
    //-
    ui->pushButtonOk->setEnabled(false);
    ui->treeWidgetParents->hideColumn(2);//id column
    ui->treeWidgetParents->hideColumn(3);//parent_id column
    ui->treeWidgetParents->setSortingEnabled(false);
    ui->lineEditInpDia_name->setFocus();
    settings(false);//load & set settings
    //-
    connect(ui->lineEditInpDia_name,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->treeWidgetParents,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(treewidget_clicked(QTreeWidgetItem*)));
    connect(ui->pushButtonOk,SIGNAL(clicked()),this,SLOT(press_ok()));
    connect(ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(press_cancel()));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDialogWaregroup::~CInputDialogWaregroup()
{
    settings(true);//save settings if update
    delete ui;
}

void CInputDialogWaregroup::set_thread(CWorkThread * pThread)
{
    m_pThread=pThread;
}

void CInputDialogWaregroup::set_data(CWaregroup & wg)
{
    //for edit mode
    m_iMarkId=wg.get_id();
    m_iMarkParentId=wg.get_parent_id();
    m_sMarkName=wg.get_name();

    ui->lineEditInpDia_name->setText(wg.get_name());//set name
    ui->lineEditInpDia_comment->setText(wg.get_comment());//set comment
    update_waregroup_tree(wg.get_parent_id());//fill tree and select parent
    update_parent_label(wg.get_parent_id());//update path
    ui->pushButtonOk->setText(QString("Änderung(en) anwenden"));
    ui->pushButtonOk->setEnabled(true);
}

void CInputDialogWaregroup::get_data(CWaregroup & wg)
{
    wg.set_name(ui->lineEditInpDia_name->text());//get name
    wg.set_comment(ui->lineEditInpDia_comment->text());//get comment

    //parent
    QList<QTreeWidgetItem*> ls=ui->treeWidgetParents->selectedItems();
    int id;
    bool b;
    if(ls.count()<=0)
        wg.set_parent_id(-1);//parent is root dir
    else
    {
        id=ls[0]->text(2).toInt(&b,10);
        if(b)//number?
            wg.set_parent_id(id);
    }
    ls.clear();
}

void CInputDialogWaregroup::update_waregroup_tree(int iSelectId)
{
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
            m_pThread->m_pWidgets->waregroup_update_treewidget(ui->treeWidgetParents,iSelectId);
    }
}

void CInputDialogWaregroup::update_parent_label(int iId)
{
    QString s;
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            if(iId<=0)
                ui->label_SelectedParent->setText("Hauptverzeichnis");
            else
            {
                s=m_pThread->m_pDbInterface->waregroup_get_path(iId);
                ui->label_SelectedParent->setText(s);
            }
        }
    }
}

void CInputDialogWaregroup::update_parent_label(void)
{
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            bool b;
            QString s;
            int id=-1;
            QList <QTreeWidgetItem*> ls=ui->treeWidgetParents->selectedItems();
            if(ls.count()<=0)//nothing selected
                ui->label_SelectedParent->setText("Hauptverzeichnis");
            else//is selected
            {
                id=ls[0]->text(2).toInt(&b,10);
                if(b)
                {
                    s=m_pThread->m_pDbInterface->waregroup_get_path(id);
                    ui->label_SelectedParent->setText(s);
                }
            }
        }
    }
}

bool CInputDialogWaregroup::check_waregroupname(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    int i,max=0,iParentId=-1;
    bool b,bReturn=true;
    QString s,sError,sInput=ui->lineEditInpDia_name->text();//get input name
    QList<QTreeWidgetItem*> ls=ui->treeWidgetParents->selectedItems();//get selected
    //-
    if(ls.count()>0)//selected?
    {
        iParentId=ls[0]->text(2).toInt(&b,10);//get parent-id
        if(!b)//number?
            iParentId=-1;
    }
    //-
    if(sInput.length()<=0)
    {
        sError=QString("Fehler: Bezeichnung fehlt");
        bReturn=false;
    }
    else
    {
        if(sInput==m_sMarkName && iParentId==m_iMarkParentId)
            max=1;//edit name & p.id same
        s=QString("name = '%1' AND ").arg(sInput);
        s+=QString("parent_id = %1").arg(iParentId);
        i=m_pThread->m_pDbInterface->waregroup_get_count(s);
        if(i>max)
        {
            if(iParentId==-1)
                sError=QString("Fehler: Warengruppe gibt es schon im Hauptverzeichnis");
            else
                sError=QString("Fehler: Warengruppe gibt es schon in diesem Unterverzeichnis");
            bReturn=false;
        }
    }
    //-
    if(sError.length()>0)
        ui->label_warning->setText(sError);
    else
        ui->label_warning->setText(QString(""));
    //-
    ls.clear();
    return bReturn;
}

bool CInputDialogWaregroup::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;
    lsSType.push_back(QString("DLG_WAREGROUP_TREE"));
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //tree width
            if(lsSValue.count()>0)
                settings.set_tree_width(ui->treeWidgetParents,lsSValue[0],500);
        }
        if(bUpdate)//write
        {
            //tree width
            if(lsSValue.count()>0)
            {
                if(settings.get_tree_width(ui->treeWidgetParents,lsSValue[0]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[0]);
                    lsSUpdateValue.push_back(lsSValue[0]);
                }
            }

            //write updates
            if(lsSUpdateType.count()>0 && lsSUpdateType.count()==lsSUpdateValue.count())
                b=settings.write(lsSUpdateType,lsSUpdateValue);
        }
    }
    //-
    lsSValue.clear();
    lsSType.clear();
    lsSUpdateValue.clear();
    lsSUpdateType.clear();
    return b;
}

bool CInputDialogWaregroup::check_user_input()
{
    bool b=check_waregroupname();
    ui->pushButtonOk->setEnabled(b);
    return true;
}

bool CInputDialogWaregroup::treewidget_clicked(QTreeWidgetItem* pItem)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    CPointerMemory memory;
    QList<int> lsInt;
    bool b=true;
    QString s;
    static QTreeWidgetItem * pMarkItem=NULL;
    int i,id=-1;

    //check edit mode and selected yourself or subwaregroups of yourself?
    if(pItem!=NULL && m_iMarkId>0 && m_pThread!=NULL)
    {
        id=pItem->text(2).toInt(&b,10);//get id
        if(b)//number?
        {
            if(id==m_iMarkId)//yourself?
            {
                s=QString("Fehler: Die Warengruppe kann keine Untergruppe von sich selbst sein");
                b=false;
            }
            else
            {
                //is subwaregroup of yourself?
                memory.set_int(&m_iMarkId);
                memory.set_int_list(&lsInt);
                if(!m_pThread->set_work(WORK_WAREGROUP_GET_ALL_SUBWAREGROUPS,&memory))
                {
                }
                else
                {
                    for(i=0;i<lsInt.count();i++)
                    {
                        if(lsInt[i]==id)//found?
                        {
                            b=false;
                            s=QString("Fehler: Die Warengruppe kann keine Untergruppe einer Untergruppe von sich selbst sein");
                            break;
                        }
                    }
                }
            }
            //-
            ui->label_warning->setText(s);
            ui->pushButtonOk->setEnabled(b);
        }
    }

    //select / un-select
    if(b)//no error
    {
        if(pMarkItem!=pItem)
        {//selected
            pMarkItem=pItem;
            pItem->setSelected(true);
        }
        else if(pMarkItem==pItem)
        {//deselected
            pMarkItem=NULL;
            pItem->setSelected(false);
        }
        update_parent_label();
        check_waregroupname();
    }
    return true;
}

void CInputDialogWaregroup::press_ok(void)
{
    int i;
    QString s;
    bool bOk=true;
    QMessageBox msg(QMessageBox::Information,"","");
    msg.setWindowTitle("!");
    msg.setText("Der Datensatz wurde von einer anderen Programminstanz gelöscht oder die Struktur verändert!");
    //-
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            //check name
            if(!check_waregroupname())
                bOk=false;

            //another client delete this record?
            if(m_iMarkId>0)//edit-mode?
            {
                s=QString("id = %1").arg(m_iMarkId);
                s+=QString(" AND name = '%1' AND ").arg(m_sMarkName);
                s+=QString("parent_id = %1").arg(m_iMarkParentId);
                i=m_pThread->m_pDbInterface->waregroup_get_count(s);
                if(i<=0)//record delete?
                {
                    msg.exec();
                    bOk=false;
                }
            }
        }
    }
    //-
    if(bOk)
        done(1);
    else
        close();
}

void CInputDialogWaregroup::press_cancel(void)
{
    close();
}
