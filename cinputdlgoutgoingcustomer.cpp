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

#include "cinputdlgoutgoingcustomer.h"
#include "ui_cinputdlgoutgoingcustomer.h"

CInputDlgOutgoingCustomer::CInputDlgOutgoingCustomer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDlgOutgoingCustomer)
{
    ui->setupUi(this);
    m_dtMark=QDate::currentDate();
    settings(false);//load & set settings

    ui->dateEdit->setDate(QDate().currentDate());//set current date
    ui->pushButtonOk->setEnabled(false);
    ui->pushButtonDelete->setEnabled(false);
    ui->pushButtonEdit->setEnabled(false);

    //disable auto default buttons
    ui->pushButtonAddBarcode->setAutoDefault(false);
    ui->pushButtonCancel->setAutoDefault(false);
    ui->pushButtonDelete->setAutoDefault(false);
    ui->pushButtonEdit->setAutoDefault(false);
    ui->pushButtonMaskSet->setAutoDefault(false);
    ui->pushButtonNew->setAutoDefault(false);
    ui->pushButtonNomination->setAutoDefault(false);
    ui->pushButtonOk->setAutoDefault(false);

    //date
    QDate dt=QDate().currentDate();
    ui->dateEdit->setMaximumDate(dt);
    dt=QDate(2009,1,1);
    ui->dateEdit->setMinimumDate(dt);
    ui->dateEdit->setFocus();

    //context menu
    ui->tableWidgetWares->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pContextMenu=new QMenu(QString("context_default"),this);
    if(m_pContextMenu!=NULL)
    {
        m_pContextMenu->addAction(QString::fromUtf8("Hinzufügen"));
        m_pContextMenu->addAction(QString("Bearbeiten"));
        m_pContextMenu->addAction(QString::fromUtf8("Löschen"));
    }

    //connects context menus
    connect(m_pContextMenu,SIGNAL(triggered(QAction*)),this,SLOT(receiv_context_menu(QAction*)));
    connect(ui->tableWidgetWares,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));

    //connects
    connect(ui->checkBoxAuto,SIGNAL(clicked()),this,SLOT(checkbox_auto_clicked()));
    connect(ui->lineEditMask,SIGNAL(textChanged(QString)),this,SLOT(mask_edit()));
    connect(ui->comboBoxMask,SIGNAL(currentIndexChanged(QString)),this,SLOT(mask_edit()));
    connect(ui->pushButtonMaskSet,SIGNAL(clicked()),this,SLOT(update_customer_table()));
    connect(ui->pushButtonNew,SIGNAL(clicked()),this,SLOT(add_ware()));
    connect(ui->pushButtonAddBarcode,SIGNAL(clicked()),this,SLOT(add_ware_barcode()));
    connect(ui->pushButtonDelete,SIGNAL(clicked()),this,SLOT(delete_ware()));
    connect(ui->pushButtonEdit,SIGNAL(clicked()),this,SLOT(edit_ware()));
    connect(ui->pushButtonNomination,SIGNAL(clicked()),this,SLOT(set_booking_number_nomination()));
    connect(ui->lineEditBookingnumber,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->tableWidgetWares,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(edit_ware()));
    connect(ui->tableWidgetWares,SIGNAL(itemSelectionChanged()),this,SLOT(check_user_input()));
    connect(ui->tableWidgetCustomer,SIGNAL(itemSelectionChanged()),this,SLOT(check_user_input()));
    connect(ui->dateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(date_changed(QDate)));
    connect(ui->pushButtonOk,SIGNAL(clicked()),this,SLOT(press_ok()));
    connect(ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(press_cancel()));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDlgOutgoingCustomer::~CInputDlgOutgoingCustomer()
{
    killTimer(m_iTimerId);
    settings(true);//save settings if update

    //context menu's
    if(m_pContextMenu!=NULL)
        delete m_pContextMenu;

    delete ui;
}

void CInputDlgOutgoingCustomer::keyPressEvent(QKeyEvent * event)
{
    //ENTER-key
    if(event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return)
    {
        if(ui->lineEditMask->hasFocus())
            update_customer_table();
    }

    //DELETE-key
    QList<QTableWidgetItem*> ls;
    if(event->key()==Qt::Key_Delete)
    {
        if(ui->tableWidgetWares->hasFocus())//trade
        {
            ls=ui->tableWidgetWares->selectedItems();
            if(ls.count()>0)//row selected?
                delete_ware();
        }
    }
    ls.clear();
}

