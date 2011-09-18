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

#include "cinputdialogbrowsewaregroup.h"
#include "ui_cinputdialogbrowsewaregroup.h"

CInputdialogBrowseWaregroup::CInputdialogBrowseWaregroup(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CInputdialogBrowseWaregroup)
{
    m_ui->setupUi(this);
    m_pThread=NULL;
    settings(false);//load & set settings
    m_ui->treeWidget->hideColumn(2);//id column
    m_ui->treeWidget->hideColumn(3);//parent_id column
    m_ui->treeWidget->setSortingEnabled(false);
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputdialogBrowseWaregroup::~CInputdialogBrowseWaregroup()
{
    settings(true);//save settings if update
    delete m_ui;
}

void CInputdialogBrowseWaregroup::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool CInputdialogBrowseWaregroup::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;
    lsSType.push_back(QString("DLG_WAREGROUP_TREE_BROWSE"));
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //tree width
            if(lsSValue.count()>0)
                settings.set_tree_width(m_ui->treeWidget,lsSValue[0],500);
        }
        if(bUpdate)//write
        {
            //tree width
            if(lsSValue.count()>0)
            {
                if(settings.get_tree_width(m_ui->treeWidget,lsSValue[0]))//setting change?
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

bool CInputdialogBrowseWaregroup::set_thread(CWorkThread * pThread)
{
    m_pThread=pThread;
    return true;
}

bool CInputdialogBrowseWaregroup::get_selected_waregroup(QString & s, int & id)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    bool b=false;
    s=QString("");
    QList <QTreeWidgetItem*> ls=m_ui->treeWidget->selectedItems();
    if(ls.count()==0)
        id=-1;
    else
    {
        id=ls[0]->text(2).toInt(&b,10);// get id
        if(b)//it's number?
            s=m_pThread->m_pDbInterface->waregroup_get_path(id);//set waregroup path
    }
    return b;
}

bool CInputdialogBrowseWaregroup::updateWaregroup(int iIdSelect)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;
    //-
    if(!m_pThread->m_pWidgets->waregroup_update_treewidget(m_ui->treeWidget,iIdSelect))
    {//error
    }
    return true;
}

