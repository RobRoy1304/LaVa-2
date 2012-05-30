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

#ifndef CWIDGETINTERFACE_H
#define CWIDGETINTERFACE_H

#include <QtGui>
#include "data_classes.h"
#include "cdbconnection.h"

//item position top,bottom or row number(for tables)
#define ITEM_POSITION_TOP -1
#define ITEM_POSITION_BOTTOM -2

//formats
#define FORMAT_ONE 0
#define FORMAT_TWO 1
#define FORMAT_THREE 2
#define FORMAT_FOUR 3


//item for better sort----------------------------------------------
class CMyTableWidgetItem: public QTableWidgetItem
 {
 public:
     bool operator< ( const QTableWidgetItem & other ) const
     {
         double d1,d2;
         QStringList ls;
         bool b1,b2,bReturn=false;
         QString s1=this->text();
         QString s2=other.text();
         //-
         ls=s1.split(" ");
         if(ls.count()>0)
            s1=ls[0];
         ls.clear();
         ls=s2.split(" ");
         if(ls.count()>0)
            s2=ls[0];
         ls.clear();
         //-
         d1=s1.toDouble(&b1);
         d2=s2.toDouble(&b2);
         //-
         if(b1==true && b2==true)//2* number?
         {
             if(d1<d2)
                 bReturn=true;
             else{}
         }
         else
             bReturn=QTableWidgetItem::operator<(other);

         //-
         return bReturn;
     }
 };



//---------------------------------------------------------
class CWidgetInterface
{

private:
    CDbConnection * m_pDb; //database connection

    //for tables
    QList<QIcon> m_lsIconsArticle;
    QList<QIcon> m_lsIconsTrade;
    QIcon m_icoWarning;

public:
    CTableColumnsData m_tcInventory;
    CTableColumnsData m_tcArticle;

    //basic
    CWidgetInterface();
    ~CWidgetInterface();
    bool set_db(CDbConnection * pDb);
    int check_position(QTableWidget * pTable, int iPosition);//for tables
    int check_position(QListWidget * pList, int iPosition);//for lists
    bool load_icons(void);//for tables
    bool insert_list_element(QListWidget * pList, QString & sItem, QString & sData, bool bSpaceRow=true);//for lists
    bool format_and_check_date(QString s,QDate & dt);
    QString check_date(QString s);
    bool cast_count_and_id(QString sData, int & iCount, int & iId);
    bool get_table_column_width(QTableWidget * pTable, QList<int> & lsIntWidth);
    bool get_tree_column_width(QTreeWidget * pTree, QList<int> & lsIntWidth);
    QIcon get_warning_icon(void);
    bool get_trade_icons(QList<QIcon> & lsIcons);
    bool set_block_signal_and_sort(QTableWidget * pTable, bool bBlock, bool bSort);
    bool remove_all_rows(QTableWidget * pTable);
    bool remove_row(QTableWidget * pTable,int iId,int iIdColumn=-1);//iIdColumn=-1 -> last column
    bool remove_row(QTableWidget * pTable,QString sValue,int iIdColumn=-1);//iIdColumn=-1 -> last column
    int find_row(QTableWidget * pTable,int iId,int iIdColumn=-1);//iIdColumn=-1 -> last column
    int find_row(QTableWidget * pTable,QString sValue,int iIdColumn=-1);//iIdColumn=-1 -> last column
    bool find_and_select_row(QTableWidget * pTable, int iId, int iColumn=-1);//iIdColumn=-1 -> last column
    bool find_and_select_row(QTableWidget * pTable, QString sValue, int iColumn=-1);//iIdColumn=-1 -> last column
    bool select_row(QTableWidget * pTable,int iId,int iIdColumn=-1);//iIdColumn=-1 -> last column
    bool select_row(QTableWidget * pTable,QString sValue,int iIdColumn=-1);//iIdColumn=-1 -> last column
    bool get_selected_table_item_value(QTableWidget *pTable,int & iValue, int iColumn=-1);//iColumn=-1 -> last column
    bool get_selected_table_item_value(QTableWidget *pTable,QString & sValue, int iColumn=-1);//iColumn=-1 -> last column
    bool mod_row(QTableWidget * pTable, QList<CTableItemData> & lsData, int iPosition=ITEM_POSITION_BOTTOM, bool bBlock=false, bool bUpdate=false, int iSelectId=-1,int iIdColumn=-1);//bUpdate=false->insert,true->update | iSelectId=-1 -> no select  |  iIdColumn=-1 -> last column
    bool mod_row(QTableWidget * pTable, QList<CTableItemData> & lsData, int iPosition=ITEM_POSITION_BOTTOM, bool bBlock=false, bool bUpdate=false, QString sSelectValue=QString(""),int iIdColumn=-1);//bUpdate=false->insert,true->update | iSelectValue="" -> no select  |  iIdColumn=-1 -> last column
    bool fill_row(QTableWidget * pTable,int iRow,QList<CTableItemData> & lsData);
    bool update_table_by_tablecolumnsdata(CTableColumnsData & tcdOld,CTableColumnsData & tcdNew, QList<QString> & lsHeaderNames);