void CInputDlgOutgoingCustomer::timerEvent(QTimerEvent *event)
{
    bool b=false;
    QString s;
    int id,type;
    //-
    if(event!=NULL && m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL && m_pThread->m_pWidgets!=NULL && m_LastDbChange.check_update_last_change())
        {//db change from another client?
            s=m_LastDbChange.get_last_change();//get fom db
            if(m_LastDbChange.get_data_from_last_change_string(type,id,s))//split string
            {
                //orderings update?
                if(type==LC_ACTION_UPDATE_CUSTOMER || type==LC_ACTION_UPDATE_ALL )
                {
                    update_customer_table();
                    b=true;
                }

                //article's update?
                if(type==LC_ACTION_UPDATE_MAKER || type==LC_ACTION_UPDATE_WAREGROUP || type==LC_ACTION_UPDATE_ARTICLE_INV
                        || type==LC_ACTION_UPDATE_ALL || type==LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG || type==LC_ACTION_UPDATE_INVENTORY_TRADE)
                {//update all rows
                    update_wares_table();
                    b=true;
                }
            }
        }
    }

    //message in window title, if another client edit db
    static QString mark_title=QString("");
    static int counter=0;
    static bool mark_message_on=false;
    //-
    if(b)
    {
        if(!mark_message_on)//message not on screen
        {
            counter=3;
            mark_message_on=true;
            mark_title=windowTitle();
            s=mark_title+QString::fromUtf8(" - Datenbank wurde von einer anderen Programminstanz verändert");
            setWindowTitle(s);
        }
    }

    //counter run?
    if(mark_message_on)
    {
        counter-=1;
        if(counter<0)
        {
            setWindowTitle(mark_title);
            mark_message_on=false;
        }
    }
}

bool CInputDlgOutgoingCustomer::open_context_menu()
{
    if(m_pContextMenu==NULL)
        return false;
    //-
    bool bEditDeleteEnable=true;
    QList<QAction*> lsAct;
    QList<QTableWidgetItem*> ls;

    ls=ui->tableWidgetWares->selectedItems();
    if(ls.count()<=0)//nothing item selected?
        bEditDeleteEnable=false;
    //-
    lsAct=m_pContextMenu->actions();
    if(lsAct.count()>2)
    {
        lsAct[1]->setEnabled(bEditDeleteEnable);
        lsAct[2]->setEnabled(bEditDeleteEnable);
    }
    //-
    m_pContextMenu->popup(QCursor::pos());
    //-
    ls.clear();
    lsAct.clear();
    return true;
}

bool CInputDlgOutgoingCustomer::receiv_context_menu(QAction * pAction)
{
    if(pAction==NULL)
        return false;

    if(pAction->text()==QString::fromUtf8("Hinzufügen"))
        add_ware();
    if(pAction->text()==QString("Bearbeiten"))
        edit_ware();
    if(pAction->text()==QString::fromUtf8("Löschen"))
        delete_ware();
    return true;
}

