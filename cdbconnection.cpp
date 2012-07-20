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

#include "cdbconnection.h"

//basic------------------------------------------------------------------------------------------
CDbConnection::CDbConnection()
{
    m_bConnect=false;
    m_db=QSqlDatabase::addDatabase(QString("QSQLITE"));
}

CDbConnection::~CDbConnection()
{
   m_db.removeDatabase(QString("QSQLITE"));
   close_db();
}

bool CDbConnection::open_db(QString sDbName, QString sHost)
{
    if(m_bConnect)//db open?
        close_db();
    bool b=false;
    m_db.setHostName(sHost);
    m_db.setDatabaseName(sDbName);
    if(!m_db.open())
        b=false;
    else
        m_bConnect=b=true;
    return b;
}

bool CDbConnection::close_db(void)
{
    if(!m_bConnect)//db not open?
        return false;
    m_db.close();
    m_bConnect=false;
    return true;
}

QString CDbConnection::last_error(void)
{
    return m_db.lastError().text();
}

bool CDbConnection::is_db_connect(void)
{
    return m_bConnect;
}

QString CDbConnection::get_db_version(void)
{
    QString sVersion;
    if(m_bConnect)
    {
        QSqlQuery query;
        QString s=QString("SELECT * FROM info");
        if(query.exec(s))
        {
            query.first();
            if(query.isValid())
            {
                QSqlRecord record=query.record();
                sVersion=query.value( record.indexOf("version")).toString();
            }
        }
    }
    return sVersion;
}

QDateTime CDbConnection::get_last_session(void)
{
    QDateTime dtTi(QDate(0,0,0),QTime(0,0,0));
    if(m_bConnect)
    {
        QSqlQuery query;
        QString s=QString("SELECT * FROM info");
        if(query.exec(s))
        {
            query.first();
            if(query.isValid())
            {
                QSqlRecord record=query.record();
                dtTi=query.value( record.indexOf("last_session")).toDateTime();
            }
        }
    }
    return dtTi;
}

bool CDbConnection::set_last_session(QDateTime dtTi)
{
    bool b=false;
    QString s;
    QSqlQuery query;
    if(m_bConnect)
    {
        s=QString("UPDATE info SET last_session='%1'").arg(dtTi.toString("yyyy-MM-dd-hh-mm-ss"));
        b=query.exec(s);
    }
    return b;
}

bool CDbConnection::check_database_file(bool bCheckDbVersion)
{
    int i;
    QString s;
    bool b=true,bReturn=false;
    //-
    if(m_db.isOpen())
    {
        s=get_db_version();
        if(bCheckDbVersion)
        {
            if(s!=QString(CURRENT_DB_VERSION))
                b=false;
        }
        //-
        if(s.length()>0 && b==true)
        {
            for(i=0;i<8;i++)
            {
                //check give it table's
                if(i==0)//check waregroup
                {
                    s=QString("SELECT * FROM waregroup WHERE name = 'xgtzasgl'");
                    if(!exec_sql(s))
                        break;
                }
                if(i==1)//check maker
                {
                    s=QString("SELECT * FROM maker WHERE name = 'xgtzasgl'");
                    if(!exec_sql(s))
                        break;
                }
                if(i==2)//check dealer
                {
                    s=QString("SELECT * FROM dealer WHERE name = 'xgtzasgl'");
                    if(!exec_sql(s))
                        break;
                }
                if(i==3)//check customer
                {
                    s=QString("SELECT * FROM customer WHERE name = 'xgtzasgl'");
                    if(!exec_sql(s))
                        break;
                }
                if(i==4)//check article
                {
                    s=QString("SELECT * FROM article WHERE name = 'xgtzasgl'");
                    if(!exec_sql(s))
                        break;
                }
                if(i==5)//check ordering
                {
                    s=QString("SELECT * FROM ordering WHERE comment = 'xgtzasgl'");
                    if(!exec_sql(s))
                        break;
                }
                if(i==6)//check trade
                {
                    s=QString("SELECT * FROM trade WHERE info_5 = 'xgtzasgl'");
                    if(!exec_sql(s))
                        break;
                }
                if(i==7)//check logbook
                {
                    s=QString("SELECT * FROM logbook WHERE text = 'xgtzasgl'");
                    if(!exec_sql(s))
                        break;
                }
            }
            //-
            if(i==8)//all ok
                bReturn=true;
        }
    }
    //-
    return bReturn;
}

bool CDbConnection::check_and_update_db_version(void)
{
    QString s;
    QList<QString> lsSql;
    bool bReturn=true;
    //-
    if(!m_db.isOpen())
        bReturn=false;
    else
    {
        s=get_db_version();
        if(s!=QString(CURRENT_DB_VERSION))
        {
            if(s==QString("1,01"))//update to 1,02
            {
                //new column in article
                lsSql.push_back(QString("ALTER TABLE article ADD COLUMN delete_status BOOL DEFAULT (0)"));

                //rename a column name  -> sqlite not support ALTER TABLE ... RENAME COLUMN  -> another solution
                lsSql.push_back("ALTER TABLE ordering RENAME TO temp_ordering");
                lsSql.push_back("CREATE TABLE \"ordering\" (\"id\" INTEGER PRIMARY KEY ,\"date\" DATETIME,\"dealer_id\" INTEGER,\"comment\" TEXT,\"order_number\" TEXT,\"wares\" TEXT)");
                lsSql.push_back("INSERT INTO ordering (id,date,dealer_id,comment,order_number,wares) SELECT id,date,dealer_id,comment,order_number,orderings FROM temp_ordering");
                lsSql.push_back("DROP TABLE temp_ordering");

                //exec sql
                while(lsSql.count()>0)
                {
                    bReturn=exec_sql(lsSql[0]);
                    if(!bReturn)
                        break;
                    lsSql.removeFirst();
                }
                lsSql.clear();

                if(bReturn)
                {
                    //update db_version text
                    s=QString("UPDATE info SET version = '1,02'");
                    bReturn=exec_sql(s);
                }
            }
        }
    }
    return bReturn;
}

bool CDbConnection::exec_sql(QString s)
{
    if(!m_bConnect)//db open?
        return false;
    QSqlQuery query;
    bool b=query.exec(s);
    return b;
}


//---waregroup-------------------------------------------------------------------------------

bool CDbConnection::waregroup_add(CWaregroup & wg)
{
    if(!m_bConnect)//db open?
        return false;
    //-
    int id=-1;
    bool b;
    QSqlQuery query;
    QString s=QString("INSERT INTO waregroup (name,comment,parent_id) VALUES ('%1','%2',").arg(wg.get_name(),wg.get_comment());
    s+=QString("%1)").arg(wg.get_parent_id());
    b=query.exec(s);//add ok?
    if(b)
    {
        id=query.lastInsertId().toInt();
        wg.set_id(id);
    }
    //-
    return b;
}

bool CDbConnection::waregroup_remove(int iId)
{
    if(!m_bConnect)//db open?
        return false;
    QSqlQuery query;
    QString s=QString("DELETE FROM waregroup WHERE id=%1").arg(iId);
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::waregroup_update(CWaregroup & wg)
{
    if(!m_bConnect)//db open?
        return false;

    QSqlQuery query;
    QString s=QString("UPDATE OR REPLACE waregroup SET ");
    s+=QString("name='%1',").arg(wg.get_name());
    s+=QString("comment='%1',").arg(wg.get_comment());
    s+=QString("parent_id=%1").arg(wg.get_parent_id());
    s+=QString(" WHERE id=%1").arg(wg.get_id());

    //exec sql-syntax
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::waregroup_get(QString sCondition, CWaregroup & wg)
{
    if(!m_bConnect||sCondition.length()<=0)//db open?
        return false;
    bool b=true;
    QSqlQuery query;
    query.setForwardOnly(true);
    QString s=QString("SELECT * FROM waregroup WHERE %1").arg(sCondition);
    if(query.exec(s))
    {
        query.first();
        if(query.isValid())
        {
            QSqlRecord record=query.record();
            wg.set(query.value( record.indexOf("parent_id")).toInt(),
                       query.value( record.indexOf("name")).toString(),
                       query.value( record.indexOf("comment")).toString(),
                       query.value( record.indexOf("id")).toInt());
        }
    }
    if(b==true && wg.get_id()<1)//not found
        b=false;
    return b;
}

bool CDbConnection::waregroup_get(int iId, CWaregroup & wg)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition=QString("id=%1").arg(iId);
    bool b=waregroup_get(sCondition,wg);
    if(b==true && wg.get_id()<1)//not found
        b=false;
    return b;
}

QString CDbConnection::waregroup_get_name(int iId)
{
    CWaregroup wg;
    QString sCondition,name=QString("");
    if(m_bConnect)//db open?
    {
        sCondition=QString("id=%1").arg(iId);
        if(waregroup_get(sCondition,wg))
            name=wg.get_name();
    }
    return name;
}

int CDbConnection::waregroup_get_id(QString sName, int iParent)
{
    int id=-1;
    CWaregroup wg;
    QString sCondition;
    if(m_bConnect)//db open?
    {
        sCondition=QString("name='%1' AND ").arg(sName);
        sCondition+=QString("parent_id=%1").arg(iParent);
        if(waregroup_get(sCondition,wg))
            id=wg.get_id();
    }
    return id;
}

QString CDbConnection::waregroup_get_path(int id,int iMaxLevel)//if iMaxLevel=-1 then complete
{
    if(!m_bConnect)//db open?
        return QString("");
    int i=0;
    QString s;
    QList <QString> lsS;
    CWaregroup wg;
    //-
    for(i=0;id!=-1;i++)
    {
        if(!waregroup_get(id,wg))
            break;//not found->break
        lsS.push_back(wg.get_name());
        id=wg.get_parent_id();
    }
    //-
    if(iMaxLevel==-1 || iMaxLevel>=lsS.count())//complete?
        i=lsS.count()-1;
    else//part of path
    {
        i=lsS.count()-(lsS.count()-iMaxLevel)-1;
        s+=QString("../");
    }
    //-
    for(;i>=0;i--)
    {
        s+=lsS[i];
        if(i>0)
            s+=QString("/");
    }
    //-
    return s;
}

int CDbConnection::waregroup_get_count(QString sCondition)
{
    int count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QString s=QString("SELECT count(id) FROM waregroup");
        if(sCondition.length()>0)
            s+=QString(" WHERE %1").arg(sCondition);
        QSqlQuery query;
        if(query.exec(s))
        {
            query.first();
            if(query.value(0).toInt()>0)
                count=query.value(0).toInt();
        }
    }
    //-
    return count;
}

bool CDbConnection::waregroup_get_all(QString sCondition, QList<int> & lsIds)
{
    if(!m_bConnect)
        return false;
    QString s("SELECT id FROM waregroup WHERE ");
    s+=sCondition;//+condition
    s+=QString(" ORDER BY name ASC");
    QList<int> ls;
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        QSqlRecord record=query.record();
        for(;query.next();)
            ls.push_back(query.value( record.indexOf("id")).toInt());
    }

    int i,count;
    //sort by waregroup there have subwaregroups
    for(i=0;i<ls.count();i++)
    {
        s=QString("parent_id=%1").arg(ls[i]);
        count=waregroup_get_count(s);
        if(count>0)//has subwaregroups
        {
            lsIds.push_back(ls[i]);
            ls.removeAt(i);
            i--;
        }
    }
    for(i=0;i<ls.count();i++)//all no have subwaregroups
        lsIds.push_back(ls[i]);
    //-
    return true;
}

bool CDbConnection::waregroup_get_all_subwaregroup(int iId,QList<int> & lsSubWaregroupIds)
{
    if(!m_bConnect)//db open?
        return false;
    int i;
    QList<int> lsIds;
    QString sCondition=QString("parent_id=%1").arg(iId);
    waregroup_get_all(sCondition,lsIds);
    for(i=0;i<lsIds.count();i++)
    {
        lsSubWaregroupIds.push_back(lsIds[i]);//mark id
        waregroup_get_all_subwaregroup(lsIds[i],lsSubWaregroupIds);//search sub waregroups
    }
    return true;
}

bool CDbConnection::waregroup_all_change_parent(int iOldParentId,int iNewParentId)
{
    if(!m_bConnect)
        return false;
    QString s=QString("UPDATE waregroup SET parent_id=%1").arg(iNewParentId);
    s+=QString(" WHERE parent_id=%1").arg(iOldParentId);
    QSqlQuery query;
    bool b=query.exec(s);
    return b;
}


//maker----------------------------------------------------------------------------------------
bool CDbConnection::maker_add(CMaker & mk)
{
    if(!m_bConnect)//db open?
        return false;
    //-
    int id=-1;
    bool b;
    QSqlQuery query;
    QString s=QString("INSERT INTO maker (name,adress,call_numbers,fax_numbers,email_adress,homepage,contect_person,comment)"
                      "VALUES ('%1','%2','%3','%4','%5','%6','%7','%8')")
                    .arg(mk.get_name(),mk.get_adress(),mk.get_callnumber(),mk.get_faxnumber(),mk.get_email(),mk.get_homepage(),mk.get_contectperson(),mk.get_comment());
    b=query.exec(s);//add ok?
    if(b)
    {
        id=query.lastInsertId().toInt();
        mk.set_id(id);
    }
    //-
    return b;
}

