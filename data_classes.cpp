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

#include "data_classes.h"

//-ctableitemdata-------------------------------------------------------
CTableItemData::CTableItemData()
{
    m_iAlignment=TABLE_ALIGNMENT_LEFT;
}

QString CTableItemData::get_text(void)
{
    return m_sText;
}

int CTableItemData::get_alignment(void)
{
    return m_iAlignment;
}

QIcon CTableItemData::get_icon(void)
{
    return m_icon;
}

void CTableItemData::set_text(QString s)
{
    m_sText=s;
}

void CTableItemData::set_alignment(int i)
{
    m_iAlignment=i;
}

void CTableItemData::set_icon(QIcon ico)
{
    m_icon=ico;
}

void CTableItemData::set(QString sText, int iAlignment, QIcon * pIcon)
{
    m_sText=sText;
    m_iAlignment=iAlignment;
    if(pIcon!=NULL)
        m_icon=*pIcon;
}

void CTableItemData::clear(void)
{
    m_sText=QString("");
    m_iAlignment=TABLE_ALIGNMENT_LEFT;
    m_icon=QIcon();
}

//ctablecolumndata------------------------------------------------------------------------------------------------
CTableColumnsData::CTableColumnsData()
{
    m_iIdColumn=-1;
    m_pTable=NULL;
}

CTableColumnsData::~CTableColumnsData(void)
{
    m_lsColumnsOrder.clear();
    m_lsColumnsVisible.clear();
    m_lsColumnsAlignment.clear();
}

CTableColumnsData & CTableColumnsData::operator= (CTableColumnsData & right )
{
    QList<int> ls;
    QList<bool> lsb;

    if (*this != right)
    {
        right.get_columns_alignment(ls);
        m_lsColumnsAlignment.clear();
        m_lsColumnsAlignment=ls;

        ls.clear();
        right.get_columns_order(ls);
        m_lsColumnsOrder.clear();
        m_lsColumnsOrder=ls;

        right.get_columns_visible(lsb);
        m_lsColumnsVisible.clear();
        m_lsColumnsVisible=lsb;

        m_iIdColumn=right.get_id_column();
        m_pTable=right.get_table();
    }
    ls.clear();
    lsb.clear();
    return *this;
}

bool CTableColumnsData::operator != (CTableColumnsData& right)
{
    return ! (*this==right);
}

bool CTableColumnsData::operator == (CTableColumnsData & right)
{
    bool b=false;
    QList<int> ls;
    QList<bool> lsb;
    //-
    if(m_pTable==right.get_table())
    {
        right.get_columns_alignment(ls);
        if(m_lsColumnsAlignment==ls)
        {
            ls.clear();
            right.get_columns_order(ls);
            if(m_lsColumnsOrder==ls)
            {
                right.get_columns_visible(lsb);
                if(m_lsColumnsVisible==lsb)
                {
                    if(m_iIdColumn==right.get_id_column())
                        b=true;
                }
            }
        }
    }
    ls.clear();
    lsb.clear();
    return b;
}

int CTableColumnsData::get_columns_count(void)
{
    int columns=-1;
    if(m_pTable!=NULL)
    {
        columns=m_pTable->columnCount();
        if(columns!=m_lsColumnsAlignment.count())
            columns=m_lsColumnsAlignment.count();
        if(columns!=m_lsColumnsOrder.count())
            columns=m_lsColumnsOrder.count();
        if(columns!=m_lsColumnsVisible.count())
            columns=m_lsColumnsVisible.count();
    }
    return columns;
}

int CTableColumnsData::get_id_column(void)
{
    return m_iIdColumn;
}

QTableWidget * CTableColumnsData::get_table(void)
{
    return m_pTable;
}

void CTableColumnsData::get_columns_order(QList<int> & lsColumnsOrder)
{
    lsColumnsOrder=m_lsColumnsOrder;
}

void CTableColumnsData::get_columns_visible(QList<bool> & lsColumnsVisible)
{
    lsColumnsVisible=m_lsColumnsVisible;
}

void CTableColumnsData::get_columns_alignment(QList<int> & lsColumnsAlignment)
{
    lsColumnsAlignment=m_lsColumnsAlignment;
}

int CTableColumnsData::get_column_alignment(int iIndex)
{
    int agli=-1;
    if(iIndex>=0 && iIndex<m_lsColumnsAlignment.count())
        agli=m_lsColumnsAlignment[iIndex];
    return agli;
}

