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

    //disable auto default buttons
    ui->pushButtonExport->setAutoDefault(false);
    ui->pushButtonPrint->setAutoDefault(false);

    setMaximumSize(width(),height());
    setMinimumSize(width(),height());

    //connect
    connect(ui->pushButtonPrint,SIGNAL(clicked()),this,SLOT(print_button_press()));
    connect(ui->pushButtonExport,SIGNAL(clicked()),this,SLOT(export_button_press()));
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

    if(m_iType==TYPE_WARN_LIST || m_iType==TYPE_INVENTORY_LIST)//hide date select
    {
        ui->dateEditFrom->hide();
        ui->tableWidget->move(10,10);
        ui->tableWidget->setFixedHeight(height()-50);
    }
    if(m_iType==TYPE_LIST_VALUE_OF_GOODS)
    {
        ui->tableWidget->setSortingEnabled(false);
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
        lsSType.push_back(QString::fromUtf8("DLG_ARTICLE_UNDER_WARNLIMIT_TABLE_COLUMNS_WIDTHS"));
        lsSType.push_back(QString::fromUtf8("TABLE_SORT_ORDER_DLG_UNI_LIST_TYPE_WARNLIMIT"));
    }
    else if(m_iType==TYPE_INVENTORYS_ON_DATE)
    {
        lsSType.push_back(QString::fromUtf8("DLG_INVENTORYS_ON_DATE_TABLE_COLUMNS_WIDTHS"));
        lsSType.push_back(QString::fromUtf8("TABLE_SORT_ORDER_DLG_UNI_LIST_TYPE_INV_ON_DATE"));
    }
    else if(m_iType==TYPE_INVENTORY_LIST)
    {
        lsSType.push_back(QString::fromUtf8("DLG_UNI_INVENTORY_LIST_TABLE_COLUMNS_WIDTHS"));
        lsSType.push_back(QString::fromUtf8("TABLE_SORT_ORDER_DLG_UNI_INVENTORY_LIST"));
    }
    else if(TYPE_LIST_VALUE_OF_GOODS)
        lsSType.push_back(QString::fromUtf8("DLG_LIST_VALUE_OF_GOODS_TABLE_COLUMNS_WIDTHS"));
    //-
    b=settings.load(lsSType,lsSValue);//load all settings
    if(b)
    {
        if(!bUpdate)//set
        {
            //table width
            if(lsSValue.count()>0)
                settings.set_table_columns_width(ui->tableWidget,lsSValue[0],200);
            //table sort
            if(lsSValue.count()>1)
                settings.set_table_sort(ui->tableWidget,lsSValue[1],0);
        }
        if(bUpdate)//write
        {
            //table width
            if(lsSValue.count()>0)
            {
                if(settings.get_table_columns_width(ui->tableWidget,lsSValue[0]))//setting change?
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
    bool b=true;
    if(ui->tableWidget->rowCount()<=0)
        b=false;
    ui->pushButtonPrint->setEnabled(b);
    ui->pushButtonExport->setEnabled(b);
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
    //inventory list
    if(m_iType==TYPE_INVENTORY_LIST)
        b=m_pThread->set_work(WORK_INVENTORYLIST_UPDATE_TABLEWIDGET,&memory);
    //list value of goods
    if(m_iType==TYPE_LIST_VALUE_OF_GOODS)
        m_pThread->m_pWidgets->list_value_of_goods(ui->tableWidget,ui->dateEditFrom->date());
    //-
    memory.clear();
    check_user_input();
    return b;
}

bool CDlgUniList::print_button_press(void)
{
    bool b,b2;
    CSettings settings;
    QList<int> lsInt;
    qreal margin_left=15,margin_right=10,margin_top=10,margin_bottom=10;
    QDate dtFrom=ui->dateEditFrom->date();
    bool bAllColumns=true;
    QString sDocName,sTitle,sMargins,sOrientation,sOriUpdate,sMarUpdate,sPrinter,sPriUpdate;

    //list of article under warn limit
    if(m_iType==TYPE_WARN_LIST)
    {
        sTitle=QString::fromUtf8("Artikel unter Warnlimit:");
        sDocName=QString::fromUtf8("LaVa 2 - Artikel unter Warnlimit");
    }
    //inventorys on date
    if(m_iType==TYPE_INVENTORYS_ON_DATE)
    {
        sTitle=QString::fromUtf8("Artikelbestände am \"%2\" (Tagesabschluss):").arg(dtFrom.toString("dd.MM.yyyy"));
        sDocName=QString::fromUtf8("LaVa 2 - Artikelbestände");
    }
    //inventory list
    if(m_iType==TYPE_INVENTORY_LIST)
    {
        sTitle=QString::fromUtf8("Inventurliste:");
        sDocName=QString::fromUtf8("LaVa 2 - Inventurliste");
    }
    //list values of goods(date)
    if(m_iType==TYPE_LIST_VALUE_OF_GOODS)
    {
        sTitle=QString::fromUtf8("Warenwerte auf Lager am \"%1\" (Tagesabschluss):").arg(dtFrom.toString("dd.MM.yyyy"));
        sDocName=QString::fromUtf8("LaVa 2 - Warenwerte auf Lager");
    }

    //load settings
    b2=settings.load("PRINT_DIALOG_MARGINS",sMargins);
    if(b2)
    {
        b2=settings.cast_string_to_int_list(sMargins,lsInt);
        if(lsInt.count()>=4)
        {
            margin_left=lsInt[0];
            margin_top=lsInt[1];
            margin_right=lsInt[2];
            margin_bottom=lsInt[3];
        }
    }
    if(!settings.load("PRINT_DIALOG_ORIENTATION",sOrientation))
        sOrientation=QString::fromUtf8("0");//error set default

    settings.load("PRINT_DIALOG_PRINTER",sPrinter);

    //init print job
    m_print_job.m_memory.clear();
    m_print_job.set_type(PRINT_JOB_TABLE);
    m_print_job.m_memory.set_tablewidget(ui->tableWidget);
    m_print_job.m_memory.set_bool(&bAllColumns);
    m_print_job.m_sFirstRow=sTitle;

    //init preview dlg
    QPrinter printer(QPrinter::PrinterResolution);
    b=true;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("Fehler"));
    msg.setText(QString::fromUtf8("Dem Betriebssystem ist kein Drucker bekannt."));
    if(!printer.isValid())
    {
        b=false;
        msg.exec();
    }
    else
    {
        printer.setDocName(sDocName);
        QPrintPreviewDialog previewDlg(&printer, this);
        previewDlg.setWindowTitle(QString::fromUtf8("Druckvorschau"));
        previewDlg.setWindowFlags ( Qt::Window );
        connect(&previewDlg, SIGNAL(paintRequested(QPrinter* )), SLOT(print(QPrinter* )));
        previewDlg.setMinimumSize(800,500);

        //orientation
        if(sOrientation==QString("0"))
            printer.setOrientation(QPrinter::Portrait);
        else
            printer.setOrientation(QPrinter::Landscape);

        //margins
        printer.setPageMargins(margin_left ,margin_top ,margin_right ,margin_bottom ,QPrinter::Millimeter);

        //printer
        if(sPrinter.length()>0)
        {
            printer.setPrinterName(sPrinter);
            if(!printer.isValid())
            {//error, last selected printer is not online
                msg.setText(QString("Der zuletzt benutzte Drucker ist nicht erreichbar, es wird der Standarddrucker gesetzt."));
                msg.exec();
                printer.setPrinterName(QString(""));//set default printer
            }
        }
        //-
        previewDlg.showMaximized();
        previewDlg.exec();

        //get settings
        printer.getPageMargins(&margin_left ,&margin_top ,&margin_right ,&margin_bottom ,QPrinter::Millimeter); //margins
        sMarUpdate=QString("%1,").arg(margin_left);
        sMarUpdate+=QString("%1,").arg(margin_top);
        sMarUpdate+=QString("%1,").arg(margin_right);
        sMarUpdate+=QString("%1").arg(margin_bottom);

        if(printer.orientation()==0)//orientation
            sOriUpdate=QString("0");
        else
            sOriUpdate=QString("1");

        sPriUpdate=printer.printerName();

        //update settings
        if(sOrientation!=sOriUpdate && sOriUpdate.length()>0)
            settings.write(QString("PRINT_DIALOG_ORIENTATION"),sOriUpdate);
        if(sMargins!=sMarUpdate && sMarUpdate.length()>0)
            settings.write(QString("PRINT_DIALOG_MARGINS"),sMarUpdate);
        if(sPrinter!=sPriUpdate && sPriUpdate.length()>0)
            settings.write(QString("PRINT_DIALOG_PRINTER"),sPriUpdate);
    }
    lsInt.clear();
    return b;
}

bool CDlgUniList::export_button_press(void)
{
    QString s;
    if(m_iType==TYPE_WARN_LIST)
        s=QString::fromUtf8("Liste-Warnlimit");
    if(m_iType==TYPE_INVENTORYS_ON_DATE)
        s=QString::fromUtf8("Liste-Lagerbestand_%1").arg(ui->dateEditFrom->date().toString(QString::fromUtf8("dd.MM.yyyy")));
    if(m_iType==TYPE_INVENTORY_LIST)
        s=QString::fromUtf8("Inventurliste");
    //list values of goods(date)
    if(m_iType==TYPE_LIST_VALUE_OF_GOODS)
        s=QString::fromUtf8("Warenwerte auf Lager_%1").arg(ui->dateEditFrom->date().toString("dd.MM.yyyy"));

    CExportCSV exportCSV;
    return exportCSV.write_data_table(this,ui->tableWidget,s,QString::fromUtf8(""),true);
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