bool CDbConnection::maker_remove(int id)
{
    if(!m_bConnect)//db open?
        return false;
    QSqlQuery query;
    QString s=QString("DELETE FROM maker WHERE id=%1").arg(id);
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::maker_update(CMaker & mk)
{
    if(!m_bConnect)//db open?
        return false;

    QSqlQuery query;
    QString s=QString("UPDATE OR REPLACE maker SET ");
    s+=QString("name='%1',").arg(mk.get_name());
    s+=QString("comment='%1',").arg(mk.get_comment());
    s+=QString("adress='%1',").arg(mk.get_adress());
    s+=QString("call_numbers='%1',").arg(mk.get_callnumber());
    s+=QString("fax_numbers='%1',").arg(mk.get_faxnumber());
    s+=QString("email_adress='%1',").arg(mk.get_email());
    s+=QString("homepage='%1',").arg(mk.get_homepage());
    s+=QString("contect_person='%1'").arg(mk.get_contectperson());
    s+=QString(" WHERE id=%1").arg(mk.get_id());

    //exec sql-syntax
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::maker_get(QString sCondition, CMaker & mk)
{
    if(!m_bConnect||sCondition.length()<=0)
        return false;
    QString s=QString("SELECT * FROM maker WHERE %1").arg(sCondition);
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        query.first();
        if(query.isValid())
        {
            QSqlRecord record=query.record();
            mk.set_id(query.value( record.indexOf("id")).toInt());
            mk.set_name(query.value( record.indexOf("name")).toString());
            mk.set_adress(query.value( record.indexOf("adress")).toString());
            mk.set_callnumber(query.value( record.indexOf("call_numbers")).toString());
            mk.set_faxnumber(query.value( record.indexOf("fax_numbers")).toString());
            mk.set_email(query.value( record.indexOf("email_adress")).toString());
            mk.set_homepage(query.value( record.indexOf("homepage")).toString());
            mk.set_contectperson(query.value( record.indexOf("contect_person")).toString());
            mk.set_comment(query.value( record.indexOf("comment")).toString());
        }
    }
    if(b==true && mk.get_id()<1)//not found
        b=false;
    return b;
}

bool CDbConnection::maker_get(int iId,CMaker & mk)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition=QString("id=%1").arg(iId);
    bool b=maker_get(sCondition,mk);
    if(b==true && mk.get_id()<1)//not found
        b=false;
    return b;
}

QString CDbConnection::maker_get_name(int iId)
{
    CMaker mk;
    QString sCondition,name=QString("");
    if(m_bConnect)//db open?
    {
        sCondition=QString("id=%1").arg(iId);
        if(maker_get(sCondition,mk))
            name=mk.get_name();
    }
    return name;
}

int CDbConnection::maker_get_id(QString & sName)
{
    int id=-1;
    CMaker mk;
    QString sCondition=QString("name='%1'").arg(sName);
    if(m_bConnect)//db open?
    {
        if(maker_get(sCondition,mk))
            id=mk.get_id();
    }
    return id;
}

int CDbConnection::maker_get_count(QString sCondition)
{
    int count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QString s("SELECT count(id) FROM maker");
        if(sCondition.length()>0)
            s+=QString(" WHERE %1").arg(sCondition);
        QSqlQuery query;
        if(query.exec(s))
        {
            query.first();
            if(query.value(0).toInt()>0)
                count=query.value(0).toInt();
        }
    }
    //-
    return count;
}

bool CDbConnection::maker_get_all(QString sCondition, QList<int> & lsIds, QString sOrderBy)
{
    if(!m_bConnect)
        return false;
    QString s("SELECT * FROM maker");
    if(sCondition.length()>0)
        s+=QString(" WHERE %1").arg(sCondition);//+condition
    if(sOrderBy.length()>0)
        s+=QString(" ORDER BY %1").arg(sOrderBy);//+order by
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        QSqlRecord record=query.record();
        for(;query.next();)
            lsIds.push_back(query.value( record.indexOf("id")).toInt());
    }
    return true;
}

//dealer-----------------------------------------------------------------------------------------------
bool CDbConnection::dealer_add(CDealer & de)
{
    if(!m_bConnect)//db open?
        return false;
    //-
    int id=-1;
    bool b;
    QSqlQuery query;
    QString s=QString("INSERT INTO dealer (name,adress,call_numbers,fax_numbers,email_adress,homepage,contect_person,comment,customer_number)"
                      "VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9')").arg(de.get_name(),de.get_adress(),de.get_callnumber(),
                        de.get_faxnumber(),de.get_email(),de.get_homepage(),de.get_contectperson(),de.get_comment(),de.get_customernumber());
    //-
    b=query.exec(s);//add ok?
    if(b)
    {
        id=query.lastInsertId().toInt();
        de.set_id(id);
    }
    //-
    return b;
}

bool CDbConnection::dealer_remove(int id)
{
    if(!m_bConnect)//db open?
        return false;
    QSqlQuery query;
    QString s=QString("DELETE FROM dealer WHERE id=%1").arg(id);
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::dealer_update(CDealer & de)
{
    if(!m_bConnect)//db open?
        return false;

    QSqlQuery query;
    QString s=QString("UPDATE OR REPLACE dealer SET ");
    s+=QString("name='%1',").arg(de.get_name());
    s+=QString("comment='%1',").arg(de.get_comment());
    s+=QString("adress='%1',").arg(de.get_adress());
    s+=QString("call_numbers='%1',").arg(de.get_callnumber());
    s+=QString("fax_numbers='%1',").arg(de.get_faxnumber());
    s+=QString("email_adress='%1',").arg(de.get_email());
    s+=QString("homepage='%1',").arg(de.get_homepage());
    s+=QString("contect_person='%1',").arg(de.get_contectperson());
    s+=QString("customer_number='%1'").arg(de.get_customernumber());
    s+=QString(" WHERE id=%1").arg(de.get_id());

    //exec sql-syntax

    bool b=query.exec(s);
    return b;
}

bool CDbConnection::dealer_get(QString sCondition, CDealer & de)
{
    if(!m_bConnect||sCondition.length()<=0)
        return false;
    QString s=QString("SELECT * FROM dealer WHERE %1").arg(sCondition);
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        query.first();
        if(query.isValid())
        {
            QSqlRecord record=query.record();
            de.set_id(query.value( record.indexOf("id")).toInt());
            de.set_name(query.value( record.indexOf("name")).toString());
            de.set_adress(query.value( record.indexOf("adress")).toString());
            de.set_callnumber(query.value( record.indexOf("call_numbers")).toString());
            de.set_faxnumber(query.value( record.indexOf("fax_numbers")).toString());
            de.set_email(query.value( record.indexOf("email_adress")).toString());
            de.set_homepage(query.value( record.indexOf("homepage")).toString());
            de.set_contectperson(query.value( record.indexOf("contect_person")).toString());
            de.set_comment(query.value( record.indexOf("comment")).toString());
            de.set_customernumber(query.value( record.indexOf("customer_number")).toString());
        }
    }
    if(b==true && de.get_id()<1)//not found
        b=false;
    return b;
}

bool CDbConnection::dealer_get(int iId,CDealer & de)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition=QString("id=%1").arg(iId);
    bool b=dealer_get(sCondition,de);
    if(b==true && de.get_id()<1)//not found
        b=false;
    return b;
}

QString CDbConnection::dealer_get_name(int iId)
{
    CDealer de;
    QString sCondition,name=QString("");
    if(m_bConnect)//db open?
    {
        sCondition=QString("id=%1").arg(iId);
        if(dealer_get(sCondition,de))
            name=de.get_name();
    }
    return name;
}

int CDbConnection::dealer_get_id(QString & sName)
{
    int id=-1;
    CDealer de;
    QString sCondition=QString("name='%1'").arg(sName);
    if(m_bConnect)//db open?
    {
        if(dealer_get(sCondition,de))
            id=de.get_id();
    }
    return id;
}

int CDbConnection::dealer_get_count(QString sCondition)
{
    int count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QString s("SELECT count(id) FROM dealer");
        if(sCondition.length()>0)
            s+=QString(" WHERE %1").arg(sCondition);
        QSqlQuery query;
        if(query.exec(s))
        {
            query.first();
            if(query.value(0).toInt()>0)
                count=query.value(0).toInt();
        }
    }
    //-
    return count;
}

bool CDbConnection::dealer_get_all(QString sCondition, QList<int> & lsIds,QString sOrderBy)
{
    if(!m_bConnect)
        return false;
    QString s("SELECT * FROM dealer");
    if(sCondition.length()>0)
        s+=QString(" WHERE %1").arg(sCondition);//+condition
    if(sOrderBy.length()>0)
        s+=QString(" ORDER BY %1").arg(sOrderBy);//+order by
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        QSqlRecord record=query.record();
        for(;query.next();)
            lsIds.push_back(query.value( record.indexOf("id")).toInt());
    }
    return true;
}


//customer--------------------------------------
bool CDbConnection::customer_add(CCustomer & cu)
{
    if(!m_bConnect)//db open?
        return false;
    //-
    int id=-1;
    bool b;
    QSqlQuery query;
    QString s=QString("INSERT INTO customer (name,comment,customer_number,first_name,city,postcode,street,call_numbers,fax_numbers,email_adress)"
                      "VALUES ('%1','%2','%3','%4','%5','%6','%7',").arg(cu.get_name(),cu.get_comment(),
                        cu.get_customernumber(),cu.get_first_name(),cu.get_city(),cu.get_postcode(),cu.get_street());
    s+=QString("'%1','%2','%3')").arg(cu.get_callnumber(),cu.get_faxnumber(),cu.get_email());
    b=query.exec(s);//add ok?
    if(b)
    {
        id=query.lastInsertId().toInt();
        cu.set_id(id);
    }
    //-
    return b;
}

bool CDbConnection::customer_remove(int id)
{
    if(!m_bConnect)//db open?
        return false;
    QSqlQuery query;
    QString s=QString("DELETE FROM customer WHERE id=%1").arg(id);
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::customer_update(CCustomer & cu)
{
    if(!m_bConnect)//db open?
        return false;

    QSqlQuery query;
    QString s=QString("UPDATE OR REPLACE customer SET ");
    s+=QString("name='%1',").arg(cu.get_name());
    s+=QString("comment='%1',").arg(cu.get_comment());
    s+=QString("first_name='%1',").arg(cu.get_first_name());
    s+=QString("street='%1',").arg(cu.get_street());
    s+=QString("postcode='%1',").arg(cu.get_postcode());
    s+=QString("city='%1',").arg(cu.get_city());
    s+=QString("customer_number='%1',").arg(cu.get_customernumber());
    s+=QString("call_numbers='%1',").arg(cu.get_callnumber());
    s+=QString("fax_numbers='%1',").arg(cu.get_faxnumber());
    s+=QString("email_adress='%1'").arg(cu.get_email());
    s+=QString(" WHERE id=%1").arg(cu.get_id());

    //exec sql-syntax
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::customer_get(QString sCondition, CCustomer & cu)
{
    if(!m_bConnect||sCondition.length()<=0)
        return false;
    QString s=QString("SELECT * FROM customer WHERE %1").arg(sCondition);
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        query.first();
        if(query.isValid())
        {
            QSqlRecord record=query.record();
            cu.set_id(query.value( record.indexOf("id")).toInt());
            cu.set_name(query.value( record.indexOf("name")).toString());
            cu.set_comment(query.value( record.indexOf("comment")).toString());
            cu.set_customernumber(query.value( record.indexOf("customer_number")).toString());
            cu.set_city(query.value( record.indexOf("city")).toString());
            cu.set_street(query.value( record.indexOf("street")).toString());
            cu.set_postcode(query.value( record.indexOf("postcode")).toString());
            cu.set_first_name(query.value( record.indexOf("first_name")).toString());
            cu.set_callnumber(query.value( record.indexOf("call_numbers")).toString());
            cu.set_faxnumber(query.value( record.indexOf("fax_numbers")).toString());
            cu.set_email(query.value( record.indexOf("email_adress")).toString());
        }
    }
    if(b==true && cu.get_id()<1)//not found
        b=false;
    return b;
}

bool CDbConnection::customer_get(int iId,CCustomer & cu)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition=QString("id=%1").arg(iId);
    bool b=customer_get(sCondition,cu);
    if(b==true && cu.get_id()<1)//not found
        b=false;
    return b;
}

int CDbConnection::customer_get_count(QString sCondition)
{
    int count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QString s("SELECT count(id) FROM customer");
        if(sCondition.length()>0)
            s+=QString(" WHERE %1").arg(sCondition);
        QSqlQuery query;
        if(query.exec(s))
        {
            query.first();
            if(query.value(0).toInt()>0)
                count=query.value(0).toInt();
        }
    }
    //-
    return count;
}

bool CDbConnection::customer_get_all(QString sCondition, QList<int> & lsIds)
{
    if(!m_bConnect)
        return false;
    QString s("SELECT id FROM customer");
    if(sCondition.length()>0)
        s+=QString(" WHERE %1").arg(sCondition);//+condition
    s+=QString(" ORDER BY name ASC");
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        QSqlRecord record=query.record();
        for(;query.next();)
            lsIds.push_back(query.value( record.indexOf("id")).toInt());
    }
    return true;
}

