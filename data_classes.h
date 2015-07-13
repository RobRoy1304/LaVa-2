/*  LaVa 2, a inventory managment tool
    Copyright (C) 2015 - Robert Ewert - robert.ewert@gmail.com - www.robert.ewert.de.vu
    created with QtCreator(Qt 4.8)

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

#ifndef DATA_CLASSES_H
#define DATA_CLASSES_H

#include <QtGui>

//version
#define VERSION "Version 0.32"
#define CURRENT_DB_VERSION "1,03"

//trade
#define TYPE_INCOMING 1
#define TYPE_OUTGOING 2
#define TYPE_ORDERING_INCOMING 3
#define TYPE_CUSTOMER_OUTGOING 4

//logbook
#define LOGBOOK_TYPE_IMPORT 0
#define LOGBOOK_TYPE_TRADE_IN 1
#define LOGBOOK_TYPE_TRADE_OUT 2
#define LOGBOOK_TYPE_TRADE_ORD_IN 3
#define LOGBOOK_TYPE_TRADE_CUS_OUT 4
#define LOGBOOK_TYPE_TRADE_CANCEL 5
#define LOGBOOK_TYPE_ORDERING_NEW 10
#define LOGBOOK_TYPE_ORDERING_EDIT 11
#define LOGBOOK_TYPE_ORDERING_REMOVE 12
#define LOGBOOK_TYPE_ARTICLE_NEW 20
#define LOGBOOK_TYPE_ARTICLE_EDIT 21
#define LOGBOOK_TYPE_ARTICLE_COPY 22
#define LOGBOOK_TYPE_ARTICLE_REMOVE 23
#define LOGBOOK_TYPE_WAREGROUP_NEW 30
#define LOGBOOK_TYPE_WAREGROUP_EDIT 31
#define LOGBOOK_TYPE_WAREGROUP_REMOVE 32
#define LOGBOOK_TYPE_MAKER_NEW 40
#define LOGBOOK_TYPE_MAKER_EDIT 41
#define LOGBOOK_TYPE_MAKER_REMOVE 42
#define LOGBOOK_TYPE_DEALER_NEW 50
#define LOGBOOK_TYPE_DEALER_EDIT 51
#define LOGBOOK_TYPE_DEALER_REMOVE 52
#define LOGBOOK_TYPE_CUSTOMER_NEW 60
#define LOGBOOK_TYPE_CUSTOMER_EDIT 61
#define LOGBOOK_TYPE_CUSTOMER_REMOVE 62
#define LOGBOOK_TYPE_OTHER 70

//table item data
#define TABLE_ALIGNMENT_LEFT 0
#define TABLE_ALIGNMENT_RIGHT 1
#define TABLE_ALIGNMENT_CENTER 2

//EAN
#define TYPE_EAN_UNKNOW 0
#define TYPE_EAN_8 1
#define TYPE_EAN_13 2

//---------------------------------------
class CEan
{
private:
    //const
    QString m_saCodeA[10];
    QString m_saCodeB[10];
    QString m_saCodeC[10];
    QString m_saParity[10];
    QString m_sLeadTail;
    QString m_sSeperator;
    //-
    QString m_sBarcode;
    int m_iType;

public:
    CEan();
    CEan(QString sBarcode);
    ~CEan();

    void set_const(void);
    bool set_barcode(QString sBarcode);
    int get_type(void);
    bool is_ean_barcode(void);
    int get_checksum(void);
    int get_barcode_number(int index);
    QString get_barcode_number_string(int index, int length=1);
    bool get_digital_struct(QString & sDigitalStruct);
};


//---------------------------------------

class CTableItemData
{
private:
    QString m_sText;
    int m_iAlignment;
    QIcon m_icon;
public:
    CTableItemData();
    ~CTableItemData();
    bool set(CTableItemData & right );
    bool operator != (CTableItemData & right);
    bool operator == (CTableItemData & right);
    QString get_text(void);
    int get_alignment(void);
    QIcon get_icon(void);
    void set_text(QString s);
    void set_alignment(int i);
    void set_icon(QIcon ico);
    void set(QString sText, int iAlignment, QIcon * pIcon=NULL);
    void clear(void);
};

//--------------------------------------------------

//table column data
class CTableColumnsData
{
private:
    int m_iIdColumn;
    QTableWidget * m_pTable;
    QList<int> m_lsColumnsOrder;
    QList<bool> m_lsColumnsVisible;
    QList<int> m_lsColumnsAlignment;

public:
    CTableColumnsData();
    ~CTableColumnsData(void);
    CTableColumnsData & operator= (CTableColumnsData & right );
    bool operator != (CTableColumnsData & right);
    bool operator == (CTableColumnsData & right);
    int get_columns_count(void);
    int get_id_column(void);
    QTableWidget * get_table(void);
    void get_columns_order(QList<int> & lsColumnsOrder);
    void get_columns_visible(QList<bool> & lsColumnsVisible);
    void get_columns_alignment(QList<int> & lsColumnsAlignment);
    int get_column_alignment(int iIndex);
    bool get_column_visible(int iIndex);
    int get_column_order(int iIndex);
    int get_count_not_visible_columns(void);
    void set_id_column(int iColumn);
    void set_table(QTableWidget * pTable);
    void set_columns_visible(QList<bool> & lsColumnsVisible);
    void set_columns_alignment(QList<int> & lsColumnsAlignment);
    void set_column_alignment(int iIndex, int iType);
    void set_column_visible(int iIndex, bool bVisible);
    bool set(QList<int> & lsColumnsOrder,QList<bool> & lsColumnsVisible,QList<int> & lsColumnsAlignment);
    bool update_order(QList<int> & lsNewColumnsOrder);
    bool swap_columns(int iRow1, int iRow2);
};

//------------------------------------------------

//memory
class CPointerMemory
{
private:
    unsigned int * m_pUInt;
    bool * m_pBool;
    int * m_pInt;
    int * m_pInt2;
    int * m_pInt3;
    QString * m_pString;
    QString * m_pString2;
    QList<int> * m_pLsInt;
    QList<QString> * m_pLsString;
    QTreeWidget * m_pTreeWidget;
    QTableWidget * m_pTableWidget;
    QTableWidget * m_pTableWidget2;
    QListWidget * m_pListwidget;
    QComboBox * m_pCombobox;
    QDate * m_pDate;
    QDate * m_pDate2;
    QList<QDateTime> * m_p_dt_tiLsDateTime;
    QDateTime * m_p_dt_tiDateTime;

public:
    CPointerMemory();
    ~CPointerMemory();
    void clear(void);
    unsigned int * get_uint(void);
    QDate * get_date(void);
    QDate * get_date2(void);
    bool * get_bool(void);
    QString * get_string( void);
    QString * get_string2( void);
    int * get_int(void);
    int * get_int2(void);
    int * get_int3(void);
    QList<int> * get_int_list(void);
    QList<QString> * get_string_list(void);
    QTreeWidget * get_treewidget(void);
    QTableWidget * get_tablewidget(void);
    QTableWidget * get_tablewidget2(void);
    QListWidget * get_listwidget(void);
    QComboBox * get_combobox(void);
    QList<QDateTime> * get_date_time_list(void);
    QDateTime * get_date_time(void);
    void set_uint(unsigned int * pUInt);
    void set_date(QDate * pDate);
    void set_date2(QDate * pDate);
    void set_bool(bool * p);
    void set_string(QString * p);
    void set_string2(QString * p);
    void set_int(int * p);
    void set_int2(int * p);
    void set_int3(int * p);
    void set_int_list(QList<int> * p);
    void set_string_list(QList<QString> *p);
    void set_treewidget(QTreeWidget * p);
    void set_tablewidget(QTableWidget * p);
    void set_tablewidget2(QTableWidget * p);
    void set_listwidget(QListWidget * p);
    void set_combobox(QComboBox * pCombobox);
    void set_date_time_list(QList<QDateTime> * lsDateTime);
    void set_date_time(QDateTime * dt_tiDateTime);
};

//----------------------------------------------------------

//basic
class CBasicData
{
    protected:
    QString m_sName;
    QString m_sComment;
    int m_iId;

    public:
    CBasicData();
    ~CBasicData(){}
    void clear(void);
    int get_id(void);
    QString get_name(void);
    QString get_comment(void);
    void set_id(int iId);
    void set_name(QString sName);
    void set_comment(QString sComment);
};

//-------------------------------------------------------

//waregroup
class CWaregroup : public CBasicData
{
    private:
    int m_iParentId;

    public:
    CWaregroup();
    ~CWaregroup(){}
    void clear(void);
    int get_parent_id(void);
    void set_parent_id(int iParentId);
    void set(int iParent=-1,QString sName=QString::fromUtf8(""),QString sComment=QString::fromUtf8(""), int iId=-1);
    int operator != (CWaregroup & wg);
};

//----------------------------------------------------------

//maker
class CMaker : public CBasicData
{
    protected:
    QString m_sAdress;
    QString m_sCallnumber;
    QString m_sFaxnumber;
    QString m_sEmail;
    QString m_sHomepage;
    QString m_sContectperson;

    public:
    CMaker();
    ~CMaker(){}
    void clear(void);
    QString get_adress(void);
    QString get_callnumber(void);
    QString get_faxnumber(void);
    QString get_email(void);
    QString get_homepage(void);
    QString get_contectperson(void);
    void set_adress(QString s);
    void set_callnumber(QString s);
    void set_faxnumber(QString s);
    void set_email(QString s);
    void set_homepage(QString s);
    void set_contectperson(QString s);
    void set(CMaker & mk);
};

//-----------------------------------------------

//dealer
class CDealer : public CMaker
{
    private:
    QString m_sCustomernumber;

    public:
    CDealer();
    ~CDealer(){}
    void clear(void);
    QString get_customernumber(void);
    void set_customernumber(QString s);
    void set(CDealer & de);
};

//-----------------------------------------------

//customer
class CCustomer : public CBasicData
{
    protected:
    QString m_sFirstName;
    QString m_sStreet;
    QString m_sPostcode;
    QString m_sCity;
    QString m_sCustomerNumber;
    QString m_sCallnumber;
    QString m_sFaxnumber;
    QString m_sEmail;

    public:
    CCustomer();
    ~CCustomer(){}
    void clear(void);
    QString get_customernumber(void);
    QString get_first_name(void);
    QString get_street(void);
    QString get_city(void);
    QString get_postcode(void);
    QString get_callnumber(void);
    QString get_faxnumber(void);
    QString get_email(void);
    void set_customernumber(QString s);
    void set_first_name(QString s);
    void set_street(QString s);
    void set_city(QString s);
    void set_postcode(QString s);
    void set(CCustomer & cu);
    void set_callnumber(QString s);
    void set_faxnumber(QString s);
    void set_email(QString s);
};

//--------------------------------------------

//article
class CArticle : public CBasicData
{
    private:
    bool m_bDelete;
    QString m_sUnit;
    QString m_sArticlenumber;
    QString m_sArticlenumber2;
    QString m_sLocation;
    QString m_sValuta;
    int m_iMakerId;
    int m_iWaregroupId;
    int m_iWarningLimit;
    float m_fBaseprice;
    float m_fSalesprice;
    unsigned int m_uiInventory;
    unsigned int m_uiMaxInventory;

    public:
    CArticle();
    ~CArticle(){}
    bool clear(void);
    bool get_delete(void);
    QString get_valuta(void);
    float get_base_price(void);
    float get_sales_price(void);
    QString get_unit(void);
    QString get_articlenumber(void);
    QString get_articlenumber2(void);
    QString get_location(void);
    int get_maker_id(void);
    int get_warning_limit(void);
    int get_waregroup_id(void);
    unsigned int get_inventory(void);
    unsigned int get_max_inventory(void);
    void set_delete(bool b);
    void set_valuta(QString s);
    void set_base_price(float f);
    void set_sales_price(float f);
    void set_articlenumber(QString s);
    void set_articlenumber2(QString s);
    void set_unit(QString s);
    void set_location(QString s);
    void set_maker_id(int id);
    void set_warning_limit(int iLimit);
    void set_waregroup_id(int id);
    void set_inventory(unsigned int inv);
    void set_max_inventory(unsigned int max);
    void set(CArticle & ar);
};

//--------------------------------------------

//ordering
class COrdering
{
    private:
    int m_iId;
    QDate m_dtDate;
    int m_iDealerId;
    QString m_sComment;
    QString m_sOrdernumber;
    QString m_sWares;//count x article id | ... x

    public:
    COrdering();
    ~COrdering(){}
    bool clear(void);
    int get_id(void);
    QDate get_date(void);
    int get_dealer_id(void);
    QString get_comment(void);
    QString get_ordernumber(void);
    QString get_wares(void);
    void get_wares(QList<QString> & ls);
    void set_id(int id);
    void set_date(QDate & dt);
    void set_date(QString s);
    void set_dealer_id(int id);
    void set_comment(QString s);
    void set_ordernumber(QString s);
    void set_wares(QString s);
    void set_wares(QList<QString> & ls);
    void set(COrdering & ord);
};

//-------------------------------------------------

//trade
class CTrade
{
    private:
    bool m_bCanceled;
    int m_iType;
    QString m_sComment;
    QString m_sWares;//count x article id | ... x
    QString m_sBookingNumber;
    QString m_sInfo1;
    QString m_sInfo2;
    QString m_sInfo3;
    QString m_sInfo4;
    QString m_sInfo5;
    QDate m_dtDate;

    public:
    CTrade();
    ~CTrade(){}
    bool clear(void);
    bool get_canceled(void);
    int get_type(void);
    QString get_comment(void);
    QDate get_date(void);
    QString get_wares(void);
    void get_wares(QList<QString> & ls);
    void get_from_wares(QList<int> & lsInt,bool bArticleIds=true);//bArticleIds is true = only article ids , bArticleIds is false = only article counts
    QString get_booking_number(void);
    QString get_info_1(void);
    QString get_info_2(void);
    QString get_info_3(void);
    QString get_info_4(void);
    QString get_info_5(void);
    int get_article_count(int iArticleId);

    void set_canceled(bool b);
    void set_type(int i);
    void set_comment(QString s);
    void set_date(QDate dt);
    void set_wares(QString s);
    void set_wares(QList<QString> & ls);
    void set_booking_number(QString s);
    void set_info_1(QString s);
    void set_info_2(QString s);
    void set_info_3(QString s);
    void set_info_4(QString s);
    void set_info_5(QString s);
    void set_info(int iNumber, QString s);
    void set(CTrade & trade);
};

//-----------------------------------------------------------

class CLogbook
{
private:
    QString m_sText;
    int m_iType;
    QDateTime m_dt_tiDateTime;

public:
    CLogbook();
    ~CLogbook();
    bool clear(void);
    QString get_text(void);
    int get_type(void);
    QDateTime get_date_time(void);
    void set_text(QString sText);
    void set_type(int iType);
    void set_date_time(QDateTime dt_tiDateTime);
    QString get_output_string(void);
};


#endif // DATA_CLASSES_H

