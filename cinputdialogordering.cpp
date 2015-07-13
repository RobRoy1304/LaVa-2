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

#include "cinputdialogordering.h"
#include "ui_cinputdialogordering.h"

CInputDialogOrdering::CInputDialogOrdering(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInputDialogOrdering)
{
    ui->setupUi(this);
    m_pThread=NULL;
    m_iNewDealerId=m_iMarkId=-1;
    ui->pushButtonEdit->setEnabled(false),
    ui->pushButtonDelete->setEnabled(false);
    ui->dateEdit->setDate(QDate().currentDate());//set current date
    ui->lineEditOrderingnumber->setFocus();
    ui->pushButtonOk->setEnabled(false);

    //disable auto default buttons
    ui->pushButtonAddBarcode->setAutoDefault(false);
    ui->pushButtonCancel->setAutoDefault(false);
    ui->pushButtonDelete->setAutoDefault(false);
    ui->pushButtonEdit->setAutoDefault(false);
    ui->pushButtonNew->setAutoDefault(false);
    ui->pushButtonOk->setAutoDefault(false);

    //date
    QDate dt=QDate().currentDate();
    ui->dateEdit->setMaximumDate(dt);
    dt=QDate(2009,1,1);
    ui->dateEdit->setMinimumDate(dt);

    check_user_input();
    settings(false);//load & set settings

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
    connect(ui->pushButtonNew,SIGNAL(clicked()),this,SLOT(add_ware()));
    connect(ui->pushButtonAddBarcode,SIGNAL(clicked()),this,SLOT(add_ware_barcode()));
    connect(ui->pushButtonDelete,SIGNAL(clicked()),this,SLOT(delete_ware()));
    connect(ui->pushButtonEdit,SIGNAL(clicked()),this,SLOT(edit_ware()));
    connect(ui->tableWidgetWares,SIGNAL(itemSelectionChanged()),this,SLOT(check_user_input()));
    connect(ui->tableWidgetWares,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(edit_ware()));
    connect(ui->comboBoxDealer,SIGNAL(editTextChanged(QString)),this,SLOT(edit_maker_combobox(QString)));
    connect(ui->pushButtonOk,SIGNAL(clicked()),this,SLOT(press_ok()));
    connect(ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(press_cancel()));
    //-
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
}

CInputDialogOrdering::~CInputDialogOrdering()
{
    killTimer(m_iTimerId);
    settings(true);//save settings if update

    //context menu's
    if(m_pContextMenu!=NULL)
        delete m_pContextMenu;

    delete ui;
}

void CInputDialogOrdering::keyPressEvent(QKeyEvent * event)
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

void CInputDialogOrdering::timerEvent(QTimerEvent *event)
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
                if(type==LC_ACTION_UPDATE_MAKER || type==LC_ACTION_UPDATE_WAREGROUP || type==LC_ACTION_UPDATE_ARTICLE_INV ||
                        type==LC_ACTION_UPDATE_ALL || type==LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG || type==LC_ACTION_UPDATE_INVENTORY_TRADE)
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

bool CInputDialogOrdering::open_context_menu()
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

bool CInputDialogOrdering::receiv_context_menu(QAction * pAction)
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

bool CInputDialogOrdering::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;
    lsSType.push_back(QString("DLG_ORDERING_TABLE_COLUMNS_WIDTHS_WARES"));
    lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_ORDERING"));
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

bool CInputDialogOrdering::update_wares_table(void)
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

void CInputDialogOrdering::set_thread(CWorkThread * pThread)
{
    //set db in class CLastDBChange
    if(pThread!=NULL)
    {
        m_LastDbChange.set_db(pThread->m_pDbInterface);
        m_iTimerId=startTimer(1000);//start timer
    }
    //-
    m_pThread=pThread;
    fill_combo_dealer();
}

bool CInputDialogOrdering::fill_combo_dealer(void)
{
    //fill maker combobox
    CPointerMemory memory;
    memory.set_combobox(ui->comboBoxDealer);
    if(!m_pThread->set_work(WORK_DEALER_FILL_COMBOBOX,&memory))
    {//db error
    }
    return true;
}

