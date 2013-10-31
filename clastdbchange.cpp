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

#include "clastdbchange.h"

CLastDbChange::CLastDbChange()
{
    m_pDb=NULL;
    m_bYourself=true;
}

CLastDbChange::~CLastDbChange()
{
}

QString CLastDbChange::get_last_change_from_db(void)
{
    QString sReturn;
    if(m_pDb!=NULL)
    {
        if(m_pDb->is_db_connect())
            sReturn=m_pDb->get_last_change();
    }
    return sReturn;
}

bool CLastDbChange::write_last_change(QString sLastChange)
{
    bool b=false;
    if(m_pDb!=NULL)
    {
        if(m_pDb->is_db_connect())
        {
            m_bYourself=true;
            b=m_pDb->write_last_change(sLastChange);
        }
    }
    return b;
}

QString CLastDbChange::create_last_change_string(int iType, int iId)
{
    QDateTime dt=QDateTime::currentDateTime();
    QString sReturn;
    //-
    if(iType>=0)
    {
        sReturn=QString("%1|").arg(dt.toString("hh:mm:ss-dd.MM.yyyy"));//date&time
        sReturn+=QString("%1|").arg(iType);//type
        sReturn+=QString("%1").arg(iId);//id
    }
    //-
    return sReturn;
}

bool CLastDbChange::get_data_from_last_change_string(int & iType, int & iId, QString sLastChange)
{
    bool b=false;
    QStringList sls;
    if(sLastChange.length()<=0)
        sLastChange=m_sLastChange;
    //-
    sls=sLastChange.split("|");
    if(sls.count()==3)//date+time , type , id
    {
        iType=sls[1].toInt(&b);
        if(b)
            iId=sls[2].toInt(&b);
    }
    //-
    sls.clear();
    return b;
}

bool CLastDbChange::check_update_last_change(void)
{
    bool b=false;
    QString s=get_last_change_from_db();
    //-
    if(s!=m_sLastChange)
    {
        if(!m_bYourself)
            b=true;//db change -> another client
        //-
        m_bYourself=false;
        m_sLastChange=s;//mark
    }
    return b;
}
