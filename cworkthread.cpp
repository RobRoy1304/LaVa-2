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

#include "cworkthread.h"


CWorkThread::CWorkThread()
{
    m_pDbInterface=NULL;
    m_pWidgets=NULL;
    m_bBreakThread=false;
    clear();
}

CWorkThread::~CWorkThread()
{
    stop_thread();
    clear();
}

bool CWorkThread::start_thread(void)
{
    if(isRunning())//thraed is running?
        return false;
    clear();
    //start thread
    start(QThread::HighestPriority);
    return true;
}

bool CWorkThread::stop_thread(void)
{
    if(!isRunning())
        return false;
    //stop thread
    m_bBreakThread=true;
    while(isRunning()) {msleep(500);}
    return true;
}

bool CWorkThread::clear(void)
{
    m_pMemory=NULL;
    m_iWork=-1;
    m_bWork=false;
    return true;
}

bool CWorkThread::init(CDbConnection * pDb, CWidgetInterface * pWidgets)
{
    m_pDbInterface=pDb;
    m_pWidgets=pWidgets;
    return true;
}

//------work--------------------------------------------------------------------------------------
bool CWorkThread::set_work(int iWork, CPointerMemory * pMemory)
{
    if(pMemory==NULL)
        return false;

    //thread in work?? -> wait
    while(m_bWork) {usleep(25);}

    //set
    clear();
    m_pMemory=pMemory;
    m_iWork=iWork;
    m_bWork=true;

    //waiting for finish
    while(m_bWork) {usleep(25);}

    return m_bReturn;
}

