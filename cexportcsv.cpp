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

#include "cexportcsv.h"

CExportCSV::CExportCSV()
{
    m_iCodec=0;//UTF-8 is default
}

bool CExportCSV::write(QString sFile, QList<QString> & ls)
{
    if(ls.count()<=0)
        return false;

    bool b=false;
    QFile file(sFile);
    //-
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        file.close();
    else
    {
        //codec
        QTextStream stream(&file);
        if(m_iCodec==0)//utf-8 selected
            stream.setCodec("UTF-8");
        else if(m_iCodec==1)//windows 1252 selected
            stream.setCodec("Windows-1252");

        //write
        while(ls.count()>0)
        {
            stream<<ls[0];
            ls.removeFirst();
        }
        file.close();
        b=true;
    }
    //-
    return b;
}

bool CExportCSV::check_format(QList<QString> & ls)
{
    if(ls.count()<=0)
        return false;

    bool b=false;
    int i,j;
    QString s,s2;
    QStringList sls;

    for(i=0;i<ls.count();i++)
    {
        if(ls[i]==QString::fromUtf8("\n"))
            continue;
        s=ls[i];
        if(s.length()>0)
        {
            if(s[s.length()-1]==QChar('\n'))
                s.remove(s.length()-1,1);
        }
        s.remove(QChar(';'));
        //-
        sls=s.split(QString::fromUtf8("\"\""));
        s=QString::fromUtf8("");
        for(j=0;j<sls.count();j++)
        {
            s2=sls[j];
            s2.remove(QChar('\"'));
            s+=QString::fromUtf8("\"%1\"").arg(s2);
            if(j+1>=sls.count())//last
                s+=QString::fromUtf8("\n");
            else
                s+=QString::fromUtf8(";");
        }
        //-
        if(s!=ls[i])
        {
            ls[i]=s;
            b=true;
        }
        //-
        sls.clear();
    }
    return b;
}

QString CExportCSV::check_format(QString s)
{
    QString sReturn=QString::fromUtf8("");
    if(s.length()>0)
    {
        s.remove(QChar('\n'));
        s.remove(QChar('\"'));
        s.remove(QChar(';'));
        sReturn=s;
    }
    return sReturn;
}

bool CExportCSV::open_filedialog(QString & sPath, QWidget * pParent, QString sFilename)
{
    if(pParent==NULL)
        return false;

    bool b=false;

    //load backup-path
    int i;
    QDir dir;
    QStringList ls;
    CSettings settings;
    QString s,sExportPath,sFile;
    QDateTime dtTi=QDateTime::currentDateTime();
    QFileInfo file_info;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("Fehler"));

    //load last path from settings
    if(settings.load(QString::fromUtf8("EXPORT_PATH"),s))//load mark path
    {
        if(s.length()>0)
        {
            dir.setPath(s);
            if(dir.exists())
                sExportPath=s;
        }
    }

    //set path
    if(sExportPath.length()>0)//backup path load?
        sFile=sExportPath+QString::fromUtf8("/%1_%2.csv").arg(sFilename,dtTi.toString(QString::fromUtf8("hh-mm-ss_dd-MM-yyyy")));
    else//default path
        sFile=QDir::homePath()+QString::fromUtf8("/%1_%2.csv").arg(sFilename,dtTi.toString(QString::fromUtf8("hh-mm-ss_dd-MM-yyyy")));

    //open file dialog
    sFile=QFileDialog::getSaveFileName(pParent,QString::fromUtf8("Export"),sFile);
    if(sFile.length()>0)//file dialog finish with 'save'?
    {
        //file info (permission)
        s=QString::fromUtf8("");
        ls=sFile.split("/");
        for(i=0;i<ls.count()-1;i++)
            s+=ls[i]+QString::fromUtf8("/");
        file_info.setFile(s);
        if(!file_info.permission(QFile::WriteUser))
        {
            msg.setText(QString::fromUtf8("Sie haben keine Schreibrechte an diesem Ort!"));
            msg.exec();
        }
        else
        {
            //save backup path
            settings.write(QString::fromUtf8("EXPORT_PATH"),file_info.filePath());

            //check give it file?
            if(QFile(sFile).exists())
                QFile::remove(sFile);//delete old file
            //-
            b=true;
            sPath=sFile;
        }
    }
    //-
    ls.clear();
    return b;
}

bool CExportCSV::convert_to_export_setting(QList<QString> & ls)
{//convert data to user settings for csv-export

    bool bReturn=true;
    int i;
    QString sSplitText,sSplitChar;
    CInputDialogExport dlg;

    //get user settings
    bReturn=dlg.exec();
    {
        dlg.get_data(sSplitChar,sSplitText,m_iCodec);

        //convert
        if(sSplitChar==QString::fromUtf8(";") && sSplitText==QString::fromUtf8("\""))
        {}//nothing do - is default
        else
        {
            for(i=0;i<ls.count();i++)
            {
                ls[i].replace(QString::fromUtf8("\""),sSplitText);
                ls[i].replace(QString::fromUtf8(";"),sSplitChar);
            }
        }
    }
    //-
    return bReturn;
}

