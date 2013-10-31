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

#include "cwidgetinterface.h"

//basic--------------------------------------------------------------------------------

CWidgetInterface::CWidgetInterface()
{
    m_pDb=NULL;
    load_icons();
}

CWidgetInterface::~CWidgetInterface()
{
    m_lsIconsArticle.clear();
    m_lsIconsTrade.clear();
}

bool CWidgetInterface::set_db(CDbConnection * pDb)
{
    m_pDb=pDb;
    return true;
}

int CWidgetInterface::check_position(QTableWidget * pTable, int iPosition)
{
    if(pTable==NULL || m_pDb==NULL)
        return false;
    //-
    int row=iPosition;
    if(iPosition==ITEM_POSITION_TOP)
        row=0;
    else if(iPosition==ITEM_POSITION_BOTTOM)
    {
        row=pTable->rowCount();
        if(row<0)
            row=0;
        else{}
    }
    else
    {
        if(iPosition<0)
            row=0;
        else if(iPosition>=pTable->rowCount())
            row=pTable->rowCount()-1;
    }
    return row;
}

int CWidgetInterface::check_position(QListWidget * pList, int iPosition)//for lists
{
    if(pList==NULL || m_pDb==NULL)
        return false;
    //-
    int row=iPosition;
    if(iPosition==ITEM_POSITION_TOP)
        row=0;
    else if(iPosition==ITEM_POSITION_BOTTOM)
    {
        row=pList->count();
        if(row<0)
            row=0;
        else{}
    }
    else
    {
        if(iPosition<0)
            row=0;
        else if(iPosition>=pList->count())
            row=pList->count()-1;
    }
    return row;
}

bool CWidgetInterface::load_icons(void)
{
    QIcon icon;
    QString sIconDir;
    //-article icons-
    for(int i=1;i<=7;i++)
    {
        sIconDir=QString(":/images/res/%1.png").arg(i);
        icon=QIcon(sIconDir);
        m_lsIconsArticle.push_back(icon);
    }
    sIconDir=QString(":/images/res/unknow.png");
    m_lsIconsArticle.push_back(QIcon(sIconDir));
    sIconDir=QString(":/images/res/empty.png");
    m_lsIconsArticle.push_back(QIcon(sIconDir));

    //-trade icons-
    sIconDir=QString(":/images/res/incoming.png");
    m_lsIconsTrade.push_back(QIcon(sIconDir));
    sIconDir=QString(":/images/res/outgoing.png");
    m_lsIconsTrade.push_back(QIcon(sIconDir));
    sIconDir=QString(":/images/res/ordering.png");
    m_lsIconsTrade.push_back(QIcon(sIconDir));
    sIconDir=QString(":/images/res/customer.png");
    m_lsIconsTrade.push_back(QIcon(sIconDir));
    sIconDir=QString(":/images/res/incoming_cross.png");
    m_lsIconsTrade.push_back(QIcon(sIconDir));
    sIconDir=QString(":/images/res/outgoing_cross.png");
    m_lsIconsTrade.push_back(QIcon(sIconDir));
    sIconDir=QString(":/images/res/ordering_cross.png");
    m_lsIconsTrade.push_back(QIcon(sIconDir));
    sIconDir=QString(":/images/res/customer_cross.png");
    m_lsIconsTrade.push_back(QIcon(sIconDir));

    //warning icon
    sIconDir=QString(":/images/res/warning.png");
    m_icoWarning=QIcon(sIconDir);
    //-
    return true;
}

bool CWidgetInterface::insert_list_element(QListWidget * pList, QString & sItem, QString & sData,bool bSpaceRow)
{
    if(pList==NULL)
        return false;
    //-
    QFont font;
    QListWidgetItem * pItem=NULL;
    //-
    font=pList->font();
    font.setBold(true);
    //-
    pList->addItem(sItem);
    pItem=pList->item(pList->count()-1);
    if(pItem!=NULL)
        pItem->setFont(font);
    if(sData.length()!=0)
        pList->addItem(sData);
    if(bSpaceRow)
        pList->addItem(QString(""));
    return true;
}

bool CWidgetInterface::format_and_check_date(QString s, QDate & dt)
{
    int mo[]={31,28,31,30,31,30,31,31,30,31,30,31};
    int d=0,m=0,y=0;
    QStringList ls;
    bool b=false;
    QString sFormat;
    //-
    ls=s.split(".");
    if(ls.count()==3)
    {
        d=ls[0].toInt(&b,10);
        if(b)
        {
            m=ls[1].toInt(&b,10);
            if(b)
                y=ls[2].toInt(&b,10);
        }
    }
    //-
    if(b)
    {
        b=false;
        if(y>=2009 && y<=2999)
        {
            if(dt.isLeapYear(y))
                mo[1]=29;
            if(m>0 && m<=12)
            {
                if(d>0 && d<=mo[m-1])
                    b=true;//all right
            }
        }
    }
    if(b)
        dt=QDate(y,m,d);
    //-
    return b;
}

QString CWidgetInterface::check_date(QString s)
{
    int mo[]={31,28,31,30,31,30,31,31,30,31,30,31};
    int d=0,m=0,y=0;
    QStringList ls;
    bool b=false;
    QDate dt;
    QString sFormat;
    //-
    ls=s.split(".");
    if(ls.count()==3)
    {
        d=ls[0].toInt(&b,10);
        if(b)
        {
            m=ls[1].toInt(&b,10);
            if(b)
                y=ls[2].toInt(&b,10);
        }
    }
    //-
    if(b)
    {
        b=false;
        if(y>0 && y<10000)
        {
            if(dt.isLeapYear(y))
                mo[1]=29;
            if(m>0 && m<=12)
            {
                if(d>0 && d<=mo[m-1])
                    b=true;//all right
            }
        }
    }
    if(b)
    {
        sFormat=QString("%1-").arg(y);
        if(m<10)
            sFormat+=QString("0%1-").arg(m);
        else
            sFormat+=QString("%1-").arg(m);
        if(d<10)
            sFormat+=QString("0%1").arg(d);
        else
            sFormat+=QString("%1").arg(d);
    }
    //-
    return sFormat;
}

bool CWidgetInterface::cast_count_and_id(QString sData, int & iCount, int & iId)
{
    QStringList ls;
    bool b=false;
    //-
    if(sData.length()>0)
    {
        ls=sData.split("x");
        if(ls.count()==2)
        {
            iCount=ls[0].toInt(&b,10);
            if(b)
                iId=ls[1].toInt(&b,10);
        }
    }
    //-
    return b;
}

bool CWidgetInterface::get_table_column_width(QTableWidget * pTable, QList<int> & lsIntWidth)
{
    if(pTable==NULL)
        return false;
    for(int i=0;i<pTable->columnCount();i++)
        lsIntWidth.push_back(pTable->columnWidth(i));
    return true;
}

bool CWidgetInterface::get_tree_column_width(QTreeWidget * pTree, QList<int> & lsIntWidth)
{
    if(pTree==NULL)
        return false;
    for(int i=0;i<pTree->columnCount();i++)
        lsIntWidth.push_back(pTree->columnWidth(i));
    return true;
}

QIcon CWidgetInterface::get_warning_icon(void)
{
    return m_icoWarning;
}

bool CWidgetInterface::get_trade_icons(QList<QIcon> & lsIcons)
{
    for(int i=0;i<m_lsIconsTrade.count();i++)
        lsIcons.push_back(m_lsIconsTrade[i]);
    return true;
}

bool CWidgetInterface::set_block_signal_and_sort(QTableWidget * pTable, bool bBlock, bool bSort)
{
    if(pTable==NULL)
        return false;
    //-
    if(pTable->signalsBlocked()!=bBlock)
        pTable->blockSignals(bBlock);
    if(pTable->isSortingEnabled()!=bSort)
        pTable->setSortingEnabled(bSort);
    //-
    return true;
}

bool CWidgetInterface::remove_all_rows(QTableWidget * pTable)
{
    if(pTable==NULL)
        return false;
    //-clear-
    for(int i=pTable->rowCount()-1;i>=0;i--)
        pTable->removeRow(i);//delete all rows
    return true;
}

bool CWidgetInterface::remove_row(QTableWidget * pTable,int iId,int iIdColumn)
{
    if(iId<0)
        return false;
    QString s=QString("%1").arg(iId);
    bool b=remove_row(pTable,s,iIdColumn);
    return b;
}

bool CWidgetInterface::remove_row(QTableWidget * pTable,QString sValue,int iIdColumn)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    set_block_signal_and_sort(pTable,true,false);//faster
    int row=find_row(pTable,sValue,iIdColumn);
    if(row!=-1)//found
    {
        pTable->removeRow(row);
        b=true;
    }
    set_block_signal_and_sort(pTable,false,true);
    return b;
}

int CWidgetInterface::find_row(QTableWidget * pTable,int iId,int iIdColumn)
{
    if(iId<0)
        return -1;
    QString s=QString("%1").arg(iId);
    int iReturn=find_row(pTable,s,iIdColumn);
    return iReturn;
}

int CWidgetInterface::find_row(QTableWidget * pTable,QString sValue,int iIdColumn)
{
    if(pTable==NULL || sValue.length()<=0)
        return -1;
    if(iIdColumn >= pTable->columnCount())
        return -1;

    if(iIdColumn<0)//if set -1 -> last column
        iIdColumn=pTable->columnCount()-1;

    QString s;
    int iReturn=-1,i,rows=pTable->rowCount();
    for(i=0;i<rows;i++)
    {
        s=pTable->item(i,iIdColumn)->text();
        if(s==sValue)
        {
            iReturn=i;//found
            break;
        }
    }
    //-
    return iReturn;
}

bool CWidgetInterface::find_and_select_row(QTableWidget * pTable, int iId, int iColumn)
{
    if(iId<0)
        return false;
    QString s=QString("%1").arg(iId);
    bool b=find_and_select_row(pTable,s,iColumn);
    return b;
}

bool CWidgetInterface::find_and_select_row(QTableWidget * pTable, QString sValue, int iColumn)
{
    bool b=false;
    int row=find_row(pTable,sValue,iColumn);
    if(row!=-1)
        b=select_row(pTable,sValue,iColumn);
    return b;
}

bool CWidgetInterface::select_row(QTableWidget * pTable,int iId,int iIdColumn)
{
    QString s=QString("%1").arg(iId);
    bool b=select_row(pTable,s,iIdColumn);
    return b;
}

bool CWidgetInterface::select_row(QTableWidget * pTable,QString sValue,int iIdColumn)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int row=find_row(pTable,sValue,iIdColumn);
    if(row!=-1)//found
    {
        pTable->setCurrentItem(pTable->item(row,0));
        b=true;
    }
    return b;
}

bool CWidgetInterface::get_selected_table_item_value(QTableWidget *pTable,int & iValue,int iColumn)
{
    bool b=false;
    iValue=-1;
    QString s;
    if(get_selected_table_item_value(pTable,s,iColumn))
    {
        iValue=s.toInt(&b,10);
        if(!b)
            iValue=-1;
        else
            b=true;
    }
    return b;
}

bool CWidgetInterface::get_selected_table_item_value(QTableWidget *pTable,QString & sValue,int iColumn)
{
    bool b=false;
    QList<QTableWidgetItem*> ls;
    if(pTable!=NULL)
    {
        if(iColumn==-1)//last column?
            iColumn=pTable->columnCount()-1;
        ls=pTable->selectedItems();
        if(iColumn>=0 && iColumn<ls.count() && ls.count()>0)
        {
            sValue=ls[iColumn]->text();
            b=true;
        }
    }
    return b;
}

bool CWidgetInterface::mod_row(QTableWidget * pTable, QList<CTableItemData> & lsData, int iPosition, bool bBlock, bool bUpdate, int iSelectId,int iIdColumn)
{
    QString s;
    if(iSelectId>=0)
        s=QString("%1").arg(iSelectId);
    bool b=mod_row(pTable,lsData,iPosition,bBlock,bUpdate,s,iIdColumn);
    return b;
}

bool CWidgetInterface::mod_row(QTableWidget * pTable, QList<CTableItemData> & lsData, int iPosition, bool bBlock, bool bUpdate, QString sSelectValue,int iIdColumn)
{
    if(pTable==NULL || lsData.count()<=0)
        return false;
    //-
    bool b=false;
    if(bBlock)
        set_block_signal_and_sort(pTable,true,false);//block->faster
    int row=check_position(pTable,iPosition);
    if(row>=0)
    {
        if(!bUpdate)//new row?
            pTable->insertRow(row);
        fill_row(pTable,row,lsData);
        b=true;
    }
    //-
    if(bBlock)
        set_block_signal_and_sort(pTable,false,true);//unblock
    if(sSelectValue.length()>0)//do select?
        select_row(pTable,sSelectValue,iIdColumn);
    //-
    return b;
}