bool CTableColumnsData::get_column_visible(int iIndex)
{
    bool vis=false;
    if(iIndex>=0 && iIndex<m_lsColumnsVisible.count())
        vis=m_lsColumnsVisible[iIndex];
    return vis;
}

int CTableColumnsData::get_column_order(int iIndex)
{
    int order=-1;
    if(iIndex>=0 && iIndex<m_lsColumnsOrder.count())
        order=m_lsColumnsOrder[iIndex];
    return order;
}

int CTableColumnsData::get_count_not_visible_columns(void)
{
    int i,iReturn=0;
    for(i=0;i<m_lsColumnsVisible.count();i++)
    {
        if(m_lsColumnsVisible[i]==false)
            iReturn++;
    }
    if(i!=m_lsColumnsVisible.count())
        iReturn=-1;
    //-
    return iReturn;
}

void CTableColumnsData::set_id_column(int iColumn)
{
    m_iIdColumn=iColumn;
}

void CTableColumnsData::set_table(QTableWidget * pTable)
{
    m_pTable=pTable;
}

void CTableColumnsData::set_columns_visible(QList<bool> & lsColumnsVisible)
{
    if(lsColumnsVisible.count()==m_lsColumnsOrder.count())
            m_lsColumnsVisible=lsColumnsVisible;
}

void CTableColumnsData::set_columns_alignment(QList<int> & lsColumnsAlignment)
{
    if(lsColumnsAlignment.count()==m_lsColumnsOrder.count())
            m_lsColumnsAlignment=lsColumnsAlignment;
}

void CTableColumnsData::set_column_alignment(int iIndex, int iType)
{
    if(iIndex>=0 && iIndex<m_lsColumnsAlignment.count())
    {
        if(iType==TABLE_ALIGNMENT_LEFT || iType==TABLE_ALIGNMENT_RIGHT || iType==TABLE_ALIGNMENT_CENTER)
            m_lsColumnsAlignment[iIndex]=iType;
    }
}

void CTableColumnsData::set_column_visible(int iIndex, bool bVisible)
{
    if(iIndex>=0 && iIndex<m_lsColumnsVisible.count())
        m_lsColumnsVisible[iIndex]=bVisible;
}

bool CTableColumnsData::set(QList<int> & lsColumnsOrder,QList<bool> & lsColumnsVisible,QList<int> & lsColumnsAlignment)
{
    bool b=false;
    if(lsColumnsOrder.count()==lsColumnsVisible.count())
    {
        if(lsColumnsOrder.count()==lsColumnsAlignment.count())
        {
            m_lsColumnsOrder=lsColumnsOrder;
            m_lsColumnsVisible=lsColumnsVisible;
            m_lsColumnsAlignment=lsColumnsAlignment;
            b=true;
        }
    }
    return b;
}

bool CTableColumnsData::update_order(QList<int> & lsNewColumnsOrder)
{
    bool b=false;
    QList<bool> lsNewColumnsVisible;
    QList<int> lsNewColumnsAlignment;
    //-
    if(m_lsColumnsOrder.count()>0 && m_lsColumnsVisible.count()>0 && m_lsColumnsAlignment.count()>0)
    {
        if(lsNewColumnsOrder.count()==m_lsColumnsOrder.count() && lsNewColumnsOrder.count()==m_lsColumnsVisible.count() && lsNewColumnsOrder.count()==m_lsColumnsAlignment.count())
        {
            for(int i=0;i<lsNewColumnsOrder.count();i++)//change visible & alignment by new order
            {
                m_lsColumnsOrder[i]=lsNewColumnsOrder[i];
                lsNewColumnsVisible.push_back(m_lsColumnsVisible[i]);
                lsNewColumnsAlignment.push_back(m_lsColumnsAlignment[i]);
            }
            m_lsColumnsVisible.clear();
            m_lsColumnsAlignment.clear();
            m_lsColumnsVisible=lsNewColumnsVisible;
            m_lsColumnsAlignment=lsNewColumnsAlignment;
            lsNewColumnsOrder.clear();
        }
    }
    return b;
}

bool CTableColumnsData::swap_columns(int iRow1, int iRow2)
{
    bool b,bReturn=false;
    int i,columns_count=get_columns_count();
    if(iRow1>=0 && iRow2>=0 && iRow1<columns_count && iRow2<columns_count)
    {
        i=m_lsColumnsAlignment[iRow2];
        m_lsColumnsAlignment[iRow2]=m_lsColumnsAlignment[iRow1];
        m_lsColumnsAlignment[iRow1]=i;

        i=m_lsColumnsOrder[iRow2];
        m_lsColumnsOrder[iRow2]=m_lsColumnsOrder[iRow1];
        m_lsColumnsOrder[iRow1]=i;

        b=m_lsColumnsVisible[iRow2];
        m_lsColumnsVisible[iRow2]=m_lsColumnsVisible[iRow1];
        m_lsColumnsVisible[iRow1]=b;
    }
    return bReturn;
}