bool CExportCSV::create_data_table(QTableWidget * pTable, QList<QString> & ls, QString sTitle, bool bLastColumn)
{
    if(pTable==NULL)
        return false;

    int x,y,lc=0;
    QString s,sLine;
    QTableWidgetItem * pItem;

    //last column?
    if(!bLastColumn)
        lc=1;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //get table text
    for(y=-1;y<pTable->rowCount();y++)
    {
        sLine=QString::fromUtf8("");
        for(x=0;x<pTable->columnCount()-lc;x++)
        {
            s=QString::fromUtf8("");

            //get text
            if(y==-1)//header?
            {
                pItem=pTable->horizontalHeaderItem(x);
                if(pItem!=NULL)
                    s=pItem->text();
                else{}
            }
            else
            {
                pItem=pTable->item(y,x);
                if(pItem!=NULL)
                    s=pItem->text();
            }

            //text->line
            s=QString::fromUtf8("\"%1\"").arg(s);
            sLine+=s;
            if(x+1!=pTable->columnCount())
                sLine+=";";
        }
        sLine+="\n";
        ls.push_back(sLine);
    }
    //-
    return true;
}

bool CExportCSV::create_data_table(QTableWidget * pTable, QList<QString> & ls, int iFromColumn, int iToColumn, QString sTitle)
{
    bool b=true;
    int x,y;
    QString s,sLine;
    QTableWidgetItem * pItem;

    if(pTable==NULL || iFromColumn>iToColumn || iFromColumn<0 || iToColumn<0)
        b=false;
    else
    {
        if(iFromColumn>=pTable->columnCount() || iToColumn>=pTable->columnCount())
            b=false;
    }
    //-
    if(b)
    {
        //title
        if(sTitle.length()>0)//title set?
        {
            s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
            ls.push_back(s);
            ls.push_back(QString::fromUtf8("\n"));//empty row
        }

        //get table text
        for(y=-1;y<pTable->rowCount();y++)
        {
            sLine=QString::fromUtf8("");
            for(x=iFromColumn;x<=iToColumn;x++)
            {
                s=QString::fromUtf8("");

                //get text
                if(y==-1)//header?
                {
                    pItem=pTable->horizontalHeaderItem(x);
                    if(pItem!=NULL)
                        s=pItem->text();
                    else{}
                }
                else
                {
                    pItem=pTable->item(y,x);
                    if(pItem!=NULL)
                        s=pItem->text();
                }

                //text->line
                s=QString::fromUtf8("\"%1\"").arg(s);
                sLine+=s;
                if(x+1!=pTable->columnCount())
                    sLine+=";";
            }
            sLine+="\n";
            ls.push_back(sLine);
        }
    }
    //-
    return b;
}

bool CExportCSV::create_data_list(QListWidget * pList, QList<QString> & ls, QString sTitle)
{
    if(pList==NULL)
        return false;

    QString s,sLine;
    QListWidgetItem * pItem;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    for(int i=0; i<pList->count();i++)
    {
        pItem=pList->item(i);
        if(pItem!=NULL)
        {
            sLine=QString::fromUtf8("\"%1\"\n").arg(pItem->text());
            ls.push_back(sLine);
        }
    }
    return true;
}

bool CExportCSV::create_data_list_table(QListWidget * pList, QTableWidget * pTable, QList<QString> & ls, QString sTitle, bool bLastColumn)
{
    if(pTable==NULL || pList==NULL)
        return false;

    //title
    QString s;
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    create_data_list(pList,ls);
    ls.push_back(QString::fromUtf8("\n"));//empty row
    create_data_table(pTable,ls,QString::fromUtf8(""),bLastColumn);
    return true;
}