QString CDbConnection::customer_get_customer_number_nomination(void)
{
    QString s,s2;
    if(!m_bConnect)
        return QString("");
    //-get next id-
    int count,next_id=-1;
    QSqlQuery query( "SELECT MAX(id) FROM customer" );
    while ( query.next() ) {
        next_id = (query.value(0).toInt())+1;
    }
    //-check id free-
    for(;;next_id++)
    {
        s=QString("%1").arg(next_id);
        s2=QString("customer_number='%1'").arg(s);
        count=customer_get_count(s2);//is free?
        if(count<=0)
            break;
     }
    //-
    return s;
}


//--article--------------------------------------------------------------------------------------
bool CDbConnection::article_add(CArticle & ar)
{
    if(!m_bConnect)//db open?
        return false;
    //-
    int id=-1;
    bool b;
    QSqlQuery query;
    QString s=QString("INSERT INTO article (name,comment,location,articlenumber,unit,articlenumber2,valuta,inventory,max_inventory,maker_id,waregroup_id,baseprice,salesprice,warning_limit)"
                      "VALUES ('%1','%2','%3','%4','%5','%6','%7',").arg(ar.get_name(),ar.get_comment(),ar.get_location(),ar.get_articlenumber(),ar.get_unit(),ar.get_articlenumber2(),ar.get_valuta());
    s+=QString("%1,").arg(ar.get_inventory());
    s+=QString("%1,").arg(ar.get_max_inventory());
    s+=QString("%1,").arg(ar.get_maker_id());
    s+=QString("%1,").arg(ar.get_waregroup_id());
    s+=QString("%1,").arg(ar.get_base_price());
    s+=QString("%1,").arg(ar.get_sales_price());
    s+=QString("%1)").arg(ar.get_warning_limit());
    //-
    b=query.exec(s);//add ok?
    if(b)
    {
        id=query.lastInsertId().toInt();//get id
        ar.set_id(id);
    }
    //-
    return b;
}

bool CDbConnection::article_remove(int id)
{
    if(!m_bConnect)//db open?
        return false;
    QSqlQuery query;
    QString s=QString("DELETE FROM article WHERE id=%1").arg(id);
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::article_set_delete(int id)
{
    if(!m_bConnect)//db open?
        return false;

    QSqlQuery query;
    QString s=QString("UPDATE article SET delete_status=1 WHERE id=%1").arg(id);
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::article_update(CArticle & ar)
{
    if(!m_bConnect)//db open?
        return false;

    QSqlQuery query;
    QString s=QString("UPDATE OR REPLACE article SET ");
    s+=QString("name='%1',").arg(ar.get_name());
    s+=QString("comment='%1',").arg(ar.get_comment());
    s+=QString("location='%1',").arg(ar.get_location());
    s+=QString("articlenumber='%1',").arg(ar.get_articlenumber());
    s+=QString("unit='%1',").arg(ar.get_unit());
    s+=QString("articlenumber2='%1',").arg(ar.get_articlenumber2());
    s+=QString("valuta='%1',").arg(ar.get_valuta());
    s+=QString("inventory=%1,").arg(ar.get_inventory());
    s+=QString("max_inventory=%1,").arg(ar.get_max_inventory());
    s+=QString("maker_id=%1,").arg(ar.get_maker_id());
    s+=QString("baseprice=%1,").arg(ar.get_base_price());
    s+=QString("salesprice=%1,").arg(ar.get_sales_price());
    s+=QString("waregroup_id=%1,").arg(ar.get_waregroup_id());
    s+=QString("warning_limit=%1,").arg(ar.get_warning_limit());
    s+=QString("delete_status=%1").arg(ar.get_delete());
    s+=QString(" WHERE id=%1").arg(ar.get_id());

    bool b=query.exec(s);
    return b;
}

bool CDbConnection::article_get(QString sCondition, CArticle & ar, bool bWithDeleteArticle)
{
    if(!m_bConnect||sCondition.length()<=0)
        return false;
    //-
    if(!bWithDeleteArticle)
    {
        if(sCondition.length()>0)//set?
            sCondition+=QString(" AND ");
        sCondition+=QString("delete_status = 0");
    }
    //-
    QString s=QString("SELECT * FROM article WHERE %1").arg(sCondition);
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        query.first();
        if(query.isValid())
        {
            QSqlRecord record=query.record();
            ar.set_id(query.value( record.indexOf("id")).toInt());
            ar.set_name(query.value( record.indexOf("name")).toString());
            ar.set_unit(query.value( record.indexOf("unit")).toString());
            ar.set_valuta(query.value( record.indexOf("valuta")).toString());
            ar.set_comment(query.value( record.indexOf("comment")).toString());
            ar.set_articlenumber(query.value( record.indexOf("articlenumber")).toString());
            ar.set_articlenumber2(query.value( record.indexOf("articlenumber2")).toString());
            ar.set_location(query.value( record.indexOf("location")).toString());
            ar.set_inventory(query.value( record.indexOf("inventory")).toUInt());
            ar.set_max_inventory(query.value( record.indexOf("max_inventory")).toUInt());
            ar.set_maker_id(query.value( record.indexOf("maker_id")).toInt());
            ar.set_waregroup_id(query.value( record.indexOf("waregroup_id")).toInt());
            ar.set_base_price((float)query.value(record.indexOf("baseprice")).toDouble());
            ar.set_sales_price((float)query.value(record.indexOf("salesprice")).toDouble());
            ar.set_warning_limit(query.value( record.indexOf("warning_limit")).toInt());
            ar.set_delete(query.value( record.indexOf("delete_status")).toBool());;
        }
    }
    if(b==true && ar.get_id()<1)//not found
        b=false;
    return b;
}

bool CDbConnection::article_get(int iId,CArticle & ar, bool bWithDeleteArticle)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition=QString("id=%1").arg(iId);
    bool b=article_get(sCondition,ar,bWithDeleteArticle);
    if(b==true && ar.get_id()<1)//not found
        b=false;
    return b;
}

QString CDbConnection::article_get_name(int iId, bool bWithDeleteArticle)
{
    CArticle ar;
    QString sCondition,name=QString("");
    if(m_bConnect)//db open?
    {
        sCondition=QString("id=%1").arg(iId);
        if(article_get(sCondition,ar,bWithDeleteArticle))
            name=ar.get_name();
    }
    return name;
}

int CDbConnection::article_get_id(QString sName, bool bWithDeleteArticle)
{
    int id=-1;
    CArticle ar;
    QString sCondition=QString("name='%1'").arg(sName);
    if(m_bConnect)//db open?
    {
        if(article_get(sCondition,ar,bWithDeleteArticle))
            id=ar.get_id();
    }
    return id;
}

int CDbConnection::article_get_count(QString sCondition, bool bWithDeleteArticle)
{
    int count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QString s("SELECT count(id) FROM article");
        //-
        if(!bWithDeleteArticle)
        {
            if(sCondition.length()>0)//set?
                sCondition+=QString(" AND ");
            sCondition+=QString("delete_status = 0");
        }
        //-
        if(sCondition.length()>0)
            s+=QString(" WHERE %1").arg(sCondition);
        QSqlQuery query;
        if(query.exec(s))
        {
            query.first();
            if(query.value(0).toInt()>0)
                count=query.value(0).toInt();
        }
    }
    //-
    return count;
}

bool CDbConnection::article_get_all(QString sCondition, QList<int> & lsIds, bool bWithDeleteArticle)
{
    if(!m_bConnect)
        return false;
    QString s("SELECT id FROM article");
    if(!bWithDeleteArticle)
    {
        if(sCondition.length()>0)//set?
            sCondition+=QString(" AND ");
        sCondition+=QString("delete_status = 0");
    }
    if(sCondition.length()>0)
        s+=QString(" WHERE %1").arg(sCondition);//+condition
    s+=QString(" ORDER BY name ASC");
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        QSqlRecord record=query.record();
        for(;query.next();)
            lsIds.push_back(query.value( record.indexOf("id")).toInt());
    }
    return true;
}

bool CDbConnection::article_all_change_waregroup(int iOldWaregroupId,int iNewWaregroupId)
{
    if(!m_bConnect)
        return false;
    QString s=QString("UPDATE article SET waregroup_id=%1").arg(iNewWaregroupId);
    s+=QString(" WHERE waregroup_id=%1").arg(iOldWaregroupId);
    QSqlQuery query;
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::article_all_change_maker(int iOldMakerId, int iNewMakerId)
{
    if(!m_bConnect)
        return false;
    QString s=QString("UPDATE article SET maker_id=%1").arg(iNewMakerId);
    s+=QString(" WHERE maker_id=%1").arg(iOldMakerId);
    QSqlQuery query;
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::article_get_all_by_maker(QString sName,QList <int> & lsIds,bool bLike, bool bWithDeleteArticle)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition;
    if(bLike)//search name like
        sCondition=QString("name like '%%1%'").arg(sName);
    else//search name=
        sCondition=QString("name = '%1'").arg(sName);
    QList <int> lsIdsMaker;
    //-search maker-
    maker_get_all(sCondition,lsIdsMaker);
    while(lsIdsMaker.count()>0)
    {
        //search all article where this maker
        sCondition=QString("maker_id = %1").arg(lsIdsMaker[0]);
        article_get_all(sCondition,lsIds,bWithDeleteArticle);
        //-
        lsIdsMaker.removeFirst();
    }
    lsIdsMaker.clear();
    return true;
}

bool CDbConnection::article_get_all_by_waregroup(QString sName,QList <int> & lsIds, bool bLike, bool bWithDeleteArticle)
{
    if(!m_bConnect)//db open?
        return false;
    int i,j;
    QString sCondition;
    if(bLike)//search name like
        sCondition=QString("name like '%%1%'").arg(sName);
    else//search name=
        sCondition=QString("name = '%1'").arg(sName);
    QList <int> lsIdsWaregroup;
    QList <int> lsIdsSubwaregroup;
    //-search waregroups-
    waregroup_get_all(sCondition,lsIdsWaregroup);
    while(lsIdsWaregroup.count()>0)
    {
        //search all article where this waregroup
        sCondition=QString("waregroup_id = %1").arg(lsIdsWaregroup[0]);
        article_get_all(sCondition,lsIds,bWithDeleteArticle);
        //search in subwaregroups
        waregroup_get_all_subwaregroup(lsIdsWaregroup[0],lsIdsSubwaregroup);
        while(lsIdsSubwaregroup.count()>0)
        {
            sCondition=QString("waregroup_id = %1").arg(lsIdsSubwaregroup[0]);
            article_get_all(sCondition,lsIds,bWithDeleteArticle);
            lsIdsSubwaregroup.removeFirst();
        }
        //-
        lsIdsWaregroup.removeFirst();
        lsIdsSubwaregroup.clear();
    }
    lsIdsWaregroup.clear();

    //delete double article
    for(i=0;i<lsIds.count();i++)
    {
        for(j=i+1;j<lsIds.count();j++)
        {
            if(lsIds[i]==lsIds[j])//double found?
            {
                lsIds.removeAt(j);//delete double
                j--;
            }
        }
    }
    //-
    return true;
}

bool CDbConnection::article_get_all_by_percent(QList<int> & lsIds, int iPercent,bool bLess)
{
    if(!m_bConnect || iPercent<0)//db open?
        return false;
    CArticle ar;
    QList<int> ls;
    int i;
    bool b=article_get_all("",ls);
    if(b)
    {
        while(ls.count()>0)
        {
            b=article_get(ls[0],ar);
            if(!b)
                break;
            i=(int)(((float)ar.get_inventory())/((float)ar.get_max_inventory())*((float)100));
            if(bLess==true && i<iPercent)//less?
                lsIds.push_back(ls[0]);
            else if(bLess==false && i>iPercent)//greater?
                lsIds.push_back(ls[0]);
            ls.removeFirst();
        }
        ls.clear();
    }
    //-
    return b;
}

bool CDbConnection::article_update_all_count(QList<QString> & ls, int iType)
{
    if(!m_bConnect)//db open?
        return false;
    bool b;
    int iId;
    unsigned int count;
    CArticle ar;
    QStringList lsS;
    //-
    for(int i=0;i<ls.count();i++)
    {
        lsS=ls[i].split("x");//2 element count x article id
        if(lsS.count()==2)
        {
            iId=lsS[1].toInt(&b,10);
            if(b)//its number
            {
                if(article_get(iId,ar))
                {//article found
                    count=lsS[0].toUInt(&b,10);
                    if(b)//number
                    {
                        if(iType==TYPE_OUTGOING || iType ==TYPE_CUSTOMER_OUTGOING)
                            ar.set_inventory(ar.get_inventory()-count);
                        else if(iType==TYPE_INCOMING || iType ==TYPE_ORDERING_INCOMING)
                            ar.set_inventory(ar.get_inventory()+count);
                        article_update(ar);//write in db
                    }
                }
            }
        }
        lsS.clear();
    }
    ls.clear();
    return true;
}

unsigned int CDbConnection::article_get_inventory_on_date(int iArticleId,QDate dt)
{
    if(!m_bConnect)//db open?
        return 0;
    //-get article
    CArticle ar;
    if(!article_get(iArticleId,ar))
        return 0;//not found
    int count=ar.get_inventory();//current count (default)
    //-
    QString sCondition=QString("SELECT * FROM trade WHERE wares like '%x%1|%'").arg(iArticleId);
    sCondition+=QString(" AND date > '%1' AND canceled=0").arg(dt.toString("yyyy-MM-dd"));
    int i,id,type,j;
    QStringList ls,ls2;
    QString sWares;
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(sCondition);
    if(b)
    {
        QSqlRecord record=query.record();
        for(;query.next();)
        {
            type=query.value( record.indexOf("type")).toInt();
            sWares=query.value( record.indexOf("wares")).toString();
            ls=sWares.split("|");
            for(i=0;i<ls.count();i++)
            {
                ls2=ls[i].split("x");
                if(ls2.count()==2)//two elements(count, article id)
                {
                    id=ls2[1].toInt(&b,10);
                    if(b)
                    {//number? id
                        j=ls2[0].toInt(&b,10);
                        if(b)
                        {//number? count
                            if(id==iArticleId)//found?
                            {
                                if(type==TYPE_OUTGOING || type==TYPE_CUSTOMER_OUTGOING)
                                    count+=j;
                                else if(type==TYPE_INCOMING || type==TYPE_ORDERING_INCOMING)
                                    count-=j;
                                break;
                            }
                        }
                    }
                }
            }
            ls.clear();
            ls2.clear();
        }
    }
    return count;
}


//ordering---------------------------------------------------------
bool CDbConnection::ordering_add(COrdering & ord)
{
    if(!m_bConnect)//db open?
        return false;
    //-
    int id=-1;
    bool b;
    QSqlQuery query;
    QString s=QString("INSERT INTO ordering (comment,order_number,wares,dealer_id,date)"
                      "VALUES ('%1','%2','%3',").arg(ord.get_comment(),ord.get_ordernumber(),ord.get_wares());
    s+=QString("%1,").arg(ord.get_dealer_id());
    QDate dt=ord.get_date();
    s+=QString("'%1')").arg(dt.toString("yyyy-MM-dd"));

    b=query.exec(s);//add ok?
    if(b)
    {
        id=query.lastInsertId().toInt();
        ord.set_id(id);
    }
    //-
    return b;
}

bool CDbConnection::ordering_remove(int id)
{
    if(!m_bConnect)//db open?
        return false;
    QSqlQuery query;
    QString s=QString("DELETE FROM ordering WHERE id=%1").arg(id);
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::ordering_update(COrdering & ord)
{
    if(!m_bConnect)//db open?
        return false;

    QSqlQuery query;
    QDate dt;
    QString s=QString("UPDATE OR REPLACE ordering SET ");
    s+=QString("comment='%1',").arg(ord.get_comment());
    s+=QString("order_number='%1',").arg(ord.get_ordernumber());
    s+=QString("wares='%1',").arg(ord.get_wares());
    s+=QString("dealer_id=%1,").arg(ord.get_dealer_id());
    dt=ord.get_date();
    s+=QString("date='%1'").arg(dt.toString("yyyy-MM-dd"));
    s+=QString(" WHERE id=%1").arg(ord.get_id());

    //exec sql-syntax
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::ordering_get(QString sCondition, COrdering & ord)
{
    if(!m_bConnect||sCondition.length()<=0)
        return false;
    QDate dt;
    QString s=QString("SELECT * FROM ordering WHERE %1").arg(sCondition);
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        query.first();
        if(query.isValid())
        {
            QSqlRecord record=query.record();
            ord.set_id(query.value( record.indexOf("id")).toInt());
            ord.set_dealer_id(query.value( record.indexOf("dealer_id")).toInt());
            ord.set_comment(query.value( record.indexOf("comment")).toString());
            ord.set_ordernumber(query.value( record.indexOf("order_number")).toString());
            ord.set_wares(query.value( record.indexOf("wares")).toString());
            dt=query.value( record.indexOf("date")).toDate();
            ord.set_date(dt);
        }
    }
    if(b==true && ord.get_id()<1)//not found
        b=false;
    return b;
}

bool CDbConnection::ordering_get(int iId,COrdering & ord)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition=QString("id=%1").arg(iId);
    bool b=ordering_get(sCondition,ord);
    if(b==true && ord.get_id()<1)//not found
        b=false;
    return b;
}

int CDbConnection::ordering_get_count(QString sCondition)
{
    int count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QString s("SELECT count(id) FROM ordering");
        if(sCondition.length()>0)
            s+=QString(" WHERE %1").arg(sCondition);
        QSqlQuery query;
        if(query.exec(s))
        {
            query.first();
            if(query.value(0).toInt()>0)
                count=query.value(0).toInt();
        }
    }
    //-
    return count;
}

