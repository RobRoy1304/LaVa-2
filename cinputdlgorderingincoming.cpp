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

#include "cinputdlgorderingincoming.h"
#include "ui_cinputdlgorderingincoming.h"

CInputDlgOrderingIncoming::CInputDlgOrderingIncoming(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDlgOrderingIncoming)
{
    ui->setupUi(this);
    m_pThread=NULL;
    m_dtMark=QDate::currentDate();
    //-
    ui->pushButtonEdit->setEnabled(false);
    ui->pushButtonOk->setEnabled(false);
    settings(false);//load & set settings
    ui->dateEdit->setDate(QDate().currentDate());

    //date
    QDate dt=QDate().currentDate();
    ui->dateEdit->setMaximumDate(dt);
    dt=QDate(2009,1,1);
    ui->dateEdit->setMinimumDate(dt);

    //context menu
    ui->tableWidgetWares->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pContextMenu=new QMenu(QString("context_default"),this);
    if(m_pContextMenu!=NULL)
        m_pContextMenu->addAction(QString("Anzahl eingegangener Artikel ändern"));

    //connects context menus
    connect(m_pContextMenu,SIGNAL(triggered(QAction*)),this,SLOT(receiv_context_menu(QAction*)));
    connect(ui->tableWidgetWares,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));

    //connects-
    connect(ui->tableWidgetOrdering,SIGNAL(itemSelectionChanged()),this,SLOT(ordering_table_selected()));
    connect(ui->tableWidgetWares,SIGNAL(itemSelectionChanged()),this,SLOT(wares_table_selected()));
    connect(ui->pushButtonEdit,SIGNAL(clicked()),this,SLOT(ware_count_edit()));
    connect(ui->tableWidgetWares,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(double_click_warelist_table()));
    connect(ui->pushButtonNomination,SIGNAL(clicked()),this,SLOT(set_booking_number_nomination()));
    connect(ui->lineEditBookingnumber,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->dateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(date_changed(QDate)));
    connect(ui->checkBoxAuto,SIGNAL(clicked()),this,SLOT(checkbox_auto_clicked()));
    connect(ui->lineEditMask,SIGNAL(textChanged(QString)),this,SLOT(mask_edit()));
    connect(ui->comboBoxMask,SIGNAL(currentIndexChanged(QString)),this,SLOT(mask_edit()));
    connect(ui->pushButtonMaskSet,SIGNAL(clicked()),this,SLOT(mask_changed()));
    connect(ui->pushButtonOk,SIGNAL(clicked()),this,SLOT(press_ok()));
    connect(ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(press_cancel()));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDlgOrderingIncoming::~CInputDlgOrderingIncoming()
{
    killTimer(m_iTimerId);
    settings(true);//save settings if update

    //context menu's
    if(m_pContextMenu!=NULL)
        delete m_pContextMenu;

    delete ui;
}

void CInputDlgOrderingIncoming::keyPressEvent(QKeyEvent * event)
{
    if(event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return)
    {
        if(ui->lineEditMask->hasFocus())
            mask_changed();
    }
}

void CInputDlgOrderingIncoming::timerEvent(QTimerEvent *event)
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
                if(type==LC_ACTION_UPDATE_ORDERING || type==LC_ACTION_UPDATE_ORDERING_DEALER || type==LC_ACTION_UPDATE_ALL )
                {
                    fill_ordering_table();
                    b=true;
                }

                //article's update?
                if(type==LC_ACTION_UPDATE_MAKER || type==LC_ACTION_UPDATE_WAREGROUP || type==LC_ACTION_UPDATE_ARTICLE_INV
                        || type==LC_ACTION_UPDATE_ALL || type==LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG || type==LC_ACTION_UPDATE_INVENTORY_TRADE)
                {//update all rows
                    fill_wares_table();
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
            s=mark_title+QString(" - Datenbank wurde von einer anderen Programminstanz verändert");
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

bool CInputDlgOrderingIncoming::open_context_menu()
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
    if(lsAct.count()>0)
        lsAct[0]->setEnabled(bEditDeleteEnable);
    //-
    m_pContextMenu->popup(QCursor::pos());
    //-
    ls.clear();
    lsAct.clear();
    return true;
}

bool CInputDlgOrderingIncoming::receiv_context_menu(QAction * pAction)
{
    if(pAction==NULL)
        return false;
    else
        ware_count_edit();
    return true;
}

