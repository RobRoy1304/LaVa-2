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

#include "csettings.h"

//------
CSettings::CSettings()
{
    //settings path
    if(QFile::exists(QDir::homePath()+QString("/lava2/")))//aplication(lava2) installed?
        m_sSettingsPath=QDir::homePath()+QString("/lava2/");
    else
        m_sSettingsPath=QDir::currentPath()+QString("/");//not installed
}

bool CSettings::create_default_setting_file(void)
{
    bool b=true;
    QString s,sFile=m_sSettingsPath+QString("settings.txt");
    //-
    QFile file(sFile);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        b=false;
        file.close();
    }
    else
    {
        //main
        s=QString("###MAINWINDOW###\n");
        file.write(s.toAscii());
        s=QString("TAB_ORDER#0,1,2,3,4,5,6,7,8\n");
        file.write(s.toAscii());
        //-
        s=QString("INVENTORY_TABLE_COLUMNS_WIDTHS#100,80,200,200,200,200,200,200,200,200,200,0\n");
        file.write(s.toAscii());
        s=QString("SORT_ORDER_INVENTORY#0,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_MAIN_INV#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_INVENTORY#0\n");
        file.write(s.toAscii());
        //-
        s=QString("TRADE_TABLE_COLUMNS_WIDTHS_OVIEW#200,200,200,200,200,200,200\n");
        file.write(s.toAscii());
        s=QString("SORT_ORDER_TRADE_OVIEW#3,0\n");
        file.write(s.toAscii());
        s=QString("TRADE_TABLE_COLUMNS_WIDTHS_WARES#90,170,130,130,130,130\n");
        file.write(s.toAscii());
        s=QString("SORT_ORDER_TRADE_WARES#1,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_MAIN_TRADE#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_TRADE#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_TRADE_PERIOD#0\n");
        file.write(s.toAscii());
        //-
        s=QString("ORDERING_TABLE_COLUMNS_WIDTHS_OVIEW#230,230,230,230,350,0\n");
        file.write(s.toAscii());
        s=QString("SORT_ORDER_ORDERING_OVIEW#0,0\n");
        file.write(s.toAscii());
        s=QString("ORDERING_TABLE_COLUMNS_WIDTHS_WARES#100,200,180,180,180,180\n");
        file.write(s.toAscii());
        s=QString("SORT_ORDER_ORDERING_WARES#1,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_MAIN_ORDERING#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_ORDERING#0\n");
        file.write(s.toAscii());
        //-
        s=QString("ARTICLE_TABLE_COLUMNS_WIDTHS#200,200,200,200,150,150,150,150,250,250,250,0\n");
        file.write(s.toAscii());
        s=QString("SORT_ORDER_ARTICLE#0,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_MAIN_ARTICLE#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_ARTICLE#0\n");
        file.write(s.toAscii());
        //-
        s=QString("WAREGROUP_TREE_COLUMNS_WIDTHS#500,500,0,0\n");
        file.write(s.toAscii());
        //-
        s=QString("MAKER_TABLE_COLUMNS_WIDTHS#500,500,0\n");
        file.write(s.toAscii());
        s=QString("SORT_ORDER_MAKER#0,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_MAIN_MAKER#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_MAKER#0\n");
        file.write(s.toAscii());
        //-
        s=QString("DEALER_TABLE_COLUMNS_WIDTHS#500,500,0\n");
        file.write(s.toAscii());
        s=QString("SORT_ORDER_DEALER#0,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_MAIN_DEALER#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_DEALER#0\n");
        file.write(s.toAscii());
        //-
        s=QString("CUSTOMER_TABLE_COLUMNS_WIDTHS#150,280,280,280,0\n");
        file.write(s.toAscii());
        s=QString("SORT_ORDER_CUSTOMER#1,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_MAIN_CUSTOMER#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_CUSTOMER#0\n");
        file.write(s.toAscii());
        //-
        s=QString("MASK_SELECTION_LOGBOOK#0\n");
        file.write(s.toAscii());
        s=QString("LOGBOOK_COUNT#100\n");
        file.write(s.toAscii());

        //dlg trade in/out
        s=QString("###DLG_TRADE###\n");
        file.write(s.toAscii());
        s=QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_IN_OUT_WARES#120,200,250,200,200,0\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_TRADE_IN_OUT_WARES#1,0\n");
        file.write(s.toAscii());

        //dlg ordering incoming
        s=QString("###DLG_TRADE_ORDERING_INCOMING###\n");
        file.write(s.toAscii());
        s=QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_ORD_IN_ORD#230,230,230,230,0\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_TRADE_ORDERING_ORD#0,0\n");
        file.write(s.toAscii());
        s=QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_ORD_IN_WARES#230,230,230,230,0\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_TRADE_ORDERING_WARES#0,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_TRADE_ORDERING_INCOMING#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_DLG_TRADE_ORDERING#0\n");
        file.write(s.toAscii());

        //dlg customer outgoing
        s=QString("###DLG_TRADE_CUSTOMER_OUTGOING###\n");
        file.write(s.toAscii());
        s=QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_CUS_OUT_CUS#140,250,280,280,0\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_TRADE_CUSTOMER_CUS#1,0\n");
        file.write(s.toAscii());
        s=QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_CUS_OUT_WARES#120,200,250,200,200,0\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_TRADE_CUSTOMER_WARES#1,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_TRADE_CUSTOMER_OUTGOING#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_DLG_TRADE_CUSTOMER#0\n");
        file.write(s.toAscii());

        //dlg ordering
        s=QString("###DLG_ORDERING###\n");
        file.write(s.toAscii());
        s=QString("DLG_ORDERING_TABLE_COLUMNS_WIDTHS_WARES#120,200,250,200,200,0\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_ORDERING#1,0\n");
        file.write(s.toAscii());

        //dlg article browse
        s=QString("###DLG_ARTICLE_BROWSE###\n");
        file.write(s.toAscii());
        s=QString("DLG_ARTICLE_BROWSE_TABLE_COLUMNS_WIDTHS#210,210,210,210,0\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_ARTICLE_BROWSE#0,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_ARTICLE_BROWSE#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_DLG_ARTICLE_BROWSE#0\n");
        file.write(s.toAscii());

        //dlg article allowance
        s=QString("###DLG_ARTICLE_ALLOWANCE###\n");
        file.write(s.toAscii());
        s=QString("DLG_ARTICLE_ALLOWANCE_TABLE_COLUMNS_WIDTHS#230,160,160,160,160,0\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_ALLOWANCE#1,0\n");
        file.write(s.toAscii());
        s=QString("AUTO_CHECKBOX_ARTICLE_ALLOWANCE#0\n");
        file.write(s.toAscii());
        s=QString("MASK_SELECTION_DLG_ALLOWANCE#0\n");
        file.write(s.toAscii());

        //dlg browse waregroup
        s=QString("###DLG_BROWSE_WAREGROUP###\n");
        file.write(s.toAscii());
        s=QString("DLG_WAREGROUP_TREE#300,300,0,0\n");
        file.write(s.toAscii());

        //dlg uni list
        s=QString("###DLG_UNI_LIST###\n");
        file.write(s.toAscii());
        s=QString("DLG_ARTICLE_UNDER_WARNLIMIT_TABLE_COLUMNS_WIDTHS#230,140,140,130,130,100,100\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_UNI_LIST_TYPE_WARNLIMIT#0,0\n");
        file.write(s.toAscii());
        s=QString("DLG_INVENTORYS_ON_DATE_TABLE_COLUMNS_WIDTHS#160,160,160,160,150,150\n");
        file.write(s.toAscii());
        s=QString("TABLE_SORT_ORDER_DLG_UNI_LIST_TYPE_INV_ON_DATE#1,0\n");
        file.write(s.toAscii());

        //dlg balance list
        s=QString("###DLG_BALANCE_LIST###\n");
        file.write(s.toAscii());
        s=QString("DLG_BALANCE_LIST_TABLE_COLUMNS_WIDTHS#160,160,160,160,150,150\n");
        file.write(s.toAscii());

        //end
        s=QString("###END###\n");
        file.write(s.toAscii());

        //-
        file.close();
    }
    //-
    return b;
}