QString CDbConnection::ordering_get_wares_list(int iId, int iMaxLength, bool bWithCount)
{
    bool b;
    int iArticleId;
    QStringList lsString;
    QList<QString> lsS;
    COrdering ord;
    QString s,sReturn;
    if(m_bConnect==true && iId>0)//db open?
    {
        if(ordering_get(iId,ord))//get ordering data
        {
            ord.get_wares(lsS);//get count+article list
            while(lsS.count()>0)
            {
                lsString=lsS[0].split("x");
                if(lsString.count()==2)// 2 elements count and article id
                {
                    iArticleId=lsString[1].toInt(&b,10);//convert id to int
                    if(b)//convert ok?
                    {
                        s=article_get_name(iArticleId);//get article name by id
                        if(s.length()>0)//found?
                        {
                            if(sReturn.length()>0)//not first article
                                sReturn+=QString(" | ");
                            if(s.length()+sReturn.length()>iMaxLength)
                            {
                                sReturn+=QString("...");
                                break;
                            }
                            else
                            {
                                if(!bWithCount)//article name only
                                    sReturn+=QString("\"%1\"").arg(s);
                                else//with count
                                    sReturn+=QString("\"%1x%2\"").arg(lsString[0],s);
                            }
                        }
                    }
                }
                lsString.clear();
                lsS.removeFirst();
            }
            lsS.clear();
        }
    }
    return sReturn;
}

QString CDbConnection::ordering_get_wares_list(QList<QString> & lsOrdering, int iMaxLength)
{
    bool b;
    int i,iArticleId;
    QStringList lsString;
    QString s,sReturn;
    if(m_bConnect==true && lsOrdering.count()>0)//db open?
    {
        for(i=0;i<lsOrdering.count();i++)
        {
            lsString=lsOrdering[i].split("x");
            if(lsString.count()==2)// 2 elements count and article id
            {
                iArticleId=lsString[1].toInt(&b,10);//convert id to int
                if(b)//convert ok?
                {
                    s=article_get_name(iArticleId);//get article name by id
                    if(s.length()>0)//found?
                    {
                        if(sReturn.length()>0)//not first article
                            sReturn+=QString("|");
                        if(s.length()+sReturn.length()>iMaxLength)
                        {
                            sReturn+=QString("...");
                            break;
                        }
                        else
                            sReturn+=s;
                    }
                }
            }
            lsString.clear();
        }
    }
    return sReturn;
}

bool CDbConnection::ordering_get_all(QString sCondition, QList<int> & lsIds)
{
    if(!m_bConnect)
        return false;
    QString s("SELECT id FROM ordering");
    if(sCondition.length()>0)
        s+=QString(" WHERE %1").arg(sCondition);//+condition
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        QSqlRecord record=query.record();
        for(;query.next();)
            lsIds.push_back(query.value( record.indexOf("id")).toInt());
    }
    return true;
}

bool CDbConnection::ordering_get_all_by_dealer(QString sName,QList <int> & lsIds,bool bLike)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition;
    if(bLike)//search name like
        sCondition=QString("name like '%%1%'").arg(sName);
    else//search name=
        sCondition=QString("name = '%1'").arg(sName);
    QList <int> lsIdsDealer;
    //-search dealer-
    dealer_get_all(sCondition,lsIdsDealer);
    while(lsIdsDealer.count()>0)
    {
        //search all ordering where this dealer
        sCondition=QString("dealer_id = %1").arg(lsIdsDealer[0]);
        ordering_get_all(sCondition,lsIds);
        //-
        lsIdsDealer.removeFirst();
    }
    lsIdsDealer.clear();
    return true;
}

bool CDbConnection::ordering_get_all_by_maker(QString sName,QList <int> & lsIds,bool bLike)
{
    if(!m_bConnect)//db open?
        return false;

    QString sCondition;
    //search article by maker
    QList<int> lsIdsArticle;
    article_get_all_by_maker(sName,lsIdsArticle,bLike);

    //search all orderings with article
    while(lsIdsArticle.count()>0)
    {
        //search all ordering where this article
        sCondition=QString("wares like '%x%1|%'").arg(lsIdsArticle[0]);
        ordering_get_all(sCondition,lsIds);
        //-
        lsIdsArticle.removeFirst();
    }
    lsIdsArticle.clear();
    return true;
}

bool CDbConnection::ordering_get_all_by_waregroup(QString sName,QList <int> & lsIds,bool bLike)
{
    if(!m_bConnect)//db open?
        return false;

    QString sCondition;
    //search article by waregroup
    QList<int> lsIdsArticle;
    article_get_all_by_waregroup(sName,lsIdsArticle,bLike);

    //search all orderings with article
    while(lsIdsArticle.count()>0)
    {
        //search all ordering where this article
        sCondition=QString("wares like '%x%1|%'").arg(lsIdsArticle[0]);
        ordering_get_all(sCondition,lsIds);
        //-
        lsIdsArticle.removeFirst();
    }
    lsIdsArticle.clear();
    return true;
}

bool CDbConnection::ordering_get_all_by_article(QString sName,QList <int> & lsIds, bool bLike)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition;
    if(bLike)//search name like
        sCondition=QString("name like '%%1%'").arg(sName);
    else//search name=
        sCondition=QString("name = '%1'").arg(sName);
    QList <int> lsIdsArticle;
    //-search article-
    article_get_all(sCondition,lsIdsArticle);
    while(lsIdsArticle.count()>0)
    {
        //search all ordering where this article
        sCondition=QString("wares like '%x%1|%'").arg(lsIdsArticle[0]);
        ordering_get_all(sCondition,lsIds);
        //-
        lsIdsArticle.removeFirst();
    }
    lsIdsArticle.clear();
    return true;
}

bool CDbConnection::ordering_get_all_by_articlenumber(QString sNumber,QList<int> & lsIds, bool bArticlenumber1, bool bLike) //bArticlenumber1 true->nr.1 else false->nr.2
{
    if(!m_bConnect)//db open?
        return false;

    QString sCondition("articlenumber");
    if(!bArticlenumber1)//nr.2?
        sCondition+=QString("2");
    //-
    if(bLike)//search name like
        sCondition+=QString(" like '%%1%'").arg(sNumber);
    else
        sCondition+=QString(" = '%1'").arg(sNumber);
    //-
    QList <int> lsIdsArticle;
    //-search article-
    article_get_all(sCondition,lsIdsArticle);
    while(lsIdsArticle.count()>0)
    {
        //search all ordering where this article
        sCondition=QString("wares like '%x%1|%'").arg(lsIdsArticle[0]);
        ordering_get_all(sCondition,lsIds);
        //-
        lsIdsArticle.removeFirst();
    }
    lsIdsArticle.clear();
    return true;
}

int CDbConnection::ordering_get_count_by_article(int iArticleId)
{//how many article from id are ordering???

    if(!m_bConnect)//db open?
        return -1;
    QString sCondition=QString("wares like '%x%1|%'").arg(iArticleId);
    QList <int> lsIds;
    QList<QString> lsS;
    QStringList lsS2;
    COrdering ord;
    bool b;
    int i,id,count=0;
    //-search ordering with article-
    ordering_get_all(sCondition,lsIds);
    while(lsIds.count()>0)
    {
        if(ordering_get(lsIds[0],ord))//get ordering
        {
            ord.get_wares(lsS);//split ordering
            while(lsS.count()>0)
            {
                lsS2=lsS[0].split("x");//split in count and article id
                if(lsS2.count()==2)//2 elements must be, count and article id
                {
                    id=lsS2[1].toInt(&b,10);
                    if(b)//number?
                    {
                        if(id==iArticleId)
                        {//found
                            i=lsS2[0].toInt(&b,10);
                            if(b)//number?
                            {
                                count+=i;
                                break;
                            }
                        }
                    }
                }
                //-
                lsS2.clear();
                lsS.removeFirst();
            }
            lsS.clear();
        }
        lsIds.removeFirst();
    }
    lsIds.clear();
    return count;
}

int CDbConnection::ordering_get_ordering_count_by_article(int iArticleId)
{
    QString sCondition=QString("wares like '%x%1|%'").arg(iArticleId);
    int iReturn=ordering_get_count(sCondition);
    //-
    return iReturn;
}

bool CDbConnection::ordering_all_change_dealer(int iOldDealerId, int iNewDealerId)
{
    if(!m_bConnect)
        return false;
    QString s=QString("UPDATE ordering SET dealer_id=%1").arg(iNewDealerId);
    s+=QString(" WHERE dealer_id=%1").arg(iOldDealerId);
    QSqlQuery query;
    bool b=query.exec(s);
    return b;
}