bool CInputDlgOrderingIncoming::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;
    lsSType.push_back(QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_ORD_IN_ORD"));
    lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_TRADE_ORDERING_ORD"));
    lsSType.push_back(QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_ORD_IN_WARES"));
    lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_TRADE_ORDERING_WARES"));
    lsSType.push_back(QString("AUTO_CHECKBOX_TRADE_ORDERING_INCOMING"));
    lsSType.push_back(QString("MASK_SELECTION_DLG_TRADE_ORDERING"));
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //table width
            if(lsSValue.count()>0)
                settings.set_table_columns_width(ui->tableWidgetOrdering,lsSValue[0],200);
            //table sort
            if(lsSValue.count()>1)
                settings.set_table_sort(ui->tableWidgetOrdering,lsSValue[1],0);
            //table2 width
            if(lsSValue.count()>2)
                settings.set_table_columns_width(ui->tableWidgetWares,lsSValue[2],200);
            //table2 sort
            if(lsSValue.count()>3)
                settings.set_table_sort(ui->tableWidgetWares,lsSValue[3],0);
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
                if(settings.get_table_columns_width(ui->tableWidgetOrdering,lsSValue[0]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[0]);
                    lsSUpdateValue.push_back(lsSValue[0]);
                }
            }
            //table sort
            if(lsSValue.count()>1)
            {
                if(settings.get_table_sort(ui->tableWidgetOrdering,lsSValue[1]))//setting change?
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

bool CInputDlgOrderingIncoming::set_booking_number_nomination(void)
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

bool CInputDlgOrderingIncoming::date_changed(QDate dtNew)
{
    if(m_pThread==NULL)
        return false;

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

void CInputDlgOrderingIncoming::mask_changed(void)
{
    fill_ordering_table();//fill table
}

void CInputDlgOrderingIncoming::mask_edit(void)
{
    if(ui->checkBoxAuto->isChecked())
        mask_changed();
}

void CInputDlgOrderingIncoming::checkbox_auto_clicked(void)
{
    if(ui->checkBoxAuto->isChecked())
        ui->pushButtonMaskSet->setEnabled(false);
    else
        ui->pushButtonMaskSet->setEnabled(true);
}

bool CInputDlgOrderingIncoming::double_click_warelist_table(void)
{
    ware_count_edit();
    return true;
}

bool CInputDlgOrderingIncoming::init(void)
{
    fill_ordering_table();
    set_booking_number_nomination();
    return true;
}

bool CInputDlgOrderingIncoming::set_thread(CWorkThread * pThread)
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

bool CInputDlgOrderingIncoming::check_user_input(void)
{
    if(m_pThread==NULL)
        return false;

    bool b=true;
    int count;
    QString s,s2,sError=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    QList <QTableWidgetItem*> ls;

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
        //check ordering select?
        ls=ui->tableWidgetOrdering->selectedItems();
        if(ls.count()<=0)
        {
            sError=QString("Fehler: keine Bestellung ausgewählt");
            b=false;//error
        }
    }
    //-
    ui->textBrowser->setText(sError);
    ui->pushButtonOk->setEnabled(b);
    memory.clear();
    ls.clear();
    return b;
}

bool CInputDlgOrderingIncoming::fill_ordering_table(void)
{
    if(m_pThread==NULL)
        return false;  
    if(m_pThread->m_pWidgets==NULL)
        return false;
    //-
    bool b=true;
    QString s,sMask=ui->lineEditMask->text();
    int iMask=ui->comboBoxMask->currentIndex();
    int iFormatType=FORMAT_TWO;
    QList<int> lsIds;
    CPointerMemory memory;
    memory.set_string(&sMask);
    memory.set_int_list(&lsIds);

    //-
    if(sMask.length()<=0)//no mask->get all
    {
        if(!m_pThread->set_work(WORK_ORDERING_GET_ALL,&memory))
            b=false;//error
        else{}
    }
    else if(iMask==0)//mask by dealer
    {
        if(!m_pThread->set_work(WORK_ORDERING_GET_ALL_BY_DEALER,&memory))
            b=false;//error
        else{}
    }
    else if(iMask==1)//mask by date
    {
        s=m_pThread->m_pWidgets->check_date(sMask);
        sMask=QString("date = '%1'").arg(s);
        if(!m_pThread->set_work(WORK_ORDERING_GET_ALL,&memory))
            b=false;//error
        else{}
    }
    else if(iMask==2)//mask by orderingnumber
    {
        s=QString("order_number like '%1%'").arg(sMask);
        sMask=s;
        if(!m_pThread->set_work(WORK_ORDERING_GET_ALL,&memory))
            b=false;//error
        else{}
    }
    else if(iMask==3)//mask by articlename
    {
        if(!m_pThread->set_work(WORK_ORDERING_GET_ALL_BY_ARTICLE,&memory))
            b=false;//error
        else{}
    }

    //update table
    memory.set_tablewidget(ui->tableWidgetOrdering);
    memory.set_int(&iFormatType);
    if(!m_pThread->set_work(WORK_ORDERING_UPDATE_TABLEWIDGET,&memory))
        b=false;//error
    //-
    ordering_table_selected();//update wareslist table
    //-
    return b;
}

bool CInputDlgOrderingIncoming::ordering_table_selected(void)
{
    fill_wares_table();
    check_user_input();
    ui->pushButtonEdit->setEnabled(false);
    return true;
}

bool CInputDlgOrderingIncoming::fill_wares_table(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL)
        return false;
    //-
    bool b=true;
    int i,id=-1;
    QString s;
    int iFormatType=FORMAT_TWO;
    CPointerMemory memory;
    memory.set_int(&id);
    memory.set_int2(&iFormatType);
    memory.set_tablewidget(ui->tableWidgetWares);
    QList <QTableWidgetItem*> ls=ui->tableWidgetOrdering->selectedItems();
    if(ls.count()<=0)
    {
        //clear wareslist table
        i=ui->tableWidgetWares->rowCount()-1;
        while(i>=0)
        {
            ui->tableWidgetWares->removeRow(i);
            i--;
        }
    }
    else
    {
        //get ordering id + cast to int
        s=ls[4]->text();//get ord.id
        id=s.toInt(&b,10);//cast
        if(b)//cast ok?
            b=m_pThread->set_work(WORK_ORDERING_UPDATE_TABLEWIDGET_WARES_LIST,&memory);
    }
    //-
    ls.clear();
    memory.clear();
    return b;
}

bool CInputDlgOrderingIncoming::wares_table_selected(void)
{
    bool b;
    QList <QTableWidgetItem*> ls=ui->tableWidgetWares->selectedItems();
    if(ls.count()<=0)//wares selected?
        b=false;
    else
        b=true;
    ui->pushButtonEdit->setEnabled(b);
    return true;
}

bool CInputDlgOrderingIncoming::ware_count_edit(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pWidgets==NULL)
        return false;

    CArticle ar;
    QString s,sError=QString("");
    bool b,bButton=true;
    int iNewCount=0,iCount=0,iArticleId=-1,inv,max_inv,i,j;
    QList <QTableWidgetItem*> ls=ui->tableWidgetWares->selectedItems();

    //block signals (sort)
    m_pThread->m_pWidgets->set_block_signal_and_sort(ui->tableWidgetWares,true,false);

    if(m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetWares,iCount,3))
    {
        if(m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetWares,iArticleId))
        {
            if(m_pThread->m_pDbInterface->article_get(iArticleId,ar))//get article - found?
            {
                //get new count
                s=QString("eingegangene Anzahl");
                if(ar.get_unit().length()>0)//unit?
                    s+=QString("(%1)").arg(ar.get_unit());
                s+=QString(":");
                iNewCount=QInputDialog::getInt(this,QString("Anzahl ändern"),s,iCount,0,99999,1,&b);
                if(b)
                {//pressed ok

                    //clear warning
                    ui->textBrowser->clear();

                    if(iNewCount!=iCount)
                    {//changed?

                        if(ls.count()>4)
                        {
                            //set new count
                            s=QString("%1").arg(iNewCount);
                            ls[3]->setText(s);//set new count

                            //warning?
                            max_inv=ar.get_max_inventory();
                            if(max_inv>0)//max.inventory set?
                            {
                                inv=ar.get_inventory();
                                if(inv+iNewCount > max_inv)
                                {
                                    sError=QString("Warnung: Die maximale Kapazität des Artikels \"%1\" ").arg(ar.get_name());
                                    sError+=QString("wird um \"%1").arg((max_inv-(inv+iNewCount))*-1);
                                    if(ar.get_unit().length()>0)
                                        sError+=QString(" %1").arg(ar.get_unit());
                                    sError+=QString("\" überschritten.");
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //check all count of article , all 0 ? -> error
    for(i=0,j=0;i<ui->tableWidgetWares->rowCount() && ui->tableWidgetWares->columnCount()>3;i++)
    {
        s=ui->tableWidgetWares->item(i,3)->text();
        if(s==QString("0"))
            j++;
    }
    if(j>=ui->tableWidgetWares->rowCount())//all article in table count of incoming 0? -> error
    {
        sError=QString("Fehler: der Wareneingang aller Artikel ist 0");
        bButton=false;//ok-button disable
    }
    //-
    ui->textBrowser->insertPlainText(sError);
    ui->pushButtonOk->setEnabled(bButton);
    ls.clear();

    //unblock signals (sort)
    m_pThread->m_pWidgets->set_block_signal_and_sort(ui->tableWidgetWares,false,true);
    //-
    return b;
}

bool CInputDlgOrderingIncoming::get_data(CTrade & trade, int & iOrderingId, bool & bCompleteIncoming)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pDbInterface==NULL || m_pThread->m_pWidgets==NULL)
        return false;
    //-
    bCompleteIncoming=true;
    bool b;
    QDate dt;
    int c1,c2;
    QString s,sDealerName,sCount1,sCount2,sId,s2;
    QTableWidgetItem * pItem=NULL;
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_date(&dt);

    //ordering selected?
    if(!m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetOrdering,iOrderingId))
        return false;

    //get  ordering
    COrdering ord;
    if(!m_pThread->m_pDbInterface->ordering_get(iOrderingId,ord))
        return false;

    //dealername
    sDealerName=m_pThread->m_pDbInterface->dealer_get_name(ord.get_dealer_id());

    //wares
    s=QString("");
    for(int i=0;i<ui->tableWidgetWares->rowCount();i++)
    {
        //count of article by ordering
        pItem=ui->tableWidgetWares->item(i,2);
        if(pItem!=NULL)
            sCount1=pItem->text();
        //count now
        pItem=ui->tableWidgetWares->item(i,3);
        if(pItem!=NULL)
            sCount2=pItem->text();
        //article id
        pItem=ui->tableWidgetWares->item(i,4);
        if(pItem!=NULL)
            sId=pItem->text();  
        //check  
        if(sCount1!=sCount2)
        {
            if(sCount2==QString("0"))
                bCompleteIncoming=false;
            else
            {
                c1=sCount1.toInt(&b,10);
                if(b)
                {
                    c2=sCount2.toInt(&b,10);
                    if(b)
                    {
                        if(c2<c1)
                            bCompleteIncoming=false;
                    }
                }
            }
        }
        //format
        if(sCount2!=QString("0"))
        {
            s2=QString("%1x%2").arg(sCount2,sId);
            s+=s2;
            s+=QString("|");
        }
    }
    trade.set_wares(s);

    //info
    s=ord.get_ordernumber();
    trade.set_info_1(s);
    //-
    s=sDealerName;
    trade.set_info_2(s);
    //-
    dt=ord.get_date();
    s=QString("%1").arg(dt.toString("dd.MM.yyyy"));
    trade.set_info_3(s);
    //-
    if(!bCompleteIncoming)
    {
        s=QString("Teillieferung");
        trade.set_info_4(s);
    }
    //-

    //-basic data-
    dt=ui->dateEdit->date();
    trade.set_date(dt);
    s=ui->lineEditComment->text();
    trade.set_comment(s);
    trade.set_type(TYPE_ORDERING_INCOMING);
    trade.set_canceled(false);
    //-booking number-
    s=ui->lineEditBookingnumber->text();
    trade.set_booking_number(s);
    //-
    return true;
}


void CInputDlgOrderingIncoming::press_ok(void)
{
    bool b,bOk=true;
    CTrade tr;
    int o_id;
    QString s;
    QMessageBox msg(QMessageBox::Information,"","");
    msg.setWindowTitle("!");
    msg.setText("Artikel aus der Warenliste wurde(n) von einer anderen Programminstanz gelöscht!");

    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            if(check_user_input())
            {
                //check article in list -> another client delete?
                get_data(tr,o_id,b);
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

void CInputDlgOrderingIncoming::press_cancel(void)
{
    close();
}
