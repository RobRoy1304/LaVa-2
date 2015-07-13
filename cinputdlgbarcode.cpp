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

#include "cinputdlgbarcode.h"
#include "ui_cinputdlgbarcode.h"

CInputDlgBarcode::CInputDlgBarcode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDlgBarcode)
{
    ui->setupUi(this);
    //-
    settings(false);//load & set settings
    //-
    m_iParentColumnId=m_iParentColumnCount=m_iType=-1;
    m_pParentTable=NULL;
    m_pThread=NULL;
    m_dtDate=QDate(0,0,0);

    //focus
    ui->spinBoxCount->setFocus();

    //connects
    connect(ui->comboBoxArticlenumber,SIGNAL(currentIndexChanged(int)),this,SLOT(articlenumber_changed()));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDlgBarcode::~CInputDlgBarcode()
{
    settings(true);//save settings if update
    delete ui;
}

void CInputDlgBarcode::keyPressEvent(QKeyEvent * event)
{
    if(event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return)
    {
        if(ui->spinBoxCount->hasFocus())
            ui->lineEditBarcode->setFocus();
        else
        {
            add_ware();
            ui->spinBoxCount->setFocus();
            ui->spinBoxCount->findChild<QLineEdit*>()->setSelection(0,9999999);//select text value
        }
    }
}

bool CInputDlgBarcode::set(CWorkThread * pThread, QTableWidget * pParentTable, int iColumnCount, int iColumnId, int iType, QDate dtDate)
{
    m_pThread=pThread;
    m_pParentTable=pParentTable;
    m_iParentColumnCount=iColumnCount;
    m_iParentColumnId=iColumnId;
    m_iType=iType;
    m_dtDate=dtDate;
    //-
    return true;
}

bool CInputDlgBarcode::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;
    lsSType.push_back(QString("DLG_BARCODE_SELECT_ARTICLENUMBER"));

    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            if(lsSValue.count()>0)
                settings.set_combobox(ui->comboBoxArticlenumber,lsSValue[0],0);
        }
        if(bUpdate)//write
        {
            if(lsSValue.count()>0)
            {
                if(settings.get_combobox(ui->comboBoxArticlenumber,lsSValue[0]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[0]);
                    lsSUpdateValue.push_back(lsSValue[0]);
                }
            }

            //write updates
            if(lsSUpdateType.count()>0 && lsSUpdateType.count()==lsSUpdateValue.count())
                b=settings.write(lsSUpdateType,lsSUpdateValue);
        }
    }
    //-
    lsSValue.clear();
    lsSType.clear();
    lsSUpdateValue.clear();
    lsSUpdateType.clear();
    return b;
}