bool CInputDialogOrdering::edit_maker_combobox(QString s)
{
    bool b=true;
    int count=-1;
    CPointerMemory memory;
    QString sCondition;
    memory.set_string(&sCondition);
    memory.set_int(&count);
    if(s.length()>0 && m_pThread!=NULL)
    {//input?
        sCondition=QString("name='%1'").arg(s);
        m_pThread->set_work(WORK_DEALER_GET_COUNT,&memory);
        if(count<=0)
        {//dealer not exis.?
            ui->comboBoxDealer->setItemText(0,s);
            ui->comboBoxDealer->setCurrentIndex(0);
            ui->labelError->setText(QString::fromUtf8("Warnung: Händler existiert nicht"));
            b=false;
        }
    }
    if(b)
        ui->labelError->setText("");
    return true;
}

bool CInputDialogOrdering::check_user_input(void)
{
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
    int count=ui->tableWidgetWares->rowCount();
    if(count<=0)//no article at table?
    {
        ui->labelError->setText(QString("Fehler: keine Artikel in der Artikelliste"));
        ui->pushButtonOk->setEnabled(false);
    }
    else
    {
        ui->labelError->setText(QString(""));
        ui->pushButtonOk->setEnabled(true);
    }
    return true;
}

bool CInputDialogOrdering::add_ware(void)
{
    CInputDialogArticleAllowance dlg;
    //-
    if(m_pThread!=NULL)
    {
        dlg.set(m_pThread,ui->tableWidgetWares,0,6,ARTICLE_DLG_TYPE_ORDERING,-1);
        dlg.setWindowTitle(QString::fromUtf8("hinzufügen"));
        if(dlg.exec())
            check_user_input();
    }
    //-
    return true;
}

bool CInputDialogOrdering::add_ware_barcode(void)
{
    CInputDlgBarcode dlg;
    //-
    if(m_pThread!=NULL)
    {
        dlg.set(m_pThread,ui->tableWidgetWares,0,6,ARTICLE_DLG_TYPE_ORDERING);
        dlg.setWindowTitle(QString::fromUtf8("hinzufügen (Barcode)"));
        if(dlg.exec())
            check_user_input();
    }
    //-
    return true;
}

bool CInputDialogOrdering::delete_ware(void)
{
    int iRow=ui->tableWidgetWares->currentRow();
    QList <QTableWidgetItem*> ls=ui->tableWidgetWares->selectedItems();
    if(ls.count()>0)//selected?
        ui->tableWidgetWares->removeRow(iRow);
    check_user_input();
    return true;
}