bool CExportCSV::create_data_tree(QTreeWidget * pTree, QList<QString> & ls, QString sTitle)
{
    if(pTree==NULL)
        return false;

    int i;
    QString s;
    QTreeWidgetItem * pItemTop=NULL;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //table title
    s=QString::fromUtf8("\"Warengruppe(mit Pfad)\";\"Kommentar\"\n");
    ls.push_back(s);

    //toplevel waregroups
    for(i=0;i<pTree->topLevelItemCount();i++)
    {
        pItemTop=pTree->topLevelItem(i);
        if(pItemTop!=NULL)
        {
            s=QString::fromUtf8("\"%1\";\"%2\"\n").arg(check_format(pItemTop->text(0)),check_format(pItemTop->text(1)));
            ls.push_back(s);

            //cildren
            if(pItemTop->childCount()>0)
                create_data_tree_children(pItemTop,ls);
        }
    }
    return true;
}
bool CExportCSV::create_data_tree_children(QTreeWidgetItem * pItemParent, QList<QString> & ls)
{
    if(pItemParent==NULL)
        return false;

    int j;
    QString s,sLine;
    QStringList sls;

    //path before
    if(ls.count()>0)
    {
        sLine=ls[ls.count()-1];
        sLine.remove("\"");
        sLine.remove("\n");
        sls=sLine.split(";");
        if(sls.count()>0)
            sLine=sls[0];
        sls.clear();
    }

    for(j=0;j<pItemParent->childCount();j++)
    {
        //create row
        s=sLine+QString::fromUtf8("\"/%1\";\"%2\"\n").arg(check_format(pItemParent->child(j)->text(0)),check_format(pItemParent->child(j)->text(1)));
        ls.push_back(s);

        //cildren from children
        if(pItemParent->child(j)->childCount()>0)
            create_data_tree_children(pItemParent->child(j),ls);
    }
    return true;
}

bool CExportCSV::create_data_tree_arrow(QTreeWidget * pTree, QList<QString> & ls, QString sTitle)
{
    if(pTree==NULL)
        return false;

    int i;
    QString s;
    QTreeWidgetItem * pItemTop=NULL;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //toplevel waregroups
    for(i=0;i<pTree->topLevelItemCount();i++)
    {
        pItemTop=pTree->topLevelItem(i);
        if(pItemTop!=NULL)
        {
            s=QString::fromUtf8("\"%1\"\n").arg(check_format(pItemTop->text(0)));
            ls.push_back(s);

            //cildren
            if(pItemTop->childCount()>0)
                create_data_tree_children_arrow(pItemTop,ls,1);
        }
    }
    return true;
}

bool CExportCSV::create_data_tree_children_arrow(QTreeWidgetItem * pItemParent, QList<QString> & ls, int iSubLevel)
{
    if(pItemParent==NULL || iSubLevel<0 || iSubLevel>20)
        return false;

    int i,j;
    QString s,sLine;
    QStringList sls;

    //set
    for(j=0;j<pItemParent->childCount();j++)
    {
        sLine=QString::fromUtf8("");
        sls.clear();

        //mark & split row before
        if(ls.count()>0)
            sls=ls[ls.count()-1].split(";");

        for(i=0;i<iSubLevel;i++)
        {
            s=QString::fromUtf8("\"\";");
            if(i+1==iSubLevel)
            {
                if(j+1==pItemParent->childCount())//last
                    s=QString::fromUtf8("\"*→\";");
                else
                    s=QString::fromUtf8("\"↓→\";");
            }
            else
            {
                if(i<sls.count())
                {
                    if(sls[i]==QString::fromUtf8("\"↓\"") || sls[i]==QString::fromUtf8("\"↓→\""))
                        s=QString::fromUtf8("\"↓\";");
                }
            }
            //-
            sLine+=s;
        }
        sLine+=QString::fromUtf8("\"%1\"\n").arg(check_format(pItemParent->child(j)->text(0)));
        ls.push_back(sLine);

        //cildren from children
        if(pItemParent->child(j)->childCount()>0)
            create_data_tree_children_arrow(pItemParent->child(j),ls,iSubLevel+1);
    }
    //-
    sls.clear();
    return true;
}

bool CExportCSV::write_data_table(QWidget * pParent, QTableWidget * pTable, QString sFileDescription, QString sTitle,bool bLastColumn)
{
    if(pParent==NULL || pTable==NULL || sFileDescription.length()<=0)
        return false;

    QString sFile;
    QList<QString> ls;

    if(create_data_table(pTable,ls,sTitle,bLastColumn))
    {
        if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
        {
            if(open_filedialog(sFile,pParent,sFileDescription))
            {
                write(sFile,ls);
            }
        }
    }
    ls.clear();
    //-
    return true;
}

bool CExportCSV::write_data_list(QWidget * pParent, QListWidget * pList, QString sFileDescription, QString sTitle)
{
    if(pParent==NULL || pList==NULL || sFileDescription.length()<=0)
        return false;

    QString sFile;
    QList<QString> ls;

    if(create_data_list(pList,ls,sTitle))
    {
        if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
        {
            if(open_filedialog(sFile,pParent,sFileDescription))
            {
                write(sFile,ls);
            }
        }
    }
    ls.clear();
    //-
    return true;
}

bool CExportCSV::write_data_list_table(QWidget * pParent, QListWidget * pList, QTableWidget * pTable, QString sFileDescription, QString sTitle, bool bLastColumn)
{
    if(pParent==NULL || pTable==NULL || pList==NULL || sFileDescription.length()<=0)
        return false;

    QString sFile;
    QList<QString> ls;

    if(create_data_list_table(pList,pTable,ls,sTitle,bLastColumn))
    {
        if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
        {
            if(open_filedialog(sFile,pParent,sFileDescription))
            {
                write(sFile,ls);
            }
        }

    }
    ls.clear();
    //-
    return true;
}

