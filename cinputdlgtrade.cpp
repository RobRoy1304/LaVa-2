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

#include "cinputdlgtrade.h"
#include "ui_cinputdlgtrade.h"

CInputDlgTrade::CInputDlgTrade(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDlgTrade)
{
    ui->setupUi(this);

    m_pThread=NULL;
    m_bOutgoing=false;
    m_dtMark=QDate::currentDate();

    settings(false);//load & set settings
    ui->pushButtonDelete->setEnabled(false);
    ui->pushButtonEdit->setEnabled(false);
    ui->dateEdit->setDate(QDate().currentDate());//set current date
    ui->buttonBox->setEnabled(false);

    check_user_input();

    //date
    QDate dt=QDate().currentDate();
    ui->dateEdit->setMaximumDate(dt);
    dt=QDate(2009,1,1);
    ui->dateEdit->setMinimumDate(dt);

    //context menu
    ui->tableWidgetWares->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pContextMenu=new QMenu(QString("context_default"),this);
    if(m_pContextMenu!=NULL)
    {
        m_pContextMenu->addAction(QString("Hinzufügen"));
        m_pContextMenu->addAction(QString("Bearbeiten"));
        m_pContextMenu->addAction(QString("Löschen"));
    }

    //connects context menus
    connect(m_pContextMenu,SIGNAL(triggered(QAction*)),this,SLOT(receiv_context_menu(QAction*)));
    connect(ui->tableWidgetWares,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));

    //connects
    connect(ui->pushButtonNew,SIGNAL(clicked()),this,SLOT(add_ware()));
    connect(ui->pushButtonDelete,SIGNAL(clicked()),this,SLOT(delete_ware()));
    connect(ui->pushButtonEdit,SIGNAL(clicked()),this,SLOT(edit_ware()));
    connect(ui->pushButtonNomination,SIGNAL(clicked()),this,SLOT(set_booking_number_nomination()));
    connect(ui->lineEditBookingnumber,SIGNAL(textChanged(QString)),this,SLOT(check_user_input()));
    connect(ui->tableWidgetWares,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(edit_ware()));
    connect(ui->tableWidgetWares,SIGNAL(itemSelectionChanged()),this,SLOT(check_user_input()));
    connect(ui->dateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(date_changed(QDate)));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDlgTrade::~CInputDlgTrade()
{
    settings(true);//save settings if update

    //context menu's
    if(m_pContextMenu!=NULL)
        delete m_pContextMenu;

    delete ui;
}

void CInputDlgTrade::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool CInputDlgTrade::open_context_menu()
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

bool CInputDlgTrade::receiv_context_menu(QAction * pAction)
{
    if(pAction==NULL)
        return false;

    if(pAction->text()==QString("Hinzufügen"))
        add_ware();
    if(pAction->text()==QString("Bearbeiten"))
        edit_ware();
    if(pAction->text()==QString("Löschen"))
        delete_ware();
    return true;
}