bool CInputDialogOrdering::edit_ware(void)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pWidgets==NULL)
        return false;

    bool b,bReturn=false;
    QString sData;
    int iCount,iArticleId,iArticleIdAfter;
    CInputDialogArticleAllowance dlg;
    //-
    if(m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetWares,iCount,0))//get count from selected row
    {
        if(m_pThread->m_pWidgets->get_selected_table_item_value(ui->tableWidgetWares,iArticleId))//get article id from selected row
        {
            dlg.set_count(iCount);
            dlg.set(m_pThread,ui->tableWidgetWares,0,6,ARTICLE_DLG_TYPE_ORDERING,iArticleId);
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
                        {//antoher article? add mode
                            insert_ware_at_table(sData,false,true);
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

bool CInputDialogOrdering::insert_ware_at_table(QString sData,bool bEdit,bool bSelect)
{
    if(m_pThread==NULL)
        return false;
    if(m_pThread->m_pWidgets==NULL)
        return false;

    bool b=m_pThread->m_pWidgets->article_update_row_wareslist(ui->tableWidgetWares,sData,bEdit,bSelect);
    return b;
}

bool CInputDialogOrdering::get_data(COrdering & ord)
{
    ord.set_comment(ui->lineEditComment->text());
    ord.set_ordernumber(ui->lineEditOrderingnumber->text());
    //date
    QDate dt=ui->dateEdit->date();
    ord.set_date(dt);
    //dealer
    QString name=ui->comboBoxDealer->itemText(ui->comboBoxDealer->currentIndex());
    int dealer_id=-1;
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            dealer_id=m_pThread->m_pDbInterface->dealer_get_id(name);
            ord.set_dealer_id(dealer_id);
        }
    }
    //wares
    QList <QString> lsWares;
    QString sWares;
    for(int i=0;i<ui->tableWidgetWares->rowCount();i++)
    {
        sWares=QString("%1x%2").arg(ui->tableWidgetWares->item(i,0)->text(),ui->tableWidgetWares->item(i,6)->text());//how many x article_id
        lsWares.push_back(sWares);
    }
    ord.set_wares(lsWares);
    //-
    return true;
}

bool CInputDialogOrdering::set_data(COrdering & ord)
{
    m_iMarkId=ord.get_id();
    m_sMarkWares=ord.get_wares();
    ui->pushButtonOk->setText(QString::fromUtf8("Änderung(en) anwenden"));
    ui->pushButtonOk->setEnabled(true);
    ui->lineEditComment->setText(ord.get_comment());
    ui->lineEditOrderingnumber->setText(ord.get_ordernumber());
    //date
    QDate dt=ord.get_date();
    ui->dateEdit->setDate(dt);
    //dealer
    int i;
    CDealer de;
    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            if(m_pThread->m_pDbInterface->dealer_get(ord.get_dealer_id(),de))//dealer found
            {
                for(i=0;i<ui->comboBoxDealer->count();i++)
                {
                    if(ui->comboBoxDealer->itemText(i)==de.get_name())//found in combo
                    {
                        ui->comboBoxDealer->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
    }
    //wares
    QList <QString> lsS;
    ord.get_wares(lsS);
    for(int i=0;i<lsS.count();i++)
        insert_ware_at_table(lsS[i],false,false);//fill table
    //-
    return true;
}

void CInputDialogOrdering::press_ok(void)
{  
    CDealer de;
    bool bOk=true;
    COrdering ord;
    int i,dealer_id=-1;
    QString s,name=ui->comboBoxDealer->itemText(ui->comboBoxDealer->currentIndex());;

    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    msg.setInformativeText(QString::fromUtf8("Soll dieser Händler angelegt werden?"));

    QMessageBox msg2(QMessageBox::Information,"","");
    msg2.setWindowTitle("!");
    msg2.setText(QString::fromUtf8("Der Datensatz wurde von einer anderen Programminstanz gelöscht!"));

    QMessageBox msg3(QMessageBox::Information,"","");
    msg3.setWindowTitle("!");
    msg3.setText(QString::fromUtf8("Artikel aus der Warenliste wurde(n) von einer anderen Programminstanz gelöscht!"));


    if(m_pThread!=NULL)
    {
        if(m_pThread->m_pDbInterface!=NULL)
        {
            //another client delete this record?
            if(m_iMarkId>0)//edit-mode?
            {
                s=QString("id = %1").arg(m_iMarkId);
                s+=QString(" AND wares = '%1'").arg(m_sMarkWares);
                i=m_pThread->m_pDbInterface->ordering_get_count(s);
                if(i<=0)//record delete?
                {
                    msg2.exec();
                    bOk=false;
                }
            }
            //-
            if(bOk)
            {
                //check wares -> article delete from another client?
                s=QString("");
                get_data(ord);
                if(!m_pThread->m_pDbInterface->check_article_at_wares(ord.get_wares(),s))
                {//error article was delete
                    msg3.exec();
                    bOk=false;
                }

                //-check dealer-=
                if(name.length()>0 && bOk==true)
                {
                    dealer_id=m_pThread->m_pDbInterface->dealer_get_id(name);//search
                    if(dealer_id==-1)
                    {//dealer not found?
                        s=QString::fromUtf8("Der Händler '%1' existiert nicht.").arg(name);
                        msg.setText(s);
                        msg.exec();
                        if(msg.clickedButton()==yesButton)
                        {
                            //insert new maker in db
                            de.set_name(name);
                            m_pThread->m_pDbInterface->dealer_add(de);
                            m_iNewDealerId=de.get_id();
                        }
                    }
                }
            }
        }
    }
    //-
    if(bOk)
        done(1);
    else
        close();
}

void CInputDialogOrdering::press_cancel(void)
{
    close();
}