//memory----------------------------------------------------------------------------------------------------------
CPointerMemory::CPointerMemory()
{
    clear();
}

CPointerMemory::~CPointerMemory()
{
    clear();
}

void CPointerMemory::clear()
{
    m_pUInt=NULL;
    m_pBool=NULL;
    m_pInt=m_pInt2=m_pInt3=NULL;
    m_pString=m_pString2=NULL;
    m_pLsInt=NULL;
    m_pLsString=NULL;
    m_pTreeWidget=NULL;
    m_pTableWidget=m_pTableWidget2=NULL;
    m_pListwidget=NULL;
    m_pCombobox=NULL;
    m_pDate=m_pDate2=NULL;
    m_p_dt_tiLsDateTime=NULL;
    m_p_dt_tiDateTime=NULL;
}

unsigned int * CPointerMemory::get_uint( void)
{
    return m_pUInt;
}

QDate * CPointerMemory::get_date( void)
{
    return m_pDate;
}

QDate * CPointerMemory::get_date2( void)
{
    return m_pDate2;
}

bool * CPointerMemory::get_bool( void)
{
    return m_pBool;
}

QString * CPointerMemory::get_string( void)
{
    return m_pString;
}

QString * CPointerMemory::get_string2( void)
{
    return m_pString2;
}

int * CPointerMemory::get_int(void)
{
    return m_pInt;
}

int * CPointerMemory::get_int2(void)
{
    return m_pInt2;
}

int * CPointerMemory::get_int3(void)
{
    return m_pInt3;
}

QList<int> * CPointerMemory::get_int_list(void)
{
    return m_pLsInt;
}

QList<QString> * CPointerMemory::get_string_list(void)
{
    return m_pLsString;
}

QTreeWidget * CPointerMemory::get_treewidget(void)
{
    return m_pTreeWidget;
}

QTableWidget * CPointerMemory::get_tablewidget(void)
{
    return m_pTableWidget;
}

QTableWidget * CPointerMemory::get_tablewidget2(void)
{
    return m_pTableWidget2;
}

QListWidget * CPointerMemory::get_listwidget(void)
{
    return m_pListwidget;
}

QComboBox * CPointerMemory::get_combobox(void)
{
    return m_pCombobox;
}

QList<QDateTime> * CPointerMemory::get_date_time_list(void)
{
    return m_p_dt_tiLsDateTime;
}

QDateTime * CPointerMemory::get_date_time(void)
{
    return m_p_dt_tiDateTime;
}

void CPointerMemory::set_uint(unsigned int * pUInt)
{
    m_pUInt=pUInt;
}

void CPointerMemory::set_date(QDate * pDate)
{
    m_pDate=pDate;
}

void CPointerMemory::set_date2(QDate * pDate)
{
    m_pDate2=pDate;
}

void CPointerMemory::set_bool(bool * p)
{
    m_pBool=p;
}

void CPointerMemory::set_string(QString * p)
{
    m_pString=p;
}

void CPointerMemory::set_string2(QString * p)
{
    m_pString2=p;
}

void CPointerMemory::set_int(int * p)
{
    m_pInt=p;
}

void CPointerMemory::set_int2(int * p)
{
    m_pInt2=p;
}

void CPointerMemory::set_int3(int * p)
{
    m_pInt3=p;
}

void CPointerMemory::set_int_list(QList<int> * p)
{
    m_pLsInt=p;
}

void CPointerMemory::set_string_list(QList<QString> *p)
{
    m_pLsString=p;
}

void CPointerMemory::set_treewidget(QTreeWidget * p)
{
    m_pTreeWidget=p;
}

void CPointerMemory::set_tablewidget(QTableWidget * p)
{
    m_pTableWidget=p;
}

void CPointerMemory::set_tablewidget2(QTableWidget * p)
{
    m_pTableWidget2=p;
}

void CPointerMemory::set_listwidget(QListWidget * p)
{
    m_pListwidget=p;
}

void CPointerMemory::set_combobox(QComboBox * pCombobox)
{
    m_pCombobox=pCombobox;
}