bool CSettings::load(QList<QString> & lsSType, QList<QString> & lsSValue)
{
    if(lsSType.count()<=0)
        return false;

    //add empty
    lsSValue.clear();
    while(lsSValue.count()<lsSType.count())
    {
        lsSValue.push_back(QString(""));
    }
    if(lsSValue.count()!=lsSType.count())
        return false;

    bool b=false;
    int i,j,iFound=0;
    QString s,sValue, sFile=m_sSettingsPath+QString("settings.txt");
    QStringList lsString;
    //-
    QFile file(sFile);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        file.close();
    else
    {
        while (!file.atEnd())
        {
            s=QString(file.readLine());
            lsString=s.split("#");
            if(lsString.count()>1)
            {
                //search
                for(i=0;i<lsSType.count();i++)
                {
                    if(lsSType[i]==lsString[0])//found
                    {
                        sValue=QString("");
                        for(j=1;j<lsString.count();j++)
                            sValue+=lsString[j];
                        lsString.clear();
                        sValue.remove(QChar('\n'),Qt::CaseSensitive);
                        lsSValue[i]=sValue;
                        iFound++;
                        break;
                    }
                }
            }
        }
        //-
        file.close();
    }
    //-
    if(iFound==lsSType.count())
        b=true;
    return b;
}

