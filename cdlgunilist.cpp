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

#include "cdlgunilist.h"
#include "ui_cdlgunilist.h"



CDlgUniList::CDlgUniList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgUniList)
{
    ui->setupUi(this);
    m_pThread=NULL;
    m_iType=-1;
    ui->dateEditFrom->setDate(QDate::currentDate());
    ui->dateEditFrom->setMaximumDate(QDate::currentDate());

    setMaximumSize(width(),height());
    setMinimumSize(width(),height());

    //connect
    connect(ui->pushButtonPrint,SIGNAL(clicked()),this,SLOT(print_button_press()));
    connect(ui->dateEditFrom,SIGNAL(dateChanged(QDate)),this,SLOT(date_changed()));
}

CDlgUniList::~CDlgUniList()
{
    settings(true);//save settings if update
    delete ui;
}

void CDlgUniList::set_type(int iType)
{
    m_iType=iType;

    if(m_iType==TYPE_WARN_LIST)//hide date select
    {
        ui->dateEditFrom->hide();
        ui->tableWidget->move(10,10);
        ui->tableWidget->setFixedHeight(height()-50);
    }
}

void CDlgUniList::date_changed(void)
{
    update_table();
}

bool CDlgUniList::settings(bool bUpdate)
{
    bool b;
    CSettings settings;
    QList<QString> lsSType,lsSValue,lsSUpdateType,lsSUpdateValue;

    //type?
    if(m_iType==TYPE_WARN_LIST)
    {
        lsSType.push_back(QString("DLG_ARTICLE_UNDER_WARNLIMIT_TABLE_COLUMNS_WIDTHS"));
        lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_UNI_LIST_TYPE_WARNLIMIT"));
    }
    else if(m_iType==TYPE_INVENTORYS_ON_DATE)
    {
        lsSType.push_back(QString("DLG_INVENTORYS_ON_DATE_TABLE_COLUMNS_WIDTHS"));
        lsSType.push_back(QString("TABLE_SORT_ORDER_DLG_UNI_LIST_TYPE_INV_ON_DATE"));
    }
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //table width
            if(lsSValue.count()>0)
                settings.set_table_width(ui->tableWidget,lsSValue[0],200);
            //table sort
            if(lsSValue.count()>1)
                settings.set_table_sort(ui->tableWidget,lsSValue[1],0);
        }
        if(bUpdate)//write
        {
            //table width
            if(lsSValue.count()>0)
            {
                if(settings.get_table_width(ui->tableWidget,lsSValue[0]))//setting change?
                {
                    lsSUpdateType.push_back(lsSType[0]);
                    lsSUpdateValue.push_back(lsSValue[0]);
                }
            }
            //table sort
            if(lsSValue.count()>1)
            {
                if(settings.get_table_sort(ui->tableWidget,lsSValue[1]))//setting change?
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

bool CDlgUniList::check_user_input(void)
{
    if(ui->tableWidget->rowCount()>0)
        ui->pushButtonPrint->setEnabled(true);
    else
        ui->pushButtonPrint->setEnabled(false);
    return true;
}

bool CDlgUniList::create_table_columns(QList<QString> & lsStrColumnName)
{
    bool b=true;
    QTableWidgetItem * pItem=NULL;
    for(int i=0;i<lsStrColumnName.count();i++)
    {
        pItem=new QTableWidgetItem(lsStrColumnName[i]);
        if(pItem!=NULL)
        {
            ui->tableWidget->insertColumn(i);
            ui->tableWidget->setHorizontalHeaderItem(i,pItem);
        }
        else
        {
            b=false;
            break;
        }
    }
    if(b)
        settings(false);//load & set settings
    return b;
}

bool CDlgUniList::update_table(void)
{
    if(m_pThread==NULL || m_iType<=0)
        return false;
    if(m_pThread->m_pDbInterface==NULL || m_pThread->m_pWidgets==NULL)
        return false;

    bool b=true;
    QDate dateSel=ui->dateEditFrom->date();
    CPointerMemory memory;
    memory.set_date(&dateSel);
    memory.set_tablewidget(ui->tableWidget);

    //list of article under warn limit
    if(m_iType==TYPE_WARN_LIST)
        b=m_pThread->set_work(WORK_WARNLIST_UPDATE_TABLEWIDGET,&memory);
    //inventorys on date
    if(m_iType==TYPE_INVENTORYS_ON_DATE)
        b=m_pThread->set_work(WORK_INVENTORYS_ON_DATE_UPDATE_TABLEWIDGET,&memory);
    //-
    memory.clear();
    check_user_input();
    return b;
}

bool CDlgUniList::print_button_press(void)
{
    QDate dtFrom=ui->dateEditFrom->date();
    QDateTime dtTi=QDateTime::currentDateTime();
    bool bAllColumns=true;
    QString sTitle;

    //list of article under warn limit
    if(m_iType==TYPE_WARN_LIST)
        sTitle=QString("Artikel unter Warnlimit (%1 Uhr , %2):").arg(dtTi.toString(QString("hh:mm:ss")),dtTi.toString("dd.MM.yyyy"));
    //inventorys on date
    if(m_iType==TYPE_INVENTORYS_ON_DATE)
        sTitle=QString("Artikelbestände am \"%2\" (Tagesabschluss):").arg(dtFrom.toString("dd.MM.yyyy"));

    //init print job
    m_print_job.m_memory.clear();
    m_print_job.m_memory.set_tablewidget(ui->tableWidget);
    m_print_job.m_memory.set_bool(&bAllColumns);
    m_print_job.m_sFirstRow=sTitle;

    //init preview dlg
    QPrinter printer(QPrinter::PrinterResolution);
    bool b=true;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString("Fehler"));
    msg.setText(QString("Dem Betriebssystem ist kein Drucker bekannt."));
    if(!printer.isValid())
    {
        b=false;
        msg.exec();
    }
    else
    {
        printer.setDocName(QString("LaVa 2"));
        printer.setFullPage( true );
        QPrintPreviewDialog previewDlg(&printer, this);
        previewDlg.setWindowTitle(QString("Druckvorschau"));
        previewDlg.setWindowFlags ( Qt::Window );
        connect(&previewDlg, SIGNAL(paintRequested(QPrinter* )), SLOT(print(QPrinter* )));
        previewDlg.setMinimumSize(800,500);
        printer.setPageMargins((qreal)5,(qreal)5,(qreal)5,(qreal)5,QPrinter::Millimeter);
        previewDlg.showMaximized();
        previewDlg.exec();
    }
    return b;
}

bool CDlgUniList::print(QPrinter * pPrinter)
{
    bool b=false;
    if(pPrinter!=NULL)
    {
        m_print_job.m_pPrinter=pPrinter;
        b=m_print_job.print_table_and_list();
    }
    return b;
}