bool CWidgetInterface::fill_row(QTableWidget * pTable,int iRow,QList<CTableItemData> & lsData)
{
    if(pTable==NULL || lsData.count()<=0)
        return false;
    if(iRow<0||iRow>=pTable->rowCount())
        return false;
    //-
    QIcon ico;
    int iColumn;
    bool b=true;
    CMyTableWidgetItem * pItem=NULL;
    //-
    for(iColumn=0;iColumn<pTable->columnCount() && iColumn<lsData.count();iColumn++)
    {
        pItem=new CMyTableWidgetItem;
        if(pItem!=NULL)
        {
            pItem->setText(lsData[iColumn].get_text());
            if(lsData[iColumn].get_alignment()==TABLE_ALIGNMENT_LEFT)
                pItem->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            else if(lsData[iColumn].get_alignment()==TABLE_ALIGNMENT_RIGHT)
                pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            else
                pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            //-
            //icon?
            ico=lsData[iColumn].get_icon();
            if(!ico.isNull())
                pItem->setIcon(ico);
            //-
            pTable->setItem(iRow,iColumn,pItem);
        }
        else
        {//error
            b=false;
            break;
        }
    }
    return b;
}

bool CWidgetInterface::update_table_by_tablecolumnsdata(CTableColumnsData & tcdOld,CTableColumnsData & tcdNew, QList<QString> & lsHeaderNames)
{
    int i,pos,w,o;
    bool bReturn=false;
    QTableWidget * pTable=tcdOld.get_table();

    if(tcdOld.get_table()!=NULL && tcdNew.get_table()==tcdOld.get_table() && lsHeaderNames.count()>0)
    {
        if(tcdOld!=tcdNew)
        {//settings updated?

            //swap columns widths(old & new pos)
            for(i=0;i<pTable->columnCount();i++)
            {
                w=pTable->columnWidth(i);
                o=tcdOld.get_column_order(i);
                if(o>=0 && o<pTable->columnCount())//index in range?
                {
                    for(pos=0;pos<pTable->columnCount();pos++)//find new pos
                    {
                        if(tcdNew.get_column_order(pos)==o)
                            break;
                    }
                    if(pos>=0 && pos<pTable->columnCount())//found
                    {
                        if(tcdNew.get_column_visible(pos)==false)//not visible?
                            w=0;
                        else
                        {
                            if(w<=0)//now visible?
                                w=100;
                        }
                        pTable->setColumnWidth(pos,w);
                    }
                }
            }

            //update table headers
            for(i=0;i<pTable->columnCount() && i<lsHeaderNames.count();i++)
                pTable->horizontalHeaderItem(i)->setText(lsHeaderNames[i]);

            //-
            bReturn=true;
        }
    }
    return bReturn;
}