bool CSettings::load(QString sType, QString & sValue)
{
    bool bFound=false;
    int i;
    QString s,sFile=m_sSettingsPath+QString("settings.txt");
    QStringList lsString;
    //-
    QFile file(sFile);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        file.close();
    else
    {
        while (!file.atEnd())
        {
            s=QString(file.readLine());
            lsString=s.split("#");
            if(lsString.count()>1)
            {
                if(lsString[0]==sType)//found
                {
                    for(i=1;i<lsString.count();i++)
                        sValue+=lsString[i];
                    lsString.clear();
                    sValue.remove(QChar('\n'),Qt::CaseSensitive);
                    bFound=true;
                }
            }
            if(bFound)
                break;
        }
        //-
        file.close();
    }
    //-
    return bFound;
}

bool CSettings::write(QList<QString> & lsSType, QList<QString> & lsSValue)
{
    int i;
    bool b=true;
    QString s,sFile=m_sSettingsPath+QString("settings.txt"),sTempFile=m_sSettingsPath+QString("settings_temp.txt");
    QStringList lsS;
    QFile file(sFile);
    QFile file_temp(sTempFile);
    //-
    if(lsSType.count()!=lsSValue.count()) // counts of 2 lists = ?
        b=false;
    else
    {
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            b=false;
            file.close();
        }
        else
        {
            if(!file_temp.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                b=false;
                file_temp.close();
                file.close();
            }
            else
            {
                while(!file.atEnd())
                {
                    //read
                    s=QString(file.readLine());
                    lsS=s.split("#");
                    if(lsS.count()>1)
                    {
                        //search
                        for(i=0;i<lsSType.count();i++)
                        {
                            if(lsSType[i]==lsS[0])//found
                            {
                                s=QString("%1#%2\n").arg(lsSType[i],lsSValue[i]);
                                lsSType.removeAt(i);//remove
                                lsSValue.removeAt(i);
                                break;
                            }
                        }
                    }
                    lsS.clear();
                    //write
                    file_temp.write(s.toAscii()); //write in temp
                }
                //-
                if(lsSType.count()>0)//not all found->new lines?
                {
                    for(i=0;i<lsSType.count();i++)
                    {
                        s=QString("%1#%2\n").arg(lsSType[i],lsSValue[i]);
                        file_temp.write(s.toAscii()); //write in temp
                    }
                    lsSType.clear();
                    lsSValue.clear();
                }
                //-
                file.close();
                file_temp.close();
                QFile::remove(sFile);//delete old
                QFile::rename(sTempFile,sFile); // rename temp
            }
        }
    }
    //-
    return b;
}