bool CInputDlgOutgoingCustomer::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;
    lsSType.push_back(QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_CUS_OUT_CUS"));
    lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_TRADE_CUSTOMER_CUS"));
    lsSType.push_back(QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_CUS_OUT_WARES"));
    lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_TRADE_CUSTOMER_WARES"));
    lsSType.push_back(QString("AUTO_CHECKBOX_TRADE_CUSTOMER_OUTGOING"));
    lsSType.push_back(QString("MASK_SELECTION_DLG_TRADE_CUSTOMER"));
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //table width
            if(lsSValue.count()>0)
                settings.set_table_columns_width(ui->tableWidgetCustomer,lsSValue[0],200);
            //table sort
            if(lsSValue.count()>1)
                settings.set_table_sort(ui->tableWidgetCustomer,lsSValue[1],1);
            //table2 width
            if(lsSValue.count()>2)
                settings.set_table_columns_width(ui->tableWidgetWares,lsSValue[2],200);
            //table2 sort
            if(lsSValue.count()>3)
                settings.set_table_sort(ui->tableWidgetWares,lsSValue[3],1);
            //auto checkbox
            if(lsSValue.count()>4)
                settings.set_checkbox(ui->checkBoxAuto,lsSValue[4],false);
            //mask selection
            if(lsSValue.count()>5)
                settings.set_combobox(ui->comboBoxMask,lsSValue[5],0);
        }
        if(bUpdate)//write
        {
            //table width
            if(lsSValue.count()>0)
            {
                if(settings.get_table_columns_width(ui->tableWidgetCustomer,lsSValue[0]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[0]);
                    lsSUpdateValue.push_back(lsSValue[0]);
                }
            }
            //table sort
            if(lsSValue.count()>1)
            {
                if(settings.get_table_sort(ui->tableWidgetCustomer,lsSValue[1]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[1]);
                    lsSUpdateValue.push_back(lsSValue[1]);
                }
            }
            //table2 width
            if(lsSValue.count()>2)
            {
                if(settings.get_table_columns_width(ui->tableWidgetWares,lsSValue[2]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[2]);
                    lsSUpdateValue.push_back(lsSValue[2]);
                }
            }
            //table2 sort
            if(lsSValue.count()>3)
            {
                if(settings.get_table_sort(ui->tableWidgetWares,lsSValue[3]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[3]);
                    lsSUpdateValue.push_back(lsSValue[3]);
                }
            }
            //checkbox
            if(lsSValue.count()>4)
            {
                if(settings.get_checkbox(ui->checkBoxAuto,lsSValue[4]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[4]);
                    lsSUpdateValue.push_back(lsSValue[4]);
                }
            }
            //combobox
            if(lsSValue.count()>5)
            {
                if(settings.get_combobox(ui->comboBoxMask,lsSValue[5]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[5]);
                    lsSUpdateValue.push_back(lsSValue[5]);
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

bool CInputDlgOutgoingCustomer::set_thread(CWorkThread * pThread)
{
    m_pThread=pThread;
    //set db in class CLastDBChange
    if(pThread!=NULL)
    {
        m_LastDbChange.set_db(pThread->m_pDbInterface);
        m_iTimerId=startTimer(1000);//start timer
    }
    return true;
}

bool CInputDlgOutgoingCustomer::set_booking_number_nomination(void)
{
    if(m_pThread==NULL)
        return false;

    QString s;
    QDate dt=ui->dateEdit->date();
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_date(&dt);

    //-booking number nomination-
    m_pThread->set_work(WORK_TRADE_GET_NEXT_FREE_BOOKING_NUMBER,&memory);
    if(s.length()>0)//found?
        ui->lineEditBookingnumber->setText(s);

    memory.clear();
    return true;
}

bool CInputDlgOutgoingCustomer::date_changed(QDate dtNew)
{
    if(m_pThread==NULL)
        return false;

    //check article counts
    check_article_counts_on_date();

    //if not handmake booking number set nomination
    QString s,sBookingNumber=ui->lineEditBookingnumber->text();
    CPointerMemory memory;
    memory.set_date(&m_dtMark);
    memory.set_string(&s);
    m_pThread->set_work(WORK_TRADE_GET_NEXT_FREE_BOOKING_NUMBER,&memory);
    if(s.length()>0)
    {
        if(s==sBookingNumber)//not handmake-> update
            set_booking_number_nomination();
    }
    m_dtMark=dtNew;
    memory.clear();
    return true;
}

bool CInputDlgOutgoingCustomer::init(void)
{
    update_customer_table();//update customer
    set_booking_number_nomination();
    return true;
}

bool CInputDlgOutgoingCustomer::update_customer_table(void)
{
    if(m_pThread==NULL)
        return false;
    //-
    bool b=true;
    int iIndex=ui->comboBoxMask->currentIndex();
    QString sCondition,sMask=ui->lineEditMask->text();
    QList<int> lsIds;
    CPointerMemory memory;
    memory.set_string(&sCondition);
    memory.set_int_list(&lsIds);
    memory.set_tablewidget(ui->tableWidgetCustomer);

    //-search by?
    if(sMask.length()<=0)//no mask->all
        sCondition=QString("");
    else if(iIndex==0)//name
        sCondition=QString("name like '%%1%'").arg(sMask);
    else if(iIndex==1)//first name
        sCondition=QString("first_name like '%%1%'").arg(sMask);
    else if(iIndex==2)//street
        sCondition=QString("street like '%%1%'").arg(sMask);
    else if(iIndex==3)//city
        sCondition=QString("city like '%%1%'").arg(sMask);
    else if(iIndex==4)//postcode
        sCondition=QString("postcode like '%%1%'").arg(sMask);
    else if(iIndex==5)//customer number
        sCondition=QString("customer_number like '%%1%'").arg(sMask);
    else if(iIndex==6)//comment
        sCondition=QString("comment like '%%1%'").arg(sMask);
    //-
    if(!m_pThread->set_work(WORK_CUSTOMER_GET_ALL,&memory))//get all by mask
        return false;//error

    //update table
    if(!m_pThread->set_work(WORK_CUSTOMER_UPDATE_TABLEWIDGET,&memory))
        b=false;//error
    //-
    check_user_input();
    //-
    return b;
}

bool CInputDlgOutgoingCustomer::update_wares_table(void)
{
    bool b;
    int i,j,id;
    QString s;
    CArticle ar;
    QTableWidgetItem * pItem;
    QList<CTableItemData> lsData;
    //-
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL && m_pThread->m_pWidgets!=NULL)
        {
            for(i=0;i<ui->tableWidgetWares->rowCount();i++)
            {
                //get article id
                pItem=ui->tableWidgetWares->item(i,ui->tableWidgetWares->columnCount()-1);//last column->id
                if(pItem!=NULL)
                {
                    s=pItem->text();
                    id=s.toInt(&b);
                }
                if(b)
                {
                    //get article
                    if(!m_pThread->m_pDbInterface->article_get(id,ar))
                        b=false;//article not found
                    else
                    {
                        //update article data in table
                        if(m_pThread->m_pWidgets->article_format(ar,lsData,FORMAT_THREE))//get format text
                        for(j=1;j<ui->tableWidgetWares->columnCount()-1 && j<lsData.count();j++)
                        {
                            pItem=ui->tableWidgetWares->item(i,j);
                            if(pItem!=NULL)
                            {
                                if(pItem->text() != lsData[j-1].get_text())//article data update?
                                    pItem->setText(lsData[j-1].get_text());
                            }
                        }
                    }
                }
                //-
                if(!b)
                {//error->delete row
                    ui->tableWidgetWares->removeRow(i);
                    i--;
                }
                //-
                lsData.clear();
            }
        }
    }
    //-
    if(ui->tableWidgetWares->rowCount()<=0)//all rows delete?
        ui->pushButtonOk->setEnabled(false);
    //-
    return true;
}

void CInputDlgOutgoingCustomer::mask_edit(void)
{
    if(ui->checkBoxAuto->isChecked())
        update_customer_table();
}

void CInputDlgOutgoingCustomer::checkbox_auto_clicked(void)
{
    if(ui->checkBoxAuto->isChecked())
        ui->pushButtonMaskSet->setEnabled(false);
    else
        ui->pushButtonMaskSet->setEnabled(true);
}

bool CInputDlgOutgoingCustomer::add_ware(void)
{
    CInputDialogArticleAllowance dlg;
    QDate dt=ui->dateEdit->date();//get selected date
    //-
    if(m_pThread!=NULL)
    {
        dlg.set(m_pThread,ui->tableWidgetWares,0,6,ARTICLE_DLG_TYPE_OUTGOING,-1,dt);
        dlg.setWindowTitle(QString::fromUtf8("hinzufügen"));
        if(dlg.exec())
            check_user_input();
    }
    //-
    return true;
}

bool CInputDlgOutgoingCustomer::add_ware_barcode(void)
{
    CInputDlgBarcode dlg;
    QDate dt=ui->dateEdit->date();//get selected date
    //-
    if(m_pThread!=NULL)
    {
        dlg.set(m_pThread,ui->tableWidgetWares,0,6,ARTICLE_DLG_TYPE_OUTGOING,dt);
        dlg.setWindowTitle(QString::fromUtf8("hinzufügen (Barcode)"));
        if(dlg.exec())
            check_user_input();
    }
    //-
    return true;
}

bool CInputDlgOutgoingCustomer::delete_ware(void)
{
    int iRow=ui->tableWidgetWares->currentRow();
    QList <QTableWidgetItem*> ls=ui->tableWidgetWares->selectedItems();
    if(ls.count()>0)//selected?
        ui->tableWidgetWares->removeRow(iRow);
    check_user_input();
    return true;
}

bool CInputDlgOutgoingCustomer::edit_ware(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pWidgets==NULL)
        return false;

    bool b,bReturn=false;
    QString sData;
    int iCount,iArticleId,iArticleIdAfter;
    CInputDialogArticleAllowance dlg;
    QDate dt=ui->dateEdit->date();//get selected date
    //-
    if(m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetWares,iCount,0))//get count from selected row
    {
        if(m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetWares,iArticleId))//get article id from selected row
        {
            dlg.set_count(iCount);
            dlg.set(m_pThread,ui->tableWidgetWares,0,6,ARTICLE_DLG_TYPE_OUTGOING,iArticleId,dt);
            dlg.setWindowTitle("bearbeiten");
            b=dlg.exec();
            if(b)
            {
                if(dlg.get_data(sData))
                {
                    if(m_pThread->m_pWidgets->cast_count_and_id(sData,iCount,iArticleIdAfter))
                    {
                        if(iArticleId==iArticleIdAfter)//same article? edit mode
                            insert_ware_at_table(sData,true,true);
                        else
                        {
                            insert_ware_at_table(sData,false,true);//antoher article? add mode
                            m_pThread->m_pWidgets->remove_row(ui->tableWidgetWares,iArticleId);//remove old item
                        }
                        bReturn=true;
                    }
                }
            }
        }
    }
    return bReturn;
}