//trade-------------------------------------------------------------------------------
bool CDbConnection::trade_add(CTrade & tr)
{
    if(!m_bConnect)//db open?
        return false;
    //-
    bool b;
    QSqlQuery query;
    QString s=QString("INSERT INTO trade (comment,booking_number,wares,info_1,info_2,info_3,info_4,info_5,type,canceled,date)"
                      "VALUES ('%1','%2','%3','%4','%5','%6','%7','%8',").arg(tr.get_comment(),tr.get_booking_number(),tr.get_wares(),tr.get_info_1(),tr.get_info_2(),
                                                                              tr.get_info_3(),tr.get_info_4(),tr.get_info_5());
    s+=QString("%1,").arg(tr.get_type());
    if(tr.get_canceled()==false)
        s+=QString("0,");
    else
        s+=QString("1,");
    QDate dt=tr.get_date();
    s+=QString("'%1')").arg(dt.toString("yyyy-MM-dd"));
    //-
    b=query.exec(s);//add ok?
    return b;
}

bool CDbConnection::trade_update(CTrade & tr)
{
    if(!m_bConnect)//db open?
        return false;

    QSqlQuery query;
    QDate dt;
    QString s=QString("UPDATE OR REPLACE trade SET ");
    s+=QString("comment='%1',").arg(tr.get_comment());
    s+=QString("booking_number='%1',").arg(tr.get_booking_number());
    s+=QString("wares='%1',").arg(tr.get_wares());
    dt=tr.get_date();
    s+=QString("date='%1',").arg(dt.toString("yyyy-MM-dd"));
    s+=QString("info_1='%1',").arg(tr.get_info_1());
    s+=QString("info_2='%1',").arg(tr.get_info_2());
    s+=QString("info_3='%1',").arg(tr.get_info_3());
    s+=QString("info_4='%1',").arg(tr.get_info_4());
    s+=QString("info_5='%1',").arg(tr.get_info_5());
    if(tr.get_canceled()==false)
        s=QString("canceled=0,");
    else
        s=QString("canceled=1,");
    s+=QString("type=%1").arg(tr.get_type());
    s+=QString(" WHERE booking_number='%1'").arg(tr.get_booking_number());

    //exec sql-syntax
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::trade_remove_all(QString sCondition)
{
    if(!m_bConnect || sCondition.length()<=0)//db open?  condition set?
        return false;
    QSqlQuery query;
    QString s=QString("DELETE FROM trade WHERE %1").arg(sCondition);
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::trade_get(QString sBookingNumber ,CTrade & tr)
{   
    if(!m_bConnect)
        return false;
    QDate dt;
    QString s=QString("SELECT * FROM trade WHERE booking_number='%1'").arg(sBookingNumber);
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        query.first();
        if(query.isValid())
        {
            QSqlRecord record=query.record();
            tr.set_canceled(query.value( record.indexOf("canceled")).toBool());
            tr.set_comment(query.value( record.indexOf("comment")).toString());
            tr.set_booking_number(query.value( record.indexOf("booking_number")).toString());
            tr.set_wares(query.value( record.indexOf("wares")).toString());
            tr.set_type(query.value( record.indexOf("type")).toInt());
            dt=query.value( record.indexOf("date")).toDate();
            tr.set_date(dt);
            tr.set_info_1(query.value( record.indexOf("info_1")).toString());
            tr.set_info_2(query.value( record.indexOf("info_2")).toString());
            tr.set_info_3(query.value( record.indexOf("info_3")).toString());
            tr.set_info_4(query.value( record.indexOf("info_4")).toString());
            tr.set_info_5(query.value( record.indexOf("info_5")).toString());
        }
    }
    return b;
}

bool CDbConnection::trade_get_all(QString sCondition,QList <QString> & lsBookingNumbers)
{
    if(!m_bConnect)
        return false;

    QString s("SELECT * FROM trade");
    if(sCondition.length()>0)
        s+=QString(" WHERE %1").arg(sCondition);//+condition
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        QSqlRecord record=query.record();
        for(;query.next();)
            lsBookingNumbers.push_back(query.value( record.indexOf("booking_number")).toString());
    }
    return b;
}

bool CDbConnection::trade_give_it_trades_with_article(int iArticleId)
{
    if(!m_bConnect)//db open?
        return false;
    QString s=QString("SELECT * FROM trade WHERE wares like '%x%1|%'").arg(iArticleId);
    QSqlQueryModel model;
    QSqlQuery query;
    query.exec(s);
    model.setQuery(query);
    if(model.rowCount()>0)
        return true;
    return false;
}

bool CDbConnection::trade_set_canceled(QString sBookingNumber)
{
    if(!m_bConnect)//db open?
        return false;
    QSqlQuery query;
    QString s=QString("UPDATE trade SET canceled=1 WHERE booking_number='%1'").arg(sBookingNumber);
    bool b=query.exec(s);
    return b;
}

int CDbConnection::trade_get_count(QString sCondition)
{
    int count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QString s("SELECT count(booking_number) FROM trade");
        if(sCondition.length()>0)
            s+=QString(" WHERE %1").arg(sCondition);
        QSqlQuery query;
        if(query.exec(s))
        {
            query.first();
            if(query.value(0).toInt()>0)
                count=query.value(0).toInt();
        }
    }
    //-
    return count;
}

QString CDbConnection::trade_get_next_free_booking_number(QDate dt)
{//get next id of this date

    if(!m_bConnect)//db open?
        return "";
    QString s2,s=QString("SELECT * FROM trade WHERE date='%1'").arg(dt.toString("yyyy-MM-dd"));
    QSqlQueryModel model;
    QSqlQuery query;
    query.exec(s);
    model.setQuery(query);
    int i,count=model.rowCount();

    //-check is free-
    for(;;count++)
    {
        s=QString("%1/").arg(dt.toString("yyyy-MM-dd"));
        s+=QString("%1").arg(count);
        s2=QString("booking_number='%1'").arg(s);
        i=trade_get_count(s2);//is free?
        if(i<=0)
            break;
    }
    //-
    return s;
}

QString CDbConnection::trade_get_article_list(QString sBookingNumber, int iMaxLength, bool bWithCount)
{
    bool b;
    int iArticleId;
    QStringList lsString;
    QList<QString> lsS;
    CTrade tr;
    QString s,sReturn;
    if(m_bConnect==true)//db open?
    {
        if(trade_get(sBookingNumber,tr))//get data
        {
            tr.get_wares(lsS);//get count+article list
            while(lsS.count()>0)
            {
                lsString=lsS[0].split("x");
                if(lsString.count()==2)// 2 elements count and article id
                {
                    iArticleId=lsString[1].toInt(&b,10);//convert id to int
                    if(b)//convert ok?
                    {
                        s=article_get_name(iArticleId,true);//get article name by id
                        if(s.length()>0)//found?
                        {
                            if(sReturn.length()>0)//not first article
                                sReturn+=QString(" | ");
                            if(s.length()+sReturn.length()>iMaxLength)
                            {
                                sReturn+=QString("...");
                                break;
                            }
                            else
                            {
                                if(!bWithCount)
                                    sReturn+=QString("\"%1\"").arg(s);//only article name
                                else//with count
                                    sReturn+=QString("\"%1x%2\"").arg(lsString[0],s);
                            }
                        }
                    }
                }
                lsString.clear();
                lsS.removeFirst();
            }
            lsS.clear();
        }
    }
    return sReturn;
}

bool CDbConnection::trade_get_all_by_maker(QString sName,QString sDates,QList <QString> & lsBookingNumber,bool bLike)
{
    if(!m_bConnect)//db open?
        return false;

    QString sCondition;
    //search article by maker
    QList<int> lsIdsArticle;
    article_get_all_by_maker(sName,lsIdsArticle,bLike,true);

    //search all trades with article
    while(lsIdsArticle.count()>0)
    {
        //search all ordering where this article
        sCondition=QString("wares like '%x%1|%'").arg(lsIdsArticle[0]);
        sCondition+=QString(" AND %1").arg(sDates);
        trade_get_all(sCondition,lsBookingNumber);
        //-
        lsIdsArticle.removeFirst();
    }
    lsIdsArticle.clear();
    return true;
}

bool CDbConnection::trade_get_all_by_waregroup(QString sName,QString sDates,QList <QString> & lsBookingNumber,bool bLike)
{
    if(!m_bConnect)//db open?
        return false;

    QString sCondition;
    //search article by waregroup
    QList<int> lsIdsArticle;
    article_get_all_by_waregroup(sName,lsIdsArticle,bLike,true);

    //search all trades with article
    while(lsIdsArticle.count()>0)
    {
        //search all trades where this article
        sCondition=QString("wares like '%x%1|%'").arg(lsIdsArticle[0]);
        sCondition+=QString(" AND %1").arg(sDates);
        trade_get_all(sCondition,lsBookingNumber);
        //-
        lsIdsArticle.removeFirst();
    }
    lsIdsArticle.clear();
    return true;
}

bool CDbConnection::trade_get_all_by_article(QString sName,QString sDates,QList <QString> & lsBookingNumber, bool bLike)
{
    if(!m_bConnect)//db open?
        return false;
    QString sCondition;
    if(bLike)//search name like
        sCondition=QString("name like '%%1%'").arg(sName);
    else//search name=
        sCondition=QString("name = '%1'").arg(sName);
    QList <int> lsIdsArticle;
    //-search article-
    article_get_all(sCondition,lsIdsArticle,true);
    while(lsIdsArticle.count()>0)
    {
        //search all trades where this article
        sCondition=QString("wares like '%x%1|%'").arg(lsIdsArticle[0]);
        sCondition+=QString(" AND %1").arg(sDates);
        trade_get_all(sCondition,lsBookingNumber);
        //-
        lsIdsArticle.removeFirst();
    }
    lsIdsArticle.clear();
    return true;
}

bool CDbConnection::trade_get_all_by_articlenumber(QString sNumber,QString sDates,QList <QString> & lsBookingNumber, bool bArticlenumber1, bool bLike) //bArticlenumber1 true->nr.1 else false->nr.2
{
    if(!m_bConnect)//db open?
        return false;

    QString sCondition("articlenumber");
    if(!bArticlenumber1)//nr.2?
        sCondition+=QString("2");
    //-
    if(bLike)//search name like
        sCondition+=QString(" like '%%1%'").arg(sNumber);
    else
        sCondition+=QString(" = '%1'").arg(sNumber);
    //-
    QList <int> lsIdsArticle;
    //-search article-
    article_get_all(sCondition,lsIdsArticle,true);
    while(lsIdsArticle.count()>0)
    {
        //search all trades where this article
        sCondition=QString("wares like '%x%1|%'").arg(lsIdsArticle[0]);
        sCondition+=QString(" AND %1").arg(sDates);
        trade_get_all(sCondition,lsBookingNumber);
        //-
        lsIdsArticle.removeFirst();
    }
    lsIdsArticle.clear();
    return true;
}

int CDbConnection::trade_get_all_count_by_maker(QString sName, QDate dtFrom, QDate dtTo, bool bLike)
{
    bool b=true;
    int j,i=0,count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QList<int> lsIdsM,lsIdsA;
        QString sCondition;
        if(bLike)
            sCondition=QString("name like '%%1%'").arg(sName);
        else
            sCondition=QString("name = '%1'").arg(sName);
        //-
        if(maker_get_all(sCondition,lsIdsM))//get all maker
        {
            while(lsIdsM.count()>0)//get all article by maker id
            {
                sCondition=QString("maker_id = %1").arg(lsIdsM[0]);
                if(!article_get_all(sCondition,lsIdsA,true))
                {
                    b=false;
                    break;
                }
                //-
                lsIdsM.removeFirst();
            }
            lsIdsM.clear();
            //-
            if(b)
            {
                while(lsIdsA.count()>0)
                {
                    sCondition=QString("wares like '%x%1|%'").arg(lsIdsA[0]);
                    if(dtTo>dtFrom)
                        sCondition+=QString(" AND date >= '%1' AND date <='%2'").arg(dtFrom.toString("yyyy-MM-dd"),dtTo.toString("yyyy-MM-dd"));
                    else
                        sCondition+=QString(" AND date = '%1'").arg(dtFrom.toString("yyyy-MM-dd"));
                    j=trade_get_count(sCondition);
                    if(j>0)
                        i+=j;
                    else if(j<0)//error
                    {
                        b=false;
                        break;
                    }
                    //-
                    lsIdsA.removeFirst();
                }
                lsIdsA.clear();
                if(b)
                    count=i;
            }
        }
    }
    //-
    return count;
}

