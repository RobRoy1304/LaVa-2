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

#ifndef CLASTDBCHANGE_H
#define CLASTDBCHANGE_H

#include "cdbconnection.h"

//last change    //update logbook ever
#define LC_ACTION_UPDATE_ALL 0
#define LC_ACTION_UPDATE_MAKER 1
#define LC_ACTION_UPDATE_DEALER 2
#define LC_ACTION_UPDATE_WAREGROUP 3
#define LC_ACTION_UPDATE_CUSTOMER 4
#define LC_ACTION_UPDATE_ARTICLE_INV 5
#define LC_ACTION_UPDATE_ORDERING 6
#define LC_ACTION_UPDATE_ORDERING_DEALER 7
#define LC_ACTION_UPDATE_INVENTORY_TRADE 8
#define LC_ACTION_UPDATE_INVENTORY_TRADE_ORDERING 9
#define LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG 10
#define LC_ACTION_UPDATE_LOGBOOK 11
#define LC_ACTION_UPDATE_LOGBOOK_COUNT 12
#define LC_ACTION_UPDATE_ORDER_QUIT 100

class CLastDbChange
{
private:
    CDbConnection * m_pDb;
    bool m_bYourself; //last change yourself?
    QString m_sLastChange;

public:
    CLastDbChange();
    ~CLastDbChange();
    void set_db(CDbConnection * pDb){m_pDb=pDb;}
    QString get_last_change(void){return m_sLastChange;}
    QString get_last_change_from_db(void);
    bool write_last_change(QString sLastChange);
    QString create_last_change_string(int iType, int iId=-1);// iId==-1 -> all id's
    bool get_data_from_last_change_string(int & iType, int & iId, QString sLastChange=QString::fromUtf8("")); //last change string empty -> member last change
    bool check_update_last_change(void);
};

#endif // CLASTDBCHANGE_H
