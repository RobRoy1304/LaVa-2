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

#ifndef CDBCONNECTION_H
#define CDBCONNECTION_H

#include <QtGui>
#include <QtSql>
#include "data_classes.h"

class CDbConnection
{
private:
    QSqlDatabase m_db;
    bool m_bConnect;

public:
    //basic----------------------------------
    CDbConnection();
    ~CDbConnection();
    bool open_db(QString sDbName, QString sHost=QString("localhost"));
    bool close_db(void);
    QString last_error(void);
    bool is_db_connect(void);
    QString get_db_version(void);
    QDateTime get_last_session(void);
    bool set_last_session(QDateTime dtTi);
    bool check_database_file(bool bCheckDbVersion=false);
    bool check_and_update_db_version(void);
    bool exec_sql(QString s);

    //waregroups-----------------------------
    bool waregroup_add(CWaregroup & wg);
    bool waregroup_remove(int id);
    bool waregroup_update(CWaregroup & wg);
    bool waregroup_get(QString sCondition, CWaregroup & wg);
    bool waregroup_get(int iId, CWaregroup & wg);
    QString waregroup_get_name(int iId);
    int waregroup_get_id(QString sName,int iParent);
    QString waregroup_get_path(int id,int iMaxLevel=-1);
    int waregroup_get_count(QString sCondition);
    bool waregroup_get_all(QString sCondition, QList<int> & lsIds);
    bool waregroup_get_all_subwaregroup(int iId,QList<int> & lsSubWaregroupIds);
    bool waregroup_all_change_parent(int iOldParentId,int iNewParentId);

    //maker---------------------------------------
    bool maker_add(CMaker & mk);
    bool maker_remove(int id);
    bool maker_update(CMaker & mk);
    bool maker_get(QString sCondition, CMaker & mk);
    bool maker_get(int iId,CMaker & mk);
    QString maker_get_name(int iId);
    int maker_get_id(QString & sName);
    int maker_get_count(QString sCondition);
    bool maker_get_all(QString sCondition, QList<int> & lsIds,QString sOrderBy=QString(""));

    //dealer---------------------------------------
    bool dealer_add(CDealer & de);
    bool dealer_remove(int id);
    bool dealer_update(CDealer & de);
    bool dealer_get(QString sCondition, CDealer & de);
    bool dealer_get(int iId,CDealer & de);
    QString dealer_get_name(int iId);
    int dealer_get_id(QString & sName);
    int dealer_get_count(QString sCondition);
    bool dealer_get_all(QString sCondition, QList<int> & lsIds,QString sOrderBy=QString(""));

    //customer--------------------------------------
    bool customer_add(CCustomer & cu);
    bool customer_remove(int id);
    bool customer_update(CCustomer & cu);
    bool customer_get(QString sCondition, CCustomer & cu);
    bool customer_get(int iId,CCustomer & cu);
    int customer_get_count(QString sCondition);
    bool customer_get_all(QString sCondition, QList<int> & lsIds);
    QString customer_get_customer_number_nomination(void);

    //article---------------------------------------
    bool article_add(CArticle & ar);
    bool article_remove(int id);
    bool article_set_delete(int id);
    bool article_update(CArticle & ar);
    bool article_get(QString sCondition, CArticle & ar, bool bWithDeleteArticle=false);
    bool article_get(int iId,CArticle & ar, bool bWithDeleteArticle=false);
    QString article_get_name(int iId, bool bWithDeleteArticle=false);
    int article_get_id(QString sName, bool bWithDeleteArticle=false);
    int article_get_count(QString sCondition, bool bWithDeleteArticle=false);
    bool article_get_all(QString sCondition, QList<int> & lsIds, bool bWithDeleteArticle=false);
    bool article_get_all_by_maker(QString sName,QList <int> & lsIds,bool bLike, bool bWithDeleteArticle=false);
    bool article_get_all_by_waregroup(QString sName,QList <int> & lsIds, bool bLike, bool bWithDeleteArticle=false);
    bool article_get_all_by_percent(QList<int> & lsIds, int iPercent,bool bLess=true);
    bool article_all_change_waregroup(int iOldWaregroupId,int iNewWaregroupId);
    bool article_all_change_maker(int iOldMakerId, int iNewMakerId);
    bool article_update_all_count(QList<QString> & lsWares, int iType);
    unsigned int article_get_inventory_on_date(int iArticleId,QDate dt);