bool CInputDlgBarcode::add_ware(void)
{
    bool b=false,ok=true;
    CArticle ar;
    QColor color(0,170,0);//green
    QString s,sWg,sMk,sBarcode,sData,sInfo,sWarning;
    int parent_table_count,count_spinbox,count_on_date,articlenumber_select,iReturn,count,ordering_count;
    //-
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL && m_pThread->m_pWidgets!=NULL && m_pParentTable!=NULL)
        {
            //get data
            sBarcode=ui->lineEditBarcode->text();
            if(sBarcode.length()>0)
            {
                count_spinbox=ui->spinBoxCount->value();//count user input
                if(count_spinbox>=1)
                {
                    articlenumber_select=ui->comboBoxArticlenumber->currentIndex();

                    //get article by data
                    iReturn=get_article_id_by_barcode(sBarcode,articlenumber_select, ar);

                    if(iReturn==RETURN_ARTICLE_NOT_FOUND)
                    {//article not found
                        ok=false;
                        color=QColor(170,0,0);//red
                        sInfo=QString("Artikel nicht gefunden");
                    }
                    else if(iReturn!=RETURN_USER_EXIT)
                    {
                        //info text
                        sWg=m_pThread->m_pDbInterface->waregroup_get_path(ar.get_waregroup_id());
                        sMk=m_pThread->m_pDbInterface->maker_get_name(ar.get_maker_id());
                        sInfo=QString("Anzahl: \"%1").arg(count_spinbox);
                        if(ar.get_unit().length()>0)
                            sInfo+=QString(" %1").arg(ar.get_unit());
                        sInfo+=QString::fromUtf8("\" zur Liste hinzugefügt...\n");
                        sInfo+=QString("%1.Artikelnummer \t Barcode: ").arg(articlenumber_select+1);
                        sInfo+=QString("%1\n").arg(sBarcode);
                        sInfo+=QString("Artikel: %1 \t Hersteller: %2\n").arg(ar.get_name(),sMk);
                        sInfo+=QString("Warengruppe: %1").arg(sWg);

                        //get count's
                        if(ar.get_max_inventory()>0 || m_iType == ARTICLE_DLG_TYPE_OUTGOING)
                        {
                            parent_table_count=get_parent_table_count(ar.get_id());//get count from parent dlg table
                            count_on_date=get_count_on_date(ar.get_id());//get date from article on selected date
                        }

                        //incoming
                        if(m_iType == ARTICLE_DLG_TYPE_INCOMING && ar.get_max_inventory()>0)//max.inv.set?
                        {
                            count=parent_table_count + count_spinbox + count_on_date;
                            if(count > (int)ar.get_max_inventory())
                            {
                                sWarning=QString(QString::fromUtf8("Warnung: die max.Lagerkapazität wird um %1 überschritten.")).arg(count-ar.get_max_inventory());
                                color=QColor(200,200,0);
                            }
                        }

                        //orderling
                        if(m_iType == ARTICLE_DLG_TYPE_ORDERING && ar.get_max_inventory()>0)//max.inv.set?
                        {
                            ordering_count=get_ordering_count(ar.get_id());
                            count=count_spinbox + parent_table_count + ordering_count + ar.get_inventory();
                            if(count > (int)ar.get_max_inventory())
                                sWarning=QString(QString::fromUtf8("Warnung: die max.Lagerkapazität dieses Artikels\nwird beim Eingang der Bestellung um %1 überschritten.")).arg(ar.get_max_inventory()-count);
                            color=QColor(200,200,0);
                        }

                        //outgoing
                        if(m_iType == ARTICLE_DLG_TYPE_OUTGOING)
                        {
                            if(count_spinbox > count_on_date - parent_table_count)
                            {//error , count to high on selected date
                                ok=false;
                                color=QColor(170,0,0);//red
                                sWarning=QString("Fehler:");
                                sInfo=QString("Am '%1' liegt der Bestand dieses Artikels bei ").arg(m_dtDate.toString("dd.MM.yyyy"));
                                sInfo+=QString("%1").arg(count_on_date);
                                if(ar.get_unit().length()>0)
                                    sInfo+=QString(" %1").arg(ar.get_unit());
                                if(parent_table_count>0)
                                {
                                    sInfo+=QString(", in der Liste sind schon %1").arg(parent_table_count);
                                    if(ar.get_unit().length()>0)
                                        sInfo+=QString(" %1").arg(ar.get_unit());
                                }
                                sInfo+=QString(", beachten Sie ggf. die chronologische Reihenfolge von Warenein-/ausgang.");
                            }
                        }
                    }
                }
            }
        }
    }

    //set info text
    ui->textBrowserArticleInfo->clear(); //clear textbrowser
    ui->textBrowserArticleInfo->setTextColor(color);
    s=QString("");
    if(sWarning.length()>0)
        s=sWarning+QString("\n");
    s+=sInfo;
    if(s.length()>0)
        ui->textBrowserArticleInfo->setText(s);

    //-
    if(ok)
    {
        //add to table from parent dialog
        sData=QString("%1x").arg(ui->spinBoxCount->text());
        sData+=QString("%1").arg(ar.get_id());
        m_pThread->m_pWidgets->article_update_row_wareslist(m_pParentTable,sData,false,true);
    }
    //-
    ui->lineEditBarcode->setText(QString(""));
    //-
    return b;
}