    //waregroup
    bool waregroup_update_treewidget(QTreeWidget * pTree, int iSelectedID=-1);
    bool waregroup_insert_children_treeitems(QTreeWidget * pTree, QTreeWidgetItem * pParent, int iSelectedID=-1);
    bool waregroup_clear_treewidget(QTreeWidget * pTree);


    //maker -tables-
    bool maker_update_tablewidget(QTableWidget * pTable, QList<int> & lsIDs, int iSelectedID=-1);
    bool maker_insert_row(QTableWidget * pTable, CMaker & mk, int iPosition=ITEM_POSITION_BOTTOM, bool bSelect=false);
    bool maker_update_row(QTableWidget * pTable, CMaker & mk, bool bSelect=false);
    bool maker_format(CMaker &mk, QList<CTableItemData> & lsData);
    //maker -list-
    bool maker_update_list(QListWidget * pList, int iID);//if iId==-1 ->empty list
    //maker -combobox-
    bool maker_fill_combobox(QComboBox * pCombobox);


    //dealer -tables-
    bool dealer_update_tablewidget(QTableWidget * pTable, QList<int> & lsIDs, int iSelectedID=-1);
    bool dealer_insert_row(QTableWidget * pTable, CDealer & de, int iPosition=ITEM_POSITION_BOTTOM, bool bSelect=false);
    bool dealer_update_row(QTableWidget * pTable, CDealer & de, bool bSelect=false);
    bool dealer_format(CDealer &de, QList<CTableItemData> & lsData);
    //dealer -list-
    bool dealer_update_list(QListWidget * pList, int iID=-1);//if iId==-1 ->empty list
    //dealer -combobox-
    bool dealer_fill_combobox(QComboBox * pCombobox);


    //customer -tables-
    bool customer_update_tablewidget(QTableWidget * pTable, QList<int> & lsIDs, int iSelectedID=-1);
    bool customer_insert_row(QTableWidget * pTable, CCustomer & cu, int iPosition=ITEM_POSITION_BOTTOM, bool bSelect=false);
    bool customer_update_row(QTableWidget * pTable, CCustomer & cu, bool bSelect=false);
    bool customer_format(CCustomer & cu, QList<CTableItemData> & lsData);
    //customer -list-
    bool customer_update_list(QListWidget * pList, int iID=-1);//if iId==-1 ->empty list


    //article -tables-
    bool article_update_tablewidget(QTableWidget * pTable, QList<int> & lsIDs, int iFormatType, int iSelectedID=-1);
    bool article_update_tablewidget_wares_list(QTableWidget * pTable,QList<QString> & lsWares,int iFormatType);
    bool article_insert_row(QTableWidget * pTable, CArticle & ar, int iFormatType, int iPosition=ITEM_POSITION_BOTTOM, bool bSelect=false);
    bool article_update_row(QTableWidget * pTable, CArticle & ar, int iFormatType, bool bSelect=false);
    bool article_format(CArticle & ar, QList<CTableItemData> & lsData, int iFormatType);
    bool article_update_row_wareslist(QTableWidget * pTable,QString sData,bool bEdit, bool bSelect);