bool CSettings::write(QString sType,QString sValue)
{
    QList<QString> lsSType;
    lsSType.push_back(sType);
    QList<QString> lsSValue;
    lsSValue.push_back(sValue);
    bool b=write(lsSType,lsSValue);
    lsSType.clear();
    lsSValue.clear();
    return b;
}

bool CSettings::set_table_width(QTableWidget * pTable, QString sValues, int iDefaultValue)
{
    if(pTable==NULL || sValues.length()<=0)
        return false;

    int i;
    QList<int> lsValues;
    bool b=cast_string_to_int_list(sValues,lsValues);
    if(b)
    {
        //check values
        for(i=0;i<lsValues.count();i++)
        {
            if(lsValues[i]<0)
                lsValues[i]=iDefaultValue;
            if(lsValues[i]>5000)
                lsValues[i]=iDefaultValue;
        }
        //missing values?
        for(;i<pTable->columnCount();i++)
            lsValues.push_back(iDefaultValue);//push back default

        //to many values?
        while(lsValues.count()>pTable->columnCount())
            lsValues.removeLast();

        //set
        for(i=0;i<pTable->columnCount();i++)
            pTable->setColumnWidth(i,lsValues[i]);
        b=true;
    }
    //-
    lsValues.clear();
    return b;
}

bool CSettings::set_table_sort(QTableWidget * pTable, QString sValues, int iDefaultValue)
{
    if(pTable==NULL || sValues.length()<=0)
        return false;

    int i=0;
    QList<int> lsValues;
    bool b=cast_string_to_int_list(sValues,lsValues);
    if(b && lsValues.count()>0)
    {
        //check
        if(lsValues[0]<0 || lsValues[0]>=pTable->columnCount())//column index not in range?
            lsValues[0]=iDefaultValue;
        if(lsValues.count()>1)
            i=lsValues[1];
        if(i<0||i>1)
            i=0;
        //set sort order
        if(i==0)
            pTable->horizontalHeader()->setSortIndicator(lsValues[0],Qt::AscendingOrder);
        if(i==1)
            pTable->horizontalHeader()->setSortIndicator(lsValues[0],Qt::DescendingOrder);
        b=true;
    }
    lsValues.clear();
    return b;
}

bool CSettings::set_checkbox(QCheckBox * pCheckBox,QString sValue, bool bDefaultValue)
{
    if(pCheckBox==NULL || sValue.length()<=0)
        return false;

    bool bSet=bDefaultValue;

    //check
    if(sValue[0]=='1')
       bSet=true;
    else if(sValue[0]=='0')
       bSet=false;

    //set
    pCheckBox->setChecked(bSet);
    return true;
}

bool CSettings::set_combobox(QComboBox * pComboBox, QString sValue, int iDefaultValue)
{
    if(pComboBox==NULL || sValue.length()<=0)
        return false;

    bool b;
    int iValue;

    iValue=sValue.toInt(&b,10);
    if(b)//cast ok?
    {
        if(iValue<0 || iValue>=pComboBox->count())
            iValue=iDefaultValue;

        //set
        pComboBox->setCurrentIndex(iValue);
    }
    return b;
}

bool CSettings::set_tree_width(QTreeWidget * pTree,QString sValues, int iDefaultValue)
{
    if(pTree==NULL || sValues.length()<=0)
        return false;

    int i;
    QList<int> lsValues;
    bool b=cast_string_to_int_list(sValues,lsValues);
    if(b)
    {
        //check values
        for(i=0;i<lsValues.count();i++)
        {
            if(lsValues[i]<0)
                lsValues[i]=iDefaultValue;
            if(lsValues[i]>5000)
                lsValues[i]=iDefaultValue;
        }
        //missing values?
        for(;i<pTree->columnCount();i++)
            lsValues.push_back(iDefaultValue);//push back default

        //to many values?
        while(lsValues.count()>pTree->columnCount())
            lsValues.removeLast();

        //set
        for(i=0;i<pTree->columnCount();i++)
            pTree->setColumnWidth(i,lsValues[i]);
        b=true;
    }
    //-
    lsValues.clear();
    return b;
}