int CInputDlgBarcode::get_article_id_by_barcode(QString sBarcode, int number, CArticle & ar)//number = articlenumber 1 or 2
{
    QList<int> lsIds;
    int iArticleId=-1,iReturn=RETURN_ARTICLE_NOT_FOUND;
    CInputDialogBrowseArticle dlgBrowseArticle;
    QString sCondition=QString("articlenumber");
    //-
    if(sBarcode.length()>0 && number >=0 && number <=1 && m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            //condition
            if(number==1)//search in articlenumber 2
                sCondition+=QString("2");
            sCondition+=QString("=\"%1\"").arg(sBarcode);
            if(m_pThread->m_pDbInterface->article_get_all(sCondition,lsIds))
            {//article found

                if(lsIds.count()==1)
                {
                    iArticleId=lsIds[0];//only 1 article found by barcode
                    iReturn=RETURN_ARTICLE_FOUND;
                }
                else if(lsIds.count()>1)
                {//many article with this barcode -> user select
                    dlgBrowseArticle.set_thread(m_pThread);
                    dlgBrowseArticle.set_mask_and_disable_change(sBarcode,number);
                    dlgBrowseArticle.setWindowTitle(QString("Es wurden mehrere Artikel mit dem Barcode gefunden..."));
                    dlgBrowseArticle.update_table();
                    dlgBrowseArticle.select_first_row();
                    if(!dlgBrowseArticle.exec())
                        iReturn=RETURN_USER_EXIT;
                    else
                    {
                        dlgBrowseArticle.get_data(iArticleId);
                    }
                }
            }

            //get article data
            if(iArticleId>0)
            {
                if(m_pThread->m_pDbInterface->article_get(iArticleId,ar))
                    iReturn=RETURN_ARTICLE_FOUND;
            }
        }
    }
    //-
    lsIds.clear();
    //-
    return iReturn;
}

int CInputDlgBarcode::get_parent_table_count(int iArticleId)
{
    if(m_pParentTable==NULL)
        return false;
    if(m_iParentColumnCount<0 || m_iParentColumnCount>=m_pParentTable->columnCount() || m_iParentColumnId<0 || m_iParentColumnId>=m_pParentTable->columnCount())
        return false;
    //-
    QString sCount,sArticleId=QString("%1").arg(iArticleId);
    int i,iOldCount=0;
    bool b;
    QTableWidgetItem * pItem;
    //-
    for(i=0;i<m_pParentTable->rowCount();i++)
    {
        pItem=m_pParentTable->item(i,m_iParentColumnId);
        if(pItem!=NULL)
        {
            if(pItem->text()==sArticleId)//found
            {
                pItem=m_pParentTable->item(i,m_iParentColumnCount);
                sCount=pItem->text();
                break;
            }
        }
    }
    //-
    if(sCount.length()>0)
    {
        iOldCount=sCount.toInt(&b,10);
        if(!b)//error by cast
            iOldCount=-1;
    }
    //-
    return iOldCount;
}

int CInputDlgBarcode::get_count_on_date(int iArticleId)
{
    if(m_pThread==NULL || iArticleId<0 || m_dtDate==QDate(0,0,0))
        return -1;
    //-
    CPointerMemory memory;
    unsigned int uiCountOnDate=0;
    memory.set_int(&iArticleId);
    memory.set_uint(&uiCountOnDate);
    memory.set_date(&m_dtDate);
    if(!m_pThread->set_work(WORK_ARTICLE_GET_INVENTORY_ON_DATE,&memory))
        return -1;
    return (int)uiCountOnDate;
}

int CInputDlgBarcode::get_ordering_count(int iArticleId)
{
    if(m_pThread==NULL || iArticleId<0)
        return -1;
    //-
    CPointerMemory memory;
    int iOrderingCount=0;
    memory.set_int(&iArticleId);
    memory.set_int2(&iOrderingCount);
    if(!m_pThread->set_work(WORK_ORDERING_GET_COUNT_WITH_ARTICLE,&memory))
        return -1;
    return iOrderingCount;
}

void CInputDlgBarcode::articlenumber_changed(void)
{
    ui->lineEditBarcode->setText(QString(""));
    ui->lineEditBarcode->setFocus();
}