bool CExportCSV::write_data_tree(QWidget * pParent,QTreeWidget * pTree, QString sFileDescription, QString sTitle)
{
    if(pParent==NULL || pTree==NULL || sFileDescription.length()<=0)
        return false;

    QString sFile;
    QList<QString> ls;

    if(create_data_tree(pTree,ls,sTitle))
    {
        if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
        {
            if(open_filedialog(sFile,pParent,sFileDescription))
            {
                write(sFile,ls);
            }
        }
    }
    ls.clear();
    //-
    return true;
}

bool CExportCSV::write_data_maker(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, QString sFileDescription, QString sTitle)
{
    if(pParent==NULL || pTable==NULL || !db.is_db_connect() || sFileDescription.length()<=0)
        return false;

    bool b;
    CMaker mk;
    QString s,sLine,sFile;
    QList<QString> ls;
    int i,id_column,id;
    QTableWidgetItem * pItem;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //header
    ls.push_back(QString::fromUtf8("\"Name\";\"Adresse\";\"Telefonnummer\";\"Faxnummer\";\"Emailadresse\";\"homepage\";\"Kontaktperson\";\"Kommentar\"\n"));

    //get maker info
    id_column=pTable->columnCount()-1;
    if(id_column>=0)// last column = id
    {
        for(i=0;i<pTable->rowCount();i++)
        {
            pItem=pTable->item(i,id_column);
            if(pItem!=NULL)
            {
                s=pItem->text();
                if(s.length()>0)
                {
                    id=s.toInt(&b,10);
                    if(b)//cast id to int ok?
                    {
                        if(db.maker_get(id,mk)) // get maker by id
                        {
                            sLine=QString::fromUtf8("\"%1\";\"%2\";\"%3\";\"%4\";\"%5\";\"%6\";\"%7\";\"%8\"\n")
                                        .arg(mk.get_name(),mk.get_adress(),mk.get_callnumber(),mk.get_faxnumber(),mk.get_email(),mk.get_homepage(),mk.get_contectperson(),mk.get_comment());
                            ls.push_back(sLine);
                        }
                    }
                }
            }
        }
    }

    //write
    if(ls.count()>0)
    {
        if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
        {
            if(open_filedialog(sFile,pParent,sFileDescription))
                write(sFile,ls);
        }
    }
    //-
    ls.clear();
    return true;
}

bool CExportCSV::write_data_dealer(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, QString sFileDescription, QString sTitle)
{
    if(pParent==NULL || pTable==NULL || !db.is_db_connect() || sFileDescription.length()<=0)
        return false;

    bool b;
    CDealer de;
    QString s,sLine,sFile;
    QList<QString> ls;
    int i,id_column,id;
    QTableWidgetItem * pItem;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //header
    ls.push_back(QString::fromUtf8("\"Name\";\"Adresse\";\"Telefonnummer\";\"Faxnummer\";\"Emailadresse\";\"homepage\";\"Kontaktperson\";\"Kundennummer\";\"Kommentar\"\n"));

    //get maker info
    id_column=pTable->columnCount()-1;
    if(id_column>=0)// last column = id
    {
        for(i=0;i<pTable->rowCount();i++)
        {
            pItem=pTable->item(i,id_column);
            if(pItem!=NULL)
            {
                s=pItem->text();
                if(s.length()>0)
                {
                    id=s.toInt(&b,10);
                    if(b)//cast id to int ok?
                    {
                        if(db.dealer_get(id,de)) // get maker by id
                        {
                            sLine=QString::fromUtf8("\"%1\";\"%2\";\"%3\";\"%4\";\"%5\";\"%6\";\"%7\";\"%8\";\"%9\"\n")
                                        .arg(de.get_name(),de.get_adress(),de.get_callnumber(),
                                             de.get_faxnumber(),de.get_email(),de.get_homepage(),de.get_contectperson(),de.get_customernumber(),de.get_comment());
                            ls.push_back(sLine);
                        }
                    }
                }
            }
        }
    }

    //write
    if(ls.count()>0)
    {
        if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
        {
            if(open_filedialog(sFile,pParent,sFileDescription))
                write(sFile,ls);
        }
    }
    //-
    ls.clear();
    return true;
}