bool CSettings::set_tab_order(QTabWidget * pTab, QString sValues)
{
    if(pTab==NULL || sValues.length()<=0)
        return false;
    if(pTab->count()<=0)
        return false;
    //-
    int i,j;
    QList<int> lsIntMissing,lsIntFound,lsValues;
    QList<QWidget*> lsW;
    QList<QString> lsS;

    //mark
    for(i=0;i<pTab->count();i++)
    {
        lsS.push_back(pTab->tabText(i));
        lsW.push_back(pTab->widget(i));
    }

    bool b=cast_string_to_int_list(sValues,lsValues);
    if(b)
    {//check

        //missing values?
        while(pTab->count()>lsValues.count())
            lsValues.push_back(0);//push back default

        //to many?
        while(lsValues.count()>pTab->count())
            lsValues.removeLast();

        //for mark
        for(i=0;i<lsValues.count();i++)
            lsIntFound.push_back(-1);

        //check index missing?
        for(i=0;i<lsValues.count();i++)
        {
            for(j=0;j<lsValues.count();j++)
            {
                if(lsValues[j]==i)//index found
                {
                    lsIntFound[j]=i;
                    break;
                }
            }
            //-
            if(j>=lsValues.count())//index not found?
                lsIntMissing.push_back(i);
        }

        //set missing index
        for(i=0;i<lsIntFound.count() && lsIntMissing.count()>0;i++)
        {
            if(lsIntFound[i]==-1)
            {
                lsIntFound[i]=lsIntMissing[0];
                lsIntMissing.removeFirst();
            }
        }

        //copy mark
        lsValues=lsIntFound;

        //set
        pTab->clear();
        for(i=0;i<lsValues.count();i++)
            pTab->addTab(lsW[lsValues[i]],lsS[lsValues[i]]);
    }
    //-
    lsS.clear();
    lsW.clear();
    lsIntMissing.clear();
    lsIntFound.clear();
    lsValues.clear();
    return b;
}

bool CSettings::get_table_width(QTableWidget * pTable, QString & sValues)
{
    if(pTable==NULL)
        return false;

    int i;
    bool bUpdate=false;
    QString sNew;

    //get
    for(i=0;i<pTable->columnCount();i++)
    {
        sNew+=QString("%1").arg(pTable->columnWidth(i));
        if(i!=pTable->columnCount()-1)//not last column
            sNew+=QString(",");
    }

    //check update?
    if(sValues!=sNew)
    {
        bUpdate=true;
        sValues=sNew;
    }
    //-
    return bUpdate;
}

bool CSettings::get_table_sort(QTableWidget * pTable, QString & sValues)
{
    if(pTable==NULL)
        return false;

    bool bUpdate=false;
    QString sNew;

    sNew+=QString("%1").arg(pTable->horizontalHeader()->sortIndicatorSection());
    sNew+=QString(",%1").arg(pTable->horizontalHeader()->sortIndicatorOrder());

    //check update?
    if(sValues!=sNew)
    {
        bUpdate=true;
        sValues=sNew;
    }
    //-
    return bUpdate;
}

bool CSettings::get_checkbox(QCheckBox * pCheckBox,QString & sValue)
{
    if(pCheckBox==NULL)
        return false;

    bool bUpdate=false;
    QString sNew;

    if(pCheckBox->isChecked())
        sNew=QString("1");
    else
        sNew=QString("0");

    //check update?
    if(sValue!=sNew)
    {
        bUpdate=true;
        sValue=sNew;
    }
    //-
    return bUpdate;
}

bool CSettings::get_combobox(QComboBox * pComboBox, QString  & sValue)
{
    if(pComboBox==NULL)
        return false;
    if(pComboBox->count()<=0)
        return false;

    QString sNew;
    bool bUpdate=false;

    //check update?
    sNew=QString("%1").arg(pComboBox->currentIndex());
    if(sValue!=sNew)
    {
        bUpdate=true;
        sValue=sNew;
    }
    //-
    return bUpdate;
}