void CPointerMemory::set_date_time_list(QList<QDateTime> * p_dt_tiLsDateTime)
{
    m_p_dt_tiLsDateTime=p_dt_tiLsDateTime;
}

void CPointerMemory::set_date_time(QDateTime * p_dt_tiDateTime)
{
    m_p_dt_tiDateTime=p_dt_tiDateTime;
}

//basic------------------------------------------------------------------------------------------------------------
CBasicData::CBasicData()
{
    m_sName=m_sComment=QString("");
    m_iId=-1;
}

int CBasicData::get_id(void)
{
    return m_iId;
}

QString CBasicData::get_name(void)
{
    return m_sName;
}

QString CBasicData::get_comment(void)
{
    return m_sComment;
}

void CBasicData::set_id(int iId)
{
    m_iId=iId;
}

void CBasicData::set_name(QString sName)
{
    m_sName=sName;
}

void CBasicData::set_comment(QString sComment)
{
    m_sComment=sComment;
}


//waregroup-----------------------------------------------------------------------------------------------
CWaregroup::CWaregroup()
{
    m_iParentId=-1;
    m_sName=m_sComment=QString("");
    m_iId=-1;
}

int CWaregroup::get_parent_id(void)
{
    return m_iParentId;
}

void CWaregroup::set_parent_id(int iParentId)
{
    m_iParentId=iParentId;
}

void CWaregroup::set(int iParent,QString sName,QString sComment, int iId)
{
    m_iParentId=iParent;
    m_sName=sName;
    m_sComment=sComment;
    m_iId=iId;
}

int CWaregroup::operator != (CWaregroup & wg)
{
    if(m_sName==wg.get_name()&&m_sComment==wg.get_comment()&&m_iId==wg.get_id()&&m_iParentId==wg.get_parent_id())
        return 0;
    return 1;
}


//maker----------------------------------------------------------------------------------------------------------------
CMaker::CMaker()
{
    m_sName=m_sComment=m_sAdress=m_sCallnumber=m_sFaxnumber=m_sEmail=m_sHomepage=m_sContectperson=QString("");
    m_iId=-1;
}

QString CMaker::get_adress(void)
{
    return m_sAdress;
}

QString CMaker::get_callnumber(void)
{
    return m_sCallnumber;
}

QString CMaker::get_faxnumber(void)
{
    return m_sFaxnumber;
}

QString CMaker::get_email(void)
{
    return m_sEmail;
}

QString CMaker::get_homepage(void)
{
    return m_sHomepage;
}

QString CMaker::get_contectperson(void)
{
    return m_sContectperson;
}

void CMaker::set_adress(QString s)
{
    m_sAdress=s;
}

void CMaker::set_callnumber(QString s)
{
    m_sCallnumber=s;
}

void CMaker::set_faxnumber(QString s)
{
    m_sFaxnumber=s;
}

void CMaker::set_email(QString s)
{
    m_sEmail=s;
}

void CMaker::set_homepage(QString s)
{
    m_sHomepage=s;
}

void CMaker::set_contectperson(QString s)
{
    m_sContectperson=s;
}

void CMaker::set(CMaker & mk)
{
    set_name(mk.get_name());
    set_id(mk.get_id());
    set_comment(mk.get_comment());
    set_adress(mk.get_adress());
    set_callnumber(mk.get_callnumber());
    set_contectperson(mk.get_contectperson());
    set_email(mk.get_email());
    set_faxnumber(mk.get_faxnumber());
    set_homepage(mk.get_homepage());
}


//dealer-------------------------------------------------------------------------------------------------------------------------------------
CDealer::CDealer()
{
    m_sName=m_sComment=m_sCustomernumber=m_sAdress=m_sCallnumber=m_sFaxnumber=m_sEmail=m_sHomepage=m_sContectperson=QString("");
    m_iId=-1;
}

QString CDealer::get_customernumber(void)
{
    return m_sCustomernumber;
}

void CDealer::set_customernumber(QString s)
{
    m_sCustomernumber=s;
}

void CDealer::set(CDealer & de)
{
    set_name(de.get_name());
    set_id(de.get_id());
    set_comment(de.get_comment());
    set_adress(de.get_adress());
    set_callnumber(de.get_callnumber());
    set_contectperson(de.get_contectperson());
    set_email(de.get_email());
    set_faxnumber(de.get_faxnumber());
    set_homepage(de.get_homepage());
    set_customernumber(de.get_customernumber());
}