bool CExportCSV::write_data_customer(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, QString sFileDescription, QString sTitle)
{
    if(pParent==NULL || pTable==NULL || !db.is_db_connect() || sFileDescription.length()<=0)
        return false;

    bool b;
    CCustomer cu;
    QString s,sLine,sFile;
    QList<QString> ls;
    int i,id_column,id;
    QTableWidgetItem * pItem;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //header
    ls.push_back(QString::fromUtf8("\"Name\";\"Vorname\";\"Strasse\";\"Postleitzahl\";\"Ort\";\"Kundennummer\";\"Telefonnummer\";\"Faxnummer\";\"Emailadresse\";\"Kommentar\"\n"));

    //get maker info
    id_column=pTable->columnCount()-1;
    if(id_column>=0)// last column = id
    {
        for(i=0;i<pTable->rowCount();i++)
        {
            pItem=pTable->item(i,id_column);
            if(pItem!=NULL)
            {
                s=pItem->text();
                if(s.length()>0)
                {
                    id=s.toInt(&b,10);
                    if(b)//cast id to int ok?
                    {
                        if(db.customer_get(id,cu)) // get maker by id
                        {
                            sLine=QString::fromUtf8("\"%1\";\"%2\";\"%3\";\"%4\";\"%5\";\"%6\";\"%7\";\"%8\";\"%9\";")
                                        .arg(cu.get_name(),cu.get_first_name(),cu.get_street(),cu.get_postcode(),cu.get_city(),cu.get_customernumber(),
                                             cu.get_callnumber(),cu.get_faxnumber(),cu.get_email());
                            sLine+=QString::fromUtf8("\"%1\"\n").arg(cu.get_comment());
                            ls.push_back(sLine);
                        }
                    }
                }
            }
        }
    }

    //write
    if(ls.count()>0)
    {
        if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
        {
            if(open_filedialog(sFile,pParent,sFileDescription))
                write(sFile,ls);
        }
    }
    //-
    ls.clear();
    return true;
}

bool CExportCSV::write_data_inventory(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, CWidgetInterface & widget, QString sFileDescription, QString sTitle)
{
    if(pParent==NULL || pTable==NULL || !db.is_db_connect() || sFileDescription.length()<=0)
        return false;

    bool b;
    CArticle ar;
    QString s,sLine,sFile;
    QList<QString> ls;
    QStringList lss;
    int j,i,id_column,id;
    QTableWidgetItem * pItem;
    CTableItemData ti;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //header
    ls.push_back(QString::fromUtf8("\"Auslastung(%)\";\"Warnung\";\"Bestand\";\"bestellte Menge\";\"Anzahl Bestellungen\";\"Lagerkapazität\";\"Einheit\";\"Artikelbezeichnung\";"
                         "\"1.Artikelnummer\";\"2.Artikelnummer\";\"Hersteller\";\"Warengruppe\";\"Standort\";\"Kommentar\"\n"));

    //get maker info
    id_column=pTable->columnCount()-1;
    if(id_column>=0)// last column = id
    {
        for(i=0;i<pTable->rowCount();i++)
        {
            pItem=pTable->item(i,id_column);
            if(pItem!=NULL)
            {
                s=pItem->text();
                if(s.length()>0)
                {
                    id=s.toInt(&b,10);
                    if(b)//cast id to int ok?
                    {
                        if(db.article_get(id,ar)) // get maker by id
                        {
                            sLine=QString::fromUtf8("");

                            //%
                            widget.inventory_get_icon_and_precent(ar,ti);
                            s=ti.get_text();
                            lss=s.split(QString::fromUtf8(" "));
                            if(lss.count()>1)
                                sLine+=QString::fromUtf8("\"%1\";").arg(lss[0]);
                            else
                                sLine+=QString::fromUtf8("\"0\";");
                            lss.clear();

                            //warning
                            widget.inventory_get_icon_and_warning_limit(ar,ti);
                            sLine+=QString::fromUtf8("\"%1\";").arg(ti.get_text());
                            //-
                            sLine+=QString::fromUtf8("\"%1\";").arg(ar.get_inventory());//inv
                            sLine+=QString::fromUtf8("\"%1\";").arg(db.ordering_get_count_by_article(ar.get_id()));//ordering(article count)
                            j=db.ordering_get_ordering_count_by_article(ar.get_id());
                            if(j>0)
                                sLine+=QString::fromUtf8("\"%1\";").arg(j);//ordering(article count)
                            else
                                sLine+=QString::fromUtf8("\"\";");
                            sLine+=QString::fromUtf8("\"%1\";").arg(ar.get_max_inventory());//max inv
                            sLine+=QString::fromUtf8("\"%1\";\"%2\";\"%3\";\"%4\";\"%5\";\"%6\";\"%7\";\"%8\"\n").arg(ar.get_unit(),ar.get_name(),ar.get_articlenumber(),ar.get_articlenumber2(),
                                                                                                     db.maker_get_name(ar.get_maker_id()),db.waregroup_get_path(ar.get_waregroup_id()),
                                                                                                     ar.get_location(),ar.get_comment());
                            ls.push_back(sLine);
                        }
                    }
                }
            }
        }
    }

    //write
    if(ls.count()>0)
    {
        if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
        {
            if(open_filedialog(sFile,pParent,sFileDescription))
                write(sFile,ls);
        }
    }
    //-
    ls.clear();
    return true;
}

