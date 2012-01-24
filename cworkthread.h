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

#ifndef CWORKTHREAD_H
#define CWORKTHREAD_H

#include <QtGui>
#include <QThread>
#include "data_classes.h"
#include "cdbconnection.h"
#include "cwidgetinterface.h"

//work-defines----------------------------------------------

//waregroup
#define WORK_WAREGROUP_GET_ALL 10
#define WORK_WAREGROUP_GET_COUNT 11
#define WORK_WAREGROUP_GET_ALL_SUBWAREGROUPS 12
#define WORK_WAREGROUP_ALL_CHANGE_PARENT 13

//maker
#define WORK_MAKER_GET_ALL 20
#define WORK_MAKER_UPDATE_TABLEWIDGET 21
#define WORK_MAKER_GET_COUNT 22
#define WORK_MAKER_UPDATE_LISTWIDGET 23
#define WORK_MAKER_FILL_COMBOBOX 24

//dealer
#define WORK_DEALER_GET_ALL 30
#define WORK_DEALER_UPDATE_TABLEWIDGET 31
#define WORK_DEALER_GET_COUNT 32
#define WORK_DEALER_UPDATE_LISTWIDGET 33
#define WORK_DEALER_FILL_COMBOBOX 34

//article
#define WORK_ARTICLE_GET_ALL 40
#define WORK_ARTICLE_UPDATE_TABLEWIDGET 41
#define WORK_ARTICLE_GET_COUNT 42
#define WORK_ARTICLE_ALL_CHANGE_WAREGROUP 43
#define WORK_ARTICLE_ALL_CHANGE_MAKER 44
#define WORK_ARTICLE_GET_ALL_BY_PERCENT 45
#define WORK_ARTICLE_GET_ALL_BY_MAKER 46
#define WORK_ARTICLE_GET_ALL_BY_WAREGROUP 47
#define WORK_ARTICLE_GET_INVENTORY_ON_DATE 48
#define WORK_ARTICLE_UPDATE_ALL_COUNT 49

//ordering
#define WORK_ORDERING_GET_ALL 60
#define WORK_ORDERING_UPDATE_TABLEWIDGET 61
#define WORK_ORDERING_UPDATE_TABLEWIDGET_WARES_LIST 62
#define WORK_ORDERING_GET_COUNT 63
#define WORK_ORDERING_ALL_CHANGE_DEALER 64
#define WORK_ORDERING_GET_COUNT_WITH_ARTICLE 65
#define WORK_ORDERING_GET_ALL_BY_DEALER 66
#define WORK_ORDERING_GET_ALL_BY_ARTICLE 67
#define WORK_ORDERING_GET_ALL_BY_MAKER 68
#define WORK_ORDERING_GET_ALL_BY_WAREGROUP 69
#define WORK_ORDERING_GET_ALL_BY_ARTICLENUMBER 70

//trade
#define WORK_TRADE_GET_ALL 80
#define WORK_TRADE_UPDATE_TABLEWIDGET 81
#define WORK_TRADE_UPDATE_TABLEWIDGET_WARES_LIST 82
#define WORK_TRADE_GET_COUNT 83
#define WORK_TRADE_GIVE_IT_WITH_ARTICLE 84
#define WORK_TRADE_GET_NEXT_FREE_BOOKING_NUMBER 85
#define WORK_TRADE_GET_ALL_BY_ARTICLE 86
#define WORK_TRADE_GET_ALL_BY_MAKER 87
#define WORK_TRADE_GET_ALL_BY_WAREGROUP 88
#define WORK_TRADE_GET_ALL_BY_ARTICLENUMBER 89
#define WORK_TRADE_REFRESH_DATE_LIST 90
#define WORK_TRADE_UPDATE_DATE_LIST 91
#define WORK_TRADE_CHECK_CANCEL_POSSIBLE 92
#define WORK_TRADE_UPDATE_ALL_INFO 93

//customer
#define WORK_CUSTOMER_GET_ALL 100
#define WORK_CUSTOMER_UPDATE_TABLEWIDGET 101
#define WORK_CUSTOMER_GET_COUNT 102
#define WORK_GET_CUSTOMER_NUMBER_NOMINATION 103
#define WORK_CUSTOMER_UPDATE_LISTWIDGET 104

//inventory
#define WORK_INVENTORY_UPDATE_TABLEWIDGET 110

//logbook
#define WORK_LOGBOOK_GET_COUNT 120
#define WORK_LOGBOOK_GET_ALL 121
#define WORK_LOGBOOK_UPDATE_LISTWIDGET 122
#define WORK_LOGBOOK_REFRESH_DATE_LIST 123
#define WORK_LOGBOOK_UPDATE_DATE_LIST 124
#define WORK_LOGBOOK_REMOVE_ALL 125

//tables (lists)
#define WORK_BALANCELIST_UPDATE_TABLEWIDGET 130
#define WORK_WARNLIST_UPDATE_TABLEWIDGET 131
#define WORK_INVENTORYS_ON_DATE_UPDATE_TABLEWIDGET 132


//----------------------------------------------------------

class CWorkThread : public QThread
{
protected:
    void run();

public:
    CDbConnection * m_pDbInterface;//database interface
    CWidgetInterface * m_pWidgets;//fill-functions for tables/lists/tree...

    bool m_bWork;//give it work
    int m_iWork;//typ of work
    bool m_bBreakThread;//for stop the thread
    bool m_bReturn;// return valuve

    CPointerMemory * m_pMemory;//memory

    //basic
    CWorkThread();
    ~CWorkThread();
    bool start_thread(void);
    bool stop_thread(void);
    bool clear(void);
    bool init(CDbConnection * pDb, CWidgetInterface * pWidgets);

    //work
    bool set_work(int iWork, CPointerMemory * pMemory);
};


#endif // CWORKTHREAD_H