int CDbConnection::trade_get_all_count_by_waregroup(QString sName, QDate dtFrom, QDate dtTo, bool bLike)
{
    bool b=true;
    int j,i=0,count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QList<int> lsIdsW,lsIdsA;
        QString sCondition;
        if(bLike)
            sCondition=QString("name like '%%1%'").arg(sName);
        else
            sCondition=QString("name = '%1'").arg(sName);
        //-
        if(waregroup_get_all(sCondition,lsIdsW))//get all waregroup
        {
            while(lsIdsW.count()>0)//get all article by waregroup id
            {
                sCondition=QString("waregroup_id = %1").arg(lsIdsW[0]);
                if(!article_get_all(sCondition,lsIdsA,true))
                {
                    b=false;
                    break;
                }
                //-
                lsIdsW.removeFirst();
            }
            lsIdsW.clear();
            //-
            if(b)
            {
                while(lsIdsA.count()>0)
                {
                    sCondition=QString("wares like '%x%1|%'").arg(lsIdsA[0]);
                    if(dtTo>dtFrom)
                        sCondition+=QString(" AND date >= '%1' AND date <='%2'").arg(dtFrom.toString("yyyy-MM-dd"),dtTo.toString("yyyy-MM-dd"));
                    else
                        sCondition+=QString(" AND date = '%1'").arg(dtFrom.toString("yyyy-MM-dd"));
                    j=trade_get_count(sCondition);
                    if(j>0)
                        i+=j;
                    else if(j<0)//error
                    {
                        b=false;
                        break;
                    }
                    //-
                    lsIdsA.removeFirst();
                }
                lsIdsA.clear();
                if(b)
                    count=i;
            }
        }
    }
    //-
    return count;
}

int CDbConnection::trade_get_all_count_by_article(QString sName, QDate dtFrom, QDate dtTo, bool bLike)
{
    bool b=true;
    int j,i=0,count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QList<int> lsIdsA;
        QString sCondition;
        if(bLike)
            sCondition=QString("name like '%%1%'").arg(sName);
        else
            sCondition=QString("name = '%1'").arg(sName);
        //-
        if(article_get_all(sCondition,lsIdsA,true))//get all article
        {
            while(lsIdsA.count()>0)
            {
                sCondition=QString("wares like '%x%1|%'").arg(lsIdsA[0]);
                if(dtTo>dtFrom)
                    sCondition+=QString(" AND date >= '%1' AND date <='%2'").arg(dtFrom.toString("yyyy-MM-dd"),dtTo.toString("yyyy-MM-dd"));
                else
                    sCondition+=QString(" AND date = '%1'").arg(dtFrom.toString("yyyy-MM-dd"));
                j=trade_get_count(sCondition);
                if(j>0)
                    i+=j;
                else if(j<0)//error
                {
                    b=false;
                    break;
                }
                //-
                lsIdsA.removeFirst();
            }
            lsIdsA.clear();
            if(b)
                count=i;
        }
    }
    //-
    return count;
}

int CDbConnection::trade_get_all_count_by_articlenumber(QString sNumber, QDate dtFrom, QDate dtTo, bool bArticlenumber1, bool bLike)
{
    bool b=true;
    int j,i=0,count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QList<int> lsIdsA;
        QString sCondition;
        if(bArticlenumber1)
        {
            if(bLike)
                sCondition=QString("articlenumber like '%%1%'").arg(sNumber);
            else
                sCondition=QString("articlenumber = '%1'").arg(sNumber);
        }
        else
        {
            if(bLike)
                sCondition=QString("articlenumber2 like '%%1%'").arg(sNumber);
            else
                sCondition=QString("articlenumber2 = '%1'").arg(sNumber);
        }
        //-
        if(article_get_all(sCondition,lsIdsA,true))//get all article
        {
            while(lsIdsA.count()>0)
            {
                sCondition=QString("wares like '%x%1|%'").arg(lsIdsA[0]);
                if(dtTo>dtFrom)
                    sCondition+=QString(" AND date >= '%1' AND date <='%2'").arg(dtFrom.toString("yyyy-MM-dd"),dtTo.toString("yyyy-MM-dd"));
                else
                    sCondition+=QString(" AND date = '%1'").arg(dtFrom.toString("yyyy-MM-dd"));
                j=trade_get_count(sCondition);
                if(j>0)
                    i+=j;
                else if(j<0)//error
                {
                    b=false;
                    break;
                }
                //-
                lsIdsA.removeFirst();
            }
            lsIdsA.clear();
            if(b)
                count=i;
        }
    }
    //-
    return count;
}

bool CDbConnection::trade_check_cancel_possible(QString sBookingNumber,QString & sErrorString)
{
    if(!m_bConnect || sBookingNumber.length()<=0)//db open?
        return false;

    bool b=true,bReturn=false;
    QDate dtTrade,dt,dtMark=QDate(0,0,0);
    QString sCondition,s;
    QList<int> lsArticleIds;
    QList<int> lsArticleCounts;
    QList<QString> lsBookingnumbers;
    int i,inv,type,c,invMark=0;
    CTrade tr;
    CArticle ar;

    if(trade_get(sBookingNumber,tr))//get trade
    {
        type=tr.get_type();
        if(type==TYPE_INCOMING || type==TYPE_ORDERING_INCOMING)//but canceled outgoing trade = always possible
        {
            dtTrade=tr.get_date();//get trade date
            tr.get_from_wares(lsArticleCounts,false);// get all article_counts from trade
            tr.get_from_wares(lsArticleIds,true);// get all article_ids from trade
            if(lsArticleIds.count()==lsArticleCounts.count())
            {
                for(i=0;i<lsArticleIds.count();i++)
                {
                    b=article_get(lsArticleIds[i],ar,true);//get article
                    if(b)
                    {
                        if(ar.get_delete()==true)//ignore deleted article
                            continue;
                        //-
                        c=(int)ar.get_inventory()-lsArticleCounts[i];
                        if(c<0)//if cur.inventory - trade article count < 0 -> error
                        {
                            sErrorString=QString("Der aktuelle Bestand des Artikels  \"%1\" ist kleiner als dessen Anzahl in der zu stornierendenn Buchung.").arg(ar.get_name());
                            b=false;
                            break;//error
                        }
                        //-
                        inv=c; //cur.inv. - article count
                        dt=QDate::currentDate();//cur.date
                        while(dt>dtTrade)
                        {
                            sCondition=QString("wares like '%x%1|%'").arg(lsArticleIds[i]);
                            sCondition+=QString(" AND date = '%1' AND canceled=0").arg(dt.toString("yyyy-MM-dd"));
                            b=trade_get_all(sCondition,lsBookingnumbers);//get all trades from trade on date
                            if(b)
                            {
                                while(lsBookingnumbers.count()>0)
                                {
                                    b=trade_get(lsBookingnumbers[0],tr);
                                    if(b)
                                    {
                                        c=tr.get_article_count(lsArticleIds[i]);
                                        type=tr.get_type();
                                        //-
                                        if(type==TYPE_OUTGOING || type==TYPE_CUSTOMER_OUTGOING)
                                            inv+=c;
                                        else if(type==TYPE_INCOMING || type==TYPE_ORDERING_INCOMING)
                                            inv-=c;
                                        //-
                                        lsBookingnumbers.removeFirst();
                                    }
                                    if(!b)
                                        break;
                                }
                                //-
                                if(inv<0)//inv on this date <0?
                                {
                                    dtMark=dt;
                                    invMark=inv;
                                }
                                //-
                                lsBookingnumbers.clear();
                                dt=dt.addDays(-1);//day before
                            }
                            if(!b)
                                break;
                        }
                    }
                    //-
                    if(b==false || dtMark!=QDate(0,0,0))
                        break;
                }
            }
        }
    }
    //---
    if(b)
    {
        if(dtMark==QDate(0,0,0))
            bReturn=true;//canceled possible
        else
        {
            sErrorString=QString("Der Bestand des Artikels \"%1\" wäre nach der Stornierung der Buchung am '%2'").arg(ar.get_name(),dtMark.toString("dd.MM.yyyy"));
            sErrorString+=QString(" bei '%1").arg(invMark);
            s=ar.get_unit();
            if(s.length()>0)
                sErrorString+=QString(" %1'").arg(s);
            sErrorString+=QString("'.");
        }
    }
    //-
    lsBookingnumbers.clear();
    lsArticleIds.clear();
    lsArticleCounts.clear();
    return bReturn;
}

bool CDbConnection::trade_is_canceled(QString sBookingNumber)
{
    bool bReturn=false;
    CTrade tr;
    if(trade_get(sBookingNumber,tr))
        bReturn=tr.get_canceled();
    return bReturn;
}

bool CDbConnection::trade_update_all_info(int iNumber, int iTradeType, QString sOld, QString sNew)
{
    if(!m_bConnect || iNumber<1 || iNumber>5)//db open?
        return false;

    CTrade trade;
    QList<QString> ls;
    QString sTableRowName=QString("info_%1").arg(iNumber);
    QString sCondition=QString("type = %1").arg(iTradeType);
    sCondition+=QString(" AND %1 = '%2'").arg(sTableRowName,sOld);
    //-
    if(trade_get_all(sCondition, ls))//search all trades where info = old
    {
        //update info
        while(ls.count()>0)
        {
            if(trade_get(ls[0],trade))
            {
                trade.set_info(iNumber,sNew);
                trade_update(trade);
            }
            //-
            ls.removeFirst();
        }
    }
    //-
    ls.clear();
    return true;
}

//logbook----------------------------------------------------------------------------------------------------------------------
bool CDbConnection::logbook_add(CLogbook & lg)
{
    if(!m_bConnect)//db open?
        return false;
    //-
    bool b;
    QDateTime dt_ti=lg.get_date_time();
    QSqlQuery query;
    QString s=QString("INSERT INTO logbook (date_time, text, type)"
                      "VALUES ('%1','%2',").arg(dt_ti.toString("yyyy-MM-dd hh:mm:ss"),lg.get_text());
    s+=QString("%1)").arg(lg.get_type());
    //-
    b=query.exec(s);//add ok?
    //-
    return b;
}

bool CDbConnection::logbook_remove_all(QString sCondition)
{
    if(!m_bConnect)//db open?
        return false;
    QSqlQuery query;
    QString s=QString("DELETE FROM logbook");
    if(sCondition.length()>0)//set?
        s+=QString(" WHERE %1").arg(sCondition);
    bool b=query.exec(s);
    return b;
}

bool CDbConnection::logbook_get(QDateTime dt_tiDateTime,CLogbook & lg)
{
    if(!m_bConnect)
        return false;
    QString s=QString("SELECT * FROM logbook WHERE date_time='%1'").arg(dt_tiDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        query.first();
        if(query.isValid())
        {
            QSqlRecord record=query.record();
            lg.set_type(query.value( record.indexOf("type")).toInt());
            lg.set_text(query.value( record.indexOf("text")).toString());
            lg.set_date_time(query.value( record.indexOf("date_time")).toDateTime());
        }
    }
    return b;
}

bool CDbConnection::logbook_get_all(QString sCondition, QList<QDateTime> & lsDtTi, QString sOrderBy)
{
    if(!m_bConnect)
        return false;

    QString s("SELECT * FROM logbook");
    if(sCondition.length()>0)
        s+=QString(" WHERE %1").arg(sCondition);//+condition
    if(sOrderBy.length()>0)
        s+=QString(" ORDER BY %1").arg(sOrderBy);//+condition
    QSqlQuery query;
    query.setForwardOnly(true);
    bool b=query.exec(s);
    if(b)
    {
        QSqlRecord record=query.record();
        for(;query.next();)
            lsDtTi.push_back(query.value( record.indexOf("date_time")).toDateTime());
    }
    return b;
}

int CDbConnection::logbook_get_count(QString sCondition)
{
    int count=-1;
    //-
    if(m_bConnect)//db open?
    {
        QString s("SELECT count(date_time) FROM logbook");
        if(sCondition.length()>0)
            s+=QString(" WHERE %1").arg(sCondition);
        QSqlQuery query;
        if(query.exec(s))
        {
            query.first();
            if(query.value(0).toInt()>0)
                count=query.value(0).toInt();
        }
    }
    //-
    return count;
}

bool CDbConnection::logbook_create_new(CWaregroup & wg, CLogbook & lg)
{
    if(!m_bConnect)//db open?
        return false;

    QString s2,s;
    //-
    s2=waregroup_get_path(wg.get_id());
    s=QString("\"%1\"").arg(s2);
    if(s.length()>300)
    {
        s.resize(297);
        s+=QString("...");
    }
    lg.set_text(s);
    //-
    lg.set_type(LOGBOOK_TYPE_WAREGROUP_NEW);
    //-
    QDateTime dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
    lg.set_date_time(dt_ti);
    //-
    return true;
}