bool CInputDlgTrade::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;
    lsSType.push_back(QString("DLG_TRADE_TABLE_COLUMNS_WIDTHS_IN_OUT_WARES"));
    lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_TRADE_IN_OUT_WARES"));
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //table width
            if(lsSValue.count()>0)
                settings.set_table_columns_width(ui->tableWidgetWares,lsSValue[0],200);
            //table sort
            if(lsSValue.count()>1)
                settings.set_table_sort(ui->tableWidgetWares,lsSValue[1],1);
        }
        if(bUpdate)//write
        {
            //table width
            if(lsSValue.count()>0)
            {
                if(settings.get_table_columns_width(ui->tableWidgetWares,lsSValue[0]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[0]);
                    lsSUpdateValue.push_back(lsSValue[0]);
                }
            }
            //table sort
            if(lsSValue.count()>1)
            {
                if(settings.get_table_sort(ui->tableWidgetWares,lsSValue[1]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[1]);
                    lsSUpdateValue.push_back(lsSValue[1]);
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

void CInputDlgTrade::keyPressEvent(QKeyEvent * event)
{
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

bool CInputDlgTrade::date_changed(QDate dtNew)
{
    if(m_pThread==NULL)
        return false;

    if(m_bOutgoing)//only by outgoing
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

void CInputDlgTrade::set_db(CWorkThread * pThread)
{
    m_pThread=pThread;
}

bool CInputDlgTrade::set_booking_number_nomination(void)
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

bool CInputDlgTrade::check_user_input(void)
{
    if(m_pThread==NULL)
        return false;

    int count;
    bool b=true;
    QString s,s2,sError=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);

    //edit & delete button
    QList<QTableWidgetItem*> ls=ui->tableWidgetWares->selectedItems();
    if(ls.count()<=0)//nothing selected
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
        //article count
        count=ui->tableWidgetWares->rowCount();
        if(count<=0)//no article at table?
        {
            sError=QString("Fehler: kein Artikel in der Artikelliste");
            b=false;
        }
    }
    //-
    ui->labelError->setText(sError);
    ui->buttonBox->setEnabled(b);
    memory.clear();
    return true;
}

bool CInputDlgTrade::add_ware(void)
{
    if(m_pThread==NULL)
        return false;
    int iType=ARTICLE_DLG_TYPE_INCOMING;
    if(m_bOutgoing)
        iType=ARTICLE_DLG_TYPE_OUTGOING;
    QString s;
    bool b=false;
    QDate dt=ui->dateEdit->date();//get selected date

    //do -> user quit the article allowance dialog
    QRect rMarkDlgGeometry(0,0,0,0);
    CInputDialogArticleAllowance * pDlg=NULL;
    do
    {
        pDlg=new CInputDialogArticleAllowance;
        if(pDlg==NULL)
            break;
        if(rMarkDlgGeometry!=QRect(0,0,0,0))
            pDlg->setGeometry(rMarkDlgGeometry);
        pDlg->set(m_pThread,ui->tableWidgetWares,0,5,iType,-1,dt);
        pDlg->setWindowTitle("hinzufügen");
        b=pDlg->exec();
        if(b)
        {
            if(pDlg->get_data(s))
                insert_ware_at_table(s,false,true);
            else
                b=false;
        }
        if(!pDlg->get_checkbox_not_close_the_dialog())//dialog not close? (easy to add many article)
            b=false;
        if(pDlg!=NULL)
        {
            rMarkDlgGeometry=pDlg->geometry();
            delete pDlg;
            pDlg=NULL;
        }
    }while(b);
    //-
    check_user_input();
    //-
    return b;
}

bool CInputDlgTrade::delete_ware(void)
{
    int iRow=ui->tableWidgetWares->currentRow();
    QList <QTableWidgetItem*> ls=ui->tableWidgetWares->selectedItems();
    if(ls.count()>0)//selected?
        ui->tableWidgetWares->removeRow(iRow);
    check_user_input();
    return true;
}

bool CInputDlgTrade::edit_ware(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pWidgets==NULL)
        return false;

    int iType=ARTICLE_DLG_TYPE_INCOMING;
    if(m_bOutgoing)
        iType=ARTICLE_DLG_TYPE_OUTGOING;
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
            dlg.set(m_pThread,ui->tableWidgetWares,0,5,iType,iArticleId,dt);
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

void CInputDlgTrade::set_outgoing(void)
{
    m_bOutgoing=true;
}

bool CInputDlgTrade::insert_ware_at_table(QString sData,bool bEdit,bool bSelect)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pWidgets==NULL)
        return false;

    bool b=m_pThread->m_pWidgets->article_update_row_wareslist(ui->tableWidgetWares,sData,bEdit,bSelect);
    return b;
}

bool CInputDlgTrade::get_data(CTrade & tr)
{
    if(m_pThread==NULL)
        return true;

    QList <QString> lsWares;
    QString s,sWare;
    QDate dt;

    //type
    int type=TYPE_INCOMING;
    if(m_bOutgoing)
        type=TYPE_OUTGOING;
    tr.set_type(type);
    //comment
    tr.set_comment(ui->lineEditComment->text());
    //date
    dt=ui->dateEdit->date();
    tr.set_date(dt);
    //wares
    for(int i=0;i<ui->tableWidgetWares->rowCount();i++)
    {
        sWare=QString("%1x%2").arg(ui->tableWidgetWares->item(i,0)->text(),ui->tableWidgetWares->item(i,5)->text());//how many x article_id
        lsWares.push_back(sWare);
    }
    tr.set_wares(lsWares);
    //-booking number-
    s=ui->lineEditBookingnumber->text();
    tr.set_booking_number(s);
    //-
    return true;
}

bool CInputDlgTrade::check_article_counts_on_date(void)
{
    QMessageBox msg(QMessageBox::Information,"","");
    QDate dt=ui->dateEdit->date();
    QString s;
    bool b,bToMany=false;
    QTableWidgetItem * pItem=NULL;
    int i,rows,count,id;
    unsigned int max;
    CPointerMemory memory;
    memory.set_uint(&max);
    memory.set_date(&dt);
    memory.set_int(&id);
    //-
    if(m_bOutgoing==true && m_pThread!=NULL)//outgoing? db connection?
    {
        //check article counts of max. count on the date
        rows=ui->tableWidgetWares->rowCount();
        for(i=0;i<rows;i++)
        {
            pItem=ui->tableWidgetWares->item(i,5);
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
        s=QString("Der/Die max.Warenausgang/-gänge der Artikel in der Liste an dem Datum wurde(n) überschritten,");
        s+=QString("beachten Sie die chronologische Reihenfolge von Warenein-/ausgang! Die Anzahl wurde jeweils angepasst.");
        msg.setWindowTitle("Information:");
        msg.setText(s);
        msg.exec();
    }
    //-
    return true;
}
