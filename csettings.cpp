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

#include "csettings.h"

//------
CSettings::CSettings()
{
    //settings path
    if(QFile::exists(QDir::homePath()+QString::fromUtf8("/lava2/")))//aplication(lava2) installed?
        m_sSettingsPath=QDir::homePath()+QString::fromUtf8("/lava2/");
    else
        m_sSettingsPath=QDir::currentPath()+QString::fromUtf8("/");//not installed
}

bool CSettings::create_default_setting_file(void)
{
    bool b=true;
    QString s,sFile=m_sSettingsPath+QString::fromUtf8("settings.txt");
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
        s=QString::fromUtf8("//MAINWINDOW\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TAB_ORDER#0,1,2,3,4,5,6,7,8\n");
        file.write(s.toLatin1());
        //-
        s=QString::fromUtf8("INVENTORY_TABLE_COLUMNS_WIDTHS#100,80,200,200,200,200,200,200,200,200,200,200,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("SORT_ORDER_INVENTORY#0,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_MAIN_INV#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_INVENTORY#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("INVENTORY_TABLE_COLUMNS_ORDER_VISIBLE_ALIGMENT#0,1,0,1,1,1,2,1,1,3,1,1,4,1,1,5,1,0,6,1,0,7,1,0,8,1,0,9,1,0,10,1,0,11,1,0,12,0,0\n");//column order, bool visible, aligment 0-left,1-right,2-center
        file.write(s.toLatin1());
        //-
        s=QString::fromUtf8("TRADE_TABLE_COLUMNS_WIDTHS_OVIEW#200,200,200,200,200,200,200\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("SORT_ORDER_TRADE_OVIEW#3,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TRADE_TABLE_COLUMNS_WIDTHS_WARES#90,170,130,130,130,130,130,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("SORT_ORDER_TRADE_WARES#1,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_MAIN_TRADE#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_TRADE#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_TRADE_PERIOD#0\n");
        file.write(s.toLatin1());
        //-
        s=QString::fromUtf8("ORDERING_TABLE_COLUMNS_WIDTHS_OVIEW#230,230,230,230,350,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("SORT_ORDER_ORDERING_OVIEW#0,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("ORDERING_TABLE_COLUMNS_WIDTHS_WARES#100,200,180,180,180,180,180,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("SORT_ORDER_ORDERING_WARES#1,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_MAIN_ORDERING#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_ORDERING#0\n");
        file.write(s.toLatin1());
        //-
        s=QString::fromUtf8("ARTICLE_TABLE_COLUMNS_WIDTHS#200,200,200,200,150,150,150,150,250,250,250,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("SORT_ORDER_ARTICLE#0,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_MAIN_ARTICLE#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_ARTICLE#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("ARTICLE_TABLE_COLUMNS_ORDER_VISIBLE_ALIGMENT#0,1,0,1,1,0,2,1,0,3,1,0,4,1,0,5,1,1,6,1,1,7,1,1,8,1,1,9,1,0,10,1,0,11,0,0\n");//column order, bool visible, aligment 0-left,1-right,2-center
        file.write(s.toLatin1());
        //-
        s=QString::fromUtf8("WAREGROUP_TREE_COLUMNS_WIDTHS#500,500,0,0\n");
        file.write(s.toLatin1());
        //-
        s=QString::fromUtf8("MAKER_TABLE_COLUMNS_WIDTHS#500,500,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("SORT_ORDER_MAKER#0,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_MAIN_MAKER#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_MAKER#0\n");
        file.write(s.toLatin1());
        //-
        s=QString::fromUtf8("DEALER_TABLE_COLUMNS_WIDTHS#500,500,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("SORT_ORDER_DEALER#0,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_MAIN_DEALER#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_DEALER#0\n");
        file.write(s.toLatin1());
        //-
        s=QString::fromUtf8("CUSTOMER_TABLE_COLUMNS_WIDTHS#150,280,280,280,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("SORT_ORDER_CUSTOMER#1,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_MAIN_CUSTOMER#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_CUSTOMER#0\n");
        file.write(s.toLatin1());
        //-
        s=QString::fromUtf8("MASK_SELECTION_LOGBOOK#0\n");
        file.write(s.toLatin1());

        //dlg trade in/out
        s=QString::fromUtf8("//DLG_TRADE\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_TRADE_TABLE_COLUMNS_WIDTHS_IN_OUT_WARES#120,200,250,200,200,200,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_TRADE_IN_OUT_WARES#1,0\n");
        file.write(s.toLatin1());

        //dlg ordering incoming
        s=QString::fromUtf8("//DLG_TRADE_ORDERING_INCOMING\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_TRADE_TABLE_COLUMNS_WIDTHS_ORD_IN_ORD#230,230,230,230,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_TRADE_ORDERING_ORD#0,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_TRADE_TABLE_COLUMNS_WIDTHS_ORD_IN_WARES#230,230,230,230,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_TRADE_ORDERING_WARES#0,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_TRADE_ORDERING_INCOMING#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_DLG_TRADE_ORDERING#0\n");
        file.write(s.toLatin1());

        //dlg customer outgoing
        s=QString::fromUtf8("//DLG_TRADE_CUSTOMER_OUTGOING\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_TRADE_TABLE_COLUMNS_WIDTHS_CUS_OUT_CUS#140,250,280,280,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_TRADE_CUSTOMER_CUS#1,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_TRADE_TABLE_COLUMNS_WIDTHS_CUS_OUT_WARES#120,200,250,200,200,200,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_TRADE_CUSTOMER_WARES#1,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_TRADE_CUSTOMER_OUTGOING#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_DLG_TRADE_CUSTOMER#0\n");
        file.write(s.toLatin1());

        //dlg ordering
        s=QString::fromUtf8("//DLG_ORDERING\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_ORDERING_TABLE_COLUMNS_WIDTHS_WARES#120,200,250,200,200,200,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_ORDERING#1,0\n");
        file.write(s.toLatin1());

        //dlg article browse
        s=QString::fromUtf8("//DLG_ARTICLE_BROWSE\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_ARTICLE_BROWSE_TABLE_COLUMNS_WIDTHS#210,210,210,210,200,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_ARTICLE_BROWSE#0,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_ARTICLE_BROWSE#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_DLG_ARTICLE_BROWSE#0\n");
        file.write(s.toLatin1());

        //dlg article allowance
        s=QString::fromUtf8("//DLG_ARTICLE_ALLOWANCE\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_ARTICLE_ALLOWANCE_TABLE_COLUMNS_WIDTHS#230,160,160,160,160,160,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_ALLOWANCE#1,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("AUTO_CHECKBOX_ARTICLE_ALLOWANCE#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("MASK_SELECTION_DLG_ALLOWANCE#0\n");
        file.write(s.toLatin1());

        //dlg browse waregroup
        s=QString::fromUtf8("//DLG_BROWSE_WAREGROUP\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_WAREGROUP_TREE#300,300,0,0\n");
        file.write(s.toLatin1());

        //dlg uni list
        s=QString::fromUtf8("//DLG_UNI_LIST\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_ARTICLE_UNDER_WARNLIMIT_TABLE_COLUMNS_WIDTHS#230,140,140,130,130,100,100\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_UNI_LIST_TYPE_WARNLIMIT#0,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_INVENTORYS_ON_DATE_TABLE_COLUMNS_WIDTHS#160,160,160,160,150,150\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_UNI_LIST_TYPE_INV_ON_DATE#1,0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_UNI_INVENTORY_LIST_TABLE_COLUMNS_WIDTHS#130,160,200,130,130,170\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("TABLE_SORT_ORDER_DLG_UNI_INVENTORY_LIST#0,0\n");
        file.write(s.toLatin1());

        //dlg balance list
        s=QString::fromUtf8("//DLG_BALANCE_LIST\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_BALANCE_LIST_TABLE_COLUMNS_WIDTHS#160,160,160,160,150,150,150,150,150\n");
        file.write(s.toLatin1());

        //list values of goods
        s=QString::fromUtf8("//DLG_LIST_VALUE_OF_GOODS\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_LIST_VALUE_OF_GOODS_TABLE_COLUMNS_WIDTHS#250,120,180,100,150,100,100,100\n");
        file.write(s.toLatin1());

        //input dlg barcode
        s=QString::fromUtf8("//DLG_BARCODE\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_BARCODE_SELECT_ARTICLENUMBER#0\n");
        file.write(s.toLatin1());

        //export csv user settings dialog
        s=QString::fromUtf8("//DLG_EXPORT_CSV_DIALOG\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_EXPORT_CSV_DIALOG_CODEC#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_EXPORT_CSV_DIALOG_SPLIT_TEXT_CHAR#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_EXPORT_CSV_DIALOG_SPLIT_CHAR#;\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_EXPORT_CSV_DIALOG_FIRST_ROW_HEADER#1\n");
        file.write(s.toLatin1());

        //import csv user setting dialog
        s=QString::fromUtf8("//DLG_IMPORT_CSV_DIALOG\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_IMPORT_CSV_DIALOG_CODEC#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_IMPORT_CSV_DIALOG_SPLIT_TEXT_CHAR#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_IMPORT_CSV_DIALOG_SPLIT_CHAR#;\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("DLG_IMPORT_CSV_DIALOG_SELECT_TYPE#0\n");
        file.write(s.toLatin1());

        //print dialog setting
        s=QString::fromUtf8("//PRINT_DIALOG\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("PRINT_DIALOG_MARGINS#15,10,10,10\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("PRINT_DIALOG_ORIENTATION#0\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("PRINT_DIALOG_PRINTER#\n");
        file.write(s.toLatin1());

        //etc.
        s=QString::fromUtf8("//ETC\n");
        file.write(s.toLatin1());

        //path's
        s=QString::fromUtf8("BACKUP_PATH#\n");
        file.write(s.toLatin1());

        s=QString::fromUtf8("EXPORT_PATH#\n");
        file.write(s.toLatin1());

        s=QString::fromUtf8("IMPORT_PATH#\n");
        file.write(s.toLatin1());

        s=QString::fromUtf8("DB_PATH#\n");
        file.write(s.toLatin1());

        s=QString::fromUtf8("PICTURE_PATH#\n");
        file.write(s.toLatin1());

        //users
        s=QString::fromUtf8("//users\n");
        file.write(s.toLatin1());
        s=QString::fromUtf8("USERS#\n");
        file.write(s.toLatin1());
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
        lsSValue.push_back(QString::fromUtf8(""));
    }
    if(lsSValue.count()!=lsSType.count())
        return false;

    bool b=false;
    int i,j,iFound=0;
    QString s,sValue, sFile=m_sSettingsPath+QString::fromUtf8("settings.txt");
    QStringList lsString;
    //-

    QFile file(sFile);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            file.close();
    else
    {
        while (!file.atEnd())
        {
            s=file.readLine();
            lsString=s.split("#");
            if(lsString.count()>1)
            {
                //search
                for(i=0;i<lsSType.count();i++)
                {
                    if(lsSType[i]==lsString[0])//found
                    {
                        sValue=QString::fromUtf8("");
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
        file.close();
    }
    //-
    if(iFound>0)//one find?
        b=true;
    return b;
}

bool CSettings::load(QString sType, QString & sValue)
{
    QList<QString> lsSType;
    lsSType.push_back(sType);
    QList<QString> lsSValue;
    lsSValue.push_back(QString::fromUtf8(""));
    bool b=load(lsSType,lsSValue);
    if(lsSValue.count()> 0)
        sValue=lsSValue[0];
    lsSType.clear();
    lsSValue.clear();
    return b;
}

bool CSettings::write(QList<QString> & lsSType, QList<QString> & lsSValue)
{
    int i;
    bool b=true;
    QString s,sFile=m_sSettingsPath+QString::fromUtf8("settings.txt"),sTempFile=m_sSettingsPath+QString::fromUtf8("settings_temp.txt");
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
                                s=QString::fromUtf8("%1#%2\n").arg(lsSType[i],lsSValue[i]);
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
                        s=QString::fromUtf8("%1#%2\n").arg(lsSType[i],lsSValue[i]);
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

bool CSettings::set_table_columns_width(QTableWidget * pTable, QString sValues, int iDefaultValue)
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

bool CSettings::set_table_columns_order_visible_alignment(CTableColumnsData * pColumnsData, QString sValues)
{
    if(pColumnsData==NULL)
        return false;
    if(pColumnsData->get_table()==NULL)
        return false;
    //-
    int i,j;
    bool b=false;
    QTableWidgetItem * pItem=NULL;
    QTableWidget * pTable=pColumnsData->get_table();
    QList<int> lsIntMissing,lsIntFound,lsValues,lsOrder,lsAlignment;
    QList<bool> lsVisible;
    QList<QString> lsS;

    if(sValues.length()<=0)//setting missing?
    {//default
        for(i=0;i<pTable->columnCount();i++)
        {
            lsOrder.push_back(i);
            lsAlignment.push_back(TABLE_ALIGNMENT_LEFT);
            if(i<pTable->columnCount()-1)
                lsVisible.push_back(true);
            else
                lsVisible.push_back(false); //last column(id column) default not visible
        }
        pColumnsData->set(lsOrder,lsVisible,lsAlignment);
    }
    else
    {
        //mark columns header name
        for(i=0;i<pTable->columnCount();i++)
        {
            pItem=pTable->horizontalHeaderItem(i);
            if(pItem!=NULL)
                lsS.push_back(pItem->text());
        }

        b=cast_string_to_int_list(sValues,lsValues);
        if(b)
        {//check

            //split values
            for(i=0;i<lsValues.count();i++)
            {
                if(i%3==0)//order
                    lsOrder.push_back(lsValues[i]);
                else if(i%3==1)//visible
                {
                    if(lsValues[i]==0)
                        lsVisible.push_back(false);
                    else
                        lsVisible.push_back(true);
                }
                else//alignment
                {
                    if(lsValues[i]>=0 && lsValues[i]<=2)//0=left 1=right 2=center
                        lsAlignment.push_back(lsValues[i]);
                    else
                        lsAlignment.push_back(0);//push back default
                }
            }

            //missing values?
            while(pTable->columnCount()>lsOrder.count())
                lsOrder.push_back(0);//push back default
            while(pTable->columnCount()>lsVisible.count())
                lsVisible.push_back(true);//push back default
            while(pTable->columnCount()>lsAlignment.count())
                lsAlignment.push_back(0);//push back default(0=left)

            //to many?
            while(lsOrder.count()>pTable->columnCount())
                lsOrder.removeLast();
            while(lsVisible.count()>pTable->columnCount())
                lsVisible.removeLast();
            while(lsAlignment.count()>pTable->columnCount())
                lsAlignment.removeLast();


            //---order---
            //for mark
            for(i=0;i<lsOrder.count();i++)
                lsIntFound.push_back(-1);

            //check index missing?
            for(i=0;i<lsOrder.count();i++)
            {
                for(j=0;j<lsOrder.count();j++)
                {
                    if(lsOrder[j]==i)//index found
                    {
                        lsIntFound[j]=i;
                        break;
                    }
                }
                //-
                if(j>=lsOrder.count())//index not found?
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
            lsOrder=lsIntFound;
            //---


            //set columns header name by order & visible
            for(i=0;i<pTable->columnCount();i++)
            {
                pItem=pTable->horizontalHeaderItem(i);
                if(pItem!=NULL)
                    pItem->setText(lsS[lsOrder[i]]);
                //-
                if(!lsVisible[i])//hide?
                    pTable->setColumnWidth(i,0);
            }

            //set
            pColumnsData->set(lsOrder,lsVisible,lsAlignment);
        }
    }
    //-
    lsS.clear();
    lsOrder.clear();
    lsVisible.clear();
    lsIntMissing.clear();
    lsIntFound.clear();
    lsValues.clear();
    lsAlignment.clear();
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

bool CSettings::get_table_columns_width(QTableWidget * pTable, QString & sValues)
{
    if(pTable==NULL)
        return false;

    int i;
    bool bUpdate=false;
    QString sNew;

    //get
    for(i=0;i<pTable->columnCount();i++)
    {
        sNew+=QString::fromUtf8("%1").arg(pTable->columnWidth(i));
        if(i!=pTable->columnCount()-1)//not last column
            sNew+=QString::fromUtf8(",");
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

    sNew+=QString::fromUtf8("%1").arg(pTable->horizontalHeader()->sortIndicatorSection());
    sNew+=QString::fromUtf8(",%1").arg(pTable->horizontalHeader()->sortIndicatorOrder());

    //check update?
    if(sValues!=sNew)
    {
        bUpdate=true;
        sValues=sNew;
    }
    //-
    return bUpdate;
}

bool CSettings::get_table_columns_order_visible_alignment(CTableColumnsData * pColumnsData, QString & sValues)
{
    if(pColumnsData==NULL)
        return false;

    QList<int> lsOrder,lsAlignment;
    QList<bool> lsVisible;
    bool bUpdate=false;
    QString sNew;

    pColumnsData->get_columns_order(lsOrder);
    pColumnsData->get_columns_visible(lsVisible);
    pColumnsData->get_columns_alignment(lsAlignment);
    sNew=QString::fromUtf8("");
    while(lsOrder.count()>0 && lsVisible.count()>0 && lsAlignment.count()>0)
    {
        if(sNew.length()>0)
            sNew+=QString::fromUtf8(",");
        sNew+=QString::fromUtf8("%1,").arg(lsOrder[0]);
        sNew+=QString::fromUtf8("%1,").arg(lsVisible[0]);
        sNew+=QString::fromUtf8("%1").arg(lsAlignment[0]);
        lsOrder.removeFirst();
        lsVisible.removeFirst();
        lsAlignment.removeFirst();
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

bool CSettings::get_checkbox(QCheckBox * pCheckBox,QString & sValue)
{
    if(pCheckBox==NULL)
        return false;

    bool bUpdate=false;
    QString sNew;

    if(pCheckBox->isChecked())
        sNew=QString::fromUtf8("1");
    else
        sNew=QString::fromUtf8("0");

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
    sNew=QString::fromUtf8("%1").arg(pComboBox->currentIndex());
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
        sNew+=QString::fromUtf8("%1").arg(pTree->columnWidth(i));
        if(i!=pTree->columnCount()-1)//not last column
            sNew+=QString::fromUtf8(",");
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
                sNew+=QString::fromUtf8("%1").arg(j);
        }
        //-
        if(i<pTab->count()-1)//not the last?
            sNew+=QString::fromUtf8(",");
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
    QString s,sFile=m_sSettingsPath+QString::fromUtf8("settings.txt"),sTempFile=m_sSettingsPath+QString::fromUtf8("settings_temp.txt");
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
                    file_temp.write(s.toLatin1()); //write in temp
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
            s=QString::fromUtf8("");
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
        sReturn=QString::fromUtf8("%1#").arg(sType);
    //-
    for(int i=0;i<lsInt.count();i++)
    {
        sReturn+=QString::fromUtf8("%1").arg(lsInt[i]);
        if(i+1<lsInt.count())
            sReturn+=QString::fromUtf8(",");
    }
    return sReturn;
}

bool CSettings::cast_table_columns_order_visible_alignment(QString & sValues, CTableColumnsData * p_tcdData)
{
    if(p_tcdData==NULL || sValues.length()<=0)
        return false;

    QTableWidget * pTable=p_tcdData->get_table();
    if(pTable==NULL)
        return false;
    //-
    int i,j;
    QList<int> lsIntMissing,lsIntFound,lsValues,lsOrder,lsAlignment;
    QList<bool> lsVisible;

    bool b=cast_string_to_int_list(sValues,lsValues);
    if(b)
    {//check

        //split values
        for(i=0;i<lsValues.count();i++)
        {
            if(i%3==0)//order
                lsOrder.push_back(lsValues[i]);
            else if(i%3==1)//visible
            {
                if(lsValues[i]==0)
                    lsVisible.push_back(false);
                else
                    lsVisible.push_back(true);
            }
            else//alignment
            {
                if(lsValues[i]>=0 && lsValues[i]<=2)//0=left 1=right 2=center
                    lsAlignment.push_back(lsValues[i]);
                else
                    lsAlignment.push_back(0);//push back default
            }
        }

        //missing values?
        while(pTable->columnCount()>lsOrder.count())
            lsOrder.push_back(0);//push back default
        while(pTable->columnCount()>lsVisible.count())
            lsVisible.push_back(true);//push back default
        while(pTable->columnCount()>lsAlignment.count())
            lsAlignment.push_back(0);//push back default(0=left)

        //to many?
        while(lsOrder.count()>pTable->columnCount())
            lsOrder.removeLast();
        while(lsVisible.count()>pTable->columnCount())
            lsVisible.removeLast();
        while(lsAlignment.count()>pTable->columnCount())
            lsAlignment.removeLast();


        //---order---
        //for mark
        for(i=0;i<lsOrder.count();i++)
            lsIntFound.push_back(-1);

        //check index missing?
        for(i=0;i<lsOrder.count();i++)
        {
            for(j=0;j<lsOrder.count();j++)
            {
                if(lsOrder[j]==i)//index found
                {
                    lsIntFound[j]=i;
                    break;
                }
            }
            //-
            if(j>=lsOrder.count())//index not found?
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
        lsOrder=lsIntFound;

        //set
        p_tcdData->set(lsOrder,lsVisible,lsAlignment);
    }
    //-
    lsOrder.clear();
    lsVisible.clear();
    lsIntMissing.clear();
    lsIntFound.clear();
    lsValues.clear();
    lsAlignment.clear();
    return b;
}

bool CSettings::give_it_setting_file(void)
{
    QString sFile=m_sSettingsPath+QString::fromUtf8("settings.txt");
    bool b=QFile::exists(sFile);
    return b;
}

bool CSettings::is_program_running(void)
{
    bool b=false;
    QString s;
    if(load(QString::fromUtf8("PROGRAM_RUNNING"),s))
        b=true;
    return b;
}