//waregroup-----------------------------------------------------------------------------
bool CWidgetInterface::waregroup_update_treewidget(QTreeWidget * pTree, int iSelectedID)
{


    if(pTree==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;

    pTree->blockSignals(true);//faster
    pTree->setSortingEnabled(false);
    waregroup_clear_treewidget(pTree);//clear
    //-
    bool b=true;
    CWaregroup wg;
    QTreeWidgetItem * pItem=NULL;
    QString s=QString("parent_id=-1");//all root dir items
    QList<int> lsIds;
    //-
    if(m_pDb->waregroup_get_all(s,lsIds))
    {
        while(lsIds.count()>0)
        {
            if(!m_pDb->waregroup_get(lsIds[0],wg))//error?
            {
                b=false;
                break;
            }
            //-
            pItem=new QTreeWidgetItem(pTree);
            if(pItem==NULL)
            {
                b=false;
                break;
            }
            //-
            pItem->setText(0,wg.get_name());
            pItem->setText(1,wg.get_comment());
            s=QString("%1").arg(wg.get_id());
            pItem->setText(2,s);
            s=QString("%1").arg(wg.get_parent_id());
            pItem->setText(3,s);
            pTree->addTopLevelItem(pItem);
            //-
            if(wg.get_id()==iSelectedID)
                pTree->setCurrentItem(pItem);
            //-
            waregroup_insert_children_treeitems(pTree, pItem, iSelectedID);//insert children
            //-
            lsIds.removeFirst();//1. element delete
        }
        lsIds.clear();
    }
    //-
    pTree->blockSignals(false);
    //-
    return b;
}

bool CWidgetInterface::waregroup_insert_children_treeitems(QTreeWidget * pTree, QTreeWidgetItem * pParent, int iSelectedID)
{
    if(pParent==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    CWaregroup wg;
    QTreeWidgetItem * pItem=NULL;
    QTreeWidgetItem * pMarkItem=NULL;
    QString s("parent_id=");
    s+=pParent->text(2);
    QList<int> lsIds;
    //-
    if(m_pDb->waregroup_get_all(s,lsIds))
    {
        while(lsIds.count()>0)
        {
            if(!m_pDb->waregroup_get(lsIds[0],wg))//error?
            {
                b=false;
                break;
            }
            //-
            pItem=new QTreeWidgetItem(pParent);
            if(pItem==NULL)
            {
                b=false;
                break;
            }
            pItem->setText(0,wg.get_name());
            pItem->setText(1,wg.get_comment());
            s=QString("%1").arg(wg.get_id());
            pItem->setText(2,s);
            s=QString("%1").arg(wg.get_parent_id());
            pItem->setText(3,s);
            pParent->addChild(pItem);
            //-
            if(wg.get_id()==iSelectedID)
            {
                pTree->setCurrentItem(pItem);
                //open all parents
                for(pMarkItem=pItem->parent();pMarkItem!=NULL;pMarkItem=pMarkItem->parent())
                    pMarkItem->setExpanded(true);
            }
            //-
            waregroup_insert_children_treeitems(pTree, pItem, iSelectedID);//insert children from child
            //-
            lsIds.removeFirst();//delete 1. element
        }
        lsIds.clear();
    }
    return b;
}

bool CWidgetInterface::waregroup_clear_treewidget(QTreeWidget * pTree)
{
    if(pTree==NULL)
        return false;
    //-
    QTreeWidgetItem * pItem;
    int i;
    for(i=pTree->topLevelItemCount()-1;i>=0;i--)
    {
        pItem=pTree->takeTopLevelItem(i);
        if(pItem!=NULL)
            pItem->takeChildren();
    }
    return true;
}


//maker------------------------------------------------------------------------
bool CWidgetInterface::maker_update_tablewidget(QTableWidget * pTable, QList<int> & lsIds, int iSelectedID)
{
    if(pTable==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    CMaker mk;
    QList<CTableItemData> ls;
    //-
    set_block_signal_and_sort(pTable,true,false);//faster
    remove_all_rows(pTable);//delete all rows
    //-
    while(lsIds.count()>0)
    {
        if(!m_pDb->maker_get(lsIds[0],mk))//get maker data
        {
            b=false;
            break;
        }
        else
        {
            maker_format(mk,ls);
            if(!mod_row(pTable,ls,ITEM_POSITION_BOTTOM,false,false,-1))
            {
                b=false;
                break;
            }
            ls.clear();
        }
        lsIds.removeFirst();
    }
    lsIds.clear();
    //-
    set_block_signal_and_sort(pTable,false,true);
    if(iSelectedID!=-1)
        select_row(pTable,iSelectedID);
    return b;
}

bool CWidgetInterface::maker_insert_row(QTableWidget * pTable, CMaker & mk, int iPosition, bool bSelect)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=mk.get_id();
    //-
    maker_format(mk,ls);
    b=mod_row(pTable,ls,iPosition,true,false,iSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::maker_update_row(QTableWidget * pTable, CMaker & mk, bool bSelect, bool bAddIfNotExis)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=mk.get_id();
    //-
    maker_format(mk,ls);
    int row=find_row(pTable,mk.get_id());
    if(row>=0 && row<pTable->rowCount())//found?
        b=mod_row(pTable,ls,row,true,true,iSelect);
    else if(bAddIfNotExis)
        b=maker_insert_row(pTable,mk,-2,bSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::maker_format(CMaker & mk, QList<CTableItemData> & lsData)
{
    QString s;
    CTableItemData ti;

    //column 0
    ti.set_text(mk.get_name());
    lsData.push_back(ti);
    //column 1
    ti.set_text(mk.get_comment());
    lsData.push_back(ti);
    //column 2
    s=QString("%1").arg(mk.get_id());
    ti.set_text(s);
    lsData.push_back(ti);
    //-
    return true;
}

bool CWidgetInterface::maker_update_list(QListWidget * pList, int iID)
{
    if(pList==NULL||m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    QString s,s2;
    CMaker mk;
    //-
    if(iID!=-1)
    {
        if(!m_pDb->maker_get(iID,mk))// get data
            b=false;
    }
    //-
    if(b)
    {
        pList->blockSignals(true);//faster insert
        pList->clear();

        s=QString("Firma:");
        s2=mk.get_name();
        insert_list_element(pList,s,s2);

        s=QString("Adresse:");
        s2=mk.get_adress();
        insert_list_element(pList,s,s2);

        s=QString("Telefonnummer(n):");
        s2=mk.get_callnumber();
        insert_list_element(pList,s,s2);

        s=QString("Faxnummer(n):");
        s2=mk.get_faxnumber();
        insert_list_element(pList,s,s2);

        s=QString("email-Adresse(n):");
        s2=mk.get_email();
        insert_list_element(pList,s,s2);

        s=QString("homepage:");
        s2=mk.get_homepage();
        insert_list_element(pList,s,s2);

        s=QString("Kontaktperson(en):");
        s2=mk.get_contectperson();
        insert_list_element(pList,s,s2);

        s=QString("Kommentar:");
        s2=mk.get_comment();
        insert_list_element(pList,s,s2);
        //-
        pList->blockSignals(false);
    }
    //-
    return b;
}

bool CWidgetInterface::maker_fill_combobox(QComboBox * pCombobox)
{
    if(m_pDb==NULL || pCombobox==NULL)
        return false;

    bool b;
    QString s;
    int count;
    QList<int> ls;
    //-
    s=QString("LOWER (name)");
    b=m_pDb->maker_get_all(QString(""),ls,s);
    //-
    if(b)//no break?
    {
        count=ls.count();
        pCombobox->clear();
        pCombobox->blockSignals(true);//no signals by inserts
        pCombobox->insertItem(0,"");//empty row
        for(int i=0;i<count;i++)
        {
            s=m_pDb->maker_get_name(ls[i]);
            pCombobox->insertItem(i+1,s);
        }
        pCombobox->blockSignals(false);//signals enable
    }
    //-
    ls.clear();
    //-
    return b;
}


//dealer--------------------------------------------------------------------------------------
bool CWidgetInterface::dealer_update_tablewidget(QTableWidget * pTable, QList<int> & lsIds, int iSelectedID)
{
    if(pTable==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    CDealer de;
    QList<CTableItemData> ls;
    //-
    set_block_signal_and_sort(pTable,true,false);//faster
    remove_all_rows(pTable);//delete all rows
    //-
    while(lsIds.count()>0)
    {
        if(!m_pDb->dealer_get(lsIds[0],de))//get data
        {
            b=false;
            break;
        }
        else
        {
            dealer_format(de,ls);
            if(!mod_row(pTable,ls,ITEM_POSITION_BOTTOM,false,false,-1))
            {
                b=false;
                break;
            }
            ls.clear();
        }
        lsIds.removeFirst();
    }
    lsIds.clear();
    //-
    set_block_signal_and_sort(pTable,false,true);
    if(iSelectedID!=-1)
        select_row(pTable,iSelectedID);
    return b;
}

bool CWidgetInterface::dealer_insert_row(QTableWidget * pTable, CDealer & de, int iPosition, bool bSelect)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=de.get_id();
    //-
    dealer_format(de,ls);
    b=mod_row(pTable,ls,iPosition,true,false,iSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::dealer_update_row(QTableWidget * pTable, CDealer & de, bool bSelect, bool bAddIfNotExis)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=de.get_id();
    //-
    dealer_format(de,ls);
    int row=find_row(pTable,de.get_id());
    if(row>=0 && row<pTable->rowCount())//found?
        b=mod_row(pTable,ls,row,true,true,iSelect);
    else if(bAddIfNotExis)
        b=dealer_insert_row(pTable,de,-2,bSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::dealer_format(CDealer &de, QList<CTableItemData> & lsData)
{
    QString s;
    CTableItemData ti;

    //column 0
    ti.set_text(de.get_name());
    lsData.push_back(ti);
    //column 1
    ti.set_text(de.get_comment());
    lsData.push_back(ti);
    //column 2
    s=QString("%1").arg(de.get_id());
    ti.set_text(s);
    lsData.push_back(ti);
    //-
    return true;
}

bool CWidgetInterface::dealer_update_list(QListWidget * pList, int iID)
{
    if(pList==NULL||m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    QString s,s2;
    CDealer de;
    //-
    if(iID!=-1)
    {
        if(!m_pDb->dealer_get(iID,de))// get data
            b=false;
    }
    //-
    if(b)
    {
        pList->blockSignals(true);//faster insert

        pList->clear();
        s=QString("Firma:");
        s2=de.get_name();
        insert_list_element(pList,s,s2);

        s=QString("Adresse:");
        s2=de.get_adress();
        insert_list_element(pList,s,s2);

        s=QString("Kundennummer:");
        s2=de.get_customernumber();
        insert_list_element(pList,s,s2);

        s=QString("Telefonnummer(n):");
        s2=de.get_callnumber();
        insert_list_element(pList,s,s2);

        s=QString("Faxnummer(n):");
        s2=de.get_faxnumber();
        insert_list_element(pList,s,s2);

        s=QString("email-Adresse(n):");
        s2=de.get_email();
        insert_list_element(pList,s,s2);

        s=QString("homepage:");
        s2=de.get_homepage();
        insert_list_element(pList,s,s2);

        s=QString("Kontaktperson(en):");
        s2=de.get_contectperson();
        insert_list_element(pList,s,s2);

        s=QString("Kommentar:");
        s2=de.get_comment();
        insert_list_element(pList,s,s2);
        //-
        pList->blockSignals(false);
    }
    return b;
}

bool CWidgetInterface::dealer_fill_combobox(QComboBox * pCombobox)
{
    if(m_pDb==NULL || pCombobox==NULL)
        return false;

    bool b;
    QString s;
    int count;
    QList<int> ls;
    //-
    s=QString("LOWER (name)");
    b=m_pDb->dealer_get_all(QString(""),ls,s);
    //-
    if(b)//no break?
    {
        count=ls.count();
        pCombobox->clear();
        pCombobox->blockSignals(true);//no signals by inserts
        pCombobox->insertItem(0,"");//empty row
        for(int i=0;i<count;i++)
        {
            s=m_pDb->dealer_get_name(ls[i]);
            pCombobox->insertItem(i+1,s);
        }
        pCombobox->blockSignals(false);//signals enable
    }
    //-
    ls.clear();
    //-
    return b;
}


//customer-------------------------------------------------------------------------------------
bool CWidgetInterface::customer_update_tablewidget(QTableWidget * pTable, QList<int> & lsIds, int iSelectedID)
{
    if(pTable==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    CCustomer cu;
    QList<CTableItemData> ls;
    //-
    set_block_signal_and_sort(pTable,true,false);//faster
    remove_all_rows(pTable);//delete all rows
    //-
    while(lsIds.count()>0)
    {
        if(!m_pDb->customer_get(lsIds[0],cu))//get data
        {
            b=false;
            break;
        }
        else
        {
            customer_format(cu,ls);
            if(!mod_row(pTable,ls,ITEM_POSITION_BOTTOM,false,false,-1))
            {
                b=false;
                break;
            }
            ls.clear();
        }
        lsIds.removeFirst();
    }
    lsIds.clear();
    //-
    set_block_signal_and_sort(pTable,false,true);
    if(iSelectedID!=-1)
        select_row(pTable,iSelectedID);
    return b;
}

bool CWidgetInterface::customer_insert_row(QTableWidget * pTable, CCustomer & cu, int iPosition, bool bSelect)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=cu.get_id();
    //-
    customer_format(cu,ls);
    b=mod_row(pTable,ls,iPosition,true,false,iSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::customer_update_row(QTableWidget * pTable, CCustomer & cu, bool bSelect, bool bAddIfNotExis)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=cu.get_id();
    //-
    customer_format(cu,ls);
    int row=find_row(pTable,cu.get_id());
    if(row>=0 && row<pTable->rowCount())//found?
        b=mod_row(pTable,ls,row,true,true,iSelect);
    else if(bAddIfNotExis)
        b=customer_insert_row(pTable,cu,-2,bSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::customer_format(CCustomer & cu, QList<CTableItemData> & lsData)
{
    QString s,s2;
    CTableItemData ti;

    //column 0
    ti.set_text(cu.get_customernumber());
    lsData.push_back(ti);
    //column 1
    s=cu.get_name();
    s2=cu.get_first_name();
    if(s2.length()>0)
        s+=QString(", %1").arg(s2);
    ti.set_text(s);
    lsData.push_back(ti);
    //column 2
    s=QString("%1, %2 %3").arg(cu.get_street(), cu.get_postcode(),cu.get_city());
    ti.set_text(s);
    lsData.push_back(ti);
    //column 3
    ti.set_text(cu.get_comment());
    lsData.push_back(ti);
    //column 4
    s=QString("%1").arg(cu.get_id());
    ti.set_text(s);
    lsData.push_back(ti);
    //-
    return true;
}

bool CWidgetInterface::customer_update_list(QListWidget * pList, int iID)
{
    if(pList==NULL||m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    QString s,s2,s3;
    CCustomer cu;
    //-
    if(iID!=-1)
    {
        if(!m_pDb->customer_get(iID,cu))// get data
            b=false;
    }
    //-
    if(b)
    {
        pList->blockSignals(true);//faster insert

        pList->clear();

        s=QString("Kundennummer:");
        s2=cu.get_customernumber();
        insert_list_element(pList,s,s2);

        s=QString("Name, Vorname:");
        s2=cu.get_name();
        s3=cu.get_first_name();
        if(s3.length()>0)
            s2+=QString(", %1").arg(s3);
        insert_list_element(pList,s,s2);

        s=QString("Adresse:");
        s2=QString("%1, %2 %3").arg(cu.get_street(),cu.get_postcode(),cu.get_city());
        insert_list_element(pList,s,s2);

        s=QString("Telefonnummer(n):");
        s2=cu.get_callnumber();
        insert_list_element(pList,s,s2);

        s=QString("Faxnummer(n):");
        s2=cu.get_faxnumber();
        insert_list_element(pList,s,s2);

        s=QString("email-Adresse(n):");
        s2=cu.get_email();
        insert_list_element(pList,s,s2);

        s=QString("Kommentar:");
        s2=cu.get_comment();
        insert_list_element(pList,s,s2);
        //-
        pList->blockSignals(false);

    }
    return b;
}



//article-------------------------------------------------------------------------------------

bool CWidgetInterface::article_update_tablewidget(QTableWidget * pTable, QList<int> & lsNewIds, int iFormatType, int iSelectedID)
{
    if(pTable==NULL || m_pDb==NULL || iFormatType<0)
        return false;
    if(!m_pDb->is_db_connect() || pTable->columnCount()<=0)
        return false;

    bool b;
    QString s;
    QIcon ico;
    CArticle ar;
    CTableItemData taDa;
    QList<int> lsOldIds,lsRowNum;
    QTableWidgetItem * pItem;
    QList<CTableItemData> lsDataOld,lsDataNew;
    int i,pos,x,y,id,id_c=pTable->columnCount()-1,a_left=Qt::AlignLeft|Qt::AlignVCenter,a_right=Qt::AlignRight|Qt::AlignVCenter;

    //block
    set_block_signal_and_sort(pTable,true,false);//faster

    if(lsNewIds.count()<=0)
        remove_all_rows(pTable);//delete all rows

    //fill table new
    if(pTable->rowCount()<=0)
    {
        //insert article by new ids
        while(lsNewIds.count()>0)
        {
            if(m_pDb->article_get(lsNewIds[0],ar))//get data
            {
                article_format(ar,lsDataNew,iFormatType);
                mod_row(pTable,lsDataNew,ITEM_POSITION_BOTTOM,false,false,-1);
                lsDataNew.clear();
            }
            else{}
            lsNewIds.removeFirst();
        }
    }
    else
    {
        //sort new ids
        qSort(lsNewIds);

        //get old id's + row number from table
        for(y=0;y<pTable->rowCount();y++)
        { 
            pItem=pTable->item(y,id_c);
            if(pItem!=NULL)
            {
                s=pItem->text();
                id=s.toInt(&b,10);
                if(b)
                {
                    //insert (sort)
                    for(i=0;i<lsOldIds.count();i++)
                    {
                        if(id<lsOldIds[i])
                            break;
                    }
                    if(i<lsOldIds.count())
                    {
                        lsOldIds.insert(i,id);
                        lsRowNum.insert(i,y);
                    }
                    else
                    {
                        lsOldIds.push_back(id);
                        lsRowNum.push_back(y);
                    }
                }
            }

        }

        //check & update
        while(lsNewIds.count()>0)
        {
            if(m_pDb->article_get(lsNewIds[0],ar))//get article data
            {
                //new data
                article_format(ar,lsDataNew,iFormatType);

                //search in new id list
                pos=lsOldIds.indexOf(lsNewIds[0]);
                if(pos<0)//not found
                    mod_row(pTable,lsDataNew,ITEM_POSITION_BOTTOM,false,false,-1);//new row
                else//found
                {
                    //old data
                    for(x=0;x<pTable->columnCount();x++)
                    {
                        pItem=pTable->item(lsRowNum[pos],x);
                        if(pItem!=NULL)
                        {
                            //alignment
                            if(pItem->textAlignment()==a_left)
                                i=TABLE_ALIGNMENT_LEFT;
                            else if(pItem->textAlignment()==a_right)
                                i=TABLE_ALIGNMENT_RIGHT;
                            else
                                i=TABLE_ALIGNMENT_CENTER;
                            //icon
                            ico=pItem->icon();
                            //set
                            taDa.set(pItem->text(),i,&ico);
                            lsDataOld.push_back(taDa);
                        }
                    }

                    //check for update
                    b=false;
                    for(x=0;lsDataNew.count()>0 && lsDataOld.count()>0;x++)
                    {
                        if(lsDataNew[0]!=lsDataOld[0])
                        {
                            pItem=new CMyTableWidgetItem;
                            if(pItem!=NULL)
                            {
                                pItem->setText(lsDataNew[0].get_text());
                                if(lsDataNew[0].get_alignment()==TABLE_ALIGNMENT_LEFT)
                                    pItem->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
                                else if(lsDataNew[0].get_alignment()==TABLE_ALIGNMENT_RIGHT)
                                    pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                                else
                                    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

                                //icon?
                                ico=lsDataNew[0].get_icon();
                                if(!ico.isNull())
                                    pItem->setIcon(ico);

                                //set
                                pTable->setItem(lsRowNum[0],x,pItem);
                            }

                        }
                        lsDataNew.removeFirst();
                        lsDataOld.removeFirst();
                    }

                    //-
                    lsOldIds.removeAt(pos);
                    lsRowNum.removeAt(pos);
                }
                lsDataNew.clear();
                lsDataOld.clear();
            }
            lsNewIds.removeFirst();
        }

        //remove not use old rows
        qSort(lsRowNum);//sort row positions
        while(lsRowNum.count()>0)
        {
            pTable->removeRow(lsRowNum.last());
            lsRowNum.removeLast();
        }
    }

    //unblock + select
    set_block_signal_and_sort(pTable,false,true);
    if(iSelectedID!=-1)
        select_row(pTable,iSelectedID);

    //clear
    lsNewIds.clear();
    lsOldIds.clear();
    lsRowNum.clear();
    lsDataNew.clear();
    lsDataOld.clear();
    return true;
}

bool CWidgetInterface::article_update_tablewidget_wares_list(QTableWidget * pTable,QList<QString> & lsWares,int iFormatType)
{
    if(pTable==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    CArticle ar;
    QString s;
    CTableItemData ti;
    QList<CTableItemData> lsData;
    int iCount,iArticleId;
    //-
    set_block_signal_and_sort(pTable,true,false);//faster
    remove_all_rows(pTable);//delete all rows

    //wares
    while(lsWares.count()>0)
    {
        b=cast_count_and_id(lsWares[0],iCount,iArticleId);//cast string to count & article id
        if(b)
        {
            b=m_pDb->article_get(iArticleId,ar,true);//get article data
            if(b)
            {
                if(iFormatType==FORMAT_ONE)
                {
                    s=QString("%1").arg(iCount);
                    if(ar.get_unit().length()>0)
                        s+=QString(" %1").arg(ar.get_unit());
                    ti.set_text(s);
                    ti.set_alignment(TABLE_ALIGNMENT_RIGHT);
                    lsData.push_back(ti);
                    b=article_format(ar,lsData,FORMAT_FOUR);
                }
                else if(iFormatType==FORMAT_TWO)
                {
                    ti.set_text(ar.get_name());
                    lsData.push_back(ti);
                    //-
                    s=ar.get_articlenumber();
                    if(ar.get_articlenumber2().length()>0)
                        s+=QString("|%1").arg(ar.get_articlenumber2());
                    ti.set_text(s);
                    lsData.push_back(ti);
                    //-
                    for(int i=0;i<2;i++)
                    {
                        s=QString("%1").arg(iCount);
                        ti.set_text(s);
                        ti.set_alignment(TABLE_ALIGNMENT_RIGHT);
                        lsData.push_back(ti);
                    }
                    //-
                    s=QString("%1").arg(iArticleId);
                    ti.set_text(s);
                    ti.set_alignment(TABLE_ALIGNMENT_LEFT);
                    lsData.push_back(ti);
                }
                //-
                if(b)
                    b=mod_row(pTable,lsData,ITEM_POSITION_BOTTOM,false,false,-1);
                lsData.clear();
            }
        }
        if(!b)
            break;//error
        lsWares.removeFirst();
    }

    set_block_signal_and_sort(pTable,false,true);
    lsWares.clear();
    return b;
}

bool CWidgetInterface::article_insert_row(QTableWidget * pTable, CArticle & ar, int iFormatType, int iPosition, bool bSelect)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=ar.get_id();
    //-
    article_format(ar,ls,iFormatType);
    b=mod_row(pTable,ls,iPosition,true,false,iSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::article_update_row(QTableWidget * pTable, CArticle & ar, int iFormatType, bool bSelect, bool bAddIfNotExis)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=ar.get_id();
    //-
    article_format(ar,ls, iFormatType);
    int row=find_row(pTable,ar.get_id());
    if(row>=0 && row<pTable->rowCount())//found?
        b=mod_row(pTable,ls,row,true,true,iSelect);
    else if(bAddIfNotExis)
        b=article_insert_row(pTable,ar,iFormatType,-2,bSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::article_format(CArticle & ar, QList<CTableItemData> & lsData, int iFormatType)
{
    if(m_pDb==NULL)
        return false;
    if(m_tcArticle.get_table()==NULL || !m_pDb->is_db_connect())
        return false;

    int i,j;
    QString s;
    float f1,f2;
    CTableItemData ti;
    QTableWidget * pTable=m_tcArticle.get_table();
    QList<int> lsOrder,lsAlignment;
    m_tcArticle.get_columns_order(lsOrder);
    m_tcArticle.get_columns_alignment(lsAlignment);

    if(iFormatType==FORMAT_ONE)
    {//all
        if(lsOrder.count()==pTable->columnCount() && lsAlignment.count()==pTable->columnCount())
        {
            for(j=0;j<pTable->columnCount();j++)
            {
                if(lsOrder[j]==0)//default column 0
                    ti.set_text(ar.get_name());
                if(lsOrder[j]==1)//d.column 1
                    ti.set_text(ar.get_articlenumber());
                if(lsOrder[j]==2)//d.column 2
                    ti.set_text(ar.get_articlenumber2());
                if(lsOrder[j]==3)//d.column 3
                {
                    s=m_pDb->maker_get_name(ar.get_maker_id());
                    ti.set_text(s);
                }
                if(lsOrder[j]==4)//d.column 4
                {
                    s=m_pDb->waregroup_get_path(ar.get_waregroup_id(),3);
                    ti.set_text(s);
                }
                if(lsOrder[j]==5)//d.column 5
                {
                    s=QString("");
                    if(ar.get_warning_limit()>=0)
                    {
                        s=QString("%1").arg(ar.get_warning_limit());
                        if(ar.get_unit().length()>0)//unit set?
                            s+=QString(" %1").arg(ar.get_unit());
                    }
                    ti.set_text(s);
                }
                if(lsOrder[j]==6)//d.column 6
                {
                    f1=ar.get_base_price();
                    if(f1>0.0)
                    {
                        s=QString("%1").arg(f1);
                        if(ar.get_valuta().length()>0)//valuta set?
                            s+=QString(" %1").arg(ar.get_valuta());
                    }
                    ti.set_text(s);
                }
                if(lsOrder[j]==7)//d.column 7
                {
                    f1=ar.get_sales_price();
                    if(f1>0.0)
                    {
                        s=QString("%1").arg(f1);
                        if(ar.get_valuta().length()>0)//valuta set?
                            s+=QString(" %1").arg(ar.get_valuta());
                    }
                    ti.set_text(s);
                }
                if(lsOrder[j]==8)//d.column 8
                {
                    f1=ar.get_base_price();
                    f2=ar.get_sales_price();
                    if(f1>0.0 && f2>0.0)
                    {
                        s=QString("%1 ").arg(f2-f1);
                        if(ar.get_valuta().length()>0)//valuta set?
                            s+=ar.get_valuta();
                    }
                    ti.set_text(s);
                }
                if(lsOrder[j]==9)//d.column 9
                     ti.set_text(ar.get_location());
                if(lsOrder[j]==10)//d.column 10
                    ti.set_text(ar.get_comment());
                if(lsOrder[j]==11)//d.column 11
                {
                    s=QString("%1").arg(ar.get_id());
                    ti.set_text(s);
                }
                //-
                ti.set_alignment(lsAlignment[j]);//alignment
                lsData.push_back(ti);//add
                //-
                ti.clear();
                s=QString("");
            }
        }
    }
    //---
    if(iFormatType==FORMAT_TWO || iFormatType==FORMAT_THREE || iFormatType==FORMAT_FOUR)
    {
        if(iFormatType==FORMAT_TWO)
        {
            ti.set_alignment(TABLE_ALIGNMENT_RIGHT);
            s=QString("%1 / ").arg(ar.get_inventory());
            i=m_pDb->ordering_get_count_by_article(ar.get_id());//count of article by wares
            if(i<0)
                i=0;
            s+=QString("%1 ").arg(i);
            s+=QString("/ %1").arg(ar.get_max_inventory());
            if(ar.get_unit().length()>0)
                s+=QString(" %1").arg(ar.get_unit());
            ti.set_text(s);
            lsData.push_back(ti);
        }
        //-
        ti.set_alignment(TABLE_ALIGNMENT_LEFT);
        //-
        ti.set_text(ar.get_name());
        lsData.push_back(ti);
        //-
        if(iFormatType==FORMAT_TWO || iFormatType==FORMAT_THREE)
        {
            s=ar.get_articlenumber();
            if(ar.get_articlenumber2().length()>0)
                s+=QString(" | %1").arg(ar.get_articlenumber2());
            ti.set_text(s);
            lsData.push_back(ti);
        }
        //-
        if(iFormatType==FORMAT_FOUR)
        {
            ti.set_text(ar.get_articlenumber());
            lsData.push_back(ti);
            //-
            ti.set_text(ar.get_articlenumber2());
            lsData.push_back(ti);
        }
        //-
        s=m_pDb->maker_get_name(ar.get_maker_id());
        ti.set_text(s);
        lsData.push_back(ti);
        //-
        s=m_pDb->waregroup_get_path(ar.get_waregroup_id(),3);
        ti.set_text(s);
        lsData.push_back(ti);
        //-
        s=ar.get_location();
        ti.set_text(s);
        lsData.push_back(ti);
        //-
        s=QString("%1").arg(ar.get_id());
        ti.set_text(s);
        lsData.push_back(ti);
    }
    //-
    lsOrder.clear();
    lsAlignment.clear();
    return true;
}

bool CWidgetInterface::article_update_row_wareslist(QTableWidget * pTable,QString sData,bool bEdit, bool bSelect)
{
        if(pTable==NULL || m_pDb==NULL)
            return false;
        if(!m_pDb->is_db_connect())
            return false;
        //-
        set_block_signal_and_sort(pTable,true,false);//faster

        QString s;
        bool b=true;
        CTableItemData ti;
        int i=0,row=-1,iCount,iArticleId;
        QList<CTableItemData> lsData;


        //-cast string to count & id
        if(!cast_count_and_id(sData,iCount,iArticleId))
            return false;
        //-
        CArticle ar;
        if(!m_pDb->article_get(iArticleId,ar))//get article by id
            return false;//not found

        //-search double article in wares-
        row=find_row(pTable,iArticleId);
        if(row<0)
        {//not found
            //-insert new at table-
            s=QString("%1").arg(iCount);
            ti.set_text(s);
            ti.set_alignment(TABLE_ALIGNMENT_RIGHT);
            lsData.push_back(ti);
            b=article_format(ar,lsData,FORMAT_THREE);
            if(b)
                b=mod_row(pTable,lsData,ITEM_POSITION_BOTTOM,false,false,-1);
            else{}
        }
        else if(row>=0 && row<pTable->rowCount())
        {//found
            if(pTable->item(row,0)!=NULL)
            {
                if(!bEdit)
                {//add mode
                    i=pTable->item(row,0)->text().toInt(&b,10);//get old count
                    if(b)//cast ok?
                        iCount+=i;//new+old count
                }
                s=QString("%1").arg(iCount);
                if(b)
                    pTable->item(row,0)->setText(s);//only update count
            }
        }

        set_block_signal_and_sort(pTable,false,true);
        if(bSelect==true && b==true)
            select_row(pTable,iArticleId);

        lsData.clear();
        return b;
}


//ordering---------------------------------------------------------------------
bool CWidgetInterface::ordering_update_tablewidget(QTableWidget * pTable, QList<int> & lsIds, int iFormatType, int iSelectedID)
{
    if(pTable==NULL || m_pDb==NULL || iFormatType<0)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    COrdering ord;
    QList<CTableItemData> ls;
    //-
    set_block_signal_and_sort(pTable,true,false);//faster
    remove_all_rows(pTable);//delete all rows
    //-
    while(lsIds.count()>0)
    {
        if(!m_pDb->ordering_get(lsIds[0],ord))//get data
        {
            b=false;
            break;
        }
        else
        {
            ordering_format(ord,ls,iFormatType);
            if(!mod_row(pTable,ls,ITEM_POSITION_BOTTOM,false,false,-1))
            {
                b=false;
                break;
            }
            ls.clear();
        }
        lsIds.removeFirst();
    }
    lsIds.clear();
    //-
    set_block_signal_and_sort(pTable,false,true);
    if(iSelectedID!=-1)
        select_row(pTable,iSelectedID);
    return b;
}

bool CWidgetInterface::ordering_update_tablewidget_wares_list(QTableWidget * pTable, int iOrderingID, int iFormatType)
{
    if(pTable==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    COrdering ord;
    QList<QString> lsWares;
    //-
    b=m_pDb->ordering_get(iOrderingID,ord);//get data
    if(b)
    {
        ord.get_wares(lsWares);//get wares
        b=article_update_tablewidget_wares_list(pTable,lsWares,iFormatType);
    }
    else
        remove_all_rows(pTable);//not found->clear table
    return b;
}

bool CWidgetInterface::ordering_insert_row(QTableWidget * pTable, COrdering & ord, int iFormatType, int iPosition, bool bSelect)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=ord.get_id();
    //-
    ordering_format(ord,ls,iFormatType);
    b=mod_row(pTable,ls,iPosition,true,false,iSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::ordering_update_row(QTableWidget * pTable, COrdering & ord, int iFormatType, bool bSelect, bool bAddIfNotExis)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=ord.get_id();
    //-
    ordering_format(ord,ls, iFormatType);
    int row=find_row(pTable,ord.get_id());
    if(row>=0 && row<pTable->rowCount())//found?
        b=mod_row(pTable,ls,row,true,true,iSelect);
    else if(bAddIfNotExis)
        b=ordering_insert_row(pTable,ord,-2,bSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::ordering_format(COrdering & ord, QList<CTableItemData> & lsData, int iFormatType)
{
    if(m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;

    QString s;
    CTableItemData ti;
    QDate dt=ord.get_date();
    QStringList slWeekDay;
    slWeekDay<<"Mo"<<"Di"<<"Mi"<<"Do"<<"Fr"<<"Sa"<<"So";

    if(iFormatType==FORMAT_ONE)
    {//all
        //column 0
        s=dt.toString("yyyy-MM-dd");
        s+=QString(" (%1.)").arg(slWeekDay[dt.dayOfWeek()-1]);
        ti.set_text(s);
        lsData.push_back(ti);
        //-column 1
        ti.set_text(ord.get_ordernumber());
        lsData.push_back(ti);
        //-column 2
        s=m_pDb->dealer_get_name(ord.get_dealer_id());
        ti.set_text(s);
        lsData.push_back(ti);
        //-column 3
        ti.set_text(ord.get_comment());
        lsData.push_back(ti);
        //-column 4
        s=m_pDb->ordering_get_wares_list(ord.get_id(),100,true);
        ti.set_text(s);
        lsData.push_back(ti);
        //-column 5
        s=QString("%1").arg(ord.get_id());
        ti.set_text(s);
        lsData.push_back(ti);
    }
    //---
    if(iFormatType==FORMAT_TWO)
    {//short
        //column 0
        s=m_pDb->dealer_get_name(ord.get_dealer_id());
        ti.set_text(s);
        lsData.push_back(ti);
        //column 1
        s=dt.toString("yyyy-MM-dd");
        s+=QString(" (%1.)").arg(slWeekDay[dt.dayOfWeek()-1]);
        ti.set_text(s);
        lsData.push_back(ti);
        //column 2
        ti.set_text(ord.get_ordernumber());
        lsData.push_back(ti);
        //column 3
        ti.set_text(ord.get_comment());
        lsData.push_back(ti);
        //column 4
        s=QString("%1").arg(ord.get_id());
        ti.set_text(s);
        lsData.push_back(ti);
    }
    return true;
}


//trade----------------------------------------------------------------------------
bool CWidgetInterface::trade_update_tablewidget(QTableWidget * pTable, QList<QString> & lsBookingNumber,  QString sSelect)
{
    if(pTable==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    CTrade trade;
    QList<CTableItemData> ls;
    //-
    set_block_signal_and_sort(pTable,true,false);//faster
    remove_all_rows(pTable);//delete all rows
    //-
    while(lsBookingNumber.count()>0)
    {
        if(!m_pDb->trade_get(lsBookingNumber[0],trade))//get data
        {
            b=false;
            break;
        }
        else
        {
            trade_format(trade,ls);
            if(!mod_row(pTable,ls,ITEM_POSITION_BOTTOM,false,false,QString(""),2))
            {
                b=false;
                break;
            }
            ls.clear();
        }
        lsBookingNumber.removeFirst();
    }
    lsBookingNumber.clear();
    //-
    set_block_signal_and_sort(pTable,false,true);
    if(sSelect.length()>0)
        select_row(pTable,sSelect,2);
    return b;
}

bool CWidgetInterface::trade_update_tablewidget_wares_list(QTableWidget * pTable, QString sBookingNumber)
{
    if(pTable==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    CTrade trade;
    QList<QString> lsWares;
    //-
    b=m_pDb->trade_get(sBookingNumber,trade);//get data
    if(b)
    {
        trade.get_wares(lsWares);//get wares
        b=article_update_tablewidget_wares_list(pTable,lsWares,FORMAT_ONE);
    }
    else
        remove_all_rows(pTable);//not found->clear table
    return b;
}

bool CWidgetInterface::trade_insert_row(QTableWidget * pTable, CTrade & trade, int iPosition, bool bSelect)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    QString sSelect;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        sSelect=trade.get_booking_number();
    //-
    trade_format(trade,ls);
    b=mod_row(pTable,ls,iPosition,true,false,sSelect,2);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::trade_update_row(QTableWidget * pTable, CTrade & trade, bool bSelect, bool bAddIfNotExis)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    QString sSelect;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        sSelect=trade.get_booking_number();
    //-
    trade_format(trade,ls);
    int row=find_row(pTable,trade.get_booking_number(),2);
    if(row>=0 && row<pTable->rowCount())//found?
        b=mod_row(pTable,ls,row,true,true,sSelect,2);
    else if(bAddIfNotExis)
        b=trade_insert_row(pTable,trade,-2,bSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::trade_format(CTrade & trade, QList<CTableItemData> & lsData)
{
    if(m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;

    QString s;
    CTableItemData ti;
    QDate dt=trade.get_date();
    QStringList slWeekDay;
    slWeekDay<<"Mo"<<"Di"<<"Mi"<<"Do"<<"Fr"<<"Sa"<<"So";


    //-column 0
    lsData.push_back(ti);
    trade_get_icon(trade,lsData[0],TABLE_ALIGNMENT_LEFT);
    ti.clear();
    //-column 1
    if(trade.get_canceled())
        s=QString("ja");
    else
        s=QString("nein");
    ti.set_text(s);
    lsData.push_back(ti);
    //-column2
    ti.set_text(trade.get_booking_number());
    lsData.push_back(ti);
    //-column 3
    s=dt.toString("yyyy-MM-dd");
    s+=QString(" (%1.)").arg(slWeekDay[dt.dayOfWeek()-1]);
    ti.set_text(s);
    lsData.push_back(ti);
    //-column 4
    s=m_pDb->trade_get_article_list(trade.get_booking_number(),100,true);
    ti.set_text(s);
    lsData.push_back(ti);
    //-column 5
    if(trade.get_type()==TYPE_ORDERING_INCOMING || trade.get_type()==TYPE_CUSTOMER_OUTGOING)
        s=trade.get_info_2();
    else
        s=QString("");
    ti.set_text(s);
    lsData.push_back(ti);
    //-column 6
    ti.set_text(trade.get_comment());
    lsData.push_back(ti);
    //-
    return true;
}

void CWidgetInterface::trade_get_icon(CTrade & trade, CTableItemData & ti, int iAlignment)
{
    QString s;
    int i,icon_index;
    i=trade.get_type();
    if(i==TYPE_INCOMING)
    {
        s=QString("Wareneingang");
        icon_index=0;
    }
    else if(i==TYPE_OUTGOING)
    {
        s=QString("Warenausgang");
        icon_index=1;
    }
    else if(i==TYPE_ORDERING_INCOMING)
    {
        s=QString("Wareneingang(Bestellung)");
        icon_index=2;
    }
    else
    {
        s=QString("Warenausgang(Kunde)");
        icon_index=3;
    }

    //trade canceled -> icon with cross
    if(trade.get_canceled())
        icon_index+=4;
    //---
    if(icon_index>=0 && icon_index<m_lsIconsTrade.count())
        ti.set(s,iAlignment,&m_lsIconsTrade[icon_index]);
}

bool CWidgetInterface::trade_update_list_info(QListWidget * pList, CTrade & tr)
{
    if(pList==NULL)
        return false;

    pList->blockSignals(true);

    //add trade info
    pList->clear();

    //fill list
    QString s,s2;
    if(tr.get_type()==TYPE_CUSTOMER_OUTGOING)
        s=QString("Kundennummer:");
    else if(tr.get_type()==TYPE_ORDERING_INCOMING)
        s=QString("Bestellnummer:");
    s2=tr.get_info_1();
    if(s2.length()>0)
        insert_list_element(pList,s,s2,false);
    //-
    if(tr.get_type()==TYPE_CUSTOMER_OUTGOING)
        s=QString("Name, Vorname:");
    else if(tr.get_type()==TYPE_ORDERING_INCOMING)
        s=QString("Händler:");
    s2=tr.get_info_2();
    if(s2.length()>0)
        insert_list_element(pList,s,s2,false);
    //-
    if(tr.get_type()==TYPE_CUSTOMER_OUTGOING)
        s=QString("Strasse:");
    else if(tr.get_type()==TYPE_ORDERING_INCOMING)
        s=QString("Bestelldatum:");
    s2=tr.get_info_3();
    if(s2.length()>0)
        insert_list_element(pList,s,s2,false);
    //-
    if(tr.get_type()==TYPE_CUSTOMER_OUTGOING)
        s=QString("Postleitzahl, Ort:");
    else if(tr.get_type()==TYPE_ORDERING_INCOMING)
        s=QString("sonstiges:");
    s2=tr.get_info_4();
    if(s2.length()>0)
        insert_list_element(pList,s,s2,false);
    //-
    pList->blockSignals(false);

    return true;
}

bool CWidgetInterface::trade_update_list_date(QListWidget * pList, int iYear, int iIndex, QString sTextMask,int iIndexMask, QDate dtSelect)
{
    if(pList==NULL || iYear<2009 || iYear>2999 || m_pDb==NULL)
        return false;
    //-
    pList->blockSignals(true);//faster insert
    //-
    int j=0,i=0,count,k;
    QString s,s2;
    QStringList sl,slWeekDay,slMonths;
    QListWidgetItem * pItem=NULL;
    //-
    slWeekDay<<"Mo"<<"Di"<<"Mi"<<"Do"<<"Fr"<<"Sa"<<"So";
    slMonths<<"Januar"<<"Februar"<<"März"<<"April"<<"Mai"<<"Juni"<<"July"<<"August"<<"September"<<"Oktober"<<"November"<<"Dezember";
    QDate dtCurrentDate=QDate().currentDate();
    QDate dt,dt2,dtFromDate,dtToDate,dtMark;
    //-
    dtFromDate=QDate(iYear,1,1);
    if(dtCurrentDate.year()==iYear)//current year?
        dtToDate=dtCurrentDate;//only to current date
    else//another year
        dtToDate=QDate(iYear,12,31);
    //-
    if(iIndex==1)//by weeks - set from date back to week start
    {
        j=dtFromDate.dayOfWeek()-1;
        if(j>0)
        {
            j*=-1;
            dtFromDate=dtFromDate.addDays(j);
        }
    }
    //-
    dt=dtFromDate;
    while(dt<=dtToDate)
    {
        i++;
        //-
        if(iIndex==0)//by days
        {
            if(dt==dtSelect)//mark index for select
                j=i;
            //-
            s=dt.toString(QString("dd.MM.yyyy"));
            s+=QString(" (%1.)").arg(slWeekDay[dt.dayOfWeek()-1]);
            //-
            dtMark=dt;
            dt2=dt;
            dt=dt.addDays(1);
        }
        if(iIndex==1)//by weeks
        {
            dt2=dt.addDays(6);
            if(dt2>QDate().currentDate())
                dt2=QDate().currentDate();
            //-
            if(dtSelect >= dt && dtSelect <= dt2)//mark index for select
                j=i;
            //-
            s=dt.toString(QString("dd.MM.yyyy"));
            s+=QString("-%1").arg(dt2.toString(QString("dd.MM.yyyy")));
            k=dt.weekNumber();
            if(k<10)
                s+=QString(" (0%1.KW)").arg(k);
            else
                s+=QString(" (%1.KW)").arg(k);
            //-
            dtMark=dt;
            dt=dt.addDays(7);
        }
        if(iIndex==2)//by months
        {
            dt2=dt.addMonths(1);
            dt2=dt2.addDays(-1);
            if(dt2>QDate().currentDate())
                dt2=QDate().currentDate();
            //-
            if(dtSelect >= dt && dtSelect <= dt2)//mark index for select
                j=i;
            //-
            s=slMonths[dt.month()-1];
            s2=dt.toString(QString("dd.MM.yyyy"));
            s2+=QString("-%1").arg(dt2.toString(QString("dd.MM.yyyy")));
            s+=QString(" (%1)").arg(s2);
            //-
            dtMark=dt;
            dt=dt.addMonths(1);
        }
        if(iIndex==3)//by quarter
        {
            dt2=dt.addMonths(3);
            dt2=dt2.addDays(-1);
            if(dt2>QDate().currentDate())
                dt2=QDate().currentDate();
            //-
            if(dtSelect >= dt && dtSelect <= dt2)//mark index for select
                j=i;
            //-
            s=dt.toString(QString("dd.MM.yyyy"));
            s+=QString("-%1").arg(dt2.toString(QString("dd.MM.yyyy")));
            s+=QString(" (%1.Quartal)").arg(i);
            //-
            dtMark=dt;
            dt=dt.addMonths(3);
        }

        //count of trade's in this time
        if(m_pDb!=NULL)
        {
            count=trade_get_count_by_date_and_mask(dtMark,dt2,iIndexMask,sTextMask);
            if(count>0)
                s+=QString(" |%1").arg(count);
        }
        //-
        sl.push_back(s);
    }
    //-insert-
    for(i=sl.count()-1;i>=0;i--)
    {
        pItem= new QListWidgetItem(sl[i]);
        if(pItem!=NULL)
        {
            pList->addItem(pItem);
            if(j-1==i)
                pList->setCurrentItem(pItem);
        }
    }
    //-
    sl.clear();
    slWeekDay.clear();
    slMonths.clear();
    //-
    pList->blockSignals(false);
    //-
    return true;
}

bool CWidgetInterface::trade_refresh_list_date(QListWidget * pList, int iIndex, QString sTextMask, int iIndexMask)
{
    if(m_pDb==NULL || pList==NULL)
        return false;

    int i,count;
    QString s,s2;
    QStringList sl;
    QDate dtFrom,dtTo;
    QListWidgetItem * pItem=NULL;
    //-
    for(i=0;i<pList->count();i++)
    {
        pItem=pList->item(i);
        if(pItem!=NULL)
        {
            s=pItem->text();
            if(trade_get_selecteted_mask_date(s,iIndex,dtFrom,dtTo))
            {
                count=trade_get_count_by_date_and_mask(dtFrom,dtTo,iIndexMask,sTextMask);
                sl=s.split("|");
                if(sl.count()>0)
                {
                    s2=sl[0];
                    if(count>0 && sl.count()<2)
                        s2+=QString(" |%1").arg(count);
                    else if(count>0 && sl.count()==2)
                        s2+=QString("|%1").arg(count);
                    if(s!=s2)//update?
                        pItem->setText(s2);
                }
            }
        }
        sl.clear();
        pItem=NULL;
    }
    //-
    return true;
}

int CWidgetInterface::trade_get_count_by_date_and_mask(QDate dtFrom, QDate dtTo, int iIndexMask, QString sTextMask)
{
    if(m_pDb==NULL)
        return -1;
    if(!m_pDb->is_db_connect())
        return -1;

    //dates
    QString sCondition;
    if(dtTo>dtFrom)
        sCondition=QString("date >= '%1' AND date <= '%2'").arg(dtFrom.toString("yyyy-MM-dd"),dtTo.toString("yyyy-MM-dd"));
    else
        sCondition=QString("date = '%1'").arg(dtFrom.toString("yyyy-MM-dd"));
    //-
    int count=-1;
    //-
    if(sTextMask.length()<=0)
        count=m_pDb->trade_get_count(sCondition);
    else
    {
        if(iIndexMask<2 || iIndexMask>6)
        {
            if(iIndexMask==0)//booking number
                sCondition+=QString(" AND booking_number like '%%1%'").arg(sTextMask);

            if(iIndexMask==1)//comment
                sCondition+=QString(" AND comment like '%%1%'").arg(sTextMask);

            if(iIndexMask==7)//dealer
            {
                sCondition+=QString(" AND info_2 like '%%1%'").arg(sTextMask);
                sCondition+=QString(" AND type = %1").arg(TYPE_ORDERING_INCOMING);
            }
            if(iIndexMask==8)//ordering number
            {
                sCondition+=QString(" AND info_1 like '%%1%'").arg(sTextMask);
                sCondition+=QString(" AND type = %1").arg(TYPE_ORDERING_INCOMING);
            }
            if(iIndexMask==9)//customer name
            {
                sCondition+=QString(" AND info_2 like '%%1%'").arg(sTextMask);
                sCondition+=QString(" AND type = %1").arg(TYPE_CUSTOMER_OUTGOING);
            }
            if(iIndexMask==10)//customer number
            {
                sCondition+=QString(" AND info_1 like '%%1%'").arg(sTextMask);
                sCondition+=QString(" AND type = %1").arg(TYPE_CUSTOMER_OUTGOING);
            }
            //-
            count=m_pDb->trade_get_count(sCondition);
        }
        if(iIndexMask==2)//article name
            count=m_pDb->trade_get_all_count_by_article(sTextMask,dtFrom,dtTo,true);

        if(iIndexMask==3)//maker(article)
            count=m_pDb->trade_get_all_count_by_maker(sTextMask,dtFrom,dtTo,true);

        if(iIndexMask==4)//waregroup(article)
            count=m_pDb->trade_get_all_count_by_waregroup(sTextMask,dtFrom,dtTo,true);

        if(iIndexMask==5)//1.article number
            count=m_pDb->trade_get_all_count_by_articlenumber(sTextMask,dtFrom,dtTo,true,true);

        if(iIndexMask==6)//2.article number
            count=m_pDb->trade_get_all_count_by_articlenumber(sTextMask,dtFrom,dtTo,false,true);
    }
    //-
    return count;
}

bool CWidgetInterface::trade_get_selecteted_mask_date(QString sDateItem, int iIndex, QDate & dtFrom, QDate & dtTo)
{
    if(sDateItem.length()<=0 || iIndex<0)
        return false;
    //-
    QDate dt;
    bool b=true;
    QString s,s2;
    int i;
    //-
    if(iIndex==0)//by days
    {
        s=sDateItem;
        s.resize(10);//copy only 10 char (dd.mm.yyyy)
        if(format_and_check_date(s,dt))
        {
            dtFrom=dt;
            dtTo=dt;
        }
        else{}
    }
    //-
    else if(iIndex==1||iIndex==3)//by weeks or by quarter
    {
        s=sDateItem;
        s.resize(10);//copy only 10 char (dd.mm.yyyy)
        if(format_and_check_date(s,dt))
            dtFrom=dt;
        else{}
        s=sDateItem;
        s.remove(0,11);//remove first date
        s.resize(10);
        if(format_and_check_date(s,dt))//format 2.date
            dtTo=dt;
        else{}
    }
    //-
    else if(iIndex==2)//by months
    {
        s2=sDateItem;
        for(i=0;i<s2.length();i++)
        {
            if(s2[i]=='(')
                break;
            else{}
        }
        if(i+1 <sDateItem.length())
        {
            sDateItem.remove(0,i+1);
            s=sDateItem;
            s.resize(10);//copy only 10 char (dd.mm.yyyy)
            if(format_and_check_date(s,dt))
                dtFrom=dt;
            else{}
            s=sDateItem;
            s.remove(0,11);//remove first date
            s.resize(10);
            if(format_and_check_date(s,dt))//format 2.date
                dtTo=dt;
            else{}
        }
        else{}
    }
    else
        b=false;//index out of range
    //-
    return b;
}

//inventory-------------------------------------------------------------------------------------

bool CWidgetInterface::inventory_update_tablewidget(QTableWidget * pTable, QList<int> & lsNewIds, int iSelectedID)
{
    if(pTable==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect() || pTable->columnCount()<=0)
        return false;

    bool b;
    QString s;
    QIcon ico;
    CArticle ar;
    CTableItemData taDa;
    QList<int> lsOldIds,lsRowNum;
    QTableWidgetItem * pItem;
    QList<CTableItemData> lsDataOld,lsDataNew;
    int i,pos,x,y,id,id_c=pTable->columnCount()-1,a_left=Qt::AlignLeft|Qt::AlignVCenter,a_right=Qt::AlignRight|Qt::AlignVCenter;

    //block
    set_block_signal_and_sort(pTable,true,false);//faster

    if(lsNewIds.count()<=0)
        remove_all_rows(pTable);//delete all rows

    //fill table new
    if(pTable->rowCount()<=0)
    {
        //insert article by new ids
        while(lsNewIds.count()>0)
        {
            if(m_pDb->article_get(lsNewIds[0],ar))//get data
            {
                inventory_format(ar,lsDataNew);
                mod_row(pTable,lsDataNew,ITEM_POSITION_BOTTOM,false,false,-1);
                lsDataNew.clear();
            }
            else{}
            lsNewIds.removeFirst();
        }
    }
    else
    {
        //sort new ids
        qSort(lsNewIds);

        //get old id's + row number from table
        for(y=0;y<pTable->rowCount();y++)
        {
            pItem=pTable->item(y,id_c);
            if(pItem!=NULL)
            {
                s=pItem->text();
                id=s.toInt(&b,10);
                if(b)
                {
                    //insert (sort)
                    for(i=0;i<lsOldIds.count();i++)
                    {
                        if(id<lsOldIds[i])
                            break;
                    }
                    if(i<lsOldIds.count())
                    {
                        lsOldIds.insert(i,id);
                        lsRowNum.insert(i,y);
                    }
                    else
                    {
                        lsOldIds.push_back(id);
                        lsRowNum.push_back(y);
                    }
                }
            }

        }

        //check & update
        while(lsNewIds.count()>0)
        {
            if(m_pDb->article_get(lsNewIds[0],ar))//get article data
            {
                //new data
                inventory_format(ar,lsDataNew);

                //search in new id list
                pos=lsOldIds.indexOf(lsNewIds[0]);
                if(pos<0)//not found
                    mod_row(pTable,lsDataNew,ITEM_POSITION_BOTTOM,false,false,-1);//new row
                else//found
                {
                    //old data
                    for(x=0;x<pTable->columnCount();x++)
                    {
                        pItem=pTable->item(lsRowNum[pos],x);
                        if(pItem!=NULL)
                        {
                            //alignment
                            if(pItem->textAlignment()==a_left)
                                i=TABLE_ALIGNMENT_LEFT;
                            else if(pItem->textAlignment()==a_right)
                                i=TABLE_ALIGNMENT_RIGHT;
                            else
                                i=TABLE_ALIGNMENT_CENTER;
                            //icon
                            ico=pItem->icon();
                            //set
                            taDa.set(pItem->text(),i,&ico);
                            lsDataOld.push_back(taDa);
                        }
                    }

                    //check for update
                    b=false;
                    for(x=0;lsDataNew.count()>0 && lsDataOld.count()>0;x++)
                    {
                        if(lsDataNew[0]!=lsDataOld[0])
                        {
                            pItem=new CMyTableWidgetItem;
                            if(pItem!=NULL)
                            {
                                pItem->setText(lsDataNew[0].get_text());
                                if(lsDataNew[0].get_alignment()==TABLE_ALIGNMENT_LEFT)
                                    pItem->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
                                else if(lsDataNew[0].get_alignment()==TABLE_ALIGNMENT_RIGHT)
                                    pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                                else
                                    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

                                //icon?
                                ico=lsDataNew[0].get_icon();
                                if(!ico.isNull())
                                    pItem->setIcon(ico);

                                //set
                                pTable->setItem(lsRowNum[0],x,pItem);
                            }

                        }
                        lsDataNew.removeFirst();
                        lsDataOld.removeFirst();
                    }

                    //-
                    lsOldIds.removeAt(pos);
                    lsRowNum.removeAt(pos);
                }
                lsDataNew.clear();
                lsDataOld.clear();
            }
            lsNewIds.removeFirst();
        }

        //remove not use old rows
        qSort(lsRowNum);//sort row positions
        while(lsRowNum.count()>0)
        {
            pTable->removeRow(lsRowNum.last());
            lsRowNum.removeLast();
        }
    }

    //unblock + select
    set_block_signal_and_sort(pTable,false,true);
    if(iSelectedID!=-1)
        select_row(pTable,iSelectedID);

    //clear
    lsNewIds.clear();
    lsOldIds.clear();
    lsRowNum.clear();
    lsDataNew.clear();
    lsDataOld.clear();
    return true;


    /*
    if(pTable==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true;
    CArticle ar;
    QList<CTableItemData> ls;
    //-
    set_block_signal_and_sort(pTable,true,false);//faster
    remove_all_rows(pTable);//delete all rows
    //-
    while(lsIds.count()>0)
    {
        if(!m_pDb->article_get(lsIds[0],ar))//get data
        {
            b=false;
            break;
        }
        else
        {
            inventory_format(ar,ls);
            if(!mod_row(pTable,ls,ITEM_POSITION_BOTTOM,false,false,-1))
            {
                b=false;
                break;
            }
            ls.clear();
        }
        lsIds.removeFirst();
    }
    lsIds.clear();
    //-
    set_block_signal_and_sort(pTable,false,true);
    if(iSelectedID!=-1)
        select_row(pTable,iSelectedID);
    return b;
    */
}

bool CWidgetInterface::inventory_insert_row(QTableWidget * pTable, CArticle & ar, int iPosition, bool bSelect)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=ar.get_id();
    //-
    inventory_format(ar,ls);
    b=mod_row(pTable,ls,iPosition,true,false,iSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::inventory_update_row(QTableWidget * pTable, CArticle & ar, bool bSelect, bool bAddIfNotExis)
{
    if(pTable==NULL)
        return false;
    //-
    bool b=false;
    int iSelect=-1;
    QList<CTableItemData> ls;
    //-
    if(bSelect)
        iSelect=ar.get_id();
    //-
    inventory_format(ar,ls);
    int row=find_row(pTable,ar.get_id());
    if(row>=0 && row<pTable->rowCount())//found?
        b=mod_row(pTable,ls,row,true,true,iSelect);
    else if(bAddIfNotExis)
        b=inventory_insert_row(pTable,ar,-2,bSelect);
    //-
    ls.clear();
    return b;
}

bool CWidgetInterface::inventory_format(CArticle & ar, QList<CTableItemData> & lsData)
{
    if(m_pDb==NULL)
        return false;
    if(m_tcInventory.get_table()==NULL || !m_pDb->is_db_connect())
        return false;

    int i,j;
    QString s;
    CTableItemData ti;
    QTableWidget * pTable=m_tcInventory.get_table();
    QList<int> lsOrder,lsAlignment;
    m_tcInventory.get_columns_order(lsOrder);
    m_tcInventory.get_columns_alignment(lsAlignment);

    if(lsOrder.count()==pTable->columnCount() && lsAlignment.count()==pTable->columnCount())
    {
        for(j=0;j<pTable->columnCount();j++)
        {
            if(lsOrder[j]==0)//default column 0
                inventory_get_icon_and_precent(ar,ti);
            if(lsOrder[j]==1)//d.column 1
                inventory_get_icon_and_warning_limit(ar,ti);
            if(lsOrder[j]==2)//d.column 2
            {
                s=QString("%1").arg(ar.get_inventory());
                if(ar.get_unit().length()>0)//unit set?
                    s+=QString(" %1").arg(ar.get_unit());
                ti.set_text(s);
            }
            if(lsOrder[j]==3)//d.column 3
            {
                i=m_pDb->ordering_get_count_by_article(ar.get_id());
                if(i>0)
                {
                    s=QString("%1").arg(i);
                    if(ar.get_unit().length()>0)
                        s+=QString(" %1").arg(ar.get_unit());
                    i=m_pDb->ordering_get_ordering_count_by_article(ar.get_id());
                    if(i>0)
                        s+=QString(" / %1").arg(i);
                }
                ti.set_text(s);
            }
            if(lsOrder[j]==4)//d.column 4
            {
                if(ar.get_max_inventory()>0)
                {
                    s=QString("%1").arg(ar.get_max_inventory());
                    if(ar.get_unit().length()>0)//unit set?
                        s+=QString(" %1").arg(ar.get_unit());
                }
                ti.set_text(s);
            }
            if(lsOrder[j]==5)//d.column 5
                ti.set_text(ar.get_name());
            if(lsOrder[j]==6)//d.column 6
                ti.set_text(ar.get_articlenumber());
            if(lsOrder[j]==7)//d.column 7
                ti.set_text(ar.get_articlenumber2());
            if(lsOrder[j]==8)//d.column 8
            {
                s=m_pDb->maker_get_name(ar.get_maker_id());
                ti.set_text(s);
            }
            if(lsOrder[j]==9)//d.column 9
            {
                s=m_pDb->waregroup_get_path(ar.get_waregroup_id(),3);
                ti.set_text(s);
            }
            if(lsOrder[j]==10)//d.column 10
                ti.set_text(ar.get_location());
            if(lsOrder[j]==11)//d.column 11
                ti.set_text(ar.get_comment());
            if(lsOrder[j]==12)//d.column 12
            {
                s=QString("%1").arg(ar.get_id());
                ti.set_text(s);
            }
            //-
            ti.set_alignment(lsAlignment[j]);//alignment
            lsData.push_back(ti);//add
            //-
            ti.clear();
            s=QString("");
        }
    }
    //-
    lsOrder.clear();
    lsAlignment.clear();
    return true;
}

void CWidgetInterface::inventory_get_icon_and_warning_limit(CArticle & ar, CTableItemData & ti, int iAlignment)
{
    QString s=QString("");
    bool bIcon=false;
    int iLimit=ar.get_warning_limit();
    int inv=ar.get_inventory();
    if(iLimit>=0)
    {
        if(inv<iLimit)
        {
            bIcon=true;
            s=QString("Bst.<%1").arg(ar.get_warning_limit());
        }
    }
    if(!bIcon)
        ti.set(s,ti.get_alignment());
    else
        ti.set(s,iAlignment,&m_icoWarning);
}

void CWidgetInterface::inventory_get_icon_and_precent(CArticle & ar, CTableItemData & ti, int iAlignment)
{
    int i=0,j=0,icon_index=0;
    QString sText;
    //-
    if(ar.get_max_inventory()<=0)//max.inventory not set
        icon_index=7;
    else if(ar.get_inventory()<=0)//empty?
        icon_index=8;
    else if(ar.get_inventory()>=ar.get_max_inventory())
        icon_index=6;
    //-
    if(ar.get_max_inventory()>0)
    {
        i=(int)((float)ar.get_inventory())/((float)ar.get_max_inventory())*((float)100);
        sText=QString("%1 %").arg(i);
    }
    //-
    if(icon_index==0)
    {
        if(i<10)
            j=1;
        else if(i<=20)
            j=2;
        else if(i<=40)
            j=3;
        else if(i<=60)
            j=4;
        else if(i<=80)
            j=5;
        else
            j=6;
        icon_index=j-1;
    }
    if(icon_index >=0 && icon_index<m_lsIconsArticle.count())
        ti.set(sText,iAlignment,&m_lsIconsArticle[icon_index]);
}


//--logbook------------------------------------------------------------------------------------------------
bool CWidgetInterface::logbook_update_list(QListWidget * pList, QList<QDateTime> & lsDateTime, QDateTime dt_tiSelect)
{
    if(pList==NULL || m_pDb==NULL)
        return false;
    if(!m_pDb->is_db_connect())
        return false;
    //-
    bool b=true,bSelect;
    CLogbook lg;
    //-
    pList->setSortingEnabled(false);
    pList->blockSignals(true);//faster insert
    //-clear-
    pList->clear();
    //-
    while(lsDateTime.count()>0)
    {
        if(!m_pDb->logbook_get(lsDateTime[0],lg))//get data
        {
            b=false;
            break;
        }
        else
        {
            if(lsDateTime[0]==dt_tiSelect)
                bSelect=true;
            else
                bSelect=false;
            if(!logbook_insert_item(pList,lg,ITEM_POSITION_BOTTOM,bSelect))
            {
                b=false;
                break;
            }
        }
        lsDateTime.removeFirst();
    }
    lsDateTime.clear();
    //-
    pList->setSortingEnabled(true);
    pList->sortItems(Qt::DescendingOrder);
    pList->blockSignals(false);
    return b;
}

bool CWidgetInterface::logbook_insert_item(QListWidget * pList, CLogbook & lg, int iPosition, bool bSelect)
{
    if(pList==NULL)
        return false;
    //-
    bool b=false;
    QString sOutput;
    int row=check_position(pList,iPosition);
    if(row>=0)
    {
        sOutput=lg.get_output_string();
        if(sOutput.length()>0)
        {
            pList->insertItem(row,sOutput);
            if(bSelect)
                pList->setCurrentRow(row);
            b=true;
        }
    }
    return b;
}

bool CWidgetInterface::logbook_get_mask_condition(QDate dt,int iIndex,QString & sCondition)
{
    int typeF,typeT;
    //-
    if(iIndex==0)//all
        typeF=typeT=-1;
    else if(iIndex==1)//trade
    {
        typeF=LOGBOOK_TYPE_TRADE_IN;
        typeT=LOGBOOK_TYPE_TRADE_CANCEL;
    }
    else if(iIndex==2)//ordering
    {
        typeF=LOGBOOK_TYPE_ORDERING_NEW;
        typeT=LOGBOOK_TYPE_ORDERING_REMOVE;
    }
    else if(iIndex==3)//article
    {
        typeF=LOGBOOK_TYPE_ARTICLE_NEW;
        typeT=LOGBOOK_TYPE_ARTICLE_REMOVE;
    }
    else if(iIndex==4)//waregroup
    {
        typeF=LOGBOOK_TYPE_WAREGROUP_NEW;
        typeT=LOGBOOK_TYPE_WAREGROUP_REMOVE;
    }
    else if(iIndex==5)//maker
    {
        typeF=LOGBOOK_TYPE_MAKER_NEW;
        typeT=LOGBOOK_TYPE_MAKER_REMOVE;
    }
    else if(iIndex==6)//dealer
    {
        typeF=LOGBOOK_TYPE_DEALER_NEW;
        typeT=LOGBOOK_TYPE_DEALER_REMOVE;
    }
    else if(iIndex==7)//customer
    {
        typeF=LOGBOOK_TYPE_CUSTOMER_NEW;
        typeT=LOGBOOK_TYPE_CUSTOMER_REMOVE;
    }
    else
        return false;//index out of range
    //-
    sCondition=QString("date_time like '%1%'").arg(dt.toString("yyyy-MM-dd"));
    if(typeF!=-1)
    {
        sCondition+=QString(" AND type >= %1").arg(typeF);
        sCondition+=QString(" AND type <= %1").arg(typeT);
    }
    //-
    return true;
}

bool CWidgetInterface::logbook_update_list_date(QListWidget * pList, int iYear, int iIndex, QDate dtSelect)
{
    if(pList==NULL || iYear<2009 || iYear>2999 || m_pDb==NULL)
        return false;
    //-
    pList->blockSignals(true);//faster insert
    //-
    bool b=true;
    int j=0,i=0,count;
    QString s,sCondition;
    QStringList sl,slWeekDay;
    QListWidgetItem * pItem=NULL;
    //-
    slWeekDay<<"Mo"<<"Di"<<"Mi"<<"Do"<<"Fr"<<"Sa"<<"So";
    QDate dtCurrentDate=QDate().currentDate();
    QDate dt,dtFromDate,dtToDate,dtMark;
    //-
    dtFromDate=QDate(iYear,1,1);
    if(dtCurrentDate.year()==iYear)//current year?
        dtToDate=dtCurrentDate;//only to current date
    else//another year
        dtToDate=QDate(iYear,12,31);
    //-
    dt=dtFromDate;
    while(dt<=dtToDate)
    {
        i++;
        //-
        if(dt==dtSelect)//mark index for select
            j=i;
        //-
        s=dt.toString(QString("dd.MM.yyyy"));
        s+=QString(" (%1.)").arg(slWeekDay[dt.dayOfWeek()-1]);
        //-
        dtMark=dt;
        dt=dt.addDays(1);

        //count of trade's in this time
        if(m_pDb!=NULL)
        {
            if(!logbook_get_mask_condition(dtMark,iIndex,sCondition))
            {
                b=false;
                break;
            }
            count=m_pDb->logbook_get_count(sCondition);
            if(count>0)
                s+=QString(" |%1").arg(count);
        }
        //-
        sl.push_back(s);
    }
    //-insert-
    if(b)
    {
        for(i=sl.count()-1;i>=0;i--)
        {
            pItem= new QListWidgetItem(sl[i]);
            if(pItem!=NULL)
            {
                pList->addItem(pItem);
                if(j-1==i)
                    pList->setCurrentItem(pItem);
            }
        }
    }
    //-
    sl.clear();
    slWeekDay.clear();
    //-
    pList->blockSignals(false);
    //-
    return b;
}

bool CWidgetInterface::logbook_refresh_list_date(QListWidget * pList, int iIndex)
{
    if(m_pDb==NULL || pList==NULL)
        return false;

    bool b=true;
    int i,count;
    QString s,s2,sCondition;
    QStringList sl;
    QDate dt;
    QListWidgetItem * pItem=NULL;
    //-
    for(i=0;i<pList->count();i++)
    {
        pItem=pList->item(i);
        if(pItem!=NULL)
        {
            s=pItem->text();
            if(logbook_get_selecteted_mask_date(s,dt))
            {
                if(!logbook_get_mask_condition(dt,iIndex,sCondition))
                {
                    b=false;
                    break;
                }
                //-
                count=m_pDb->logbook_get_count(sCondition);
                s2=QString("%1").arg(count);
                sl=s.split("|");
                if(sl.count()==2)
                {
                    if(s2!=sl[1] && count>0)//count update?
                    {
                        s=QString("%1|%2").arg(sl[0],s2);
                        pItem->setText(s);
                    }
                    if(count<=0)
                        pItem->setText(sl[0]);
                }
                if(sl.count()==1 && count>0)
                {
                    s+=QString(" |%1").arg(s2);
                    pItem->setText(s);
                }
            }
        }
        sl.clear();
        pItem=NULL;
    }
    //-
    return b;
}

bool CWidgetInterface::logbook_get_selecteted_mask_date(QString sDateItem, QDate & dtDate)
{
    if(sDateItem.length()<=0)
        return false;
    //-
    bool b=true;
    QString s;
    //-
    s=sDateItem;
    s.resize(10);//copy only 10 char (dd.mm.yyyy)
    if(!format_and_check_date(s,dtDate))
        b=false;
    //-
    return b;
}


//tables (lists)---------------------------------------------------------------------------------------------------------------------
bool CWidgetInterface::balancelist_update_tablewidget(QTableWidget * pTable, QDate dtFrom, QDate dtTo, int iArticleId)
{
    if(m_pDb==NULL || iArticleId<0 || pTable==NULL)
        return false;

    QString sDays[]={"Mo.","Di.","Mi.","Do.","Fr.","Sa.","So."};
    QDate dt=dtFrom,dtInvStart;
    if(dtTo<dtFrom)
        dtTo=dtFrom;
    dtInvStart=dtFrom.addDays(-1);
    int count_before=m_pDb->article_get_inventory_on_date(iArticleId,dtInvStart);//inventory from end of the day befor
    QList<QString> lssTrades;
    QString sUnit,s,sCondition;
    int k=0,count,i,count_in=0,count_out=0,row=0;
    double d1,d2;
    CTrade trade;
    QColor colRed(170,0,0);
    QTableWidgetItem * pItem=NULL;
    bool b=true;
    CArticle ar;
    if(m_pDb->article_get(iArticleId,ar))//article found?
        sUnit=ar.get_unit(); //unit

    set_block_signal_and_sort(pTable,true,false);//faster

    //remove all rows
    remove_all_rows(pTable);

    while(dt<=dtTo)
    {
        //get all records by condition
        sCondition=QString("date = '%1' ").arg(dt.toString(QString("yyyy-MM-dd")));
        sCondition+=QString(" AND wares like '%x%1|%' AND canceled=0").arg(iArticleId);
        m_pDb->trade_get_all(sCondition,lssTrades);//get all records by trade nummer
        if(lssTrades.count()>0)//found?
        {
            //get in - out count
            for(i=0;i<lssTrades.count();i++)
            {
                if(m_pDb->trade_get(lssTrades[i],trade))//record found?
                {
                    count=trade.get_article_count(iArticleId);
                    k=trade.get_type();
                    if(k==TYPE_OUTGOING || k==TYPE_CUSTOMER_OUTGOING)
                        count_out+=count;
                    else
                        count_in+=count;
                }
            }
            //-
            pTable->insertRow(row);//new row
            for(i=0;i<9;i++)
            {
                if(i==0)//date
                {
                    s=QString("%1").arg(dt.toString(QString("yyyy-MM-dd")));
                    s+=QString(" , (%1)").arg(sDays[dt.dayOfWeek()-1]);
                    pItem=new QTableWidgetItem(s);
                }
                if(i==1)//count in
                {
                    s=QString("%1").arg(count_in);
                    if(sUnit.length()>0)//unit set?
                        s+=QString(" %1").arg(sUnit);
                    pItem=new QTableWidgetItem(s);
                    if(pItem!=NULL)
                        pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                }
                if(i==2)//count out
                {
                    s=QString("%1").arg(count_out);
                    if(sUnit.length()>0)//unit set?
                        s+=QString(" %1").arg(sUnit);
                    pItem=new QTableWidgetItem(s);
                    if(pItem!=NULL)
                        pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                }
                if(i==3)//diff
                {
                    k=count_in-count_out;
                    s=QString("%1").arg(k);
                    if(sUnit.length()>0)//unit set?
                        s+=QString(" %1").arg(sUnit);
                    pItem=new QTableWidgetItem(s);
                    if(pItem!=NULL)
                    {
                        if(k<0)
                            pItem->setTextColor(colRed);
                        pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                    }
                }
                if(i==4)//count before
                {
                    s=QString("%1").arg(count_before);
                    if(sUnit.length()>0)//unit set?
                        s+=QString(" %1").arg(sUnit);
                    pItem=new QTableWidgetItem(s);
                    if(pItem!=NULL)
                        pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                }
                if(i==5)//count behind
                {
                    s=QString("%1").arg(count_before+k);
                    if(sUnit.length()>0)//unit set?
                        s+=QString(" %1").arg(sUnit);
                    pItem=new QTableWidgetItem(s);
                    if(pItem!=NULL)
                        pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                }
                if(i==6)
                {
                    if(ar.get_base_price()>0.0)
                    {
                        d1=count_in*ar.get_base_price();
                        s=QString("%1").arg(d1);
                        if(ar.get_valuta()!=QString(""))
                            s+=QString(" %1").arg(ar.get_valuta());
                        pItem=new QTableWidgetItem(s);
                        if(pItem!=NULL)
                            pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                    }
                }
                if(i==7)
                {
                    if(ar.get_sales_price()>0.0)
                    {
                        d2=count_out*ar.get_sales_price();
                        s=QString("%1").arg(d2);
                        if(ar.get_valuta()!=QString(""))
                            s+=QString(" %1").arg(ar.get_valuta());
                        pItem=new QTableWidgetItem(s);
                        if(pItem!=NULL)
                            pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                    }
                }
                if(i==8)
                {
                    if(ar.get_base_price()>0.0 && ar.get_sales_price()>0.0)
                    {
                        s=QString("%1").arg(d2-d1);
                        if(ar.get_valuta()!=QString(""))
                            s+=QString(" %1").arg(ar.get_valuta());
                        pItem=new QTableWidgetItem(s);
                        if(pItem!=NULL)
                        {
                            pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                            if(d2-d1<0.0)
                                pItem->setTextColor(colRed);
                        }
                    }
                }
                if(pItem==NULL)
                {
                    b=false;
                    break;
                }
                pTable->setItem(row,i,pItem);
            }
            //-
            lssTrades.clear();
            count_before+=count_in;
            count_before-=count_out;
            count_in=count_out=0;
            row++;
        }
        dt=dt.addDays(1);//next day
    }

    set_block_signal_and_sort(pTable,false,false);

    return b;
}

bool CWidgetInterface::warnlist_update_tablewidget(QTableWidget * pTable)
{
    if(m_pDb==NULL && pTable!=NULL)
        return false;

    set_block_signal_and_sort(pTable,true,false);//faster

    QString s2,s,sCondition=QString("inventory < warning_limit");
    QList<int> lsArtIds;
    int i,row=0;
    CArticle ar;
    QTableWidgetItem * pItem=NULL;

    m_pDb->article_get_all(sCondition,lsArtIds);
    while(lsArtIds.count()>0)
    {
        if(m_pDb->article_get(lsArtIds[0],ar))//art found?
        {
            pTable->insertRow(row);
            for(i=0;i<pTable->columnCount();i++)
            {
                s=QString("");
                if(i==0)//name
                    s=ar.get_name();
                if(i==1)//1.art.nr
                    s=ar.get_articlenumber();
                if(i==2)//2.art.nr
                    s=ar.get_articlenumber2();
                if(i==3)//maker
                    s=m_pDb->maker_get_name(ar.get_maker_id());
                if(i==4)//waregroup
                    s=m_pDb->waregroup_get_path(ar.get_waregroup_id());
                if(i==5)//inventory
                {
                    s=QString("%1").arg(ar.get_inventory());
                    s2=ar.get_unit();
                    if(s2.length()>0)//set?
                        s+=QString(" %1").arg(s2);
                }
                if(i==6)//warnlimit
                {
                    s=QString("%1").arg(ar.get_warning_limit());
                    s2=ar.get_unit();
                    if(s2.length()>0)//set?
                        s+=QString(" %1").arg(s2);
                }
                //-
                pItem=new QTableWidgetItem(s);
                if(pItem!=NULL)
                {
                    if(i>=5)
                        pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                    pTable->setItem(row,i,pItem);
                }
            }
        }
        //-
        lsArtIds.removeFirst();
        row++;
    }
    //-
    lsArtIds.clear();

    set_block_signal_and_sort(pTable,false,true);

    return true;
}

bool CWidgetInterface::inventorys_on_date_update_tablewidget(QTableWidget * pTable, QDate dt)
{
    if(m_pDb==NULL && pTable!=NULL)
        return false;

    set_block_signal_and_sort(pTable,true,false);//faster

    //delete old rows
    remove_all_rows(pTable);

    QString s2,s;
    QList<int> lsArtIds;
    int i,count=0,row=0;
    CArticle ar;
    QTableWidgetItem * pItem=NULL;

    m_pDb->article_get_all(QString(""),lsArtIds);
    while(lsArtIds.count()>0)
    {
        if(m_pDb->article_get(lsArtIds[0],ar))//art found?
        {
            pTable->insertRow(row);
            for(i=0;i<pTable->columnCount();i++)
            {
                s=QString("");

                if(i==0)//inventory on date
                {
                    count=(int)m_pDb->article_get_inventory_on_date(ar.get_id(),dt);
                    if(count>=0)
                    {
                        s=QString("%1").arg(count);
                        s2=ar.get_unit();
                        if(s2.length()>0)//set?
                            s+=QString(" %1").arg(s2);
                    }
                }
                if(i==1)//name
                    s=ar.get_name();
                if(i==2)//1.art.nr
                    s=ar.get_articlenumber();
                if(i==3)//2.art.nr
                    s=ar.get_articlenumber2();
                if(i==4)//maker
                    s=m_pDb->maker_get_name(ar.get_maker_id());
                if(i==5)//waregroup
                    s=m_pDb->waregroup_get_path(ar.get_waregroup_id());
                //-
                pItem=new QTableWidgetItem(s);
                if(pItem!=NULL)
                {
                    if(i==0)
                        pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                    pTable->setItem(row,i,pItem);
                }
            }
        }
        //-
        lsArtIds.removeFirst();
        row++;
    }
    //-
    lsArtIds.clear();

    set_block_signal_and_sort(pTable,false,true);

    return true;
}

bool CWidgetInterface::inventorylist_update_tablewidget(QTableWidget * pTable)
{
    if(m_pDb==NULL && pTable!=NULL)
        return false;

    set_block_signal_and_sort(pTable,true,false);//faster

    QString s;
    QList<int> lsArtIds;
    int i,row=0;
    CArticle ar;
    QTableWidgetItem * pItem=NULL;

    m_pDb->article_get_all(QString(""),lsArtIds);
    while(lsArtIds.count()>0)
    {
        if(m_pDb->article_get(lsArtIds[0],ar))//art found?
        {
            pTable->insertRow(row);
            for(i=0;i<pTable->columnCount();i++)
            {
                s=QString("");
                if(i==0)//inv/max.inv
                {
                    s=QString("%1").arg(ar.get_inventory());
                    if(ar.get_max_inventory()>0)
                        s+=QString(" / %1").arg(ar.get_max_inventory());
                    if(ar.get_unit().length()>0)
                        s+=QString(" %1").arg(ar.get_unit());
                }
                if(i==1)//art.name
                    s=ar.get_name();
                if(i==2)//1/2.art.nr
                {
                    if(ar.get_articlenumber().length()>0)
                        s+=QString("'%1'").arg(ar.get_articlenumber());
                    s+=QString(" | ");
                    if(ar.get_articlenumber2().length()>0)
                        s+=QString("'%1'").arg(ar.get_articlenumber2());
                }
                if(i==3)//maker
                    s=m_pDb->maker_get_name(ar.get_maker_id());
                if(i==4)//waregroup
                    s=m_pDb->waregroup_get_path(ar.get_waregroup_id());
                if(i==5)//location / comment
                {
                    if(ar.get_location().length()>0)
                        s+=ar.get_location();
                    s+=QString(" | ");
                    if(ar.get_comment().length()>0)
                        s+=ar.get_comment();
                }
                //-
                pItem=new QTableWidgetItem(s);
                if(pItem!=NULL)
                {
                    if(i>=5)
                        pItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);//right alignment
                    pTable->setItem(row,i,pItem);
                }
            }
        }
        //-
        lsArtIds.removeFirst();
        row++;
    }
    //-
    lsArtIds.clear();

    set_block_signal_and_sort(pTable,false,true);

    return true;
}