bool CSettings::get_tree_width(QTreeWidget * pTree, QString & sValues)
{
    if(pTree==NULL)
        return false;

    int i;
    bool bUpdate=false;
    QString sNew;

    //get
    for(i=0;i<pTree->columnCount();i++)
    {
        sNew+=QString("%1").arg(pTree->columnWidth(i));
        if(i!=pTree->columnCount()-1)//not last column
            sNew+=QString(",");
    }

    //check update?
    if(sValues!=sNew)
    {
        bUpdate=true;
        sValues=sNew;
    }
    //-
    return bUpdate;
}

bool CSettings::get_tab_order(QTabWidget * pTab, QString & sValues,QList<QWidget*> & lsPWidget)
{
    if(pTab==NULL || lsPWidget.count()<=0)
        return false;

    int i,j;
    bool bUpdate=false;
    QString sNew;

    //get order
    for(i=0;i<pTab->count();i++)
    {
        for(j=0;j<lsPWidget.count();j++)
        {
            if(pTab->indexOf(lsPWidget[j])==i)//search widget index
                sNew+=QString("%1").arg(j);
        }
        //-
        if(i<pTab->count()-1)//not the last?
            sNew+=QString(",");
    }

    if(sNew!=sValues)//update?
    {
        sValues=sNew;
        bUpdate=true;
    }
    //-
    return bUpdate;
}

bool CSettings::remove_line(QString sType)
{
    bool b=true,bWriteLine;
    QString s,sFile=m_sSettingsPath+QString("settings.txt"),sTempFile=m_sSettingsPath+QString("settings_temp.txt");
    QStringList lsS;
    QFile file(sFile);
    QFile file_temp(sTempFile);
    //-
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        b=false;
        file.close();
    }
    else
    {
        if(!file_temp.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            b=false;
            file_temp.close();
            file.close();
        }
        else
        {
            while(!file.atEnd())
            {
                bWriteLine=true;

                //read
                s=QString(file.readLine());
                lsS=s.split("#");
                if(lsS.count()>1)
                {
                    //search
                    if(lsS[0]==sType)//found
                        bWriteLine=false;
                }
                lsS.clear();
                //write
                if(bWriteLine)
                    file_temp.write(s.toAscii()); //write in temp
            }
            //-
            file.close();
            file_temp.close();
            QFile::remove(sFile);//delete old
            QFile::rename(sTempFile,sFile); // rename temp
        }
    }
    //-
    return b;
}

bool CSettings::cast_string_to_int_list(QString s, QList<int> & lsInt, bool bWithType)
{
    int i,j;
    bool b=false;
    QStringList lsString;
    //-
    if(bWithType)//split type?
    {
        lsString=s.split("#");
        if(lsString.count()>1)
            s=lsString[1];
        else
            s=QString("");
        lsString.clear();
    }
    //-
    if(s.length()>0)
    {
        lsString=s.split(",");
        for(i=0;i<lsString.count();i++)
        {
            j=lsString[i].toInt(&b,10);
            if(b)// cast to int ok?
                lsInt.push_back(j);
        }
        lsString.clear();
    }
    return b;
}

QString CSettings::cast_int_list_to_string(QList<int> & lsInt, QString sType)
{
    QString sReturn;
    //-
    if(sType.length()>0)//with type?
        sReturn=QString("%1#").arg(sType);
    //-
    for(int i=0;i<lsInt.count();i++)
    {
        sReturn+=QString("%1").arg(lsInt[i]);
        if(i+1<lsInt.count())
            sReturn+=QString(",");
    }
    return sReturn;
}

bool CSettings::give_it_setting_file(void)
{
    QString sFile=m_sSettingsPath+QString("settings.txt");
    bool b=QFile::exists(sFile);
    return b;
}

bool CSettings::is_program_running(void)
{
    bool b=false;
    QString s;
    if(load(QString("PROGRAM_RUNNING"),s))
        b=true;
    return b;
}