bool CDbConnection::logbook_create_edit(CWaregroup & wg, CLogbook & lg, bool bMark)
{
    if(!m_bConnect)//db open?
        return false;

    int i;
    bool b=true;
    QString s3,s2,s=QString("");
    QDateTime dt_ti;
    QList<QString> lsC,lsB,lsA;//before & after & comment
    static CWaregroup wgMark=CWaregroup();
    //-
    if(bMark)//mark before setting changed
        wgMark.set(wg.get_parent_id(),wg.get_name(),wg.get_comment(),wg.get_id());
    else
    {
        if(wg.get_id()!=wgMark.get_id())//not the same waregroup?(error)
            b=false;
        else
        {
            //after setting changed
            if(wg.get_name()!=wgMark.get_name())
            {
                lsB.push_back(wgMark.get_name());
                lsA.push_back(wg.get_name());
                lsC.push_back("Bez:");
            }
            if(wg.get_parent_id()!=wgMark.get_parent_id())
            {
                if(wgMark.get_parent_id()==-1)
                    s3=QString("Hauptverzeichnis");
                else
                    s3=waregroup_get_name(wgMark.get_parent_id());
                if(wg.get_parent_id()==-1)
                    s2=QString("Hauptverzeichnis");
                else
                 s2=waregroup_get_name(wg.get_parent_id());
                lsB.push_back(s3);
                lsA.push_back(s2);
                lsC.push_back("Uvv:");
            }
            if(wg.get_comment()!=wgMark.get_comment())
            {
                lsB.push_back(wgMark.get_comment());
                lsA.push_back(wg.get_comment());
                lsC.push_back("Kom:");
            }
            //check counts
            i=lsA.count()+lsB.count()+lsC.count();
            if(i%3!=0 || i<=0)//count != ? error  or not changed
                b=false;
            else
            {
                s2=waregroup_get_path(wg.get_id());
                s=QString("Pfad:\"%1\" ").arg(s2);
                while(lsB.count()>0)
                {
                    if(s.length()>0)
                        s+=QString(" ");
                    s2=QString("%1\"%2\"->\"%3\"").arg(lsC[0],lsB[0],lsA[0]);
                    s+=s2;
                    //-
                    lsC.removeFirst();
                    lsB.removeFirst();
                    lsA.removeFirst();
                }
                //-
                lsC.clear();
                lsA.clear();
                lsB.clear();
                dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
                lg.set_date_time(dt_ti);
                lg.set_type(LOGBOOK_TYPE_WAREGROUP_EDIT);
                if(s.length()>300)
                {
                    s.resize(297);
                    s+=QString("...");
                }
                lg.set_text(s);
            }
        }
    }
    //-
    return b;
}

bool CDbConnection::logbook_create_remove(CWaregroup & wg, CLogbook & lg)
{
    bool b=logbook_create_new(wg,lg);
    lg.set_type(LOGBOOK_TYPE_WAREGROUP_REMOVE);
    return b;
}

bool CDbConnection::logbook_create_new(CMaker & mk, CLogbook & lg)
{
    if(!m_bConnect)//db open?
        return false;

    QString s2,s;
    //-
    s2=mk.get_name();
    s=QString("\"%1\"").arg(s2);
    if(s.length()>300)
    {
        s.resize(297);
        s+=QString("...");
    }
    lg.set_text(s);
    //-
    lg.set_type(LOGBOOK_TYPE_MAKER_NEW);
    //-
    QDateTime dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
    lg.set_date_time(dt_ti);
    //-
    return true;
}

bool CDbConnection::logbook_create_edit(CMaker & mk, CLogbook & lg, bool bMark)
{
    if(!m_bConnect)//db open?
        return false;

    int i;
    bool b=true;
    QString s2,s=QString("");
    QDateTime dt_ti;
    QList<QString> lsC,lsB,lsA;//before & after & comment
    static CMaker mkMark=CMaker();
    //-
    if(bMark)//mark before setting changed
        mkMark.set(mk);
    else
    {
        if(mk.get_id()!=mkMark.get_id())//not the same ?
            b=false;
        else
        {
            //after setting changed
            if(mk.get_name()!=mkMark.get_name())
            {
                lsB.push_back(mkMark.get_name());
                lsA.push_back(mk.get_name());
                lsC.push_back("Name:");
            }
            if(mk.get_adress()!=mkMark.get_adress())
            {
                lsB.push_back(mkMark.get_adress());
                lsA.push_back(mk.get_adress());
                lsC.push_back("Adr:");
            }
            if(mk.get_callnumber()!=mkMark.get_callnumber())
            {
                lsB.push_back(mkMark.get_callnumber());
                lsA.push_back(mk.get_callnumber());
                lsC.push_back("Tel:");
            }
            if(mk.get_faxnumber()!=mkMark.get_faxnumber())
            {
                lsB.push_back(mkMark.get_faxnumber());
                lsA.push_back(mk.get_faxnumber());
                lsC.push_back("Fax:");
            }
            if(mk.get_email()!=mkMark.get_email())
            {
                lsB.push_back(mkMark.get_email());
                lsA.push_back(mk.get_email());
                lsC.push_back("Email:");
            }
            if(mk.get_contectperson()!=mkMark.get_contectperson())
            {
                lsB.push_back(mkMark.get_contectperson());
                lsA.push_back(mk.get_contectperson());
                lsC.push_back("Kontaktp:");
            }
            if(mk.get_homepage()!=mkMark.get_homepage())
            {
                lsB.push_back(mkMark.get_homepage());
                lsA.push_back(mk.get_homepage());
                lsC.push_back("hp:");
            }
            if(mk.get_comment()!=mkMark.get_comment())
            {
                lsB.push_back(mkMark.get_comment());
                lsA.push_back(mk.get_comment());
                lsC.push_back("Kom:");
            }
            //check counts
            i=lsA.count()+lsB.count()+lsC.count();
            if(i%3!=0 || i<=0)//count != ? error  or not changed
                b=false;
            else
            {
                s2=mk.get_name();
                s=QString("Name:\"%1\" ").arg(s2);
                while(lsB.count()>0)
                {
                    if(s.length()>0)
                        s+=QString(" ");
                    s2=QString("%1\"%2\"->\"%3\"").arg(lsC[0],lsB[0],lsA[0]);
                    s+=s2;
                    //-
                    lsC.removeFirst();
                    lsB.removeFirst();
                    lsA.removeFirst();
                }
                //-
                lsC.clear();
                lsA.clear();
                lsB.clear();
                dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
                lg.set_date_time(dt_ti);
                lg.set_type(LOGBOOK_TYPE_MAKER_EDIT);
                if(s.length()>300)
                {
                    s.resize(297);
                    s+=QString("...");
                }
                lg.set_text(s);
            }
        }
    }
    //-
    return b;
}

bool CDbConnection::logbook_create_remove(CMaker & mk, CLogbook & lg)
{
    bool b=logbook_create_new(mk,lg);
    lg.set_type(LOGBOOK_TYPE_MAKER_REMOVE);
    return b;
}

bool CDbConnection::logbook_create_new(CDealer & de, CLogbook & lg)
{
    if(!m_bConnect)//db open?
        return false;

    QString s2,s;
    //-
    s2=de.get_name();
    s=QString("\"%1\"").arg(s2);
    if(s.length()>300)
    {
        s.resize(297);
        s+=QString("...");
    }
    lg.set_text(s);
    //-
    lg.set_type(LOGBOOK_TYPE_DEALER_NEW);
    //-
    QDateTime dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
    lg.set_date_time(dt_ti);
    //-
    return true;
}

bool CDbConnection::logbook_create_edit(CDealer & de, CLogbook & lg, bool bMark)
{
    if(!m_bConnect)//db open?
        return false;

    int i;
    bool b=true;
    QString s2,s=QString("");
    QDateTime dt_ti;
    QList<QString> lsC,lsB,lsA;//before & after & comment
    static CDealer deMark=CDealer();
    //-
    if(bMark)//mark before setting changed
        deMark.set(de);
    else
    {
        if(de.get_id()!=deMark.get_id())//not the same ?
            b=false;
        else
        {
            //after setting changed
            if(de.get_name()!=deMark.get_name())
            {
                lsB.push_back(deMark.get_name());
                lsA.push_back(de.get_name());
                lsC.push_back("Name:");
            }
            if(de.get_customernumber()!=deMark.get_customernumber())
            {
                lsB.push_back(deMark.get_customernumber());
                lsA.push_back(de.get_customernumber());
                lsC.push_back("KnNr:");
            }
            if(de.get_adress()!=deMark.get_adress())
            {
                lsB.push_back(deMark.get_adress());
                lsA.push_back(de.get_adress());
                lsC.push_back("Adr:");
            }
            if(de.get_callnumber()!=deMark.get_callnumber())
            {
                lsB.push_back(deMark.get_callnumber());
                lsA.push_back(de.get_callnumber());
                lsC.push_back("Tel:");
            }
            if(de.get_faxnumber()!=deMark.get_faxnumber())
            {
                lsB.push_back(deMark.get_faxnumber());
                lsA.push_back(de.get_faxnumber());
                lsC.push_back("Fax:");
            }
            if(de.get_email()!=deMark.get_email())
            {
                lsB.push_back(deMark.get_email());
                lsA.push_back(de.get_email());
                lsC.push_back("Email:");
            }
            if(de.get_contectperson()!=deMark.get_contectperson())
            {
                lsB.push_back(deMark.get_contectperson());
                lsA.push_back(de.get_contectperson());
                lsC.push_back("Kontaktp:");
            }
            if(de.get_homepage()!=deMark.get_homepage())
            {
                lsB.push_back(deMark.get_homepage());
                lsA.push_back(de.get_homepage());
                lsC.push_back("hp:");
            }
            if(de.get_comment()!=deMark.get_comment())
            {
                lsB.push_back(deMark.get_comment());
                lsA.push_back(de.get_comment());
                lsC.push_back("Kom:");
            }
            //check counts
            i=lsA.count()+lsB.count()+lsC.count();
            if(i%3!=0 || i<=0)//count != ? error  or not changed
                b=false;
            else
            {
                s2=de.get_name();
                s=QString("\"%1\" ").arg(s2);
                while(lsB.count()>0)
                {
                    if(s.length()>0)
                        s+=QString(" ");
                    s2=QString("%1\"%2\"->\"%3\"").arg(lsC[0],lsB[0],lsA[0]);
                    s+=s2;
                    //-
                    lsC.removeFirst();
                    lsB.removeFirst();
                    lsA.removeFirst();
                }
                //-
                lsC.clear();
                lsA.clear();
                lsB.clear();
                dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
                lg.set_date_time(dt_ti);
                lg.set_type(LOGBOOK_TYPE_DEALER_EDIT);
                if(s.length()>300)
                {
                    s.resize(297);
                    s+=QString("...");
                }
                lg.set_text(s);
            }
        }
    }
    //-
    return b;
}

bool CDbConnection::logbook_create_remove(CDealer & de, CLogbook & lg)
{
    bool b=logbook_create_new(de,lg);
    lg.set_type(LOGBOOK_TYPE_DEALER_REMOVE);
    return b;
}

bool CDbConnection::logbook_create_new(CCustomer & cu, CLogbook & lg)
{
    if(!m_bConnect)//db open?
        return false;

    QString s2,s;
    //-
    s2=cu.get_name();
    s=QString("Name:\"%1, %2\" PLZ,Ort:\"%3, %4\" KnNr:\"%5\"").arg(cu.get_name(),cu.get_first_name(),cu.get_postcode(),cu.get_city(),cu.get_customernumber());
    if(s.length()>300)
    {
        s.resize(297);
        s+=QString("...");
    }
    lg.set_text(s);
    //-
    lg.set_type(LOGBOOK_TYPE_CUSTOMER_NEW);
    //-
    QDateTime dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
    lg.set_date_time(dt_ti);
    //-
    return true;
}

bool CDbConnection::logbook_create_edit(CCustomer & cu, CLogbook & lg, bool bMark)
{
    if(!m_bConnect)//db open?
        return false;

    int i;
    bool b=true;
    QString s2,s=QString("");
    QDateTime dt_ti;
    QList<QString> lsC,lsB,lsA;//before & after & comment
    static CCustomer cuMark=CCustomer();
    //-
    if(bMark)//mark before setting changed
        cuMark.set(cu);
    else
    {
        if(cu.get_id()!=cuMark.get_id())//not the same ?
            b=false;
        else
        {
            //after setting changed
            if(cu.get_name()!=cuMark.get_name() || cu.get_first_name()!=cuMark.get_first_name())
            {
                s2=QString("%1, %2").arg(cuMark.get_name(),cuMark.get_first_name());
                lsB.push_back(s2);
                s2=QString("%1, %2").arg(cu.get_name(),cu.get_first_name());
                lsA.push_back(s2);
                lsC.push_back("Name:");
            }
            if(cu.get_customernumber()!=cuMark.get_customernumber())
            {
                lsB.push_back(cuMark.get_customernumber());
                lsA.push_back(cu.get_customernumber());
                lsC.push_back("KnNr:");
            }

            if(cu.get_street()!=cuMark.get_street())
            {
                lsB.push_back(cuMark.get_street());
                lsA.push_back(cu.get_street());
                lsC.push_back("Str:");
            }
            if(cu.get_postcode()!=cuMark.get_postcode() || cu.get_city()!=cuMark.get_city())
            {
                s2=QString("%1, %2").arg(cuMark.get_postcode(),cuMark.get_city());
                lsB.push_back(s2);
                s2=QString("%1, %2").arg(cu.get_postcode(),cu.get_city());
                lsA.push_back(s2);
                lsC.push_back("PLZ,Ort:");
            }
            if(cu.get_callnumber()!=cuMark.get_callnumber())
            {
                lsB.push_back(cuMark.get_callnumber());
                lsA.push_back(cu.get_callnumber());
                lsC.push_back("Tel:");
            }
            if(cu.get_faxnumber()!=cuMark.get_faxnumber())
            {
                lsB.push_back(cuMark.get_faxnumber());
                lsA.push_back(cu.get_faxnumber());
                lsC.push_back("Fax:");
            }
            if(cu.get_email()!=cuMark.get_email())
            {
                lsB.push_back(cuMark.get_email());
                lsA.push_back(cu.get_email());
                lsC.push_back("Email:");
            }
            if(cu.get_comment()!=cuMark.get_comment())
            {
                lsB.push_back(cuMark.get_comment());
                lsA.push_back(cu.get_comment());
                lsC.push_back("Kom:");
            }
            //check counts
            i=lsA.count()+lsB.count()+lsC.count();
            if(i%3!=0 || i<=0)//count != ? error  or not changed
                b=false;
            else
            {
                s=QString("KnNr:\"%1\" ").arg(cu.get_customernumber());
                while(lsB.count()>0)
                {
                    if(s.length()>0)
                        s+=QString(" ");
                    s2=QString("%1\"%2\"->\"%3\"").arg(lsC[0],lsB[0],lsA[0]);
                    s+=s2;
                    //-
                    lsC.removeFirst();
                    lsB.removeFirst();
                    lsA.removeFirst();
                }
                //-
                lsC.clear();
                lsA.clear();
                lsB.clear();
                dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
                lg.set_date_time(dt_ti);
                lg.set_type(LOGBOOK_TYPE_CUSTOMER_EDIT);
                if(s.length()>300)
                {
                    s.resize(297);
                    s+=QString("...");
                }
                lg.set_text(s);
            }
        }
    }
    //-
    return b;
}