    //ordering -tables-
    bool ordering_update_tablewidget(QTableWidget * pTable, QList<int> & lsIDs, int iFormatType, int iSelectedID=-1);
    bool ordering_update_tablewidget_wares_list(QTableWidget * pTable, int iOrderingID, int iFormatType);
    bool ordering_insert_row(QTableWidget * pTable, COrdering & ord, int iFormatType, int iPosition=ITEM_POSITION_BOTTOM, bool bSelect=false);
    bool ordering_update_row(QTableWidget * pTable, COrdering & ord, int iFormatType, bool bSelect=false);
    bool ordering_format(COrdering & ord, QList<CTableItemData> & lsData, int iFormatType);


    //trade -tables-
    bool trade_update_tablewidget(QTableWidget * pTable, QList<QString> & lsBookingNumber, QString sSelect=QString(""));
    bool trade_update_tablewidget_wares_list(QTableWidget * pTable, QString sBookingNumber);
    bool trade_insert_row(QTableWidget * pTable, CTrade & trade, int iPosition=ITEM_POSITION_BOTTOM, bool bSelect=false);
    bool trade_update_row(QTableWidget * pTable, CTrade & trade, bool bSelect=false);
    bool trade_format(CTrade & trade, QList<CTableItemData> & lsData);
    void trade_get_icon(CTrade & trade, CTableItemData & ti, int iAlignment=TABLE_ALIGNMENT_LEFT);
    //trade -list-
    bool trade_update_list_info(QListWidget * pList, CTrade & tr);
    bool trade_update_list_date(QListWidget * pList, int iYear, int iIndexDate, QString sTextMask, int iIndexMask, QDate dtSelect=QDate(0,0,0));
    bool trade_refresh_list_date(QListWidget * pList, int iIndexDate, QString sTextMask, int iIndexMask);
    int trade_get_count_by_date_and_mask(QDate dtFrom, QDate dtTo, int iIndexMask, QString sTextMask);
    bool trade_get_selecteted_mask_date(QString sDateItem, int iIndex, QDate & dtFrom, QDate & dtTo);


    //inventory -tables-
    bool inventory_update_tablewidget(QTableWidget * pTable, QList<int> & lsIDs, int iSelectedID=-1);
    bool inventory_insert_row(QTableWidget * pTable, CArticle & ar, int iPosition=ITEM_POSITION_BOTTOM, bool bSelect=false);
    bool inventory_update_row(QTableWidget * pTable, CArticle & ar, bool bSelect=false);
    bool inventory_format(CArticle & ar, QList<CTableItemData> & lsData);
    void inventory_get_icon_and_warning_limit(CArticle & ar, CTableItemData & ti, int iAlignment=TABLE_ALIGNMENT_LEFT);
    void inventory_get_icon_and_precent(CArticle & ar, CTableItemData & ti, int iAlignment=TABLE_ALIGNMENT_LEFT);


    //logbook -lists-
    bool logbook_update_list(QListWidget * pList, QList<QDateTime> & lsDateTime, QDateTime dt_tiSelect=QDateTime(QDate(0,0,0),QTime(0,0,0,0)));
    bool logbook_insert_item(QListWidget * pList, CLogbook & lg, int iPosition, bool bSelect=false);
    bool logbook_update_list_date(QListWidget * pList, int iYear, int iIndex, QDate dtSelect=QDate(0,0,0));
    bool logbook_refresh_list_date(QListWidget * pList, int iIndex);
    bool logbook_get_selecteted_mask_date(QString sDateItem, QDate & dtDate);
    bool get_mask_condition_logbook(QDate dt,int iIndex,QString & sCondition);


    //tables (lists)
    bool balancelist_update_tablewidget(QTableWidget * pTable, QDate dtFrom, QDate dtTo, int iArticleId);
    bool warnlist_update_tablewidget(QTableWidget * pTable);
    bool inventorys_on_date_update_tablewidget(QTableWidget * pTable, QDate dt);
};

#endif // CWIDGETINTERFACE_H