bool CExportCSV::write_data_article(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, QString sFileDescription, QString sTitle)
{
    if(pParent==NULL || pTable==NULL || !db.is_db_connect() || sFileDescription.length()<=0)
        return false;

    bool b;
    CArticle ar;
    QString s,sLine,sFile;
    QList<QString> ls;
    int i,id_column,id;
    QTableWidgetItem * pItem;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //header
    ls.push_back(QString::fromUtf8("\"Artikelbezeichnung\";\"1.Artikelnummer\";\"2.Artikelnummer\";\"Hersteller\";\"Warengruppe\";\"Warnung\";"
                         "\"Bestand\";\"max.Lagerkapazität\";\"Einheit\";\"EK\";\"VK\";\"Marge\";\"Währung\";\"Standort\";\"Kommentar\";\"Dateipfad-Artikelbild\"\n"));

    //get maker info
    id_column=pTable->columnCount()-1;
    if(id_column>=0)// last column = id
    {
        for(i=0;i<pTable->rowCount();i++)
        {
            pItem=pTable->item(i,id_column);
            if(pItem!=NULL)
            {
                s=pItem->text();
                if(s.length()>0)
                {
                    id=s.toInt(&b,10);
                    if(b)//cast id to int ok?
                    {
                        if(db.article_get(id,ar)) // get article by id
                        {
                            sLine=QString::fromUtf8("\"%1\";\"%2\";\"%3\";\"%4\";\"%5\";").arg(ar.get_name(),ar.get_articlenumber(),ar.get_articlenumber2(),
                                                                                     db.maker_get_name(ar.get_maker_id()),db.waregroup_get_path(ar.get_waregroup_id()));
                            //warning
                            if(ar.get_warning_limit()>0 && ((int)ar.get_inventory())<ar.get_warning_limit())
                                sLine+=QString::fromUtf8("\"bst.<%1\";").arg(ar.get_warning_limit());
                            else
                                sLine+=QString::fromUtf8("\"\";");

                            //inv
                            sLine+=QString::fromUtf8("\"%1\";").arg(ar.get_inventory());

                            //max.inv
                            s=ar.get_max_inventory();
                            if(s.length()>0)
                                sLine+=QString::fromUtf8("\"%1\";").arg(ar.get_max_inventory());
                            else
                                sLine+=QString::fromUtf8("\"\";");

                            //unit
                            s=ar.get_unit();
                            if(s.length()>0)
                                sLine+=QString::fromUtf8("\"%1\";").arg(s);
                            else
                                sLine+=QString::fromUtf8("\"\";");

                            //baseprice
                            if(ar.get_base_price()>0.0)
                            {
                                //german format 0,00 not 0.00
                                s=QString::fromUtf8("%1").arg(ar.get_base_price());
                                s.replace(QString::fromUtf8("."),QString::fromUtf8(","));

                                sLine+=QString::fromUtf8("\"%1\";").arg(s);
                            }
                            else
                                sLine+=QString::fromUtf8("\"\";");

                            //saleprice
                            if(ar.get_sales_price()>0.0)
                            {
                                //german format 0,00 not 0.00
                                s=QString::fromUtf8("%1").arg(ar.get_sales_price());
                                s.replace(QString::fromUtf8("."),QString::fromUtf8(","));

                                sLine+=QString::fromUtf8("\"%1\";").arg(s);
                            }
                            else
                                sLine+=QString::fromUtf8("\"\";");


                            //diff
                            if(ar.get_base_price()>0.0 && ar.get_sales_price()>0.0)
                            {
                                //german format 0,00 not 0.00
                                s=QString::fromUtf8("%1").arg(ar.get_sales_price()-ar.get_base_price());
                                s.replace(QString::fromUtf8("."),QString::fromUtf8(","));

                                sLine+=QString::fromUtf8("\"%1\";").arg(s);
                            }
                            else
                                sLine+=QString::fromUtf8("\"\";");
                            //-
                            sLine+=QString::fromUtf8("\"%1\";\"%2\";\"%3\";").arg(ar.get_valuta(),ar.get_location(),ar.get_comment());

                            //filepath-articlepicture
                            sLine+=QString::fromUtf8("\"%1\"\n").arg(ar.get_path_picture());
                            //-
                            ls.push_back(sLine);
                        }
                    }
                }
            }
        }
    }

    //write
    if(ls.count()>0)
    {
        if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
        {
            if(open_filedialog(sFile,pParent,sFileDescription))
                write(sFile,ls);
        }
    }
    //-
    ls.clear();
    return true;
}