bool CDbConnection::logbook_create_remove(CCustomer & cu, CLogbook & lg)
{
    bool b=logbook_create_new(cu,lg);
    lg.set_type(LOGBOOK_TYPE_CUSTOMER_REMOVE);
    return b;
}

bool CDbConnection::logbook_create_new(CArticle & ar, CLogbook & lg)
{
    if(!m_bConnect)//db open?
        return false;

    QString sM,sP,s;
    //-
    sP=waregroup_get_path(ar.get_waregroup_id(),3);
    sM=maker_get_name(ar.get_maker_id());
    s=QString("Bez:\"%1\" Hersteller:\"%2\" Warengruppe:\"%3\"").arg(ar.get_name(),sM,sP);
    if(s.length()>300)
    {
        s.resize(297);
        s+=QString("...");
    }
    lg.set_text(s);
    //-
    lg.set_type(LOGBOOK_TYPE_ARTICLE_NEW);
    //-
    QDateTime dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
    lg.set_date_time(dt_ti);
    //-
    return true;
}

bool CDbConnection::logbook_create_edit(CArticle & ar, CLogbook & lg, bool bMark)
{
    if(!m_bConnect)//db open?
        return false;

    int i;
    bool b=true;
    QString s2,s=QString("");
    QDateTime dt_ti;
    QList<QString> lsC,lsB,lsA;//before & after & comment
    static CArticle arMark=CArticle();
    //-
    if(bMark)//mark before setting changed
        arMark.set(ar);
    else
    {
        if(ar.get_id()!=arMark.get_id())//not the same ?
            b=false;
        else
        {
            //after setting changed
            if(ar.get_name()!=arMark.get_name())
            {
                lsB.push_back(arMark.get_name());
                lsA.push_back(ar.get_name());
                lsC.push_back("Bez:");
            }
            if(ar.get_articlenumber()!=arMark.get_articlenumber())
            {
                lsB.push_back(arMark.get_articlenumber());
                lsA.push_back(ar.get_articlenumber());
                lsC.push_back("A.Nr:");
            }
            if(ar.get_articlenumber2()!=arMark.get_articlenumber2())
            {
                lsB.push_back(arMark.get_articlenumber2());
                lsA.push_back(ar.get_articlenumber2());
                lsC.push_back("A.Nr2:");
            }
            if(ar.get_maker_id()!=arMark.get_maker_id())
            {
                s2=maker_get_name(arMark.get_maker_id());
                lsB.push_back(s2);
                s2=maker_get_name(ar.get_maker_id());
                lsA.push_back(s2);
                lsC.push_back("Herst:");
            }
            if(ar.get_waregroup_id()!=arMark.get_waregroup_id())
            {
                s2=waregroup_get_path(arMark.get_waregroup_id(),3);
                lsB.push_back(s2);
                s2=waregroup_get_path(ar.get_waregroup_id(),3);
                lsA.push_back(s2);
                lsC.push_back("Warengr:");
            }
            if(ar.get_base_price()!=arMark.get_base_price())
            {
                s2=QString("%1").arg(arMark.get_base_price());
                lsB.push_back(s2);
                s2=QString("%1").arg(ar.get_base_price());
                lsA.push_back(s2);
                lsC.push_back("EK:");
            }
            if(ar.get_sales_price()!=arMark.get_sales_price())
            {
                s2=QString("%1").arg(arMark.get_sales_price());
                lsB.push_back(s2);
                s2=QString("%1").arg(ar.get_sales_price());
                lsA.push_back(s2);
                lsC.push_back("VK:");
            }
            if(ar.get_inventory()!=arMark.get_inventory())
            {
                s2=QString("%1").arg(arMark.get_inventory());
                lsB.push_back(s2);
                s2=QString("%1").arg(ar.get_inventory());
                lsA.push_back(s2);
                lsC.push_back("Bst:");
            }
            if(ar.get_max_inventory()!=arMark.get_max_inventory())
            {
                s2=QString("%1").arg(arMark.get_max_inventory());
                lsB.push_back(s2);
                s2=QString("%1").arg(ar.get_max_inventory());
                lsA.push_back(s2);
                lsC.push_back("M.Bst:");
            }
            if(ar.get_location()!=arMark.get_location())
            {
                lsB.push_back(arMark.get_location());
                lsA.push_back(ar.get_location());
                lsC.push_back("St.ort:");
            }
            if(ar.get_unit()!=arMark.get_unit())
            {
                lsB.push_back(arMark.get_unit());
                lsA.push_back(ar.get_unit());
                lsC.push_back("EH:");
            }
            if(ar.get_valuta()!=arMark.get_valuta())
            {
                lsB.push_back(arMark.get_valuta());
                lsA.push_back(ar.get_valuta());
                lsC.push_back("Währung:");
            }
            if(ar.get_comment()!=arMark.get_comment())
            {
                lsB.push_back(arMark.get_comment());
                lsA.push_back(ar.get_comment());
                lsC.push_back("Kom:");
            }
            //check counts
            i=lsA.count()+lsB.count()+lsC.count();
            if(i%3!=0 || i<=0)//count != ? error  or not changed
                b=false;
            else
            {
                s=QString("Bez:\"%1\" ").arg(ar.get_name());
                while(lsB.count()>0)
                {
                    if(s.length()>0)
                        s+=QString(" ");
                    s2=QString("%1\"%2\"->\"%3\"").arg(lsC[0],lsB[0],lsA[0]);
                    s+=s2;
                    //-
                    lsC.removeFirst();
                    lsB.removeFirst();
                    lsA.removeFirst();
                }
                //-
                lsC.clear();
                lsA.clear();
                lsB.clear();
                dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
                lg.set_date_time(dt_ti);
                lg.set_type(LOGBOOK_TYPE_ARTICLE_EDIT);
                if(s.length()>300)
                {
                    s.resize(297);
                    s+=QString("...");
                }
                lg.set_text(s);
            }
        }
    }
    //-
    return b;
}

bool CDbConnection::logbook_create_copy(QString sArticleNameSource, QString sArticleNameCopy, CLogbook & lg)
{
    QString s=QString("Kopie \"%1\" erstellt von \"%2\"").arg(sArticleNameCopy,sArticleNameSource);
    lg.set_text(s);
    lg.set_type(LOGBOOK_TYPE_ARTICLE_COPY);
    QDateTime dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
    lg.set_date_time(dt_ti);
    return true;
}

bool CDbConnection::logbook_create_remove(CArticle & ar, CLogbook & lg)
{
    bool b=logbook_create_new(ar,lg);
    lg.set_type(LOGBOOK_TYPE_ARTICLE_REMOVE);
    return b;
}

bool CDbConnection::logbook_create_new(COrdering & ord, CLogbook & lg)
{
    if(!m_bConnect)//db open?
        return false;

    QString s2,s;
    //-
    s2=dealer_get_name(ord.get_dealer_id());
    s=QString("Bestelldt:\"%1\" Bestellnr:\"%2\" Händler:\"%3\"").arg(ord.get_date().toString("dd.MM.yyyy"),ord.get_ordernumber(),s2);
    if(s.length()>300)
    {
        s.resize(297);
        s+=QString("...");
    }
    lg.set_text(s);
    //-
    lg.set_type(LOGBOOK_TYPE_ORDERING_NEW);
    //-
    QDateTime dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
    lg.set_date_time(dt_ti);
    //-
    return true;
}

bool CDbConnection::logbook_create_edit(COrdering & ord, CLogbook & lg, bool bMark)
{
    if(!m_bConnect)//db open?
        return false;

    int i;
    bool b=true;
    QList<QString> ls;
    QString s2,s=QString("");
    QDateTime dt_ti;
    QList<QString> lsC,lsB,lsA;//before & after & comment
    static COrdering ordMark=COrdering();
    //-
    if(bMark)//mark before setting changed
        ordMark.set(ord);
    else
    {
        if(ord.get_id()!=ordMark.get_id())//not the same ?
            b=false;
        else
        {
            //after setting changed
            if(ord.get_date()!=ordMark.get_date())
            {
                lsB.push_back(ordMark.get_date().toString("dd.MM.yyyy"));
                lsA.push_back(ord.get_date().toString("dd.MM.yyyy"));
                lsC.push_back("Bestelldt:");
            }
            if(ord.get_ordernumber()!=ordMark.get_ordernumber())
            {
                lsB.push_back(ordMark.get_ordernumber());
                lsA.push_back(ord.get_ordernumber());
                lsC.push_back("Bestellnr:");
            }
            if(ord.get_dealer_id()!=ordMark.get_dealer_id())
            {
                s2=dealer_get_name(ordMark.get_dealer_id());
                lsB.push_back(s2);
                s2=dealer_get_name(ord.get_dealer_id());
                lsA.push_back(s2);
                lsC.push_back("Händler:");
            }
            if(ord.get_comment()!=ordMark.get_comment())
            {
                lsB.push_back(ordMark.get_comment());
                lsA.push_back(ord.get_comment());
                lsC.push_back("Kom:");
            }
            if(ord.get_wares()!=ordMark.get_wares())
            {
                ordMark.get_wares(ls);
                s2=ordering_get_wares_list(ls,300);
                ls.clear();
                lsB.push_back(s2);
                ord.get_wares(ls);
                s2=ordering_get_wares_list(ls,300);
                lsA.push_back(s2);
                ls.clear();
                lsC.push_back("Artikel:");
            }
            //check counts
            i=lsA.count()+lsB.count()+lsC.count();
            if(i%3!=0 || i<=0)//count != ? error  or not changed
                b=false;
            else
            {
                s=QString("Bestellnr:\"%1\" ").arg(ord.get_ordernumber());
                while(lsB.count()>0)
                {
                    if(s.length()>0)
                        s+=QString(" ");
                    s2=QString("%1\"%2\"->\"%3\"").arg(lsC[0],lsB[0],lsA[0]);
                    s+=s2;
                    //-
                    lsC.removeFirst();
                    lsB.removeFirst();
                    lsA.removeFirst();
                }
                //-
                lsC.clear();
                lsA.clear();
                lsB.clear();
                dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
                lg.set_date_time(dt_ti);
                lg.set_type(LOGBOOK_TYPE_ORDERING_EDIT);
                if(s.length()>300)
                {
                    s.resize(297);
                    s+=QString("...");
                }
                lg.set_text(s);
            }
        }
    }
    //-
    return b;
}

bool CDbConnection::logbook_create_remove(COrdering & ord, CLogbook & lg)
{
    bool b=logbook_create_new(ord,lg);
    lg.set_type(LOGBOOK_TYPE_ORDERING_REMOVE);
    return b;
}

bool CDbConnection::logbook_create_new(CTrade & tr, CLogbook & lg)
{
    if(!m_bConnect)//db open?
        return false;

    QString s;
    //-
    s=QString("Buchungsnr:\"%1\"").arg(tr.get_booking_number());
    if(s.length()>300)
    {
        s.resize(297);
        s+=QString("...");
    }
    lg.set_text(s);
    //-
    if(tr.get_type()==TYPE_INCOMING)
        lg.set_type(LOGBOOK_TYPE_TRADE_IN);
    else if(tr.get_type()==TYPE_OUTGOING)
        lg.set_type(LOGBOOK_TYPE_TRADE_OUT);
    else if(tr.get_type()==TYPE_ORDERING_INCOMING)
        lg.set_type(LOGBOOK_TYPE_TRADE_ORD_IN);
    else if(tr.get_type()==TYPE_CUSTOMER_OUTGOING)
        lg.set_type(LOGBOOK_TYPE_TRADE_CUS_OUT);
    //-
    QDateTime dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
    lg.set_date_time(dt_ti);
    //-
    return true;
}

bool CDbConnection::logbook_create_canceled(CTrade & tr, CLogbook & lg)
{
    if(!m_bConnect)//db open?
        return false;

    QString s;
    //-
    s=QString("Buchungsnr:\"%1\"").arg(tr.get_booking_number());
    if(s.length()>300)
    {
        s.resize(297);
        s+=QString("...");
    }
    lg.set_text(s);
    //-
    lg.set_type(LOGBOOK_TYPE_TRADE_CANCEL);
    //-
    QDateTime dt_ti=QDateTime(QDate().currentDate(),QTime().currentTime());
    lg.set_date_time(dt_ti);
    //-
    return true;
}