//customer-------------------------------------------------------------------------------------------------------------------------
CCustomer::CCustomer()
{
    m_sCallnumber=m_sFaxnumber=m_sEmail=m_sName=m_sComment=m_sCustomerNumber=m_sFirstName=m_sStreet=m_sCity=m_sPostcode=QString("");
    m_iId=-1;
}

QString CCustomer::get_customernumber(void)
{
    return m_sCustomerNumber;
}

QString CCustomer::get_first_name(void)
{
    return m_sFirstName;
}

QString CCustomer::get_street(void)
{
    return m_sStreet;
}

QString CCustomer::get_city(void)
{
    return m_sCity;
}

QString CCustomer::get_postcode(void)
{
    return m_sPostcode;
}

QString CCustomer::get_callnumber(void)
{
    return m_sCallnumber;
}

QString CCustomer::get_faxnumber(void)
{
    return m_sFaxnumber;
}

QString CCustomer::get_email(void)
{
    return m_sEmail;
}

void CCustomer::set_customernumber(QString s)
{
    m_sCustomerNumber=s;
}

void CCustomer::set_first_name(QString s)
{
    m_sFirstName=s;
}

void CCustomer::set_street(QString s)
{
    m_sStreet=s;
}

void CCustomer::set_city(QString s)
{
    m_sCity=s;
}

void CCustomer::set_postcode(QString s)
{
    m_sPostcode=s;
}

void CCustomer::set_callnumber(QString s)
{
    m_sCallnumber=s;
}

void CCustomer::set_faxnumber(QString s)
{
    m_sFaxnumber=s;
}

void CCustomer::set_email(QString s)
{
    m_sEmail=s;
}

void CCustomer::set(CCustomer & cu)
{
    m_sCustomerNumber=cu.get_customernumber();
    m_sFirstName=cu.get_first_name();
    m_sStreet=cu.get_street();
    m_sCity=cu.get_city();
    m_sPostcode=cu.get_postcode();
    m_sName=cu.get_name();
    m_iId=cu.get_id();
    m_sComment=cu.get_comment();
    m_sCallnumber=cu.get_callnumber();
    m_sEmail=cu.get_email();
    m_sFaxnumber=cu.get_faxnumber();
}


//article----------------------------------------------------------------------------------------------------------------------
CArticle::CArticle()
{
    clear();
}

bool CArticle::clear(void)
{
    m_bDelete=false;
    m_sName=m_sComment=QString("");
    m_iId=-1;
    m_sUnit=m_sArticlenumber=m_sArticlenumber2=m_sLocation=m_sName=m_sComment=m_sValuta=QString("");
    m_iWarningLimit=m_iWaregroupId=m_iMakerId=m_iId=-1;
    m_uiInventory=m_uiMaxInventory=0;
    m_fBaseprice=m_fSalesprice=0.0;
    return true;
}

bool CArticle::get_delete(void)
{
    return m_bDelete;
}

QString CArticle::get_valuta(void)
{
    return m_sValuta;
}

QString CArticle::get_articlenumber(void)
{
    return m_sArticlenumber;
}

QString CArticle::get_articlenumber2(void)
{
    return m_sArticlenumber2;
}

float CArticle::get_base_price(void)
{
    return m_fBaseprice;
}

float CArticle::get_sales_price(void)
{
    return m_fSalesprice;
}

QString CArticle::get_unit(void)
{
    return m_sUnit;
}

QString CArticle::get_location(void)
{
    return m_sLocation;
}

int CArticle::get_maker_id(void)
{
    return m_iMakerId;
}

int CArticle::get_warning_limit(void)
{
    return m_iWarningLimit;
}

int CArticle::get_waregroup_id(void)
{
    return m_iWaregroupId;
}

unsigned int CArticle::get_inventory(void)
{
    return m_uiInventory;
}

unsigned int CArticle::get_max_inventory(void)
{
    return m_uiMaxInventory;
}

void CArticle::set_delete(bool b)
{
    m_bDelete=b;
}

void CArticle::set_articlenumber(QString s)
{
    m_sArticlenumber=s;
}

void CArticle::set_valuta(QString s)
{
    m_sValuta=s;
}

void CArticle::set_articlenumber2(QString s)
{
    m_sArticlenumber2=s;
}

void CArticle::set_base_price(float f)
{
    m_fBaseprice=f;
}

void CArticle::set_sales_price(float f)
{
    m_fSalesprice=f;
}

void CArticle::set_location(QString s)
{
    m_sLocation=s;
}