bool CInputDlgOutgoingCustomer::check_user_input(void)
{
    if(m_pThread==NULL)
        return false;

    int count;
    bool b=true;
    QString s,s2,sError=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    QList<QTableWidgetItem*> ls;

    //edit & delete button
    ls=ui->tableWidgetWares->selectedItems();
    if(ls.count()<=0)//nothing article selected
    {
        ui->pushButtonEdit->setEnabled(false);
        ui->pushButtonDelete->setEnabled(false);
    }
    else
    {
        ui->pushButtonEdit->setEnabled(true);
        ui->pushButtonDelete->setEnabled(true);
    }
    ls.clear();

    //check booking number
    s2=ui->lineEditBookingnumber->text();
    s=QString("booking_number='%1'").arg(s2);
    m_pThread->set_work(WORK_TRADE_GET_COUNT,&memory);
    if(count>0)//give it booking number ? ->error
    {
        sError=QString("Fehler: Buchungsnummer gibt es schon");
        b=false;
    }
    else
    {
        //no customer select ?
        ls=ui->tableWidgetCustomer->selectedItems();
        if(ls.count()<=0)
        {
            sError=QString::fromUtf8("Fehler: kein Kunde ausgewählt");
            b=false;
        }
        else
        {
            //no article at table?
            count=ui->tableWidgetWares->rowCount();
            if(count<=0)
            {
                sError=QString("Fehler: keine Artikel in der Warenliste");
                b=false;
            }
        }
    }
    //-
    ui->labelError->setText(sError);
    ui->pushButtonOk->setEnabled(b);
    memory.clear();
    ls.clear();
    return b;
}