bool CExportCSV::write_data_ordering(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, CWidgetInterface & widget, QString sFileDescription, QString sTitle)
{
    if(pParent==NULL || pTable==NULL || !db.is_db_connect() || sFileDescription.length()<=0)
        return false;

    bool b,bFirst;
    CArticle ar;
    COrdering ord;
    QString s,sLine,sFile;
    QTableWidgetItem * pItem;
    QList<QString> ls,lsWares;
    int i,id,id_column,iCount,iArticleId;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //header
    ls.push_back(QString::fromUtf8("\"Bestelldatum\";\"Bestellnummer\";\"Händler\";\"Kommentar\";\"Anzahl\";\"Einheit\";\"Artikelbezeichnung\";\"1.Artikelnummer\";\"2.Artikelnummer\";"
                         "\"Hersteller\";\"Warengruppe\";\"Standort\"\n"));

    //create
    id_column=pTable->columnCount()-1;
    if(id_column>=0)// last column = id
    {
        for(i=0;i<pTable->rowCount();i++)
        {
            pItem=pTable->item(i,id_column);
            if(pItem!=NULL)
            {
                s=pItem->text();
                if(s.length()>0)
                {
                    id=s.toInt(&b,10);
                    if(b)//cast id to int ok?
                    {
                        if(db.ordering_get(id,ord)) // get ordering by id
                        {
                            //data
                            sLine=QString::fromUtf8("\"%1\";\"%2\";\"%3\";\"%4\";").arg(ord.get_date().toString("dd.MM.yyyy"),ord.get_ordernumber(),db.dealer_get_name(ord.get_dealer_id()),
                                                                              ord.get_comment());
                            bFirst=true;

                            //wares
                            ord.get_wares(lsWares);
                            while(lsWares.count()>0)
                            {
                                b=widget.cast_count_and_id(lsWares[0],iCount,iArticleId);//cast string to count & article id
                                if(b)
                                {
                                    b=db.article_get(iArticleId,ar,true);//get article data
                                    if(b)
                                    {
                                        if(bFirst)
                                        {
                                            sLine+=QString::fromUtf8("\"%1\";").arg(iCount);
                                            bFirst=false;
                                        }
                                        else
                                            sLine=QString::fromUtf8("\"\";\"\";\"\";\"\";\"%1\";").arg(iCount);
                                        sLine+=QString::fromUtf8("\"%1\";\"%2\";\"%3\";\"%4\";\"%5\";\"%6\";\"%7\"\n").arg(ar.get_unit(),ar.get_name(),ar.get_articlenumber(),ar.get_articlenumber2(),
                                                                                                         db.maker_get_name(ar.get_maker_id()),db.waregroup_get_path(ar.get_waregroup_id()),
                                                                                                         ar.get_location());
                                        ls.push_back(sLine);
                                    }
                                }
                                lsWares.removeFirst();
                            }
                            ls.push_back(QString::fromUtf8("\n"));//empty row
                            lsWares.clear();
                        }
                    }
                }
            }
        }

        //write
        if(ls.count()>0)
        {
            if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
            {
                if(open_filedialog(sFile,pParent,sFileDescription))
                    write(sFile,ls);
            }
        }
    }
    //-
    lsWares.clear();
    ls.clear();
    return true;
}