//------run---------------------------------------------------------------------------------------
void CWorkThread::run(void)
{
    int i;
    if(m_pDbInterface!=NULL && m_pWidgets!=NULL)
    {
        m_bReturn=true;

        while(m_bBreakThread==false)
        {
            //wait for work
            while(m_bWork==false && m_bBreakThread==false) {usleep(100);}

            //-stop?
            if(m_bBreakThread==true)//stop thread?
                break;
            //work---------------------------------------------------------------------------------
            if(m_pMemory!=NULL)
            {
                //waregroup----------------------------
                if(m_iWork==WORK_WAREGROUP_GET_ALL)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL)
                        m_bReturn=m_pDbInterface->waregroup_get_all(*m_pMemory->get_string(),*m_pMemory->get_int_list());
                }
                if(m_iWork==WORK_WAREGROUP_GET_COUNT)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int()!=NULL)
                    {
                        i=m_pDbInterface->waregroup_get_count(*m_pMemory->get_string());
                        (*m_pMemory->get_int())=i;//return valuve mark in memory
                    }
                }
                if(m_iWork==WORK_WAREGROUP_GET_ALL_SUBWAREGROUPS)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int_list()!=NULL)
                        m_bReturn=m_pDbInterface->waregroup_get_all_subwaregroup(*m_pMemory->get_int(),*m_pMemory->get_int_list());
                }
                if(m_iWork==WORK_WAREGROUP_ALL_CHANGE_PARENT)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL)
                        m_bReturn=m_pDbInterface->waregroup_all_change_parent(*m_pMemory->get_int(),*m_pMemory->get_int2());
                }

                //maker--------------------------------
                if(m_iWork==WORK_MAKER_GET_ALL)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL)
                    {
                        if(m_pMemory->get_string2()==NULL)
                            m_bReturn=m_pDbInterface->maker_get_all(*m_pMemory->get_string(),*m_pMemory->get_int_list());
                        else
                            m_bReturn=m_pDbInterface->maker_get_all(*m_pMemory->get_string(),*m_pMemory->get_int_list(),*m_pMemory->get_string2());
                    }
                }
                if(m_iWork==WORK_MAKER_GET_COUNT)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int()!=NULL)
                    {
                        i=m_pDbInterface->maker_get_count(*m_pMemory->get_string());
                        (*m_pMemory->get_int())=i;//return valuve mark in memory
                    }
                }
                if(m_iWork==WORK_MAKER_FILL_COMBOBOX)
                {
                    if(m_pMemory->get_combobox()!=NULL)
                        m_bReturn=m_pWidgets->maker_fill_combobox(m_pMemory->get_combobox());
                }
                if(m_iWork==WORK_MAKER_UPDATE_LISTWIDGET)
                {
                    if(m_pMemory->get_listwidget()!=NULL && m_pMemory->get_int()!=NULL)
                        m_bReturn=m_pWidgets->maker_update_list(m_pMemory->get_listwidget(),*m_pMemory->get_int());
                }
                  if(m_iWork==WORK_MAKER_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_int_list() !=NULL)
                        m_bReturn=m_pWidgets->maker_update_tablewidget(m_pMemory->get_tablewidget(),*m_pMemory->get_int_list(),-1);
                }

                //dealer-------------------------------
                if(m_iWork==WORK_DEALER_GET_ALL)
                {
                    if(m_pMemory->get_string2()==NULL)
                        m_bReturn=m_pDbInterface->dealer_get_all(*m_pMemory->get_string(),*m_pMemory->get_int_list());
                    else
                        m_bReturn=m_pDbInterface->dealer_get_all(*m_pMemory->get_string(),*m_pMemory->get_int_list(),*m_pMemory->get_string2());
                }
                if(m_iWork==WORK_DEALER_GET_COUNT)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int()!=NULL)
                    {
                        i=m_pDbInterface->dealer_get_count(*m_pMemory->get_string());
                        (*m_pMemory->get_int())=i;//return valuve mark in memory
                    }
                }
                if(m_iWork==WORK_DEALER_FILL_COMBOBOX)
                {
                    if(m_pMemory->get_combobox()!=NULL)
                        m_bReturn=m_pWidgets->dealer_fill_combobox(m_pMemory->get_combobox());
                }
                if(m_iWork==WORK_DEALER_UPDATE_LISTWIDGET)
                {
                    if(m_pMemory->get_listwidget()!=NULL && m_pMemory->get_int()!=NULL)
                        m_bReturn=m_pWidgets->dealer_update_list(m_pMemory->get_listwidget(),*m_pMemory->get_int());
                }
                if(m_iWork==WORK_DEALER_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_int_list() !=NULL)
                        m_bReturn=m_pWidgets->dealer_update_tablewidget(m_pMemory->get_tablewidget(),*m_pMemory->get_int_list(),-1);
                }

                //customer-------------------------------
                if(m_iWork==WORK_CUSTOMER_GET_ALL)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL)
                        m_bReturn=m_pDbInterface->customer_get_all(*m_pMemory->get_string(),*m_pMemory->get_int_list());
                }
                if(m_iWork==WORK_CUSTOMER_GET_COUNT)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int()!=NULL)
                    {
                        i=m_pDbInterface->customer_get_count(*m_pMemory->get_string());
                        (*m_pMemory->get_int())=i;//return valuve mark in memory
                    }
                }
                if(m_iWork==WORK_GET_CUSTOMER_NUMBER_NOMINATION)
                {
                    if(m_pMemory->get_string()!=NULL)
                    {
                        (*m_pMemory->get_string())=m_pDbInterface->customer_get_customer_number_nomination();
                    }
                }
                if(m_iWork==WORK_CUSTOMER_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_int_list() !=NULL)
                        m_bReturn=m_pWidgets->customer_update_tablewidget(m_pMemory->get_tablewidget(),*m_pMemory->get_int_list(),-1);
                }
                if(m_iWork==WORK_CUSTOMER_UPDATE_LISTWIDGET)
                {
                    if(m_pMemory->get_listwidget()!=NULL && m_pMemory->get_int()!=NULL)
                        m_bReturn=m_pWidgets->customer_update_list(m_pMemory->get_listwidget(),*m_pMemory->get_int());
                }

                //article------------------------------
                if(m_iWork==WORK_ARTICLE_GET_ALL)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL)
                        m_bReturn=m_pDbInterface->article_get_all(*m_pMemory->get_string(),*m_pMemory->get_int_list());
                }
                if(m_iWork==WORK_ARTICLE_GET_COUNT)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int()!=NULL)
                    {
                        i=m_pDbInterface->article_get_count(*m_pMemory->get_string());
                        (*m_pMemory->get_int())=i;//return valuve mark in memory
                    }
                }
                if(m_iWork==WORK_ARTICLE_ALL_CHANGE_WAREGROUP)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL)
                        m_bReturn=m_pDbInterface->article_all_change_waregroup(*m_pMemory->get_int(),*m_pMemory->get_int2());
                }
                if(m_iWork==WORK_ARTICLE_ALL_CHANGE_MAKER)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL)
                        m_bReturn=m_pDbInterface->article_all_change_maker(*m_pMemory->get_int(),*m_pMemory->get_int2());
                }
                if(m_iWork==WORK_ARTICLE_GET_ALL_BY_PERCENT)
                {
                    if(m_pMemory->get_int_list()!=NULL && m_pMemory->get_int()!=NULL && m_pMemory->get_bool()!=NULL)
                        m_bReturn=m_pDbInterface->article_get_all_by_percent(*m_pMemory->get_int_list(),*m_pMemory->get_int(),*m_pMemory->get_bool());
                }
                if(m_iWork==WORK_ARTICLE_GET_ALL_BY_MAKER)
                {
                    if(m_pMemory->get_int_list()!=NULL && m_pMemory->get_string()!=NULL)
                        m_bReturn=m_pDbInterface->article_get_all_by_maker(*m_pMemory->get_string(),*m_pMemory->get_int_list(),true);
                }
                if(m_iWork==WORK_ARTICLE_GET_ALL_BY_WAREGROUP)
                {
                    if(m_pMemory->get_int_list()!=NULL && m_pMemory->get_string()!=NULL)
                        m_bReturn=m_pDbInterface->article_get_all_by_waregroup(*m_pMemory->get_string(),*m_pMemory->get_int_list(),true);
                }
                if(m_iWork==WORK_ARTICLE_GET_INVENTORY_ON_DATE)
                {
                    if(m_pMemory->get_date()!=NULL && m_pMemory->get_int()!=NULL && m_pMemory->get_uint()!=NULL)
                        (*m_pMemory->get_uint())=m_pDbInterface->article_get_inventory_on_date(*m_pMemory->get_int(),*m_pMemory->get_date());
                }
                if(m_iWork==WORK_ARTICLE_UPDATE_ALL_COUNT)
                {
                    if(m_pMemory->get_string_list()!=NULL && m_pMemory->get_int()!=NULL)
                        m_bReturn=m_pDbInterface->article_update_all_count(*m_pMemory->get_string_list(),*m_pMemory->get_int());
                }
                if(m_iWork==WORK_ARTICLE_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_int_list() !=NULL && m_pMemory->get_int()!=NULL)
                        m_bReturn=m_pWidgets->article_update_tablewidget(m_pMemory->get_tablewidget(),*m_pMemory->get_int_list(),*m_pMemory->get_int(),-1);
                }

                //ordering-----------------------------
                if(m_iWork==WORK_ORDERING_GET_ALL)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL)
                        m_bReturn=m_pDbInterface->ordering_get_all(*m_pMemory->get_string(),*m_pMemory->get_int_list());
                }
                if(m_iWork==WORK_ORDERING_GET_ALL_BY_DEALER)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL)
                        m_bReturn=m_pDbInterface->ordering_get_all_by_dealer(*m_pMemory->get_string(),*m_pMemory->get_int_list(),true);
                }
                if(m_iWork==WORK_ORDERING_GET_ALL_BY_ARTICLE)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL)
                        m_bReturn=m_pDbInterface->ordering_get_all_by_article(*m_pMemory->get_string(),*m_pMemory->get_int_list(),true);
                }
                if(m_iWork==WORK_ORDERING_GET_ALL_BY_MAKER)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL)
                        m_bReturn=m_pDbInterface->ordering_get_all_by_maker(*m_pMemory->get_string(),*m_pMemory->get_int_list(),true);
                }
                if(m_iWork==WORK_ORDERING_GET_ALL_BY_WAREGROUP)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL)
                        m_bReturn=m_pDbInterface->ordering_get_all_by_waregroup(*m_pMemory->get_string(),*m_pMemory->get_int_list(),true);
                }
                if(m_iWork==WORK_ORDERING_GET_ALL_BY_ARTICLENUMBER)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int_list()!=NULL && m_pMemory->get_bool()!=NULL)
                        m_bReturn=m_pDbInterface->ordering_get_all_by_articlenumber(*m_pMemory->get_string(),*m_pMemory->get_int_list(),*m_pMemory->get_bool());
                }
                if(m_iWork==WORK_ORDERING_GET_COUNT)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int()!=NULL)
                    {
                        i=m_pDbInterface->ordering_get_count(*m_pMemory->get_string());
                        (*m_pMemory->get_int())=i;//return valuve mark in memory
                    }
                }
                if(m_iWork==WORK_ORDERING_ALL_CHANGE_DEALER)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL)
                        m_bReturn=m_pDbInterface->ordering_all_change_dealer(*m_pMemory->get_int(),*m_pMemory->get_int2());
                }
                if(m_iWork==WORK_ORDERING_GET_COUNT_WITH_ARTICLE)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL)
                    {
                        i=m_pDbInterface->ordering_get_count_by_article(*m_pMemory->get_int());
                        (*m_pMemory->get_int2())=i;//return valuve mark in memory
                    }
                }
                if(m_iWork==WORK_ORDERING_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_int_list() !=NULL && m_pMemory->get_int()!=NULL)
                        m_bReturn=m_pWidgets->ordering_update_tablewidget(m_pMemory->get_tablewidget(),*m_pMemory->get_int_list(),*m_pMemory->get_int(),-1);
                }
                if(m_iWork==WORK_ORDERING_UPDATE_TABLEWIDGET_WARES_LIST)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL)
                        m_bReturn=m_pWidgets->ordering_update_tablewidget_wares_list(m_pMemory->get_tablewidget(),*m_pMemory->get_int(),*m_pMemory->get_int2());
                }

                //trade-------------------------------
                if(m_iWork==WORK_TRADE_GET_ALL)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_string_list()!=NULL)
                        m_bReturn=m_pDbInterface->trade_get_all(*m_pMemory->get_string(),*m_pMemory->get_string_list());
                }
                if(m_iWork==WORK_TRADE_GET_COUNT)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int()!=NULL)
                    {
                        i=m_pDbInterface->trade_get_count(*m_pMemory->get_string());
                        (*m_pMemory->get_int())=i;//return valuve mark in memory
                    }
                }
                if(m_iWork==WORK_TRADE_GIVE_IT_WITH_ARTICLE)
                {
                    if(m_pMemory->get_int()!=NULL)
                        m_bReturn=m_pDbInterface->trade_give_it_trades_with_article(*m_pMemory->get_int());
                }
                if(m_iWork==WORK_TRADE_GET_NEXT_FREE_BOOKING_NUMBER)
                {
                    if(m_pMemory->get_date()!=NULL && m_pMemory->get_string()!=NULL)
                        (*m_pMemory->get_string())=m_pDbInterface->trade_get_next_free_booking_number(*m_pMemory->get_date());
                }
                if(m_iWork==WORK_TRADE_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_string_list() !=NULL)
                        m_bReturn=m_pWidgets->trade_update_tablewidget(m_pMemory->get_tablewidget(),*m_pMemory->get_string_list(),QString(""));
                }
                if(m_iWork==WORK_TRADE_UPDATE_TABLEWIDGET_WARES_LIST)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_string()!=NULL)
                        m_bReturn=m_pWidgets->trade_update_tablewidget_wares_list(m_pMemory->get_tablewidget(),*m_pMemory->get_string());
                }
                if(m_iWork==WORK_TRADE_GET_ALL_BY_ARTICLE)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_string2()!=NULL && m_pMemory->get_string_list()!=NULL)
                        m_bReturn=m_pDbInterface->trade_get_all_by_article(*m_pMemory->get_string2(),*m_pMemory->get_string(),*m_pMemory->get_string_list(),true);
                }
                if(m_iWork==WORK_TRADE_GET_ALL_BY_MAKER)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_string2()!=NULL && m_pMemory->get_string_list()!=NULL)
                        m_bReturn=m_pDbInterface->trade_get_all_by_maker(*m_pMemory->get_string2(),*m_pMemory->get_string(),*m_pMemory->get_string_list(),true);
                }
                if(m_iWork==WORK_TRADE_GET_ALL_BY_WAREGROUP)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_string2()!=NULL && m_pMemory->get_string_list()!=NULL)
                        m_bReturn=m_pDbInterface->trade_get_all_by_waregroup(*m_pMemory->get_string2(),*m_pMemory->get_string(),*m_pMemory->get_string_list(),true);
                }
                if(m_iWork==WORK_TRADE_GET_ALL_BY_ARTICLENUMBER)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_string2()!=NULL && m_pMemory->get_string_list()!=NULL && m_pMemory->get_bool()!=NULL)
                        m_bReturn=m_pDbInterface->trade_get_all_by_articlenumber(*m_pMemory->get_string2(),*m_pMemory->get_string(),*m_pMemory->get_string_list(),*m_pMemory->get_bool(),true);
                }
                if(m_iWork==WORK_TRADE_REFRESH_DATE_LIST)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL && m_pMemory->get_string()!=NULL && m_pMemory->get_listwidget()!=NULL)
                        m_bReturn=m_pWidgets->trade_refresh_list_date(m_pMemory->get_listwidget(),*m_pMemory->get_int(),*m_pMemory->get_string(),*m_pMemory->get_int2());
                }
                if(m_iWork==WORK_TRADE_UPDATE_DATE_LIST)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL && m_pMemory->get_string()!=NULL && m_pMemory->get_listwidget()!=NULL && m_pMemory->get_int3()!=NULL && m_pMemory->get_date()!=NULL)
                        m_bReturn=m_pWidgets->trade_update_list_date(m_pMemory->get_listwidget(),*m_pMemory->get_int3(),*m_pMemory->get_int(),*m_pMemory->get_string(),*m_pMemory->get_int2(),*m_pMemory->get_date());
                }
                if(m_iWork==WORK_TRADE_CHECK_CANCEL_POSSIBLE)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_string2()!=NULL)
                        m_bReturn=m_pDbInterface->trade_check_cancel_possible(*m_pMemory->get_string(),*m_pMemory->get_string2());
                }
                if(m_iWork==WORK_TRADE_UPDATE_ALL_INFO)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL && m_pMemory->get_string()!=NULL && m_pMemory->get_string2()!=NULL)
                        m_bReturn=m_pDbInterface->trade_update_all_info(*m_pMemory->get_int(),*m_pMemory->get_int2(),*m_pMemory->get_string(),*m_pMemory->get_string2());
                }

                //inventory
                if(m_iWork==WORK_INVENTORY_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_int_list() !=NULL)
                        m_bReturn=m_pWidgets->inventory_update_tablewidget(m_pMemory->get_tablewidget(),*m_pMemory->get_int_list(),-1);
                }

                //logbook
                if(m_iWork==WORK_LOGBOOK_GET_COUNT)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_int()!=NULL)
                    {
                        i=m_pDbInterface->logbook_get_count(*m_pMemory->get_string());
                        (*m_pMemory->get_int())=i;//return valuve mark in memory
                    }
                }
                if(m_iWork==WORK_LOGBOOK_GET_ALL)
                {
                    if(m_pMemory->get_string()!=NULL && m_pMemory->get_date_time_list()!=NULL && m_pMemory->get_string2()!=NULL)
                        m_bReturn=m_pDbInterface->logbook_get_all(*m_pMemory->get_string(),*m_pMemory->get_date_time_list(),*m_pMemory->get_string2());
                }
                if(m_iWork==WORK_LOGBOOK_UPDATE_LISTWIDGET)
                {
                    if(m_pMemory->get_listwidget()!=NULL && m_pMemory->get_date_time_list()!=NULL && m_pMemory->get_date_time()!=NULL)
                        m_bReturn=m_pWidgets->logbook_update_list(m_pMemory->get_listwidget(),*m_pMemory->get_date_time_list(),*m_pMemory->get_date_time());
                }
                if(m_iWork==WORK_LOGBOOK_REFRESH_DATE_LIST)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_listwidget()!=NULL)
                        m_bReturn=m_pWidgets->logbook_refresh_list_date(m_pMemory->get_listwidget(),*m_pMemory->get_int());
                }
                if(m_iWork==WORK_LOGBOOK_UPDATE_DATE_LIST)
                {
                    if(m_pMemory->get_int()!=NULL && m_pMemory->get_int2()!=NULL && m_pMemory->get_listwidget()!=NULL && m_pMemory->get_date()!=NULL)
                        m_bReturn=m_pWidgets->logbook_update_list_date(m_pMemory->get_listwidget(),*m_pMemory->get_int(),*m_pMemory->get_int2(),*m_pMemory->get_date());
                }
                if(m_iWork==WORK_LOGBOOK_REMOVE_ALL)
                {
                    if(m_pMemory->get_string()!=NULL)
                        m_bReturn=m_pDbInterface->logbook_remove_all(*m_pMemory->get_string());
                }

                //tables(lists)
                if(m_iWork==WORK_BALANCELIST_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_int()!=NULL && m_pMemory->get_date()!=NULL && m_pMemory->get_date2()!=NULL)
                        m_bReturn=m_pWidgets->balancelist_update_tablewidget(m_pMemory->get_tablewidget(),*m_pMemory->get_date(),*m_pMemory->get_date2(),*m_pMemory->get_int());
                }
                if(m_iWork==WORK_WARNLIST_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL)
                        m_bReturn=m_pWidgets->warnlist_update_tablewidget(m_pMemory->get_tablewidget());
                }
                if(m_iWork==WORK_INVENTORYS_ON_DATE_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL && m_pMemory->get_date()!=NULL)
                        m_bReturn=m_pWidgets->inventorys_on_date_update_tablewidget(m_pMemory->get_tablewidget(),*m_pMemory->get_date());
                }
                if(m_iWork==WORK_INVENTORYLIST_UPDATE_TABLEWIDGET)
                {
                    if(m_pMemory->get_tablewidget()!=NULL)
                        m_bReturn=m_pWidgets->inventorylist_update_tablewidget(m_pMemory->get_tablewidget());
                }
            }
            //-clear-------------------------------------------------------------------------------
            clear();
        }
    }
}