    //ordering---------------------------------------
    bool ordering_add(COrdering & ord);
    bool ordering_remove(int id);
    bool ordering_update(COrdering & ord);
    bool ordering_get(QString sCondition, COrdering & ord);
    bool ordering_get(int iId,COrdering & ord);
    int ordering_get_count(QString sCondition);
    QString ordering_get_wares_list(int iId, int iMaxLength=100, bool bWithCount=false);
    QString ordering_get_wares_list(QList<QString> & lsOrdering, int iMaxLength=100);
    bool ordering_get_all(QString sCondition, QList<int> & lsIds);
    bool ordering_get_all_by_dealer(QString sName,QList <int> & lsIds,bool bLike);
    bool ordering_get_all_by_maker(QString sName,QList <int> & lsIds,bool bLike);
    bool ordering_get_all_by_waregroup(QString sName,QList <int> & lsIds,bool bLike);
    bool ordering_get_all_by_article(QString sName,QList <int> & lsIds, bool bLike);
    bool ordering_get_all_by_articlenumber(QString sNumber,QList<int> & lsIds, bool bArticlenumber1=true, bool bLike=true); //bArticlenumber1 true->nr.1 else false->nr.2
    int ordering_get_count_by_article(int iArticleId);
    int ordering_get_ordering_count_by_article(int iArticleId);
    bool ordering_all_change_dealer(int iOldDealerId, int iNewDealerId);

    //trade------------------------------------------
    bool trade_add(CTrade & tr);
    bool trade_update(CTrade & tr);
    bool trade_remove_all(QString sCondition);
    bool trade_get(QString sBookingNumber,CTrade & tr);
    bool trade_get_all(QString sCondition,QList <QString> & lsBookingNumber);
    bool trade_give_it_trades_with_article(int iArticleId);
    bool trade_set_canceled(QString sBookingNumber);
    int trade_get_count(QString sCondition);
    QString trade_get_next_free_booking_number(QDate dt);
    QString trade_get_article_list(QString sBookingNumber, int iMaxLength=100, bool bWithCount=false);
    bool trade_get_all_by_maker(QString sName,QString sDates,QList <QString> & lsBookingNumber,bool bLike);
    bool trade_get_all_by_waregroup(QString sName,QString sDates,QList <QString> & lsBookingNumber,bool bLike);
    bool trade_get_all_by_article(QString sName,QString sDates,QList <QString> & lsBookingNumber,bool bLike);
    bool trade_get_all_by_articlenumber(QString sNumber,QString sDates,QList <QString> & lsBookingNumber, bool bArticlenumber1=true, bool bLike=true); //bArticlenumber1 true->nr.1 else false->nr.2
    int trade_get_all_count_by_maker(QString sName, QDate dtFrom, QDate dtTo, bool bLike=true);
    int trade_get_all_count_by_waregroup(QString sName, QDate dtFrom, QDate dtTo, bool bLike=true);
    int trade_get_all_count_by_article(QString sName, QDate dtFrom, QDate dtTo, bool bLike=true);
    int trade_get_all_count_by_articlenumber(QString sNumber, QDate dtFrom, QDate dtTo, bool bArticlenumber1=true, bool bLike=true);
    bool trade_check_cancel_possible(QString sBookingNumber,QString & sErrorString);
    bool trade_is_canceled(QString sBookingNumber);
    bool trade_update_all_info(int iNumber, int iTradeType, QString sOld, QString sNew);

    //logbook-------------------------------------------
    bool logbook_add(CLogbook & lg);
    bool logbook_remove_all(QString sCondition);
    bool logbook_get(QDateTime dt_tiDateTime,CLogbook & lg);
    bool logbook_get_all(QString sCondition, QList<QDateTime> & lsDtTi, QString sOrderBy);
    int logbook_get_count(QString sCondition);
    //create
    bool logbook_create_new(CWaregroup & wg, CLogbook & lg);
    bool logbook_create_edit(CWaregroup & wg, CLogbook & lg, bool bMark);
    bool logbook_create_remove(CWaregroup & wg, CLogbook & lg);
    bool logbook_create_new(CMaker & mk, CLogbook & lg);
    bool logbook_create_edit(CMaker & mk, CLogbook & lg, bool bMark);
    bool logbook_create_remove(CMaker & mk, CLogbook & lg);
    bool logbook_create_new(CDealer & de, CLogbook & lg);
    bool logbook_create_edit(CDealer & de, CLogbook & lg, bool bMark);
    bool logbook_create_remove(CDealer & de, CLogbook & lg);
    bool logbook_create_new(CCustomer & cu, CLogbook & lg);
    bool logbook_create_edit(CCustomer & cu, CLogbook & lg, bool bMark);
    bool logbook_create_remove(CCustomer & cu, CLogbook & lg);
    bool logbook_create_new(CArticle & ar, CLogbook & lg);
    bool logbook_create_edit(CArticle & ar, CLogbook & lg, bool bMark);
    bool logbook_create_copy(QString sArticleNameSource, QString sArticleNameCopy, CLogbook & lg);
    bool logbook_create_remove(CArticle & ar, CLogbook & lg);
    bool logbook_create_new(COrdering & ord, CLogbook & lg);
    bool logbook_create_edit(COrdering & ord, CLogbook & lg, bool bMark);
    bool logbook_create_remove(COrdering & ord, CLogbook & lg);
    bool logbook_create_new(CTrade & tr, CLogbook & lg);
    bool logbook_create_canceled(CTrade & tr, CLogbook & lg);
};

#endif // CDBCONNECTION_H
