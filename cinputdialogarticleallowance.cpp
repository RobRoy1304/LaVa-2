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

#include "cinputdialogarticleallowance.h"
#include "ui_cinputdialogarticleallowance.h"
#include "cinputdlgtrade.h"

CInputDialogArticleAllowance::CInputDialogArticleAllowance(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogArticleAllowance)
{
    ui->setupUi(this);
    //-
    m_iParentColumnId=m_iParentColumnCount=m_iType=m_iArticleId-1;
    m_pParentTable=NULL;
    m_pThread=NULL;
    m_dtDate=QDate(0,0,0);
    //-

    //disable auto default buttons
    ui->pushButtonCancel->setAutoDefault(false);
    ui->pushButtonMaskSet->setAutoDefault(false);
    ui->pushButtonOk->setAutoDefault(false);

    settings(false);//load & set settings
    ui->pushButtonOk->setEnabled(false);   

    //focus
    ui->spinBox->setFocus();
    ui->spinBox->setMinimum(1);

    //connects
    connect(ui->pushButtonMaskSet,SIGNAL(clicked()),this,SLOT(mask_changed()));
    connect(ui->tableWidgetArticle,SIGNAL(itemSelectionChanged()),this,SLOT(article_changed()));
    connect(ui->checkBoxAuto,SIGNAL(clicked()),this,SLOT(checkbox_auto_clicked()));
    connect(ui->lineEditMask,SIGNAL(textChanged(QString)),this,SLOT(mask_edit()));
    connect(ui->comboBoxMask,SIGNAL(currentIndexChanged(QString)),this,SLOT(mask_edit()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(count_changed()));
    connect(ui->pushButtonOk,SIGNAL(clicked()),this,SLOT(press_ok()));
    connect(ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(press_cancel()));
    connect(ui->tableWidgetArticle,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(table_doubleclick()));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDialogArticleAllowance::~CInputDialogArticleAllowance()
{
    settings(true);//save settings if update
    delete ui;
}

void CInputDialogArticleAllowance::keyPressEvent(QKeyEvent * event)
{
    if(event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return)
    {
        if(ui->lineEditMask->hasFocus())
            mask_changed();
    }
}

bool CInputDialogArticleAllowance::get_checkbox_not_close_the_dialog(void)
{
    return ui->checkBoxNotCloseTheDialog->isChecked();
}

bool CInputDialogArticleAllowance::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;
    lsSType.push_back(QString("DLG_ARTICLE_ALLOWANCE_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_ALLOWANCE"));
    lsSType.push_back(QString("AUTO_CHECKBOX_ARTICLE_ALLOWANCE"));
    lsSType.push_back(QString("MASK_SELECTION_DLG_ALLOWANCE"));
    lsSType.push_back(QString("WARELIST_DLG_NOT_CLOSE_BY_ADD"));
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //table width
            if(lsSValue.count()>0)
                settings.set_table_columns_width(ui->tableWidgetArticle,lsSValue[0],200);
            //table sort
            if(lsSValue.count()>1)
                settings.set_table_sort(ui->tableWidgetArticle,lsSValue[1],0);
            //auto checkbox
            if(lsSValue.count()>2)
                settings.set_checkbox(ui->checkBoxAuto,lsSValue[2],false);
            //mask selection
            if(lsSValue.count()>3)
                settings.set_combobox(ui->comboBoxMask,lsSValue[3],0);
            //warelist not close by add
            if(lsSValue.count()>4)
                settings.set_checkbox(ui->checkBoxNotCloseTheDialog,lsSValue[4],false);
        }
        if(bUpdate)//write
        {
            //table width
            if(lsSValue.count()>0)
            {
                if(settings.get_table_columns_width(ui->tableWidgetArticle,lsSValue[0]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[0]);
                    lsSUpdateValue.push_back(lsSValue[0]);
                }
            }
            //table sort
            if(lsSValue.count()>1)
            {
                if(settings.get_table_sort(ui->tableWidgetArticle,lsSValue[1]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[1]);
                    lsSUpdateValue.push_back(lsSValue[1]);
                }
            }
            //checkbox auto
            if(lsSValue.count()>2)
            {
                if(settings.get_checkbox(ui->checkBoxAuto,lsSValue[2]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[2]);
                    lsSUpdateValue.push_back(lsSValue[2]);
                }
            }
            //combobox
            if(lsSValue.count()>3)
            {
                if(settings.get_combobox(ui->comboBoxMask,lsSValue[3]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[3]);
                    lsSUpdateValue.push_back(lsSValue[3]);
                }
            }
            //checkbox warelist
            if(lsSValue.count()>4)
            {
                if(settings.get_checkbox(ui->checkBoxNotCloseTheDialog,lsSValue[4]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[4]);
                    lsSUpdateValue.push_back(lsSValue[4]);
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

bool CInputDialogArticleAllowance::set(CWorkThread * pThread, QTableWidget * pParentTable, int iColumnCount, int iColumnId, int iType, int iArticleId, QDate dtDate)
{
    m_pThread=pThread;
    m_pParentTable=pParentTable;
    m_iParentColumnCount=iColumnCount;
    m_iParentColumnId=iColumnId;
    m_iArticleId=iArticleId;
    m_iType=iType;
    m_dtDate=dtDate;

    //set db in class CLastDBChange
    if(pThread!=NULL)
    {
        m_LastDbChange.set_db(pThread->m_pDbInterface);
    }

    //ok-button
    if(m_iArticleId>0)
    {//edit-mode
        ui->pushButtonOk->setText(QString::fromUtf8("Änderung(en) anwenden"));
        ui->pushButtonCancel->setText(QString::fromUtf8("abbrechen"));
        ui->pushButtonOk->setEnabled(true);
    }

    //update article table with article
    update_table();

    //not add ware
    if(iArticleId!=-1)
        ui->checkBoxNotCloseTheDialog->setVisible(false);

    //find & select article with article id
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pWidgets!=NULL)
            m_pThread->m_pWidgets->find_and_select_row(ui->tableWidgetArticle,iArticleId);
    }
    return true;
}

void CInputDialogArticleAllowance::checkbox_auto_clicked(void)
{
    if(ui->checkBoxAuto->isChecked())
        ui->pushButtonMaskSet->setEnabled(false);
    else
        ui->pushButtonMaskSet->setEnabled(true);
}

void CInputDialogArticleAllowance::mask_edit(void)
{
    if(ui->checkBoxAuto->isChecked())
        mask_changed();
}

void CInputDialogArticleAllowance::mask_changed(void)
{
    update_table();
    article_changed();
}

void CInputDialogArticleAllowance::count_changed(void)
{
    check_count();
}

bool CInputDialogArticleAllowance::update_table(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    bool b;
    QString sCondition;
    QString sMask=ui->lineEditMask->text();
    int iMask=ui->comboBoxMask->currentIndex();
    int iFormatType=FORMAT_TWO;
    QList<int> lsIds;
    CPointerMemory memory;
    memory.set_int(&iFormatType);
    memory.set_string(&sCondition);
    memory.set_int_list(&lsIds);

    //-condition-
    if(sMask.length()<=0)//mask not set?
        sCondition=QString("");
    else
    {
        if(iMask==0)//name?
            sCondition=QString("name like '%%1%'").arg(sMask);
        else if(iMask==1)//number
            sCondition=QString("articlenumber like '%%1%'").arg(sMask);
        else if(iMask==2)//2.number
            sCondition=QString("articlenumber2 like '%%1%'").arg(sMask);
        else if(iMask==5)//location
            sCondition=QString("location like '%%1%'").arg(sMask);
        else
            sCondition=QString(sMask);
    }

    //-get article by mask-
    if(iMask==0 || iMask==1 || iMask==2 || iMask==5 || sMask.length()<=0)//name or numbers or location or no mask
        b=m_pThread->set_work(WORK_ARTICLE_GET_ALL,&memory);
    else if(iMask==3)//maker?
        b=m_pThread->set_work(WORK_ARTICLE_GET_ALL_BY_MAKER,&memory);
    else//waregroup?
        b=m_pThread->set_work(WORK_ARTICLE_GET_ALL_BY_WAREGROUP,&memory);

    //-fill table-
    if(b)
    {
        memory.set_tablewidget(ui->tableWidgetArticle);
        b=m_pThread->set_work(WORK_ARTICLE_UPDATE_TABLEWIDGET,&memory);
    }

    if(!b)//db error
    {
    }
    //-
    ui->pushButtonOk->setEnabled(false);
    //-
    return b;
}

void CInputDialogArticleAllowance::article_changed(void)
{
    check_count();
}

int CInputDialogArticleAllowance::get_old_count(int iArticleId)
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

int CInputDialogArticleAllowance::get_count_on_date(int iArticleId)
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

bool CInputDialogArticleAllowance::check_count(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;
    QList<QTableWidgetItem*> ls=ui->tableWidgetArticle->selectedItems();
    if(ls.count()<7)
        return false;//nothing article select
    //-
    CArticle ar;
    bool b=false;
    QString s;
    QStringList lss;
    int article_id,current_count,old_count,max_inventory,ordering_count,inventory,i,max_count_on_date;
    //-
    article_id=ls[6]->text().toInt(&b,10);//get artile id
    if(!b)// cast to int ok?
        return false;
    if(!m_pThread->m_pDbInterface->article_get(article_id,ar))//get article
        return false;
    //-
    max_count_on_date=get_count_on_date(article_id);
    //-
    ui->textBrowser->setTextColor(QColor(Qt::darkRed));
    ui->textBrowser->clear(); //clear error text field
    //-
    inventory=ar.get_inventory();
    max_inventory=ar.get_max_inventory();
    //-
    current_count=ui->spinBox->value();//get current count
    s=ls[0]->text();
    lss=s.split("/");
    if(lss.count()>1)
    {
        ordering_count=lss[1].toInt(&b,10);//get ordering count
        lss.clear();
        if(b)
        {
            old_count=get_old_count(article_id);//get old count from parent dialog
            if(old_count>=0)
            {
                if(m_iType==ARTICLE_DLG_TYPE_ORDERING && max_inventory>0)
                {
                    if(m_iArticleId==article_id)//edit mode
                        i=max_inventory-current_count-ordering_count-inventory;
                    else//add mode
                        i=max_inventory-current_count-old_count-ordering_count-inventory;
                    //-
                    if(i<0)//to many
                    {
                        s=QString::fromUtf8("Warnung: Die maximale Kapazität dieses Artikels wird beim Eingang der Bestellung um \"%1").arg(i*-1);
                        if(ar.get_unit().length()>0)
                            s+=QString(" %1").arg(ar.get_unit());
                        s+=QString::fromUtf8("\" überschritten.");
                        ui->textBrowser->insertPlainText(s);
                    }
                    b=true;
                }
                //-
                if(m_iType==ARTICLE_DLG_TYPE_INCOMING && max_inventory>0)
                {
                    if(m_iArticleId==article_id)//edit mode
                        i=max_inventory-current_count-inventory;
                    else//add mode
                        i=max_inventory-current_count-old_count-inventory;
                    //-
                    if(i<0)//to many
                    {
                        s=QString::fromUtf8("Warnung: Die maximale Kapazität dieses Artikels wird um \"%1").arg(i*-1);
                        if(ar.get_unit().length()>0)
                            s+=QString::fromUtf8(" %1").arg(ar.get_unit());
                        s+=QString::fromUtf8("\" überschritten.");
                        ui->textBrowser->insertPlainText(s);
                    }
                    b=true;
                }
                //-
                if(m_iType==ARTICLE_DLG_TYPE_OUTGOING)
                {
                    i=inventory;
                    if(max_count_on_date<inventory)
                        i=max_count_on_date;
                    if(m_iArticleId==article_id)//edit mode
                    {
                        if(current_count > i)//to many
                            b=false;
                    }
                    else//add mode
                    {
                        if(current_count > (i-old_count))//to many
                            b=false;
                    }
                    //-
                    if(!b)//error
                    {
                        s=QString("Fehler: Am '%1' liegt der Bestand dieses Artikels bei").arg(m_dtDate.toString("dd.MM.yyyy"));
                        s+=QString("\"%1").arg(i);
                        if(ar.get_unit().length()>0)
                            s+=QString(" %1").arg(ar.get_unit());
                        s+=QString("\", beachten Sie ggf. die chronologische Reihenfolge von Warenein-/ausgang.");
                        ui->textBrowser->insertPlainText(s);
                    }
                }
            }
        }
    }
    //-
    ui->pushButtonOk->setEnabled(b);
    //-
    return b;
}

bool CInputDialogArticleAllowance::get_data(QString & sData)
{
    QList <QTableWidgetItem*> ls=ui->tableWidgetArticle->selectedItems();
    if(ls.count()<7)
        return false;
    if(ui->spinBox->value()<=0)
        return false;
    sData=QString("%1x").arg(ui->spinBox->text());
    sData+=ls[6]->text();
    //-
    return true;
}

bool CInputDialogArticleAllowance::set_count(int iCount)
{
    ui->spinBox->setValue(iCount);//set count
    return true;
}

int CInputDialogArticleAllowance::get_count(void)
{
    return ui->spinBox->value();//get count
}

bool CInputDialogArticleAllowance::set_mask(QString sMask)
{
    ui->lineEditMask->setText(sMask);
    return true;
}

QString CInputDialogArticleAllowance::get_mask(void)
{
    return ui->lineEditMask->text();
}

void CInputDialogArticleAllowance::press_ok(void)
{
    QString sData;

    //check interim time another client change db
    if(check_count())
    {//value ok?

        //add row in table
        if(m_iArticleId<=0)
        {//add-mode
            get_data(sData);
            insert_ware_at_parent_table(sData);
        }
        //-
        if(!ui->checkBoxNotCloseTheDialog->isChecked() || m_iArticleId>0)//not checked or edit-mode
            done(1);
        else
        {
            ui->textBrowser->setTextColor(QColor(Qt::darkGreen));
            ui->textBrowser->setText(QString::fromUtf8("Artikel zur Warenliste hinzugefügt."));
            ui->tableWidgetArticle->clearSelection();
            //-
            ui->spinBox->setFocus();
            ui->spinBox->findChild<QLineEdit*>()->setSelection(0,9999999);//select text value
        }
    }
}

void CInputDialogArticleAllowance::press_cancel(void)
{
    close();
}

bool CInputDialogArticleAllowance::insert_ware_at_parent_table(QString sData)
{
    bool b=false,bEdit=false;
    //-
    if(m_pThread!=NULL && m_pParentTable!=NULL)
    {
        if(m_pThread->m_pWidgets!=NULL)
        {
            if(m_iArticleId>0)
                bEdit=true;
            //-
            m_pThread->m_pWidgets->article_update_row_wareslist(m_pParentTable,sData,bEdit,true);
        }
    }
    //-
    return b;
}

void CInputDialogArticleAllowance::table_doubleclick(void)
{
    int iId=-1;
    QString sPath;
    CArticle ar;
    CPictureViewDialog dlg(this);
    //-
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pWidgets!=NULL && m_pThread->m_pDbInterface!=NULL)
        {
            //open dialog & draw picture
            if(m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetArticle,iId))//select?
            {
                if(m_pThread->m_pDbInterface->article_get(iId,ar))
                {
                    sPath=ar.get_path_picture();
                    if(sPath.length()>0)
                    {
                        if(dlg.set_data(sPath))
                            dlg.exec();
                    }
                }
            }
        }
    }
}