void CArticle::set_unit(QString s)
{
    m_sUnit=s;
}

void CArticle::set_maker_id(int id)
{
    m_iMakerId=id;
}

void CArticle::set_warning_limit(int iLimit)
{
    m_iWarningLimit=iLimit;
}

void CArticle::set_waregroup_id(int id)
{
    m_iWaregroupId=id;
}

void CArticle::set_inventory(unsigned int inv)
{
    m_uiInventory=inv;
}

void CArticle::set_max_inventory(unsigned int max)
{
    m_uiMaxInventory=max;
}

void CArticle::set(CArticle & ar)
{
    set_delete(ar.get_delete());
    set_unit(ar.get_unit());
    set_name(ar.get_name());
    set_id(ar.get_id());
    set_comment(ar.get_comment());
    set_articlenumber(ar.get_articlenumber());
    set_articlenumber2(ar.get_articlenumber2());
    set_base_price(ar.get_base_price());
    set_sales_price(ar.get_sales_price());
    set_inventory(ar.get_inventory());
    set_max_inventory(ar.get_max_inventory());
    set_location(ar.get_location());
    set_maker_id(ar.get_maker_id());
    set_waregroup_id(ar.get_waregroup_id());
    set_valuta(ar.get_valuta());
}


//--ordering----------------------------------------------------------------------------------------------------------
COrdering::COrdering()
{
    clear();
}

bool COrdering::clear(void)
{
    m_iId=m_iDealerId=-1;
    m_dtDate=QDate(0,0,0);
    m_sComment=m_sOrdernumber=m_sWares=QString("");
    return true;
}

int COrdering::get_id(void)
{
    return m_iId;
}

QDate COrdering::get_date(void)
{
    return m_dtDate;
}

int COrdering::get_dealer_id(void)
{
    return m_iDealerId;
}

QString COrdering::get_comment(void)
{
    return m_sComment;
}

QString COrdering::get_ordernumber(void)
{
    return m_sOrdernumber;
}

QString COrdering::get_wares(void)
{
    return m_sWares;
}

void COrdering::get_wares(QList<QString> & ls)
{
    QStringList list = m_sWares.split("|", QString::SkipEmptyParts);
    list.sort();
    for(int i=0;i<list.count();i++)
        ls.push_back(list[i]);
}

void COrdering::set_id(int id)
{
    m_iId=id;
}

void COrdering::set_date(QDate & dt)
{
    m_dtDate=dt;
}

void COrdering::set_date(QString s)//format dd.mm.yyyy
{
    QStringList ls=s.split(".");
    bool b;
    int d,m,y;
    QDate dt;
    //-
    if(ls.count()==3)
    {
        d=ls[0].toInt(&b,10);
        if(b)//number
        {
            m=ls[1].toInt(&b,10);
            if(b)//number
            {
                y=ls[2].toInt(&b,10);
                if(b)//number
                {
                    dt.setDate(y,m,d);
                    m_dtDate=dt;
                }
            }
        }
    }
}

void COrdering::set_dealer_id(int id)
{
    m_iDealerId=id;
}

void COrdering::set_comment(QString s)
{
    m_sComment=s;
}

void COrdering::set_ordernumber(QString s)
{
    m_sOrdernumber=s;
}

void COrdering::set_wares(QString s)
{
    m_sWares=s;
}

void COrdering::set_wares(QList<QString> & ls)
{
    int count=ls.count();
    QString s=QString("");
    for(int i=0;i<count;i++)
    {
        s+=ls[i];
        s+=QString("|");
    }
    m_sWares=s;
}

void COrdering::set(COrdering & ord)
{
    m_iId=ord.get_id();
    m_iDealerId=ord.get_dealer_id();
    m_sComment=ord.get_comment();
    m_sOrdernumber=ord.get_ordernumber();
    m_sWares=ord.get_wares();
    m_dtDate=ord.get_date();
}


//trade----------------------------------------------------------------------------------------------------------
CTrade::CTrade()
{
    m_iType=-1;
    m_sInfo1=m_sInfo2=m_sInfo3=m_sInfo4=m_sInfo5=m_sBookingNumber=m_sComment=m_sWares=QString("");
    m_dtDate=QDate(0,0,0);
    m_bCanceled=false;
}

bool CTrade::get_canceled(void)
{
    return m_bCanceled;
}

int CTrade::get_type(void)
{
    return m_iType;
}

QString CTrade::get_comment(void)
{
    return m_sComment;
}