bool CExportCSV::write_data_trade(QWidget * pParent, QTableWidget * pTable, CDbConnection & db, CWidgetInterface & widget, QString sFileDescription, QString sTitle)
{
    if(pParent==NULL || pTable==NULL || !db.is_db_connect() || sFileDescription.length()<=0)
        return false;

    bool b,bFirst;
    CArticle ar;
    CTrade tr;
    QString s,s2,sLine,sFile,sBookingnumber;
    QTableWidgetItem * pItem;
    QList<QString> ls,lsWares;
    int i,id_column,iCount,iArticleId;

    //title
    if(sTitle.length()>0)//title set?
    {
        s=QString::fromUtf8("\"%1\"\n").arg(sTitle);
        ls.push_back(s);
        ls.push_back(QString::fromUtf8("\n"));//empty row
    }

    //header
    ls.push_back(QString::fromUtf8("\"Art\";\"storniert\";\"Buchungsnummer\";\"Ein-/Ausgangsdatum\";\"Info1\";\"Info2\";\"Info3\";\"Info4\";\"Info5\";\"Kommentar\";\"Anzahl\";"
                         "\"Einheit\";\"Artikelbezeichnung\";\"1.Artikelnummer\";\"2.Artikelnummer\";\"Hersteller\";\"Warengruppe\";\"Standort\"\n"));

    //create
    id_column=2;//bookingnumber
    if(id_column<pTable->columnCount())
    {
        for(i=0;i<pTable->rowCount();i++)
        {
            pItem=pTable->item(i,id_column);
            if(pItem!=NULL)
            {
                sBookingnumber=pItem->text();
                if(sBookingnumber.length()>0)
                {
                    if(db.trade_get(sBookingnumber,tr)) // get trade by bookingnumber
                    {
                        //data
                        if(tr.get_type()==TYPE_INCOMING)
                            sLine=QString::fromUtf8("\"Wareneingang\";");
                        else if(tr.get_type()==TYPE_OUTGOING)
                            sLine=QString::fromUtf8("\"Warenausgang\";");
                        else if(tr.get_type()==TYPE_ORDERING_INCOMING)
                            sLine=QString::fromUtf8("\"Wareneingang(Bestellung)\";");
                        else
                            sLine=QString::fromUtf8("\"Warenausgang(Kunde)\";");

                        if(tr.get_canceled())
                            sLine+=QString::fromUtf8("\"ja\";");
                        else
                            sLine+=QString::fromUtf8("\"nein\";");

                        sLine+=QString::fromUtf8("\"%1\";\"%2\";").arg(tr.get_booking_number(),tr.get_date().toString("dd.MM.yyyy"));

                        //info ordering incoming & customer ountgoing
                        if(tr.get_type()==TYPE_INCOMING || tr.get_type()==TYPE_OUTGOING)
                            sLine+=QString::fromUtf8("\"\";\"\";\"\";\"\";\"\";");
                        else
                        {
                            if(tr.get_type()==TYPE_CUSTOMER_OUTGOING)
                                s=QString::fromUtf8("Kundennummer:");
                            else
                                s=QString::fromUtf8("Bestellnummer:");
                            s2=tr.get_info_1();
                            if(s2.length()>0)
                                sLine+=QString::fromUtf8("\"%1\";").arg(s+s2);
                            else
                                sLine+=QString::fromUtf8("\"\";");
                            //-
                            if(tr.get_type()==TYPE_CUSTOMER_OUTGOING)
                                s=QString::fromUtf8("Name, Vorname:");
                            else
                                s=QString::fromUtf8("Händler:");
                            s2=tr.get_info_2();
                            if(s2.length()>0)
                                sLine+=QString::fromUtf8("\"%1\";").arg(s+s2);
                            else
                                sLine+=QString::fromUtf8("\"\";");
                            //-
                            if(tr.get_type()==TYPE_CUSTOMER_OUTGOING)
                                s=QString::fromUtf8("Strasse:");
                            else
                                s=QString::fromUtf8("Bestelldatum:");
                            s2=tr.get_info_3();
                            if(s2.length()>0)
                                sLine+=QString::fromUtf8("\"%1\";").arg(s+s2);
                            else
                                sLine+=QString::fromUtf8("\"\";");
                            //-
                            if(tr.get_type()==TYPE_CUSTOMER_OUTGOING)
                                s=QString::fromUtf8("Postleitzahl, Ort:");
                            else
                                s=QString::fromUtf8("sonstiges:");
                            s2=tr.get_info_4();
                            if(s2.length()>0)
                                sLine+=QString::fromUtf8("\"%1\";").arg(s+s2);
                            else
                                sLine+=QString::fromUtf8("\"\";");
                            sLine+=QString::fromUtf8("\"\";");//info 5 current not use
                        }

                        sLine+=QString::fromUtf8("\"%1\";").arg(tr.get_comment());
                        bFirst=true;

                        //wares
                        tr.get_wares(lsWares);
                        while(lsWares.count()>0)
                        {
                            b=widget.cast_count_and_id(lsWares[0],iCount,iArticleId);//cast string to count & article id
                            if(b)
                            {
                                b=db.article_get(iArticleId,ar,true);//get article data
                                if(b)
                                {
                                    if(bFirst)
                                    {
                                        sLine+=QString::fromUtf8("\"%1\";").arg(iCount);
                                        bFirst=false;
                                    }
                                    else
                                        sLine=QString::fromUtf8("\"\";\"\";\"\";\"\";\"\";\"\";\"\";\"\";\"\";\"\";\"%1\";").arg(iCount);
                                    sLine+=QString::fromUtf8("\"%1\";\"%2\";\"%3\";\"%4\";\"%5\";\"%6\";\"%7\"\n").arg(ar.get_unit(),ar.get_name(),ar.get_articlenumber(),ar.get_articlenumber2(),
                                                                                                         db.maker_get_name(ar.get_maker_id()),db.waregroup_get_path(ar.get_waregroup_id()),
                                                                                                         ar.get_location());
                                    ls.push_back(sLine);
                                }
                            }
                            lsWares.removeFirst();
                        }
                        ls.push_back(QString::fromUtf8("\n"));//empty row
                        lsWares.clear();
                    }
                }
            }
        }

        //write
        if(ls.count()>0)
        {
            if(convert_to_export_setting(ls))//convert data to user settings(CInputDialogExport)
            {
                if(open_filedialog(sFile,pParent,sFileDescription))
                    write(sFile,ls);
            }
        }
    }
    //-
    lsWares.clear();
    ls.clear();
    return true;
}