bool CInputDlgOutgoingCustomer::insert_ware_at_table(QString sData,bool bEdit,bool bSelect)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pWidgets==NULL)
        return false;

    bool b=m_pThread->m_pWidgets->article_update_row_wareslist(ui->tableWidgetWares,sData,bEdit,bSelect);
    return b;
}

bool CInputDlgOutgoingCustomer::check_article_counts_on_date(void)
{
    QMessageBox msg(QMessageBox::Information,"","");
    QDate dt=ui->dateEdit->date();
    QString s;
    bool b,bToMany=false,bReturn=true;
    QTableWidgetItem * pItem=NULL;
    int i,rows,count,id;
    unsigned int max;
    CPointerMemory memory;
    memory.set_uint(&max);
    memory.set_date(&dt);
    memory.set_int(&id);
    //-
    if(m_pThread!=NULL)//db connection?
    {
        //check article counts of max. count on the date
        rows=ui->tableWidgetWares->rowCount();
        for(i=0;i<rows;i++)
        {
            pItem=ui->tableWidgetWares->item(i,6);
            if(pItem!=NULL)
            {
                s=pItem->text();
                id=s.toInt(&b,10);//article id
                if(b)//number?
                {
                    pItem=ui->tableWidgetWares->item(i,0);
                    if(pItem!=NULL)
                    {
                        s=pItem->text();
                        count=s.toInt(&b,10);//count
                        if(b)//number?
                        {
                            m_pThread->set_work(WORK_ARTICLE_GET_INVENTORY_ON_DATE,&memory);//get max.count on date
                            if((unsigned int)count>max)//to many on this date
                            {
                                bReturn=false;
                                s=QString("%1").arg(max);
                                if(max>0)
                                    pItem->setText(s);//set max.count
                                else
                                {//error 0 x article
                                    bToMany=true;
                                    ui->tableWidgetWares->removeRow(i);
                                    i--;
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //-
    if(bToMany)
    {
        s=QString::fromUtf8("Der/Die max.Warenausgang/-gänge der Artikel in der Liste an dem Datum wurde(n) überschritten,");
        s+=QString::fromUtf8("beachten Sie die chronologische Reihenfolge von Warenein-/ausgang! Die Anzahl wurde jeweils angepasst.");
        msg.setWindowTitle("Information:");
        msg.setText(s);
        msg.exec();
    }
    //-
    return bReturn;
}

bool CInputDlgOutgoingCustomer::get_data(CTrade & tr)
{
    if(m_pThread==NULL)
        return true;
    if(m_pThread->m_pDbInterface==NULL)
        return false;

    QList <QString> lsWares;
    QString sWare,s,s2;
    QDate dt;
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_date(&dt);
    int type=TYPE_CUSTOMER_OUTGOING;
    int id=-1;
    bool b=false;
    CCustomer cu;

    //customer
    if(m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetCustomer,id))//get custumer id from selected row
    {
        if(m_pThread->m_pDbInterface->customer_get(id,cu))//get customer record
        {
            //info
            tr.set_info_1(cu.get_customernumber());
            //-
            s=QString("");
            s2=cu.get_name();
            if(s2.length()>0)
                s+=s2;
            s2=cu.get_first_name();
            if(s2.length()>0)
                s+=QString(", %1").arg(s2);
            tr.set_info_2(s);
            //-
            tr.set_info_3(cu.get_street());
            //-
            s=QString("");
            s2=cu.get_postcode();
            if(s2.length()>0)
                s+=s2;
            s2=cu.get_city();
            if(s2.length()>0)
                s+=QString(", %1").arg(s2);
            tr.set_info_4(s);
            //-

            check_article_counts_on_date();//check all counts from articles on the date
            tr.set_type(type);
            //-
            tr.set_comment(ui->lineEditComment->text());
            //date
            dt=ui->dateEdit->date();
            tr.set_date(dt);
            //wares
            for(int i=0;i<ui->tableWidgetWares->rowCount();i++)
            {
                sWare=QString("%1x%2").arg(ui->tableWidgetWares->item(i,0)->text(),ui->tableWidgetWares->item(i,6)->text());//how many x article_id
                lsWares.push_back(sWare);
            }
            tr.set_wares(lsWares);
            //-booking number-
            m_pThread->set_work(WORK_TRADE_GET_NEXT_FREE_BOOKING_NUMBER,&memory);
            if(s.length()>0)//found?
            {
                tr.set_booking_number(s);
                b=true;
            }
        }
    }
    //-
    return b;
}

void CInputDlgOutgoingCustomer::press_ok(void)
{
    bool bOk=true;
    CTrade tr;
    QString s;
    QMessageBox msg(QMessageBox::Information,"","");
    msg.setWindowTitle("!");
    msg.setText(QString::fromUtf8("Artikel aus der Warenliste wurde(n) von einer anderen Programminstanz gelöscht!"));

    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            if(check_article_counts_on_date())
            {
                if(check_user_input())
                {
                    //check article in list -> another client delete?
                    get_data(tr);
                    if(!m_pThread->m_pDbInterface->check_article_at_wares(tr.get_wares(),s))
                    {//error article was delete
                        msg.exec();
                        bOk=false;
                    }
                    //-
                    if(bOk)
                        done(1);
                    else
                        close();
                }
            }
        }
    }
}

void CInputDlgOutgoingCustomer::press_cancel(void)
{
    close();
}
