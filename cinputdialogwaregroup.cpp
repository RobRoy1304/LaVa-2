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

#include "cinputdialogwaregroup.h"
#include "ui_cinputdialogwaregroup.h"

CInputDialogWaregroup::CInputDialogWaregroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogWaregroup)
{
    ui->setupUi(this);
    m_iWaregroupId=-1;
    m_pThread=NULL;
    //-
    ui->treeWidgetParents->hideColumn(2);//id column
    ui->treeWidgetParents->hideColumn(3);//parent_id column
    ui->treeWidgetParents->setSortingEnabled(false);
    ui->lineEditInpDia_name->setFocus();
    settings(false);//load & set settings
    //-
    connect(ui->lineEditInpDia_name,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->treeWidgetParents,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(treewidget_clicked(QTreeWidgetItem*)));
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
    m_iWaregroupId=wg.get_id();//for edit mode
    ui->lineEditInpDia_name->setText(wg.get_name());//set name
    ui->lineEditInpDia_comment->setText(wg.get_comment());//set comment
    update_waregroup_tree(wg.get_parent_id());//fill tree and select parent
    update_parent_label(wg.get_parent_id());//update path
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

void CInputDialogWaregroup::changeEvent(QEvent *e)
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

void CInputDialogWaregroup::update_waregroup_tree(int iSelectId)
{
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            if(!m_pThread->m_pWidgets->waregroup_update_treewidget(ui->treeWidgetParents,iSelectId))
            {}
        }
    }
}

bool CInputDialogWaregroup::check_waregroupname(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;
    //-
    QString s=ui->lineEditInpDia_name->text();//get name
    if(s.length()<=0)
    {
        ui->label_warning->setText("Fehler: Bezeichnung fehlt");
        return false;
    }
    //-
    bool b=true;
    int id;
    QList<QTreeWidgetItem*> ls=ui->treeWidgetParents->selectedItems();
    if(ls.count()<=0)//nothing selected?
        id=-1;//root dir id
    else
        id=ls[0]->text(2).toInt(&b,10);//get id
    if(!b)//number?
        return false;
    //-
    int max=0;
    CWaregroup wg;
    if(m_iWaregroupId>0)//edit mode?
    {
        if(m_pThread->m_pDbInterface->waregroup_get(m_iWaregroupId,wg))
        {
            if(s==wg.get_name() && id==wg.get_parent_id())
                max=1;
        }
    }
    //-
    int count=0;
    QList<QTreeWidgetItem*> ls2;
    if(id==-1)//root dir?
    {
        ls2=ui->treeWidgetParents->findItems(s,Qt::MatchExactly,0);//search items with name
        count=ls2.count();
        ls2.clear();
    }
    else//child?
    {
        for(int i=0;i<ls[0]->childCount();i++)
        {
            if(ls[0]->child(i)->text(0)==s)
                count++;
        }
    }
    //-
    if(count>max)
    {//found?
        if(id==-1)
            ui->label_warning->setText("Fehler: Warengruppe gibt es schon im 'Hauptverzeichnis'");
        else if(ls.count()>0)
        {
            s=QString("Fehler: Warengruppe gibt es schon in '%1'").arg(ls[0]->text(0));
            ui->label_warning->setText(s);
        }
        b=false;
    }
    if(b)
        ui->label_warning->setText("");
    //-
    return b;
}

bool CInputDialogWaregroup::check_user_input()
{
    bool b=check_waregroupname();
    ui->buttonBox->setEnabled(b);
    return true;
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
    if(pItem!=NULL && m_iWaregroupId>0 && m_pThread!=NULL)
    {
        id=pItem->text(2).toInt(&b,10);//get id
        if(b)//number?
        {
            if(id==m_iWaregroupId)//yourself?
            {
                s=QString("Fehler: Die Warengruppe kann keine Untergruppe von sich selbst sein");
                b=false;
            }
            else
            {
                //is subwaregroup of yourself?
                memory.set_int(&m_iWaregroupId);
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
            ui->buttonBox->setEnabled(b);
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