QDate CTrade::get_date(void)
{
    return m_dtDate;
}

QString CTrade::get_booking_number(void)
{
    return m_sBookingNumber;
}

QString CTrade::get_wares(void)
{
    return m_sWares;
}

void CTrade::get_wares(QList<QString> & ls)
{
    QStringList list = m_sWares.split("|", QString::SkipEmptyParts);
    for(int i=0;i<list.count();i++)
        ls.push_back(list[i]);
}

QString CTrade::get_info_1(void)
{
    return m_sInfo1;
}

QString CTrade::get_info_2(void)
{
    return m_sInfo2;
}

QString CTrade::get_info_3(void)
{
    return m_sInfo3;
}

QString CTrade::get_info_4(void)
{
    return m_sInfo4;
}

QString CTrade::get_info_5(void)
{
    return m_sInfo5;
}

int CTrade::get_article_count(int iArticleId)
{
    bool b=false;
    //-
    int id,count=-1;
    QStringList lsS2;
    QList<QString> lsS;
    get_wares(lsS);
    for(int i=0;i<lsS.count();i++)
    {
        lsS2=lsS[i].split("x");
        if(lsS2.count()==2)//two elements count x article id
        {
            id=lsS2[1].toInt(&b,10);
            if(b)//number?
            {
                if(id==iArticleId)//found?
                {
                    count=lsS2[0].toInt(&b,10);
                    if(b)//number?
                        break;
                    else
                        count=-1;
                }
            }
        }
    }
    //-
    return count;
}

void CTrade::get_from_wares(QList<int> & lsInt,bool bArticleIds)//bArticleIds is true = only article ids , bArticleIds is false = only article counts
{
    bool b;
    int i,v;
    QString s;
    QList<QString> lsS;
    QStringList lsS2;
    get_wares(lsS);
    if(lsS.count()>0)
    {
        for(i=0;i<lsS.count();i++)
        {
            s=lsS[i];
            lsS2=s.split("x");
            if(lsS2.count()==2)//2 elements must be,  count x article_id
            {
                if(!bArticleIds)
                    s=lsS2[0];//only counts
                else
                    s=lsS2[1];//only ids
                v=s.toInt(&b,10);
                if(b)//cast ok?
                    lsInt.push_back(v);
            }
            lsS2.clear();
        }
        lsS.clear();
    }
}

void CTrade::set_canceled(bool b)
{
    m_bCanceled=b;
}

void CTrade::set_comment(QString s)
{
    m_sComment=s;
}

void CTrade::set_date(QDate dt)
{
    m_dtDate=dt;
}

void CTrade::set_type(int iType)
{
    m_iType=iType;
}

void CTrade::set_wares(QString s)
{
    m_sWares=s;
}

void CTrade::set_wares(QList<QString> & ls)
{
    int count=ls.count();
    QString s;
    for(int i=0;i<count;i++)
    {
        s+=ls[i];
        s+=QString("|");
    }
    m_sWares=s;
}

void CTrade::set_booking_number(QString s)
{
    m_sBookingNumber=s;
}

void CTrade::set_info_1(QString s)
{
    m_sInfo1=s;
}

void CTrade::set_info_2(QString s)
{
    m_sInfo2=s;
}

void CTrade::set_info_3(QString s)
{
    m_sInfo3=s;
}

void CTrade::set_info_4(QString s)
{
    m_sInfo4=s;
}

void CTrade::set_info_5(QString s)
{
    m_sInfo5=s;
}

void CTrade::set_info(int iNumber, QString s)
{
    switch(iNumber)
    {
    case 1:
        set_info_1(s);
        break;
    case 2:
        set_info_2(s);
        break;
    case 3:
        set_info_3(s);
        break;
    case 4:
        set_info_4(s);
        break;
    case 5:
        set_info_5(s);
        break;
    }
}

void CTrade::set(CTrade & trade)
{
    m_iType=trade.get_type();
    m_bCanceled=trade.get_canceled();
    m_dtDate=trade.get_date();
    m_sComment=trade.get_comment();
    m_sWares=trade.get_wares();
    m_sBookingNumber=trade.get_booking_number();
    m_sInfo1=trade.get_info_1();
    m_sInfo2=trade.get_info_2();
    m_sInfo3=trade.get_info_3();
    m_sInfo4=trade.get_info_4();
    m_sInfo5=trade.get_info_5();
}


//---logbook-------------------------------------------------------------------------------------
CLogbook::CLogbook()
{
    m_sText=QString("");
    m_iType=-1;
    m_dt_tiDateTime=QDateTime(QDate(0,0,0),QTime(0,0,0,0));
}

CLogbook::~CLogbook()
{
}

QString CLogbook::get_text(void)
{
    return m_sText;
}

int CLogbook::get_type(void)
{
    return m_iType;
}

QDateTime CLogbook::get_date_time(void)
{
    return m_dt_tiDateTime;
}

void CLogbook::set_text(QString sText)
{
    m_sText=sText;
}

void CLogbook::set_type(int iType)
{
    m_iType=iType;
}

void CLogbook::set_date_time(QDateTime dt_tiDateTime)
{
    m_dt_tiDateTime=dt_tiDateTime;
}

QString CLogbook::get_output_string(void)
{
    QString s,sType;
    //-
    if(m_iType==LOGBOOK_TYPE_IMPORT)
        sType=QString("Import");
    else if(m_iType==LOGBOOK_TYPE_TRADE_IN)
        sType=QString("Wareneingang");
    else if(m_iType==LOGBOOK_TYPE_TRADE_OUT)
        sType=QString("Warenausgang");
    else if(m_iType==LOGBOOK_TYPE_TRADE_ORD_IN)
        sType=QString("Wareneingang(Bestellung)");
    else if(m_iType==LOGBOOK_TYPE_TRADE_CUS_OUT)
        sType=QString("Warenausgang(Kunde)");
    else if(m_iType==LOGBOOK_TYPE_TRADE_CANCEL)
        sType=QString("Warengang storniert");
    else if(m_iType==LOGBOOK_TYPE_ORDERING_NEW)
        sType=QString("Bestellung hinzugefügt");
    else if(m_iType==LOGBOOK_TYPE_ORDERING_EDIT)
        sType=QString("Bestellung bearbeitet");
    else if(m_iType==LOGBOOK_TYPE_ORDERING_REMOVE)
        sType=QString("Bestellung gelöscht");
    else if(m_iType==LOGBOOK_TYPE_ARTICLE_NEW)
        sType=QString("Artikel hinzugefügt");
    else if(m_iType==LOGBOOK_TYPE_ARTICLE_EDIT)
        sType=QString("Artikel bearbeitet");
    else if(m_iType==LOGBOOK_TYPE_ARTICLE_COPY)
        sType=QString("Artikel kopiert");
    else if(m_iType==LOGBOOK_TYPE_ARTICLE_REMOVE)
        sType=QString("Artikel gelöscht");
    else if(m_iType==LOGBOOK_TYPE_WAREGROUP_NEW)
        sType=QString("Warengruppe hinzugefügt");
    else if(m_iType==LOGBOOK_TYPE_WAREGROUP_EDIT)
        sType=QString("Warengruppe bearbeitet");
    else if(m_iType==LOGBOOK_TYPE_WAREGROUP_REMOVE)
        sType=QString("Warengruppe gelöscht");
    else if(m_iType==LOGBOOK_TYPE_MAKER_NEW)
        sType=QString("Hersteller hinzugefügt");
    else if(m_iType==LOGBOOK_TYPE_MAKER_EDIT)
        sType=QString("Hersteller bearbeitet");
    else if(m_iType==LOGBOOK_TYPE_MAKER_REMOVE)
        sType=QString("Hersteller gelöscht");
    else if(m_iType==LOGBOOK_TYPE_DEALER_NEW)
        sType=QString("Händler hinzugefügt");
    else if(m_iType==LOGBOOK_TYPE_DEALER_EDIT)
        sType=QString("Händler bearbeitet");
    else if(m_iType==LOGBOOK_TYPE_DEALER_REMOVE)
        sType=QString("Händler gelöscht");
    else if(m_iType==LOGBOOK_TYPE_CUSTOMER_NEW)
        sType=QString("Kunde hinzugefügt");
    else if(m_iType==LOGBOOK_TYPE_CUSTOMER_EDIT)
        sType=QString("Kunde bearbeitet");
    else if(m_iType==LOGBOOK_TYPE_CUSTOMER_REMOVE)
        sType=QString("Kunde gelöscht");
    else if(m_iType==LOGBOOK_TYPE_OTHER)
        sType=QString("sonstiges");
    //-
    s=QString("%1 %2 Uhr - %3 - %4").arg(m_dt_tiDateTime.date().toString("yyyy-MM-dd"),m_dt_tiDateTime.time().toString("hh:mm:ss"),sType,m_sText);
    //-
    return s;
}

