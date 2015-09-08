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

#include "mainwindow.h"
#include "ui_mainwindow.h"

//---basic-------------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    m_iTimerId=-1;
    m_bUpdateTableTrade=m_bUpdateTableTradeWarelist=m_bUpdateTableOrdering=m_bUpdateTableOrderingWarelist=true;
    m_bUpdateTableWaregroup=m_bUpdateTableMaker=m_bUpdateTableDealer=m_bUpdateTableCustomer=m_bUpdateListLogbook=true;
    m_bUpdateTableInventory=m_bUpdateTableArticle=false;

    //-register-
    qRegisterMetaType<Qt::SortOrder>("Qt::SortOrder");
    qRegisterMetaType<QItemSelection>("QItemSelection");

    //basic
    ui->setupUi(this);

    //hide statusbar
    ui->statusBar->hide();

    //hide columns from tree widget
    ui->treeWidgetWaregroup->hideColumn(2);//id column
    ui->treeWidgetWaregroup->hideColumn(3);//parent_id column

    //set db in class CLastDBChange
    m_LastDbChange.set_db(&m_db);

    //print
    m_print_job.set_db(&m_thread);

    //year's combo box's
    QString s;
    QDate dt=QDate::currentDate();
    for(int year=dt.year();year>=2011;year--)//fill year combo trade+logbook
    {
        s=QString::fromUtf8("%1").arg(year);
        ui->comboBoxTradeMaskYear->insertItem(ui->comboBoxTradeMaskYear->count(),s);
        ui->comboBoxLogbookMaskYear->insertItem(ui->comboBoxTradeMaskYear->count(),s);
    }
    ui->pushButtonTradeCancel->setEnabled(false);

    //context menu
    ui->tableWidgetArticle->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetMaker->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetDealer->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetCustomer->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetOrdering->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidgetWaregroup->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetTrade->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pContextMenuDefault=new QMenu(QString::fromUtf8("context_default"),this);
    if(m_pContextMenuDefault!=NULL)
    {
        m_pContextMenuDefault->addAction(QString::fromUtf8("neu"));
        m_pContextMenuDefault->addAction(QString::fromUtf8("bearbeiten"));
        m_pContextMenuDefault->addAction(QString::fromUtf8("löschen"));
    }
    m_pContextMenuTrade=new QMenu(QString::fromUtf8("context_trade"),this);
    if(m_pContextMenuTrade!=NULL)
    {
        m_pContextMenuTrade->addAction(QString::fromUtf8("Wareneingang"));
        m_pContextMenuTrade->addAction(QString::fromUtf8("Warenausgang"));
        m_pContextMenuTrade->addAction(QString::fromUtf8("Wareneingang(Bestellung)"));
        m_pContextMenuTrade->addAction(QString::fromUtf8("Warenausgang(Kunde)"));
        m_pContextMenuTrade->addAction(QString::fromUtf8("stornieren"));
    }
    m_pContextMenuArticle=new QMenu(QString::fromUtf8("context_article"),this);
    if(m_pContextMenuArticle!=NULL)
    {
        m_pContextMenuArticle->addAction(QString::fromUtf8("neu"));
        m_pContextMenuArticle->addAction(QString::fromUtf8("bearbeiten"));
        m_pContextMenuArticle->addAction(QString::fromUtf8("kopieren"));
        m_pContextMenuArticle->addAction(QString::fromUtf8("löschen"));
    }

    //connects context menus
    connect(m_pContextMenuDefault,SIGNAL(triggered(QAction*)),this,SLOT(receiv_context_menu(QAction*)));
    connect(m_pContextMenuTrade,SIGNAL(triggered(QAction*)),this,SLOT(receiv_context_menu(QAction*)));
    connect(m_pContextMenuArticle,SIGNAL(triggered(QAction*)),this,SLOT(receiv_context_menu(QAction*)));
    connect(ui->tableWidgetArticle,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));
    connect(ui->tableWidgetMaker,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));
    connect(ui->tableWidgetDealer,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));
    connect(ui->tableWidgetCustomer,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));
    connect(ui->tableWidgetOrdering,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));
    connect(ui->treeWidgetWaregroup,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));
    connect(ui->tableWidgetTrade,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(open_context_menu()));

    //connects tab
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tab_switched(int)));

    //connects waregroup
    connect(ui->pushButtonWgNew,SIGNAL(clicked()),this,SLOT(waregroup_new()));
    connect(ui->pushButtonWgEdit,SIGNAL(clicked()),this,SLOT(waregroup_edit()));
    connect(ui->pushButtonWgDelete,SIGNAL(clicked()),this,SLOT(waregroup_delete()));
    connect(ui->treeWidgetWaregroup,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(waregroup_treeitem_clicked()));
    connect(ui->treeWidgetWaregroup,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(waregroup_edit()));

    //connects maker
    connect(ui->tableWidgetMaker,SIGNAL(itemSelectionChanged()),this,SLOT(maker_widgetitem_clicked()));
    connect(ui->pushButtonMakerNew,SIGNAL(clicked()),this,SLOT(maker_new()));
    connect(ui->pushButtonMakerEdit,SIGNAL(clicked()),this,SLOT(maker_edit()));
    connect(ui->pushButtonMakerDelete,SIGNAL(clicked()),this,SLOT(maker_delete()));
    connect(ui->pushButtonMaskSetMaker,SIGNAL(clicked()),this,SLOT(maker_mask_set()));
    connect(ui->tableWidgetMaker,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(maker_edit()));
    connect(ui->checkBoxAutoSearchMaker,SIGNAL(clicked()),this,SLOT(maker_checkbox_auto_clicked()));
    connect(ui->lineEditMakerMask,SIGNAL(textChanged(QString)),this,SLOT(maker_mask_edit()));
    connect(ui->comboBoxMaskMaker,SIGNAL(currentIndexChanged(QString)),this,SLOT(maker_mask_edit()));

    //connects dealer
    connect(ui->tableWidgetDealer,SIGNAL(itemSelectionChanged()),this,SLOT(dealer_widgetitem_clicked()));
    connect(ui->pushButtonDealerNew,SIGNAL(clicked()),this,SLOT(dealer_new()));
    connect(ui->pushButtonDealerEdit,SIGNAL(clicked()),this,SLOT(dealer_edit()));
    connect(ui->pushButtonDealerDelete,SIGNAL(clicked()),this,SLOT(dealer_delete()));
    connect(ui->pushButtonMaskSetDealer,SIGNAL(clicked()),this,SLOT(dealer_mask_set()));
    connect(ui->tableWidgetDealer,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(dealer_edit()));
    connect(ui->checkBoxAutoSearchDealer,SIGNAL(clicked()),this,SLOT(dealer_checkbox_auto_clicked()));
    connect(ui->lineEditDealerMask,SIGNAL(textChanged(QString)),this,SLOT(dealer_mask_edit()));
    connect(ui->comboBoxMaskDealer,SIGNAL(currentIndexChanged(QString)),this,SLOT(dealer_mask_edit()));

    //connects customer
    connect(ui->tableWidgetCustomer,SIGNAL(itemSelectionChanged()),this,SLOT(customer_widgetitem_clicked()));
    connect(ui->pushButtonCustomerNew,SIGNAL(clicked()),this,SLOT(customer_new()));
    connect(ui->pushButtonCustomerEdit,SIGNAL(clicked()),this,SLOT(customer_edit()));
    connect(ui->pushButtonCustomerDelete,SIGNAL(clicked()),this,SLOT(customer_delete()));
    connect(ui->pushButtonMaskSetCustomer,SIGNAL(clicked()),this,SLOT(customer_mask_set()));
    connect(ui->tableWidgetCustomer,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(customer_edit()));
    connect(ui->checkBoxAutoSearchCustomer,SIGNAL(clicked()),this,SLOT(customer_checkbox_auto_clicked()));
    connect(ui->lineEditCustomerMask,SIGNAL(textChanged(QString)),this,SLOT(customer_mask_edit()));
    connect(ui->comboBoxMaskCustomer,SIGNAL(currentIndexChanged(QString)),this,SLOT(customer_mask_edit()));

    //connects article
    connect(ui->tableWidgetArticle,SIGNAL(itemSelectionChanged()),this,SLOT(article_widgetitem_clicked()));
    connect(ui->pushButtonArticleNew,SIGNAL(clicked()),this,SLOT(article_new()));
    connect(ui->pushButtonArticleEdit,SIGNAL(clicked()),this,SLOT(article_edit()));
    connect(ui->pushButtonArticleCopy,SIGNAL(clicked()),this,SLOT(article_copy()));
    connect(ui->pushButtonArticleDelete,SIGNAL(clicked()),this,SLOT(article_delete()));
    connect(ui->pushButtonMaskSetArticle,SIGNAL(clicked()),this,SLOT(article_mask_set()));
    connect(ui->tableWidgetArticle,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(article_edit()));
    connect(ui->checkBoxAutoSearchArticle,SIGNAL(clicked()),this,SLOT(article_checkbox_auto_clicked()));
    connect(ui->lineEditArticleMask,SIGNAL(textChanged(QString)),this,SLOT(article_mask_edit()));
    connect(ui->comboBoxMaskArticle,SIGNAL(currentIndexChanged(QString)),this,SLOT(article_mask_edit()));

    //connects ordering
    connect(ui->pushButtonOrderingNew,SIGNAL(clicked()),this,SLOT(ordering_new()));
    connect(ui->tableWidgetOrdering,SIGNAL(itemSelectionChanged()),this,SLOT(ordering_widgetitem_clicked()));
    connect(ui->pushButtonOrderingDelete,SIGNAL(clicked()),this,SLOT(ordering_delete()));
    connect(ui->pushButtonOrderingEdit,SIGNAL(clicked()),this,SLOT(ordering_edit()));
    connect(ui->tableWidgetOrdering,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(ordering_edit()));
    connect(ui->pushButtonMaskSetOrdering,SIGNAL(clicked()),this,SLOT(ordering_mask_set()));
    connect(ui->checkBoxAutoSearchOrdering,SIGNAL(clicked()),this,SLOT(ordering_checkbox_auto_clicked()));
    connect(ui->lineEditOrderingMask,SIGNAL(textChanged(QString)),this,SLOT(ordering_mask_edit()));
    connect(ui->comboBoxMaskOrdering,SIGNAL(currentIndexChanged(QString)),this,SLOT(ordering_mask_edit()));
    connect(ui->tableWidgetOrderingWareslist,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(ordering_table_warelist_doubleclick()));

    //trade
    connect(ui->pushButtonTradeIncoming,SIGNAL(clicked()),this,SLOT(trade_incoming()));
    connect(ui->pushButtonTradeOutgoing,SIGNAL(clicked()),this,SLOT(trade_outgoing()));
    connect(ui->pushButtonTradeOutgoingCustomer,SIGNAL(clicked()),this,SLOT(trade_outgoing_customer()));
    connect(ui->pushButtonTradeOrderingIncoming,SIGNAL(clicked()),this,SLOT(trade_ordering_incoming()));
    connect(ui->pushButtonTradeCancel,SIGNAL(clicked()),this,SLOT(trade_canceled()));
    connect(ui->tableWidgetTrade,SIGNAL(itemSelectionChanged()),this,SLOT(trade_widgetitem_clicked()));
    connect(ui->comboBoxTradeMaskDate,SIGNAL(currentIndexChanged(QString)),this,SLOT(trade_mask_date_changed()));
    connect(ui->comboBoxTradeMaskYear,SIGNAL(currentIndexChanged(QString)),this,SLOT(trade_mask_date_changed()));
    connect(ui->listWidgetTradeMaskDate,SIGNAL(itemSelectionChanged()),this,SLOT(trade_date_mask_select_changed()));    
    connect(ui->checkBoxAutoSearchTrade,SIGNAL(clicked()),this,SLOT(trade_checkbox_auto_clicked()));
    connect(ui->lineEditTradeMask,SIGNAL(textChanged(QString)),this,SLOT(trade_mask_edit()));
    connect(ui->comboBoxMaskTrade,SIGNAL(currentIndexChanged(QString)),this,SLOT(trade_mask_edit()));
    connect(ui->pushButtonMaskSetTrade,SIGNAL(clicked()),this,SLOT(trade_mask_set()));
    connect(ui->tableWidgetTradeWareslist,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(trade_table_warelist_doubleclick()));

    //inventory
    connect(ui->pushButtonMaskSetInventory,SIGNAL(clicked()),this,SLOT(inventory_mask_set()));
    connect(ui->checkBoxAutoSearchInventory,SIGNAL(clicked()),this,SLOT(inventory_checkbox_auto_clicked()));
    connect(ui->lineEditInventoryMask,SIGNAL(textChanged(QString)),this,SLOT(inventory_mask_edit()));
    connect(ui->comboBoxMaskInventory,SIGNAL(currentIndexChanged(QString)),this,SLOT(inventory_mask_edit()));
    connect(ui->tableWidgetInventory,SIGNAL(itemSelectionChanged()),this,SLOT(inventory_widgetitem_clicked()));
    connect(ui->tableWidgetInventory,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(inventory_table_doubleclick()));

    //logbook
    connect(ui->comboBoxLogbookMaskYear,SIGNAL(currentIndexChanged(QString)),this,SLOT(logbook_date_mask_year_changed()));
    connect(ui->comboBoxMaskLogbook,SIGNAL(currentIndexChanged(QString)),this,SLOT(logbook_combo_mask_changed()));
    connect(ui->listWidgetLogbookMaskDate,SIGNAL(itemSelectionChanged()),this,SLOT(logbook_mask_set()));
    connect(ui->pushButtonLogbookChangeCount,SIGNAL(clicked()),this,SLOT(logbook_change_count_of_item()));
    connect(ui->pushButtonLogbookClear,SIGNAL(clicked()),this,SLOT(logbook_clear()));

    //menu
    connect(ui->menuDbBackupCreate,SIGNAL(triggered()),this,SLOT(menu_db_backup_create()));
    connect(ui->menuDbBackupLoad,SIGNAL(triggered()),this,SLOT(menu_db_backup_load()));
    connect(ui->menuDbClear,SIGNAL(triggered()),this,SLOT(menu_db_clear()));
    connect(ui->menuQuit,SIGNAL(triggered()),this,SLOT(menu_tool_close()));
    connect(ui->menuAbout,SIGNAL(triggered()),this,SLOT(menu_about()));
    connect(ui->menuHelp,SIGNAL(triggered()),this,SLOT(menu_help()));
    connect(ui->menuListBalanceList,SIGNAL(triggered()),this,SLOT(menu_balance_list()));
    connect(ui->menuListUnderWarnLimit,SIGNAL(triggered()),this,SLOT(menu_article_under_warnlimit()));
    connect(ui->menuListInventoryDate,SIGNAL(triggered()),this,SLOT(menu_inventorys_on_date()));
    connect(ui->menuListInv,SIGNAL(triggered()),this,SLOT(menu_inventorys_list()));
    connect(ui->menuListValueOfGoods,SIGNAL(triggered()),this,SLOT(menu_list_value_of_goods()));
    connect(ui->menuPrinterLogbookDateView,SIGNAL(triggered()),this,SLOT(menu_print_logbook()));
    connect(ui->menuPrinterMakerSel,SIGNAL(triggered()),this,SLOT(menu_print_maker_selection()));
    connect(ui->menuPrinterMakerOView,SIGNAL(triggered()),this,SLOT(menu_print_maker_overview()));
    connect(ui->menuPrinterDealerSel,SIGNAL(triggered()),this,SLOT(menu_print_dealer_selection()));
    connect(ui->menuPrinterDealerOView,SIGNAL(triggered()),this,SLOT(menu_print_dealer_overview()));
    connect(ui->menuPrinterCustomerSel,SIGNAL(triggered()),this,SLOT(menu_print_customer_selection()));
    connect(ui->menuPrinterCustomerOView,SIGNAL(triggered()),this,SLOT(menu_print_customer_overview()));
    connect(ui->menuPrinterArticleOView,SIGNAL(triggered()),this,SLOT(menu_print_article_overview()));
    connect(ui->menuPrinterInventoryOView,SIGNAL(triggered()),this,SLOT(menu_print_inventory_overview()));
    connect(ui->menuPrinterOrderingOView,SIGNAL(triggered()),this,SLOT(menu_print_ordering_overview()));
    connect(ui->menuPrinterOrderingSel,SIGNAL(triggered()),this,SLOT(menu_print_ordering_selection()));
    connect(ui->menuPrinterTradeOView,SIGNAL(triggered()),this,SLOT(menu_print_trade_overview()));
    connect(ui->menuPrinterTradeSel,SIGNAL(triggered()),this,SLOT(menu_print_trade_selection()));
    connect(ui->menuPrinterWaregroupOView,SIGNAL(triggered()),this,SLOT(menu_print_waregroup()));
    connect(ui->menuSettingInventory,SIGNAL(triggered()),this,SLOT(menu_table_setting_inventory()));
    connect(ui->menuSettingArticle,SIGNAL(triggered()),this,SLOT(menu_table_setting_article()));
    connect(ui->menuExportInventory,SIGNAL(triggered()),this,SLOT(menu_export_inventory()));
    connect(ui->menuExportTrade,SIGNAL(triggered()),this,SLOT(menu_export_trade()));
    connect(ui->menuExportOrdering,SIGNAL(triggered()),this,SLOT(menu_export_ordering()));
    connect(ui->menuExportArticle,SIGNAL(triggered()),this,SLOT(menu_export_article()));
    connect(ui->menuExportWaregroup,SIGNAL(triggered()),this,SLOT(menu_export_waregroup()));
    connect(ui->menuExportMaker,SIGNAL(triggered()),this,SLOT(menu_export_maker()));
    connect(ui->menuExportDealer,SIGNAL(triggered()),this,SLOT(menu_export_dealer()));
    connect(ui->menuExportCustomer,SIGNAL(triggered()),this,SLOT(menu_export_customer()));
    connect(ui->menuExportLogbook,SIGNAL(triggered()),this,SLOT(menu_export_logbook()));
    connect(ui->menuImportFromCSV,SIGNAL(triggered()),this,SLOT(menu_import_from_csv()));
    connect(ui->menuBarcodeOverview,SIGNAL(triggered()),this,SLOT(menu_barcode_overview()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(event==NULL)
        event=NULL;

    //check article inv under warning limit
    QMessageBox msg(QMessageBox::Warning,"","");
    msg.setWindowTitle(QString::fromUtf8("Achtung!"));
    QString s;
    int count=inventory_get_count_of_arctile_under_limit();
    if(count>0)
    {
        s=QString::fromUtf8("Es gibt %1 Artikel deren Lagerbestand unter dem Warnungslimit liegt.").arg(count);
        msg.setText(s);
        msg.exec();
    }

    //set last session in db-
    QDateTime dtTi=QDateTime().currentDateTime();
    if(m_db.is_db_connect())
        m_db.write_last_session(dtTi);

    //context menu's
    if(m_pContextMenuDefault!=NULL)
        delete m_pContextMenuDefault;
    if(m_pContextMenuTrade!=NULL)
        delete m_pContextMenuTrade;
    if(m_pContextMenuArticle!=NULL)
        delete m_pContextMenuArticle;

    //clear
    if(m_iTimerId!=-1)
        killTimer(m_iTimerId);

    m_thread.stop_thread();
    m_db.close_db();

    //-
    close();
}

void MainWindow::resizeEvent  ( QResizeEvent * event )
{
    widgets_position(event->size());
    QMainWindow::resizeEvent(event);
}

void MainWindow::set_user(QString s)
{
    QString title;
    if(s.length()>0)
    {
        title=QString::fromUtf8("lava2 - user: %1").arg(s);
        setWindowTitle(title);
    }
    m_sUser=s;
}

bool MainWindow::init(void)
{
    QDesktopWidget desktop;
    QRect r=desktop.availableGeometry();
    QSize sz(r.width(),r.height());
    //-
    widgets_position(sz);// set widget positions
    m_widgets.set_db(&m_db);//set database
    m_thread.init(&m_db,&m_widgets);//init thread
    m_thread.start_thread();//start thread
    return true;
}

bool MainWindow::widgets_position(QSize szScreen)
{
    //tab-widget
    ui->tabWidget->move(5,5);
    ui->tabWidget->setFixedSize(szScreen.width()-10,szScreen.height()-40);

    //waregroup widgets positions
    ui->labelCountWaregroup->move(ui->tabWidget->width()-240,10);
    ui->labelCountWaregroup->setFixedWidth(230);
    ui->pushButtonWgNew->move(10,10);
    ui->pushButtonWgEdit->move(200,10);
    ui->pushButtonWgDelete->move(390,10);
    ui->treeWidgetWaregroup->move(10,40);
    ui->treeWidgetWaregroup->setFixedSize(ui->tabWidget->width()-25,ui->tabWidget->height()-80);

    //maker widgets positions
    ui->labelCountMaker->move(ui->tabWidget->width()-240,10);
    ui->labelCountMaker->setFixedWidth(230);
    ui->labelMaker->move(10,10);
    ui->comboBoxMaskMaker->move(100,10);
    ui->comboBoxMaskMaker->setFixedWidth(200);
    ui->lineEditMakerMask->move(300,10);
    ui->lineEditMakerMask->setFixedWidth(250);
    ui->pushButtonMaskSetMaker->move(550,10);
    ui->pushButtonMaskSetMaker->setFixedWidth(100);
    ui->checkBoxAutoSearchMaker->move(660,10);
    ui->checkBoxAutoSearchMaker->setFixedWidth(150);
    ui->tableWidgetMaker->move(10,80);
    ui->tableWidgetMaker->setFixedSize(ui->tabWidget->width()/2,ui->tabWidget->height()-120);
    ui->pushButtonMakerNew->move(10,50);
    ui->pushButtonMakerEdit->move(190,50);
    ui->pushButtonMakerDelete->move(370,50);
    ui->listWidgetMakerData->move((ui->tabWidget->width()/2)+10,80);
    ui->listWidgetMakerData->setFixedSize((ui->tabWidget->width()/2)-25,(ui->tabWidget->height()-120));
    ui->lineMaker->move(0,25);
    ui->lineMaker->setFixedWidth(ui->tabWidget->width());

    //dealer widgets positions
    ui->labelCountDealer->move(ui->tabWidget->width()-240,10);
    ui->labelCountDealer->setFixedWidth(230);
    ui->labelDealer->move(10,10);
    ui->comboBoxMaskDealer->move(100,10);
    ui->comboBoxMaskDealer->setFixedWidth(200);
    ui->lineEditDealerMask->move(300,10);
    ui->lineEditDealerMask->setFixedWidth(250);
    ui->pushButtonMaskSetDealer->move(550,10);
    ui->pushButtonMaskSetDealer->setFixedWidth(100);
    ui->checkBoxAutoSearchDealer->move(660,10);
    ui->checkBoxAutoSearchDealer->setFixedWidth(150);
    ui->tableWidgetDealer->move(10,80);
    ui->tableWidgetDealer->setFixedSize(ui->tabWidget->width()/2,ui->tabWidget->height()-120);
    ui->pushButtonDealerNew->move(10,50);
    ui->pushButtonDealerEdit->move(190,50);
    ui->pushButtonDealerDelete->move(370,50);
    ui->listWidgetDealerData->move((ui->tabWidget->width()/2)+10,80);
    ui->listWidgetDealerData->setFixedSize((ui->tabWidget->width()/2)-25,(ui->tabWidget->height()-120));
    ui->lineDealer->move(0,25);
    ui->lineDealer->setFixedWidth(ui->tabWidget->width());

    //customer widgets positions
    ui->labelCountCustomer->move(ui->tabWidget->width()-240,10);
    ui->labelCountCustomer->setFixedWidth(230);
    ui->labelMaskCustomer->move(10,10);
    ui->comboBoxMaskCustomer->move(100,10);
    ui->comboBoxMaskCustomer->setFixedWidth(200);
    ui->lineEditCustomerMask->move(300,10);
    ui->lineEditCustomerMask->setFixedWidth(250);
    ui->pushButtonMaskSetCustomer->move(550,10);
    ui->pushButtonMaskSetCustomer->setFixedWidth(100);
    ui->checkBoxAutoSearchCustomer->move(660,10);
    ui->checkBoxAutoSearchCustomer->setFixedWidth(140);
    ui->tableWidgetCustomer->move(10,80);
    ui->tableWidgetCustomer->setFixedSize(ui->tabWidget->width()/2,ui->tabWidget->height()-120);
    ui->listWidgetCustomerData->move((ui->tabWidget->width()/2)+10,80);
    ui->listWidgetCustomerData->setFixedSize((ui->tabWidget->width()/2)-25,(ui->tabWidget->height()-120));
    ui->pushButtonCustomerNew->move(10,50);
    ui->pushButtonCustomerEdit->move(190,50);
    ui->pushButtonCustomerDelete->move(370,50);
    ui->lineCustomer->move(0,25);
    ui->lineCustomer->setFixedWidth(ui->tabWidget->width());

    //article widgets positions
    ui->labelCountArticle->move(ui->tabWidget->width()-240,10);
    ui->labelCountArticle->setFixedWidth(230);
    ui->labelMaskAricle->move(10,10);
    ui->comboBoxMaskArticle->move(100,10);
    ui->comboBoxMaskArticle->setFixedWidth(200);
    ui->lineEditArticleMask->move(300,10);
    ui->lineEditArticleMask->setFixedWidth(250);
    ui->pushButtonMaskSetArticle->move(550,10);
    ui->pushButtonMaskSetArticle->setFixedWidth(100);
    ui->checkBoxAutoSearchArticle->move(660,10);
    ui->checkBoxAutoSearchArticle->setFixedWidth(140);
    ui->tableWidgetArticle->move(10,80);
    ui->tableWidgetArticle->setFixedSize(ui->tabWidget->width()-25,ui->tabWidget->height()-120);
    ui->pushButtonArticleNew->move(10,50);
    ui->pushButtonArticleEdit->move(190,50);
    ui->pushButtonArticleCopy->move(370,50);
    ui->pushButtonArticleDelete->move(550,50);
    ui->lineArticle->move(0,25);
    ui->lineArticle->setFixedWidth(ui->tabWidget->width());

    //ordering widgets positions
    ui->labelCountOrdering->move(ui->tabWidget->width()-240,10);
    ui->labelCountOrdering->setFixedWidth(230);
    ui->labelMaskOrdering->move(10,10);
    ui->comboBoxMaskOrdering->move(100,10);
    ui->comboBoxMaskOrdering->setFixedWidth(200);
    ui->lineEditOrderingMask->move(300,10);
    ui->pushButtonMaskSetOrdering->move(550,10);
    ui->pushButtonMaskSetOrdering->setFixedWidth(100);
    ui->lineEditOrderingMask->setFixedWidth(250);
    ui->checkBoxAutoSearchOrdering->move(660,10);
    ui->checkBoxAutoSearchOrdering->setFixedWidth(140);
    ui->tableWidgetOrdering->move(10,80);
    ui->tableWidgetOrdering->setFixedSize(ui->tabWidget->width()-25,(ui->tabWidget->height()/2)-80);
    ui->labelOrderingWareslist->move(10,(ui->tabWidget->height()/2)+10);
    ui->tableWidgetOrderingWareslist->move(10,(ui->tabWidget->height()/2)+30);
    ui->tableWidgetOrderingWareslist->setFixedSize(ui->tabWidget->width()-25,(ui->tabWidget->height()/2)-70);
    ui->pushButtonOrderingNew->move(10,50);
    ui->pushButtonOrderingEdit->move(190,50);
    ui->pushButtonOrderingDelete->move(370,50);
    ui->lineOrdering->move(0,25);
    ui->lineOrdering->setFixedWidth(ui->tabWidget->width());

    //trade widgets positions
    ui->labelCountTrade->move(10,ui->tabWidget->height()-55);
    ui->labelCountTrade->setFixedWidth(260);
    ui->labelMaskWareInOut->move(10,10);
    ui->comboBoxMaskTrade->move(10,40);
    ui->comboBoxMaskTrade->setFixedWidth(260);
    ui->lineEditTradeMask->move(10,70);
    ui->lineEditTradeMask->setFixedWidth(260);
    ui->pushButtonMaskSetTrade->move(10,100);
    ui->pushButtonMaskSetTrade->setFixedWidth(100);
    ui->checkBoxAutoSearchTrade->move(130,100);
    ui->comboBoxTradeMaskYear->move(10,130);
    ui->comboBoxTradeMaskYear->setFixedWidth(100);
    ui->comboBoxTradeMaskDate->move(120,130);
    ui->comboBoxTradeMaskDate->setFixedWidth(150);
    ui->listWidgetTradeMaskDate->move(10,160);
    ui->listWidgetTradeMaskDate->setFixedSize(260,ui->tabWidget->height()-220);
    ui->lineTrade->move(265,0);
    ui->lineTrade->setFixedHeight(ui->tabWidget->height());
    ui->pushButtonTradeIncoming->move(290,10);
    ui->pushButtonTradeIncoming->setFixedWidth(120);
    ui->pushButtonTradeOutgoing->move(420,10);
    ui->pushButtonTradeOutgoing->setFixedWidth(120);
    ui->pushButtonTradeOrderingIncoming->move(550,10);
    ui->pushButtonTradeOrderingIncoming->setFixedWidth(120);
    ui->pushButtonTradeOutgoingCustomer->move(680,10);
    ui->pushButtonTradeOutgoingCustomer->setFixedWidth(120);
    ui->pushButtonTradeCancel->move(810,10);
    ui->pushButtonTradeCancel->setFixedWidth(120);
    ui->tableWidgetTrade->move(290,40);
    ui->tableWidgetTrade->setFixedSize(ui->tabWidget->width()-305,(ui->tabWidget->height()/2)-40);
    ui->labelTradeInfo->move(ui->tabWidget->width()-265,(ui->tabWidget->height()/2)+10);
    ui->listWidgetTradeInfo->move(ui->tabWidget->width()-265,(ui->tabWidget->height()/2)+30);
    ui->listWidgetTradeInfo->setFixedSize(250,(ui->tabWidget->height()/2)-70);
    ui->labelTradeWareslist->move(290,(ui->tabWidget->height()/2)+10);
    ui->tableWidgetTradeWareslist->move(290,(ui->tabWidget->height()/2)+30);
    ui->tableWidgetTradeWareslist->setFixedSize(ui->tabWidget->width()-565,(ui->tabWidget->height()/2)-70);
    trade_hide_info_list(trade_info_hide_status());//hide info??

    //inventory
    ui->labelCountInventory->move(ui->tabWidget->width()-240,10);
    ui->labelCountInventory->setFixedWidth(230);
    ui->labelMaskInventory->move(10,10);
    ui->comboBoxMaskInventory->move(100,10);
    ui->comboBoxMaskInventory->setFixedWidth(200);
    ui->lineEditInventoryMask->move(300,10);
    ui->lineEditInventoryMask->setFixedWidth(250);
    ui->pushButtonMaskSetInventory->move(550,10);
    ui->pushButtonMaskSetInventory->setFixedWidth(100);
    ui->checkBoxAutoSearchInventory->move(660,10);
    ui->checkBoxAutoSearchInventory->setFixedWidth(140);
    ui->tableWidgetInventory->move(10,50);
    ui->tableWidgetInventory->setFixedSize(ui->tabWidget->width()-25,ui->tabWidget->height()-90);
    ui->lineInventory->move(0,25);
    ui->lineInventory->setFixedWidth(ui->tabWidget->width());

    //logbook
    ui->labelCountLogbook->move(10,ui->tabWidget->height()-55);
    ui->labelCountLogbook->setFixedWidth(260);
    ui->labelMaskLogbook->move(10,10);
    ui->comboBoxLogbookMaskYear->move(10,40);
    ui->comboBoxLogbookMaskYear->setFixedWidth(80);
    ui->comboBoxMaskLogbook->move(100,40);
    ui->comboBoxMaskLogbook->setFixedWidth(170);
    ui->listWidgetLogbookMaskDate->move(10,70);
    ui->listWidgetLogbookMaskDate->setFixedSize(260,ui->tabWidget->height()-130);
    ui->listWidgetLogbook->move(290,50);
    ui->listWidgetLogbook->setFixedSize(ui->tabWidget->width()-305,ui->tabWidget->height()-100);
    ui->lineLogbook->move(265,0);
    ui->lineLogbook->setFixedHeight(ui->tabWidget->height());
    ui->label_count_log_item->move(290,10);
    ui->lineEditCountItemLog->move(660,10);
    ui->pushButtonLogbookChangeCount->move(750,10);
    ui->pushButtonLogbookClear->move(870,10);
    //-
    return true;
}

bool MainWindow::fill_all_table(bool bFillArticleAndInvTableNew)
{
    m_bUpdateTableTrade=m_bUpdateTableTradeWarelist=m_bUpdateTableOrdering=m_bUpdateTableOrderingWarelist=m_bUpdateTableArticle=true;
    m_bUpdateTableWaregroup=m_bUpdateTableMaker=m_bUpdateTableDealer=m_bUpdateTableCustomer=m_bUpdateListLogbook=m_bUpdateTableInventory=true;

    if(!bFillArticleAndInvTableNew)
        update_table_by_current_tab(ui->tabWidget->currentIndex());
    else
    {
        m_widgets.set_block_signal_and_sort(ui->tableWidgetInventory,true,false);//faster
        m_widgets.remove_all_rows(ui->tableWidgetInventory);
        m_widgets.set_block_signal_and_sort(ui->tableWidgetInventory,false,true);
        m_widgets.set_block_signal_and_sort(ui->tableWidgetArticle,true,false);//faster
        m_widgets.remove_all_rows(ui->tableWidgetArticle);
        m_widgets.set_block_signal_and_sort(ui->tableWidgetArticle,false,true);
        m_widgets.set_block_signal_and_sort(ui->tableWidgetTrade,true,false);//faster
        m_widgets.remove_all_rows(ui->tableWidgetTrade);
        m_widgets.set_block_signal_and_sort(ui->tableWidgetTrade,false,true);
        //-
        inventory_mask_set();
        article_mask_set();
        trade_mask_set();
        ordering_mask_set();
        m_bUpdateTableArticle=m_bUpdateTableInventory=m_bUpdateTableTrade=m_bUpdateTableTradeWarelist=false;
    }
    return true;
}

bool MainWindow::fill_logbook_count(void)
{
    int i;
    QString s;

    if(m_db.is_db_connect())
    {
        i=m_db.get_logbook_count();
        if(i>0)
        {
            s=QString::fromUtf8("%1").arg(i);
            ui->lineEditCountItemLog->setText(s);
        }
    }
    return true;
}

bool MainWindow::update_table_by_current_tab(int index)
{
    //check for update table's by tab switch
    if(index==ui->tabWidget->indexOf(ui->tabInventory) && m_bUpdateTableInventory==true)
    {
        inventory_mask_set();
        m_bUpdateTableInventory=false;
        article_widgetitem_clicked();
    }
    if(index==ui->tabWidget->indexOf(ui->tabOrdering))
    {
        if(m_bUpdateTableOrdering==true)
            ordering_mask_set();
        else if(m_bUpdateTableOrderingWarelist==true)
            ordering_update_wares_list();
        m_bUpdateTableOrdering=m_bUpdateTableOrderingWarelist=false;
    }
    if(index==ui->tabWidget->indexOf(ui->tabWareInOut))
    {
        if(m_bUpdateTableTrade==true)
            trade_mask_set();
        else if(m_bUpdateTableTradeWarelist==true)
            trade_update_wares_list();
        m_bUpdateTableTrade=m_bUpdateTableTradeWarelist=false;
    }
    if(index==ui->tabWidget->indexOf(ui->tabArticle) && m_bUpdateTableArticle==true)
    {
        article_mask_set();
        m_bUpdateTableArticle=false;
        inventory_widgetitem_clicked();
    }
    if(index==ui->tabWidget->indexOf(ui->tabWaregroup) && m_bUpdateTableWaregroup==true)
    {
        waregroup_update_tree();
        m_bUpdateTableWaregroup=false;
    }
    if(index==ui->tabWidget->indexOf(ui->tabMaker) && m_bUpdateTableMaker==true)
    {
        maker_mask_set();
        m_bUpdateTableMaker=false;
    }
    if(index==ui->tabWidget->indexOf(ui->tabDealer) && m_bUpdateTableDealer==true)
    {
        dealer_mask_set();
        m_bUpdateTableDealer=false;
    }
    if(index==ui->tabWidget->indexOf(ui->tabCustomer) && m_bUpdateTableCustomer==true)
    {
        customer_mask_set();
        m_bUpdateTableCustomer=false;
    }
    if(index==ui->tabWidget->indexOf(ui->tabLog) && m_bUpdateListLogbook==true)
    {
        logbook_mask_set();
        m_bUpdateListLogbook=false;
    }
    return true;
}

bool MainWindow::fill_date_lists(void)
{
    QDate dt=QDate().currentDate();
    if(dt.year()>=2009)
    {
        trade_update_datelist(dt,dt.year());
        logbook_update_datelist(dt,dt.year());
    }
    return true;
}


bool MainWindow::settings(bool bUpdate)
{
    QList<QWidget*> lsPWidget;//for tab-order
    lsPWidget.push_back(ui->tabInventory);
    lsPWidget.push_back(ui->tabWareInOut);
    lsPWidget.push_back(ui->tabOrdering);
    lsPWidget.push_back(ui->tabArticle);
    lsPWidget.push_back(ui->tabWaregroup);
    lsPWidget.push_back(ui->tabMaker);
    lsPWidget.push_back(ui->tabDealer);
    lsPWidget.push_back(ui->tabCustomer);
    lsPWidget.push_back(ui->tabLog);

    CSettings settings;
    QList<QString> lsSType,lsSValues,lsSUpdateType,lsSUpdateValue;
    //-
    lsSType.push_back(QString::fromUtf8("TAB_ORDER"));
    //-
    lsSType.push_back(QString::fromUtf8("INVENTORY_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString::fromUtf8("SORT_ORDER_INVENTORY"));
    lsSType.push_back(QString::fromUtf8("AUTO_CHECKBOX_MAIN_INV"));
    lsSType.push_back(QString::fromUtf8("MASK_SELECTION_INVENTORY"));
    lsSType.push_back(QString::fromUtf8("INVENTORY_TABLE_COLUMNS_ORDER_VISIBLE_ALIGMENT"));
    //-
    lsSType.push_back(QString::fromUtf8("TRADE_TABLE_COLUMNS_WIDTHS_OVIEW"));
    lsSType.push_back(QString::fromUtf8("SORT_ORDER_TRADE_OVIEW"));
    lsSType.push_back(QString::fromUtf8("TRADE_TABLE_COLUMNS_WIDTHS_WARES"));
    lsSType.push_back(QString::fromUtf8("SORT_ORDER_TRADE_WARES"));
    lsSType.push_back(QString::fromUtf8("AUTO_CHECKBOX_MAIN_TRADE"));
    lsSType.push_back(QString::fromUtf8("MASK_SELECTION_TRADE"));
    lsSType.push_back(QString::fromUtf8("MASK_SELECTION_TRADE_PERIOD"));
    //-
    lsSType.push_back(QString::fromUtf8("ORDERING_TABLE_COLUMNS_WIDTHS_OVIEW"));
    lsSType.push_back(QString::fromUtf8("SORT_ORDER_ORDERING_OVIEW"));
    lsSType.push_back(QString::fromUtf8("ORDERING_TABLE_COLUMNS_WIDTHS_WARES"));
    lsSType.push_back(QString::fromUtf8("SORT_ORDER_ORDERING_WARES"));
    lsSType.push_back(QString::fromUtf8("AUTO_CHECKBOX_MAIN_ORDERING"));
    lsSType.push_back(QString::fromUtf8("MASK_SELECTION_ORDERING"));
    //-
    lsSType.push_back(QString::fromUtf8("ARTICLE_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString::fromUtf8("SORT_ORDER_ARTICLE"));
    lsSType.push_back(QString::fromUtf8("AUTO_CHECKBOX_MAIN_ARTICLE"));
    lsSType.push_back(QString::fromUtf8("MASK_SELECTION_ARTICLE"));
    lsSType.push_back(QString::fromUtf8("ARTICLE_TABLE_COLUMNS_ORDER_VISIBLE_ALIGMENT"));
    //-
    lsSType.push_back(QString::fromUtf8("WAREGROUP_TREE_COLUMNS_WIDTHS"));
    //-
    lsSType.push_back(QString::fromUtf8("MAKER_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString::fromUtf8("SORT_ORDER_MAKER"));
    lsSType.push_back(QString::fromUtf8("AUTO_CHECKBOX_MAIN_MAKER"));
    lsSType.push_back(QString::fromUtf8("MASK_SELECTION_MAKER"));
    //-
    lsSType.push_back(QString::fromUtf8("DEALER_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString::fromUtf8("SORT_ORDER_DEALER"));
    lsSType.push_back(QString::fromUtf8("AUTO_CHECKBOX_MAIN_DEALER"));
    lsSType.push_back(QString::fromUtf8("MASK_SELECTION_DEALER"));
    //-
    lsSType.push_back(QString::fromUtf8("CUSTOMER_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString::fromUtf8("SORT_ORDER_CUSTOMER"));
    lsSType.push_back(QString::fromUtf8("AUTO_CHECKBOX_MAIN_CUSTOMER"));
    lsSType.push_back(QString::fromUtf8("MASK_SELECTION_CUSTOMER"));
    //-
    lsSType.push_back(QString::fromUtf8("MASK_SELECTION_LOGBOOK"));

    //load
    QString s;
    bool b=settings.load(lsSType,lsSValues);
    if(b && lsSValues.count()==lsSType.count())
    {
        if(!bUpdate)//set
        {
            //--tab-order--
            settings.set_tab_order(ui->tabWidget,lsSValues[0]);
            ui->tabWidget->setCurrentWidget(ui->tabInventory);

            //--inventory--
            settings.set_table_columns_width(ui->tableWidgetInventory,lsSValues[1],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetInventory,lsSValues[2],0);
            settings.set_checkbox(ui->checkBoxAutoSearchInventory,lsSValues[3],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskInventory,lsSValues[4],0);//mask selection
            //-
            m_widgets.m_tcInventory.set_table(ui->tableWidgetInventory);
            m_widgets.m_tcInventory.set_id_column(ui->tableWidgetInventory->columnCount()-1);
            settings.set_table_columns_order_visible_alignment(&m_widgets.m_tcInventory,lsSValues[5]);//table order & visible

            //--trade--
            settings.set_table_columns_width(ui->tableWidgetTrade,lsSValues[6],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetTrade,lsSValues[7],3);
            settings.set_table_columns_width(ui->tableWidgetTradeWareslist,lsSValues[8],200);//table 2 width+sort
            settings.set_table_sort(ui->tableWidgetTradeWareslist,lsSValues[9],1);
            settings.set_checkbox(ui->checkBoxAutoSearchTrade,lsSValues[10],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskTrade,lsSValues[11],0);//mask selection
            settings.set_combobox(ui->comboBoxTradeMaskDate,lsSValues[12],0);//mask 2 selection

            //--ordering--
            settings.set_table_columns_width(ui->tableWidgetOrdering,lsSValues[13],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetOrdering,lsSValues[14],0);
            settings.set_table_columns_width(ui->tableWidgetOrderingWareslist,lsSValues[15],200);//table 2 width+sort
            settings.set_table_sort(ui->tableWidgetOrderingWareslist,lsSValues[16],1);
            settings.set_checkbox(ui->checkBoxAutoSearchOrdering,lsSValues[17],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskOrdering,lsSValues[18],0);//mask selection

            //--article--
            settings.set_table_columns_width(ui->tableWidgetArticle,lsSValues[19],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetArticle,lsSValues[20],0);
            settings.set_checkbox(ui->checkBoxAutoSearchArticle,lsSValues[21],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskArticle,lsSValues[22],0);//mask selection
            //-
            m_widgets.m_tcArticle.set_table(ui->tableWidgetArticle);
            m_widgets.m_tcArticle.set_id_column(ui->tableWidgetArticle->columnCount()-1);
            settings.set_table_columns_order_visible_alignment(&m_widgets.m_tcArticle,lsSValues[23]);//table order & visible

            //--waregroup--
            settings.set_tree_width(ui->treeWidgetWaregroup,lsSValues[24],500);//tree width

            //--maker--
            settings.set_table_columns_width(ui->tableWidgetMaker,lsSValues[25],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetMaker,lsSValues[26],0);
            settings.set_checkbox(ui->checkBoxAutoSearchMaker,lsSValues[27],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskMaker,lsSValues[28],0);//mask selection

            //--dealer--
            settings.set_table_columns_width(ui->tableWidgetDealer,lsSValues[29],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetDealer,lsSValues[30],0);
            settings.set_checkbox(ui->checkBoxAutoSearchDealer,lsSValues[31],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskDealer,lsSValues[32],0);//mask selection

            //--customer--
            settings.set_table_columns_width(ui->tableWidgetCustomer,lsSValues[33],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetCustomer,lsSValues[34],1);
            settings.set_checkbox(ui->checkBoxAutoSearchCustomer,lsSValues[35],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskCustomer,lsSValues[36],0);//mask selection

            //--logbook--
            settings.set_combobox(ui->comboBoxMaskLogbook,lsSValues[37],0);//mask selection
        }
        if(bUpdate)//write
        {
            //--tab-order--
            if(settings.get_tab_order(ui->tabWidget,lsSValues[0],lsPWidget))//setting change?
            {//setting change?
                lsSUpdateType.push_back(lsSType[0]);//mark
                lsSUpdateValue.push_back(lsSValues[0]);
            }

            //--inventory--
            if(settings.get_table_columns_width(ui->tableWidgetInventory,lsSValues[1]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[1]);
                lsSUpdateValue.push_back(lsSValues[1]);
            }
            if(settings.get_table_sort(ui->tableWidgetInventory,lsSValues[2]))
            {
                lsSUpdateType.push_back(lsSType[2]);
                lsSUpdateValue.push_back(lsSValues[2]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchInventory,lsSValues[3]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[3]);
                lsSUpdateValue.push_back(lsSValues[3]);
            }
            if(settings.get_combobox(ui->comboBoxMaskInventory,lsSValues[4]))//combobox
            {
                lsSUpdateType.push_back(lsSType[4]);
                lsSUpdateValue.push_back(lsSValues[4]);
            }
            if(settings.get_table_columns_order_visible_alignment(&m_widgets.m_tcInventory,lsSValues[5]))//table order & visible
            {
                lsSUpdateType.push_back(lsSType[5]);
                lsSUpdateValue.push_back(lsSValues[5]);
            }

            //--trade--
            if(settings.get_table_columns_width(ui->tableWidgetTrade,lsSValues[6]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[6]);
                lsSUpdateValue.push_back(lsSValues[6]);
            }
            if(settings.get_table_sort(ui->tableWidgetTrade,lsSValues[7]))
            {
                lsSUpdateType.push_back(lsSType[7]);
                lsSUpdateValue.push_back(lsSValues[7]);
            }
            if(settings.get_table_columns_width(ui->tableWidgetTradeWareslist,lsSValues[8]))//table 2 width+sort
            {
                lsSUpdateType.push_back(lsSType[8]);
                lsSUpdateValue.push_back(lsSValues[8]);
            }
            if(settings.get_table_sort(ui->tableWidgetTradeWareslist,lsSValues[9]))
            {
                lsSUpdateType.push_back(lsSType[9]);
                lsSUpdateValue.push_back(lsSValues[9]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchTrade,lsSValues[10]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[10]);
                lsSUpdateValue.push_back(lsSValues[10]);
            }
            if(settings.get_combobox(ui->comboBoxMaskTrade,lsSValues[11]))//combobox
            {
                lsSUpdateType.push_back(lsSType[11]);
                lsSUpdateValue.push_back(lsSValues[11]);
            }
            if(settings.get_combobox(ui->comboBoxTradeMaskDate,lsSValues[12]))//combobox 2
            {
                lsSUpdateType.push_back(lsSType[12]);
                lsSUpdateValue.push_back(lsSValues[12]);
            }

            //--ordering--
            if(settings.get_table_columns_width(ui->tableWidgetOrdering,lsSValues[13]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[13]);
                lsSUpdateValue.push_back(lsSValues[13]);
            }
            if(settings.get_table_sort(ui->tableWidgetOrdering,lsSValues[14]))
            {
                lsSUpdateType.push_back(lsSType[14]);
                lsSUpdateValue.push_back(lsSValues[14]);
            }
            if(settings.get_table_columns_width(ui->tableWidgetOrderingWareslist,lsSValues[15]))//table 2 width+sort
            {
                lsSUpdateType.push_back(lsSType[15]);
                lsSUpdateValue.push_back(lsSValues[15]);
            }
            if(settings.get_table_sort(ui->tableWidgetOrderingWareslist,lsSValues[16]))
            {
                lsSUpdateType.push_back(lsSType[16]);
                lsSUpdateValue.push_back(lsSValues[16]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchOrdering,lsSValues[17]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[17]);
                lsSUpdateValue.push_back(lsSValues[17]);
            }
            if(settings.get_combobox(ui->comboBoxMaskOrdering,lsSValues[18]))//combobox
            {
                lsSUpdateType.push_back(lsSType[18]);
                lsSUpdateValue.push_back(lsSValues[18]);
            }

            //--article--
            if(settings.get_table_columns_width(ui->tableWidgetArticle,lsSValues[19]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[19]);
                lsSUpdateValue.push_back(lsSValues[19]);
            }
            if(settings.get_table_sort(ui->tableWidgetArticle,lsSValues[20]))
            {
                lsSUpdateType.push_back(lsSType[20]);
                lsSUpdateValue.push_back(lsSValues[20]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchArticle,lsSValues[21]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[21]);
                lsSUpdateValue.push_back(lsSValues[21]);
            }
            if(settings.get_combobox(ui->comboBoxMaskArticle,lsSValues[22]))//combobox
            {
                lsSUpdateType.push_back(lsSType[22]);
                lsSUpdateValue.push_back(lsSValues[22]);
            }
            if(settings.get_table_columns_order_visible_alignment(&m_widgets.m_tcArticle,lsSValues[23]))//table order & visible
            {
                lsSUpdateType.push_back(lsSType[23]);
                lsSUpdateValue.push_back(lsSValues[23]);
            }

            //--waregroup--
            if(settings.get_tree_width(ui->treeWidgetWaregroup,lsSValues[24]))//combobox
            {
                lsSUpdateType.push_back(lsSType[24]);
                lsSUpdateValue.push_back(lsSValues[24]);
            }

            //--maker--
            if(settings.get_table_columns_width(ui->tableWidgetMaker,lsSValues[25]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[25]);
                lsSUpdateValue.push_back(lsSValues[25]);
            }
            if(settings.get_table_sort(ui->tableWidgetMaker,lsSValues[26]))
            {
                lsSUpdateType.push_back(lsSType[26]);
                lsSUpdateValue.push_back(lsSValues[26]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchMaker,lsSValues[27]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[27]);
                lsSUpdateValue.push_back(lsSValues[27]);
            }
            if(settings.get_combobox(ui->comboBoxMaskMaker,lsSValues[28]))//combobox
            {
                lsSUpdateType.push_back(lsSType[28]);
                lsSUpdateValue.push_back(lsSValues[28]);
            }

            //--dealer--
            if(settings.get_table_columns_width(ui->tableWidgetDealer,lsSValues[29]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[29]);
                lsSUpdateValue.push_back(lsSValues[29]);
            }
            if(settings.get_table_sort(ui->tableWidgetDealer,lsSValues[30]))
            {
                lsSUpdateType.push_back(lsSType[30]);
                lsSUpdateValue.push_back(lsSValues[30]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchDealer,lsSValues[31]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[31]);
                lsSUpdateValue.push_back(lsSValues[31]);
            }
            if(settings.get_combobox(ui->comboBoxMaskDealer,lsSValues[32]))//combobox
            {
                lsSUpdateType.push_back(lsSType[32]);
                lsSUpdateValue.push_back(lsSValues[32]);
            }

            //--customer--
            if(settings.get_table_columns_width(ui->tableWidgetCustomer,lsSValues[33]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[33]);
                lsSUpdateValue.push_back(lsSValues[33]);
            }
            if(settings.get_table_sort(ui->tableWidgetCustomer,lsSValues[34]))
            {
                lsSUpdateType.push_back(lsSType[34]);
                lsSUpdateValue.push_back(lsSValues[34]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchCustomer,lsSValues[35]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[35]);
                lsSUpdateValue.push_back(lsSValues[35]);
            }
            if(settings.get_combobox(ui->comboBoxMaskCustomer,lsSValues[36]))//combobox
            {
                lsSUpdateType.push_back(lsSType[36]);
                lsSUpdateValue.push_back(lsSValues[36]);
            }

            //--logbook--
            if(settings.get_combobox(ui->comboBoxMaskLogbook,lsSValues[37]))//combobox
            {
                lsSUpdateType.push_back(lsSType[37]);
                lsSUpdateValue.push_back(lsSValues[37]);
            }


            //----write----
            if(lsSUpdateType.count()>0 && lsSUpdateType.count()==lsSUpdateValue.count())
                b=settings.write(lsSUpdateType,lsSUpdateValue);

            //remove flag 'programm_running'
            settings.remove_line(QString::fromUtf8("PROGRAM_RUNNING"));
        }
    }
    //-
    lsSUpdateType.clear();
    lsSUpdateValue.clear();
    lsSType.clear();
    lsSValues.clear();
    lsPWidget.clear();
    return true;
}

bool MainWindow::check_first_start(void)
{
    QString s;
    bool bReturn=true;
    CSettings settings;
    QPushButton * noButton=NULL;
    QMessageBox msgAgreement(QMessageBox::Information,"","");
    bool bGiveItSetting=settings.give_it_setting_file();
    //-
    if(!bGiveItSetting)//first start?
    {
        msgAgreement.setWindowTitle(QString::fromUtf8("Nutzungsbedingungen:"));
        s=QString::fromUtf8("Dieses Programm(LaVa 2) steht unter der open-source-Lizenz GNU-GPL v3. Es entstehen für die Nutzung "
                  "keine Kosten. Der Autor entbindet sich von jeglichen Haftungsansprüchen, gleich welcher Art. "
                  "\n\nSind Sie mit diesen Bedingungen einverstanden?");
        msgAgreement.setText(s);
        msgAgreement.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
        noButton=msgAgreement.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
        msgAgreement.exec();
        if(msgAgreement.clickedButton()==noButton)
            bReturn=false;
        else
            settings.create_default_setting_file();//create default
    }
    return bReturn;
}

bool MainWindow::check_run_the_program(void)
{
    QString s;
    CSettings settings;
    bool bReturn=false;
    QMessageBox msgWarning(QMessageBox::Warning,"","");
    //-
    if(settings.is_program_running())//run the program?
    {
        msgWarning.setWindowTitle(QString::fromUtf8("Warnung"));
        s=QString::fromUtf8("Das Programm läuft bereits oder wurde das letzte Mal gewaltsam beendet. Bitte überprüfen Sie das!\n(beim nächsten Programmstart wird diese Meldung nicht mehr angezeigt)");
        msgWarning.setText(s);
        msgWarning.exec();
        settings.remove_line(QString::fromUtf8("PROGRAM_RUNNING"));
        bReturn=true;
    }
    else
    {
        s=QString::fromUtf8("PROGRAM_RUNNING");//set program running
        settings.write(s,QString::fromUtf8(""));
    }
    return bReturn;
}

bool MainWindow::open_context_menu()
{
    if(m_pContextMenuArticle==NULL || m_pContextMenuDefault==NULL || m_pContextMenuTrade==NULL || !m_db.is_db_connect())
        return false;
    //-
    QString s;
    bool bTradeCanceled=false;
    bool bEditDeleteCopyEnable=true;
    int iContextMenu=0; //0=default  1=trade  2=article
    QList<QAction*> lsAct;
    QList<QTableWidgetItem*> ls;
    QList<QTreeWidgetItem*> ls2;
    //-
    if(ui->treeWidgetWaregroup->hasFocus())
    {//tree
        ls2=ui->treeWidgetWaregroup->selectedItems();
        if(ls2.count()<=0)
            bEditDeleteCopyEnable=false;
        else{}
    }
    else
    {//table's
        if(ui->tableWidgetMaker->hasFocus())
            ls=ui->tableWidgetMaker->selectedItems();
        if(ui->tableWidgetDealer->hasFocus())
            ls=ui->tableWidgetDealer->selectedItems();
        if(ui->tableWidgetCustomer->hasFocus())
            ls=ui->tableWidgetCustomer->selectedItems();
        if(ui->tableWidgetOrdering->hasFocus())
            ls=ui->tableWidgetOrdering->selectedItems();
        if(ui->tableWidgetArticle->hasFocus())
        {
            iContextMenu=2;//article
            ls=ui->tableWidgetArticle->selectedItems();
        }
        if(ui->tableWidgetTrade->hasFocus())//trade
        {
            iContextMenu=1;//trade
            ls=ui->tableWidgetTrade->selectedItems();
            if(ls.count()>2)
            {
                s=ls[2]->text();
                bTradeCanceled=m_db.trade_is_canceled(s);
            }
        }
        //-
        if(ls.count()<=0 || bTradeCanceled==true)//nothing item selected?  or trade canceled
            bEditDeleteCopyEnable=false;
    }
    //-
    if(iContextMenu==0)//default
    {
        lsAct=m_pContextMenuDefault->actions();
        if(lsAct.count()>2)
        {
            lsAct[1]->setEnabled(bEditDeleteCopyEnable);
            lsAct[2]->setEnabled(bEditDeleteCopyEnable);
        }
        //-
        m_pContextMenuDefault->popup(QCursor::pos());
    }
    if(iContextMenu==1)//trade
    {
        lsAct=m_pContextMenuTrade->actions();
        if(lsAct.count()>4)
            lsAct[4]->setEnabled(bEditDeleteCopyEnable);
        //-
        m_pContextMenuTrade->popup(QCursor::pos());
    }
    if(iContextMenu==2)//article
    {
        lsAct=m_pContextMenuArticle->actions();
        if(lsAct.count()>3)
        {
            lsAct[1]->setEnabled(bEditDeleteCopyEnable);
            lsAct[2]->setEnabled(bEditDeleteCopyEnable);
            lsAct[3]->setEnabled(bEditDeleteCopyEnable);
        }
        //-
        m_pContextMenuArticle->popup(QCursor::pos());
    }
    //-
    ls.clear();
    ls2.clear();
    lsAct.clear();
    return true;
}

bool MainWindow::receiv_context_menu(QAction * pAction)
{
    if(pAction==NULL)
        return false;

    if(pAction->text()==QString::fromUtf8("Wareneingang"))
        trade_incoming();
    if(pAction->text()==QString::fromUtf8("Warenausgang"))
        trade_outgoing();
    if(pAction->text()==QString::fromUtf8("Wareneingang(Bestellung)"))
        trade_ordering_incoming();
    if(pAction->text()==QString::fromUtf8("Warenausgang(Kunde)"))
        trade_outgoing_customer();
    if(pAction->text()==QString::fromUtf8("stornieren"))
        trade_canceled();
    if(pAction->text()==QString::fromUtf8("neu"))
    {//new
        if(ui->tableWidgetArticle->hasFocus())//article
            article_new();
        else if(ui->tableWidgetMaker->hasFocus())//maker
            maker_new();
        else if(ui->treeWidgetWaregroup->hasFocus())//waregroup
            waregroup_new();
        else if(ui->tableWidgetDealer->hasFocus())//dealer
            dealer_new();
        else if(ui->tableWidgetCustomer->hasFocus())//customer
            customer_new();
        else if(ui->tableWidgetOrdering->hasFocus())//ordering
            ordering_new();
    }
    if(pAction->text()==QString::fromUtf8("bearbeiten"))
    {//edit
        if(ui->tableWidgetArticle->hasFocus())//article
            article_edit();
        else if(ui->tableWidgetMaker->hasFocus())//maker
            maker_edit();
        else if(ui->treeWidgetWaregroup->hasFocus())//waregroup
            waregroup_edit();
        else if(ui->tableWidgetDealer->hasFocus())//dealer
            dealer_edit();
        else if(ui->tableWidgetCustomer->hasFocus())//customer
            customer_edit();
        else if(ui->tableWidgetOrdering->hasFocus())//ordering
            ordering_edit();
    }
    if(pAction->text()==QString::fromUtf8("loeschen"))
    {//delete
        if(ui->tableWidgetArticle->hasFocus())//article
            article_delete();
        else if(ui->tableWidgetMaker->hasFocus())//maker
            maker_delete();
        else if(ui->treeWidgetWaregroup->hasFocus())//waregroup
            waregroup_delete();
        else if(ui->tableWidgetDealer->hasFocus())//dealer
            dealer_delete();
        else if(ui->tableWidgetCustomer->hasFocus())//customer
            customer_delete();
        else if(ui->tableWidgetOrdering->hasFocus())//ordering
            ordering_delete();
    }
    if(pAction->text()==QString::fromUtf8("kopieren"))
    {//copy
        article_copy();//article
    }
    return true;
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    //ENTEr-key
    if(event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return)
    {
        if(ui->lineEditInventoryMask->hasFocus())
            inventory_mask_set();
        if(ui->lineEditTradeMask->hasFocus())
            trade_mask_set();
        if(ui->lineEditOrderingMask->hasFocus())
            ordering_mask_set();
        if(ui->lineEditArticleMask->hasFocus())
            article_mask_set();
        if(ui->lineEditMakerMask->hasFocus())
            maker_mask_set();
        if(ui->lineEditDealerMask->hasFocus())
            dealer_mask_set();
        if(ui->lineEditCustomerMask->hasFocus())
            customer_mask_set();
    }

    //F1-key
    if(event->key()==Qt::Key_F1)
        menu_help();

    //DELETE-key
    QList<QTableWidgetItem*> ls;
    QList<QTreeWidgetItem*> ls2;
    if(event->key()==Qt::Key_Delete)
    {
        if(ui->tableWidgetTrade->hasFocus())//trade
        {
            ls=ui->tableWidgetTrade->selectedItems();
            if(ls.count()>0)//row selected?
                trade_canceled();
        }
        if(ui->tableWidgetOrdering->hasFocus())//ordering
        {
            ls=ui->tableWidgetOrdering->selectedItems();
            if(ls.count()>0)//row selected?
                ordering_delete();
        }
        if(ui->tableWidgetArticle->hasFocus())//article
        {
            ls=ui->tableWidgetArticle->selectedItems();
            if(ls.count()>0)//row selected?
                article_delete();
        }
        if(ui->treeWidgetWaregroup->hasFocus())//waregroup
        {
            ls2=ui->treeWidgetWaregroup->selectedItems();
            if(ls2.count()>0)//row selected?
                waregroup_delete();
        }
        if(ui->tableWidgetMaker->hasFocus())//maker
        {
            ls=ui->tableWidgetMaker->selectedItems();
            if(ls.count()>0)//row selected?
                maker_delete();
        }
        if(ui->tableWidgetDealer->hasFocus())//dealer
        {
            ls=ui->tableWidgetDealer->selectedItems();
            if(ls.count()>0)//row selected?
                dealer_delete();
        }
        if(ui->tableWidgetCustomer->hasFocus())//customer
        {
            ls=ui->tableWidgetCustomer->selectedItems();
            if(ls.count()>0)//row selected?
                customer_delete();
        }
    }
    ls.clear();
    ls2.clear();
}

void MainWindow::tab_switched(int index)
{
    ui->lineEditInventoryMask->clearFocus();
    ui->lineEditTradeMask->clearFocus();
    ui->lineEditOrderingMask->clearFocus();
    ui->lineEditArticleMask->clearFocus();
    ui->lineEditMakerMask->clearFocus();
    ui->lineEditDealerMask->clearFocus();
    ui->lineEditCustomerMask->clearFocus();
    //-
    update_table_by_current_tab(index);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    static QDate mark_date=QDate(0,0,0);
    static bool mark_messagebox_error_open=false;
    static QString mark_error_string=QString::fromUtf8("");

    bool bQuit=false;
    QMessageBox msg(QMessageBox::Critical,"","");
    QMessageBox msg2(QMessageBox::Information,"","");
    msg2.setWindowTitle(QString::fromUtf8("Achtung!"));
    msg2.setText(QString::fromUtf8("Eine andere Programminstanz ist dabei eine Sicherung zu laden, ein Neustart des Programmes zur Wiederherstellung der Datenbankverbindung ist nötig!"));
    QString s,s2,title;
    QDate dt=QDate().currentDate();
    int type,id;

    //title back
    title=windowTitle();
    if(title.indexOf(QString::fromUtf8("user"))<0)
    {
        title=QString::fromUtf8("lava2 - user: %1").arg(m_sUser);
        setWindowTitle(title);
    }

    //db change from another client?
    if(m_LastDbChange.check_update_last_change())
    {
        //info in main title
         setWindowTitle(QString::fromUtf8("lava2 - Datenbank wurde von einer anderer Programminstanz verändert..."));

         s=m_LastDbChange.get_last_change();//get fom db
         //-
         if(m_LastDbChange.get_data_from_last_change_string(type,id,s))//split string
         {
             if(type==LC_ACTION_UPDATE_ORDER_QUIT)
             {
                 msg2.exec();
                 bQuit=true;
             }
             else
             {
                 //update table's
                 //maker
                 if(type==LC_ACTION_UPDATE_MAKER || type==LC_ACTION_UPDATE_ALL || type==LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG)
                     m_bUpdateTableMaker=true;

                 //dealer
                 if(type==LC_ACTION_UPDATE_DEALER || type==LC_ACTION_UPDATE_ALL || type==LC_ACTION_UPDATE_ORDERING_DEALER)
                     m_bUpdateTableDealer=true;

                 //waregroup
                 if(type==LC_ACTION_UPDATE_WAREGROUP || type==LC_ACTION_UPDATE_ALL ||type==LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG)
                     m_bUpdateTableWaregroup=true;

                 //customer
                 if(type==LC_ACTION_UPDATE_CUSTOMER || type==LC_ACTION_UPDATE_ALL)
                     m_bUpdateTableCustomer=true;

                 //article
                 if(type==LC_ACTION_UPDATE_ARTICLE_INV || type==LC_ACTION_UPDATE_ALL || type==LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG)
                     m_bUpdateTableArticle=true;

                 //ordering
                 if(type==LC_ACTION_UPDATE_ORDERING || type==LC_ACTION_UPDATE_ALL || type==LC_ACTION_UPDATE_ORDERING_DEALER || type==LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG
                         || type==LC_ACTION_UPDATE_INVENTORY_TRADE_ORDERING)
                     m_bUpdateTableOrdering=m_bUpdateTableOrderingWarelist=true;

                 //trade
                 if(type==LC_ACTION_UPDATE_ALL || type==LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG || type==LC_ACTION_UPDATE_INVENTORY_TRADE || type==LC_ACTION_UPDATE_INVENTORY_TRADE_ORDERING)
                     m_bUpdateTableTrade=m_bUpdateTableTradeWarelist=true;

                 //inventory
                 if(type==LC_ACTION_UPDATE_ALL || type==LC_ACTION_UPDATE_ARTICLE_INV || type==LC_ACTION_UPDATE_INVENTORY_TRADE || type==LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG
                         || type==LC_ACTION_UPDATE_INVENTORY_TRADE_ORDERING)
                     m_bUpdateTableInventory=true;

                 //logbook count
                 if(type==LC_ACTION_UPDATE_LOGBOOK_COUNT || type==LC_ACTION_UPDATE_ALL)
                 {
                     fill_logbook_count();
                 }

                 //logbook ever update
                 m_bUpdateListLogbook=true;

                 //update current index of tab-widget
                 update_table_by_current_tab(ui->tabWidget->currentIndex());
             }
         }
    }

    //-
    if(!bQuit)
    {
        //check db-error
        s=m_db.last_error();
        if(s.length()>3 && s!=mark_error_string)//error db?
        {
            if(!mark_messagebox_error_open)//msg not open ?
            {
                mark_error_string=s;//mark
                mark_messagebox_error_open=true;
                s2=msg.text();
                s2+=QString::fromUtf8("\n%1").arg(s);
                msg.setWindowTitle("error - database");
                msg.exec();
                mark_messagebox_error_open=false;
            }
        }

        //new date?
        if(mark_date==QDate(0,0,0))
            mark_date=dt;
        else if(dt>mark_date)
        {//new day?->update mask trade date list

            if(dt.year()>mark_date.year() && mark_date!=QDate(0,0,0))//new year?
            {
                s=QString::fromUtf8("%1").arg(dt.year());
                ui->comboBoxTradeMaskYear->insertItem(0,s);
            }
            //-
            trade_mask_date_changed();
            mark_date=dt;
        }
        //-
        m_iTimerId=event->timerId();
    }
    if(bQuit)
    {
        close();
    }
}

bool MainWindow::open_picture_view(QTableWidget * pTable)
{
    int iId=-1;
    QString sPath;
    CArticle ar;
    CPictureViewDialog dlg(this);
    bool bReturn=false;
    //-
    if(pTable!=NULL)
    {
        //open dialog & draw picture
        if(m_widgets.get_selected_table_item_value(pTable,iId))//select?
        {
            if(m_db.article_get(iId,ar))
            {
                sPath=ar.get_path_picture();
                if(sPath.length()>0)
                {
                    if(dlg.set_data(sPath))
                    {
                        dlg.exec();
                        bReturn=true;
                    }
                }
            }
        }

    }
    return bReturn;
}


//---waregroup----------------------------------------------------------------------------------------

bool MainWindow::waregroup_update_tree(int iIdSelect)
{
    m_widgets.waregroup_update_treewidget(ui->treeWidgetWaregroup,iIdSelect);
    waregroup_check_user_input();
    waregroup_update_count();
    return true;
}

bool MainWindow::waregroup_check_user_input(void)
{
    QList <QTreeWidgetItem*> ls=ui->treeWidgetWaregroup->selectedItems();
    if(ls.count()<=0)
    {//nothing selected
        ui->pushButtonWgEdit->setEnabled(false);//disable buttons
        ui->pushButtonWgDelete->setEnabled(false);
    }
    else
    {
        ui->pushButtonWgEdit->setEnabled(true);//enable buttons
        ui->pushButtonWgDelete->setEnabled(true);
    }
    return true;
}

bool MainWindow::waregroup_update_count(QString sCondition)
{
    int i;
    CPointerMemory memory;
    memory.set_int(&i);
    memory.set_string(&sCondition);
    m_thread.set_work(WORK_WAREGROUP_GET_COUNT,&memory);
    if(i<0)//error
        return false;
    QString s=QString::fromUtf8("%1").arg(i);
    ui->labelCountWaregroup->setText(s);
    return true;
}

bool MainWindow::waregroup_treeitem_clicked(void)
{
    waregroup_check_user_input();
    return true;
}

bool MainWindow::waregroup_new()
{
    if(!m_db.is_db_connect())
        return false;
    CWaregroup wg;
    CLogbook lg;
    QString s;
    CInputDialogWaregroup dlg;
    //-
    if(m_db.is_db_connect()==true)
    {
        dlg.set_thread(&m_thread);
        dlg.update_waregroup_tree(-1);
        dlg.update_parent_label(-1);
        dlg.setWindowTitle("Neue Warengruppe");
        if(dlg.exec())//press ok?
        {
            dlg.get_data(wg);
            if(!m_db.waregroup_add(wg))//add ok?
            {}
            else
            {
                waregroup_update_tree(wg.get_id());//update treewidget

                //logbook
                if(m_db.logbook_create_new(wg,lg))
                    logbook_insert(lg);

                //set last change in db -> another clients update table
                s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_WAREGROUP,wg.get_id());// create string
                m_LastDbChange.write_last_change(s);//write in db
            }
        }
    }
    return true;
}

bool MainWindow::waregroup_edit(void)
{
    if(!m_db.is_db_connect())
        return false;
    CWaregroup wg;
    CLogbook lg;
    QString s;
    CInputDialogWaregroup dlg;
    bool b=true;
    int id=-1;
    QList <QTreeWidgetItem*>ls=ui->treeWidgetWaregroup->selectedItems();//get selected
    if(ls.count()>0)
        id=ls[0]->text(2).toInt(&b,10);//convert id to int
    if(b==true && m_db.is_db_connect()==true)//id are number and db connection?
    {
        if(m_db.waregroup_get(id,wg))//waregroup found?
        {
            //logbook - mark before
            m_db.logbook_create_edit(wg,lg,true);
            //-
            dlg.set_thread(&m_thread);
            dlg.set_data(wg);
            dlg.setWindowTitle("Warengruppe bearbeiten");
            if(dlg.exec())//press ok?
            {
                dlg.get_data(wg);
                if(!m_db.waregroup_update(wg))//update ok?
                {}
                else
                {
                    waregroup_update_tree(wg.get_id());//update treewidget
                    m_bUpdateTableInventory=true; //update inventory table
                    m_bUpdateTableOrderingWarelist=true;//update ordering articlelist
                    m_bUpdateTableTradeWarelist=true;//update trade articlelist
                    m_bUpdateTableArticle=true;//update article table

                    //logbook after
                    if(m_db.logbook_create_edit(wg,lg,false))
                        logbook_insert(lg);

                    //set last change in db -> another clients update table
                    s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG,-1);// create string
                    m_LastDbChange.write_last_change(s);//write in db
                }
            }
        }
    }
    return true;
}

bool MainWindow::waregroup_delete(void)
{
    if(!m_db.is_db_connect())
        return false;
    bool b=true;
    int count,iId=-1,iParentId=-1;
    QList <QTreeWidgetItem*>ls=ui->treeWidgetWaregroup->selectedItems();
    if(ls.count()<=0)
       return false;
    //-
    iId=ls[0]->text(2).toInt(&b,10);//get id
    if(!b)
        return false;
    //-
    CLogbook lg;
    CWaregroup wg;
    m_db.waregroup_get(iId,wg);
    m_db.logbook_create_remove(wg,lg);//create logbook
    //-
    QString s;
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");

    //-how many article with this waregroup??
    s=QString::fromUtf8("waregroup_id=%1").arg(iId);
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    if(!m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory))
        return false;
    //-
    if(count>0)//article with this waregroup found?
    {
        s=QString::fromUtf8("Es gibt %1 Artikel in der Warengruppe ").arg(count);
        s+=QString::fromUtf8("'%1' !!!").arg(ls[0]->text(0));
        s+=QString::fromUtf8("\nSoll die Warengruppe '%1' wirklich gelöscht werden?").arg(ls[0]->text(0));
        s+=QString::fromUtf8("\n(Artikel werden in die übergeordnete Gruppe verschoben)");
        msg.setText(s);
    }
    else
    {
        s=QString::fromUtf8("Soll die Warengruppe '%1' wirklich gelöscht werden?").arg(ls[0]->text(0));
        msg.setText(s);
    }
    //-
    msg.exec();
    if(msg.clickedButton()==yesButton)
    {
        //give it subwaregroups? -> change there to parent
        memory.clear();
        memory.set_int(&iId);
        memory.set_int2(&iParentId);
        if(!m_thread.set_work(WORK_WAREGROUP_ALL_CHANGE_PARENT,&memory))
            return false;
        //-remove
        if(!m_db.waregroup_remove(iId))//remove in db ok?
        {}
        else
        {
            //update treewidget
            iParentId=ls[0]->text(3).toInt(&b,10);
            if(b)
                waregroup_update_tree(iParentId);
            //update all article with this waregroup
            if(count>0)
            {
                memory.clear();
                memory.set_int(&iId);
                memory.set_int2(&iParentId);
                if(!m_thread.set_work(WORK_ARTICLE_ALL_CHANGE_WAREGROUP,&memory))
                {}
                else
                {
                    m_bUpdateTableInventory=true; //update inventory table
                    m_bUpdateTableOrderingWarelist=true;//update ordering articlelist
                    m_bUpdateTableTradeWarelist=true;//update trade articlelist
                    m_bUpdateTableArticle=true;//update article table
                }
            }
            //logbook
            logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG,-1);// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return b;
}


//---maker-----------------------------------------------------------------------------------------------

bool MainWindow::maker_update_info(void)
{
    if(!m_db.is_db_connect())
        return false;

    CPointerMemory memory;
    bool b;
    int iId=-1;
    QList <QTableWidgetItem*> ls=ui->tableWidgetMaker->selectedItems();
    if(ls.count()<=0)//nothing select, fill list empty
        m_widgets.maker_update_list(ui->listWidgetMakerData,-1);//empty list
    else if(ls.count()>0)//selected
    {
        iId=ls[2]->text().toInt(&b,10);
        if(b==true && iId>0)
        {
            memory.set_listwidget(ui->listWidgetMakerData);
            memory.set_int(&iId);
            if(!m_thread.set_work(WORK_MAKER_UPDATE_LISTWIDGET,&memory))
            {}
        }
    }
    ls.clear();
    //-
    return b;
}

bool MainWindow::maker_check_user_input(void)
{
    QList <QTableWidgetItem*> ls=ui->tableWidgetMaker->selectedItems();
    if(ls.count()==0)
    {//no selected
        ui->pushButtonMakerEdit->setEnabled(false);
        ui->pushButtonMakerDelete->setEnabled(false);
    }
    else
    {//selected
        ui->pushButtonMakerEdit->setEnabled(true);
        ui->pushButtonMakerDelete->setEnabled(true);
    }
    return true;
}

bool MainWindow::maker_update_count(void)
{
    if(!m_db.is_db_connect())
        return false;

    int row=ui->tableWidgetMaker->rowCount();
    int count;
    QString s=QString::fromUtf8("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_MAKER_GET_COUNT,&memory);
    if(count<0)//error
        return false;
    //-
    if(row!=count)
        s=QString::fromUtf8("%1/").arg(row);
    s+=QString::fromUtf8("%1").arg(count);
    ui->labelCountMaker->setText(s);
    //-
    return true;
}

bool MainWindow::maker_mask_edit(void)
{
    if(ui->checkBoxAutoSearchMaker->isChecked())
        maker_mask_set();
    return true;
}

bool MainWindow::maker_checkbox_auto_clicked()
{
    if(ui->checkBoxAutoSearchMaker->isChecked())
        ui->pushButtonMaskSetMaker->setEnabled(false);
    else
        ui->pushButtonMaskSetMaker->setEnabled(true);
    return true;
}

bool MainWindow::maker_widgetitem_clicked()
{
    maker_check_user_input();
    maker_update_info();
    return true;
}

bool MainWindow::maker_mask_set(void)
{
    if(!m_db.is_db_connect())
        return false;

    QString sCondition,s=ui->lineEditMakerMask->text();
    int iIndex=ui->comboBoxMaskMaker->currentIndex();
    QList<int> lsInt;
    bool b;

    CPointerMemory memory;
    memory.set_tablewidget(ui->tableWidgetMaker);
    memory.set_string(&sCondition);
    memory.set_int_list(&lsInt);
    //-
    if(s.length()<=0)//no mask
        sCondition=QString::fromUtf8("");
    else if(iIndex==0)//name
        sCondition=QString::fromUtf8("name like '%%1%'").arg(s);
    else if(iIndex==1)//adress
        sCondition=QString::fromUtf8("adress like '%%1%'").arg(s);
    else if(iIndex==2)//call number
        sCondition=QString::fromUtf8("call_numbers like '%%1%'").arg(s);
    else if(iIndex==3)//fax
        sCondition=QString::fromUtf8("fax_numbers like '%%1%'").arg(s);
    else if(iIndex==4)//email
        sCondition=QString::fromUtf8("email_adress like '%%1%'").arg(s);
    else if(iIndex==5)//homepage
        sCondition=QString::fromUtf8("homepage like '%%1%'").arg(s);
    else if(iIndex==6)//contakt pers.
        sCondition=QString::fromUtf8("cotakt_person like '%%1%'").arg(s);
    else if(iIndex==7)//comment
        sCondition=QString::fromUtf8("comment like '%%1%'").arg(s);
    //-
    b=m_thread.set_work(WORK_MAKER_GET_ALL,&memory);
    if(!b)
    {}
    else
    {
        if(!m_thread.set_work(WORK_MAKER_UPDATE_TABLEWIDGET,&memory))
        {}
        else
            maker_update_count();
    }
    //clear maker info
    m_widgets.maker_update_list(ui->listWidgetMakerData,-1);//empty list
    //-
    maker_check_user_input();
    return b;
}

bool MainWindow::maker_new(void)
{
    if(!m_db.is_db_connect())
        return false;
    bool b=true;
    QString s;
    CMaker mk;
    CLogbook lg;
    CInputDialogMaker dlg;
    dlg.set_thread(&m_thread);
    dlg.setWindowTitle("neuer Hersteller");
    if(dlg.exec())
    {//pressed ok
        dlg.get_data(mk);// get input data
        if(!m_db.maker_add(mk))
        {}
        else
        {//ok?
            m_widgets.maker_insert_row(ui->tableWidgetMaker,mk,ITEM_POSITION_BOTTOM,true);
            maker_update_info();
            maker_update_count();

            //logbook
            if(m_db.logbook_create_new(mk,lg))
                logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_MAKER,mk.get_id());// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return b;
}

bool MainWindow::maker_edit(void)
{
    if(!m_db.is_db_connect())
        return false;
    CMaker mk;
    CLogbook lg;
    CInputDialogMaker dlg;
    bool b=true;
    int iId;
    QString s;

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetMaker,iId))//select?
    {
        if(!m_db.maker_get(iId,mk))//get maker data
            b=false;
        else
        {
            //logbook - mark before
            m_db.logbook_create_edit(mk,lg,true);
            //-
            dlg.set_data(mk);
            dlg.set_thread(&m_thread);
            dlg.setWindowTitle("Hersteller bearbeiten");
            if(dlg.exec())
            {
                dlg.get_data(mk);
                if(!m_db.maker_update(mk))
                    b=false;
                else
                {
                    b=m_widgets.maker_update_row(ui->tableWidgetMaker,mk,true);//update table item
                    maker_update_info();

                    m_bUpdateTableInventory=true; //update inventory table
                    m_bUpdateTableOrderingWarelist=true;//update ordering articlelist
                    m_bUpdateTableTradeWarelist=true;//update trade articlelist
                    m_bUpdateTableArticle=true;//update article table

                    //logbook after
                    if(m_db.logbook_create_edit(mk,lg,false))
                        logbook_insert(lg);

                    //set last change in db -> another clients update table
                    s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG,-1);// create string
                    m_LastDbChange.write_last_change(s);//write in db
                }
            }
        }
    }

    //check delete by another client & user will not created new
    if(mk.get_id()>0)
    {
        s=QString::fromUtf8("id = %1").arg(mk.get_id());
        if(m_db.maker_get_count(s)<=0)
            maker_mask_set();//update table
    }
    //-
    return b;
}

bool MainWindow::maker_delete(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=false;
    int i=-1,iId;
    CLogbook lg;
    CMaker mk;
    QString s;
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    QList <int> lsArIds;
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int_list(&lsArIds);
    //-
    m_widgets.get_selected_table_item_value(ui->tableWidgetMaker,iId);//get id
    if(m_db.maker_get(iId,mk))//get maker
    {
        m_db.logbook_create_remove(mk,lg);//create logbook
        s=QString::fromUtf8("maker_id=%1").arg(iId);

        b=m_thread.set_work(WORK_ARTICLE_GET_ALL,&memory);
        if(b)
        {
            if(lsArIds.count()>0)//article with this maker found?
            {
                s=QString::fromUtf8("Es gibt %1 Artikel von dem Hersteller ").arg(lsArIds.count());
                s+=QString::fromUtf8("'%1' !!!").arg(mk.get_name());
                s+=QString::fromUtf8("\nSoll der Hersteller wirklich gelöscht werden?");
                msg.setText(s);
            }
            else
            {
                s=QString::fromUtf8("Soll der Hersteller '%1' wirklich gelöscht werden?").arg(mk.get_name());
                msg.setText(s);
            }
            //-
            msg.exec();
            if(msg.clickedButton()==yesButton)
            {
                if(m_db.maker_remove(iId))//remove in db ok?
                {
                    //update all article with this maker
                    if(lsArIds.count()>0)
                    {
                        memory.clear();
                        memory.set_int(&iId);
                        memory.set_int2(&i);
                        b=m_thread.set_work(WORK_ARTICLE_ALL_CHANGE_MAKER,&memory);
                        if(b)
                        {
                            m_bUpdateTableInventory=true; //update inventory table
                            m_bUpdateTableOrderingWarelist=true;//update ordering articlelist
                            m_bUpdateTableTradeWarelist=true;//update trade articlelist
                            m_bUpdateTableArticle=true;//update article table
                        }
                    }
                    //-
                    b=m_widgets.remove_row(ui->tableWidgetMaker,iId);
                    maker_update_info();
                    maker_update_count();
                    maker_check_user_input();

                    //logbook insert
                    logbook_insert(lg);

                    //set last change in db -> another clients update table
                    s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG,-1);// create string
                    m_LastDbChange.write_last_change(s);//write in db
                }
            }
        }
    }
    lsArIds.clear();
    return b;
}


//-dealer---------------------------------------------------------------------------------

bool MainWindow::dealer_update_info(void)
{
    if(!m_db.is_db_connect())
        return false;

    CPointerMemory memory;
    bool b;
    int iId=-1;
    QList <QTableWidgetItem*> ls=ui->tableWidgetDealer->selectedItems();
    if(ls.count()<=0)//nothing select, fill list empty
        m_widgets.dealer_update_list(ui->listWidgetDealerData,-1);//empty list
    else if(ls.count()>0)//selected
    {
        iId=ls[2]->text().toInt(&b,10);
        if(b==true && iId>0)
        {
            memory.set_listwidget(ui->listWidgetDealerData);
            memory.set_int(&iId);
            if(!m_thread.set_work(WORK_DEALER_UPDATE_LISTWIDGET,&memory))
            {}
        }
    }
    ls.clear();
    //-
    return b;
}

bool MainWindow::dealer_check_user_input(void)
{
    QList <QTableWidgetItem*> ls=ui->tableWidgetDealer->selectedItems();
    if(ls.count()==0)
    {//no selected
        ui->pushButtonDealerEdit->setEnabled(false);
        ui->pushButtonDealerDelete->setEnabled(false);
    }
    else
    {//selected
        ui->pushButtonDealerEdit->setEnabled(true);
        ui->pushButtonDealerDelete->setEnabled(true);
    }
    return true;
}

bool MainWindow::dealer_update_count(void)
{
    if(!m_db.is_db_connect())
        return false;

    int row=ui->tableWidgetDealer->rowCount();
    int count;
    QString s=QString::fromUtf8("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_DEALER_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString::fromUtf8("%1/").arg(row);
    s+=QString::fromUtf8("%1").arg(count);
    ui->labelCountDealer->setText(s);
    //-
    return true;
}

bool MainWindow::dealer_mask_edit(void)
{
    if(ui->checkBoxAutoSearchDealer->isChecked())
        dealer_mask_set();
    return true;
}

bool MainWindow::dealer_checkbox_auto_clicked()
{
    if(ui->checkBoxAutoSearchDealer->isChecked())
        ui->pushButtonMaskSetDealer->setEnabled(false);
    else
        ui->pushButtonMaskSetDealer->setEnabled(true);
    return true;
}

bool MainWindow::dealer_widgetitem_clicked()
{
    dealer_check_user_input();
    dealer_update_info();
    return true;
}

bool MainWindow::dealer_mask_set(void)
{
    if(!m_db.is_db_connect())
        return false;

    QString sCondition,s=ui->lineEditDealerMask->text();
    int iIndex=ui->comboBoxMaskDealer->currentIndex();
    QList<int> lsInt;
    bool b;
    CPointerMemory memory;
    memory.set_tablewidget(ui->tableWidgetDealer);
    memory.set_string(&sCondition);
    memory.set_int_list(&lsInt);
    //-
    if(s.length()<=0)//no mask
        sCondition=QString::fromUtf8("");
    else if(iIndex==0)//name
        sCondition=QString::fromUtf8("name like '%%1%'").arg(s);
    else if(iIndex==1)//customer_number
        sCondition=QString::fromUtf8("customer_number like '%%1%'").arg(s);
    else if(iIndex==2)//adress
        sCondition=QString::fromUtf8("adress like '%%1%'").arg(s);
    else if(iIndex==3)//call number
        sCondition=QString::fromUtf8("call_numbers like '%%1%'").arg(s);
    else if(iIndex==4)//fax
        sCondition=QString::fromUtf8("fax_numbers like '%%1%'").arg(s);
    else if(iIndex==5)//email
        sCondition=QString::fromUtf8("email_adress like '%%1%'").arg(s);
    else if(iIndex==6)//homepage
        sCondition=QString::fromUtf8("homepage like '%%1%'").arg(s);
    else if(iIndex==7)//contakt pers.
        sCondition=QString::fromUtf8("cotakt_person like '%%1%'").arg(s);
    else if(iIndex==8)//comment
        sCondition=QString::fromUtf8("comment like '%%1%'").arg(s);
    //-
    b=m_thread.set_work(WORK_DEALER_GET_ALL,&memory);
    if(!b)
    {}
    else
    {
        if(!m_thread.set_work(WORK_DEALER_UPDATE_TABLEWIDGET,&memory))
        {}
        else
            dealer_update_count();
    }
    //clear
    m_widgets.dealer_update_list(ui->listWidgetDealerData,-1);//empty list
    //-
    dealer_check_user_input();
    return b;
}

bool MainWindow::dealer_new(void)
{
    if(!m_db.is_db_connect())
        return false;
    bool b=true;
    CDealer de;
    QString s;
    CLogbook lg;
    CInputDialogDealer dlg;
    dlg.set_thread(&m_thread);
    dlg.setWindowTitle(QString::fromUtf8("neuer Händler"));
    if(dlg.exec())
    {//pressed ok
        dlg.get_data(de);// get input data
        if(!m_db.dealer_add(de))
            b=false;
        else
        {//ok?
            m_widgets.dealer_insert_row(ui->tableWidgetDealer,de,ITEM_POSITION_BOTTOM,true);
            dealer_update_info();
            dealer_update_count();

            //logbook
            if(m_db.logbook_create_new(de,lg))
                logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_DEALER,de.get_id());// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return b;
}

bool MainWindow::dealer_edit(void)
{
    if(!m_db.is_db_connect())
        return false;
    CDealer de;
    QString s;
    CLogbook lg;
    CInputDialogDealer dlg;
    bool b=true;
    int iId=-1;

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetDealer,iId))//select?
    {
        if(!m_db.dealer_get(iId,de))//get dealer data
            b=false;
        else
        {
            //logbook - mark before
            m_db.logbook_create_edit(de,lg,true);
            //-
            dlg.set_data(de);
            dlg.set_thread(&m_thread);
            dlg.setWindowTitle(QString::fromUtf8("Händler bearbeiten"));
            if(dlg.exec())
            {
                dlg.get_data(de);
                if(!m_db.dealer_update(de))
                    b=false;
                else
                {
                    b=m_widgets.dealer_update_row(ui->tableWidgetDealer,de,true);//update table item
                    dealer_update_info();
                    m_bUpdateTableOrdering=true;//update ordering table

                    //logbook after
                    if(m_db.logbook_create_edit(de,lg,false))
                        logbook_insert(lg);

                    //set last change in db -> another clients update table
                    s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_ORDERING_DEALER,-1);// create string
                    m_LastDbChange.write_last_change(s);//write in db
                }
            }
        }
    }

    //check delete by another client & user will not created new
    if(de.get_id()>0)
    {
        s=QString::fromUtf8("id = %1").arg(de.get_id());
        if(m_db.dealer_get_count(s)<=0)
            dealer_mask_set();//update table
    }
    //-
    return b;
}

bool MainWindow::dealer_delete(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=false;
    int i=-1,iId;
    QString s;
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    QList <int> lsOrdIds;
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int_list(&lsOrdIds);
    CLogbook lg;
    CDealer de;
    //-
    m_widgets.get_selected_table_item_value(ui->tableWidgetDealer,iId);//get id
    if(m_db.dealer_get(iId,de))//get dealer
    {
        s=QString::fromUtf8("dealer_id=%1").arg(iId);
        m_db.logbook_create_remove(de,lg);//create logbook
        b=m_thread.set_work(WORK_ORDERING_GET_ALL,&memory);
        if(b)
        {
            if(lsOrdIds.count()>0)//ordering with dealer found?
            {
                s=QString::fromUtf8("Es gibt %1 Bestellungen beim Händler").arg(lsOrdIds.count());
                s+=QString::fromUtf8(" '%1'").arg(de.get_name());
                s+=QString::fromUtf8("\nSoll der Händler wirklich gelöscht werden?");
                msg.setText(s);
            }
            else
            {
                s=QString::fromUtf8("Soll der Händler '%1' wirklich gelöscht werden?").arg(de.get_name());
                msg.setText(s);
            }
            //-
            msg.exec();
            if(msg.clickedButton()==yesButton)
            {
                if(m_db.dealer_remove(iId))//remove in db ok?
                {
                    //update all orderings with this dealer
                    if(lsOrdIds.count()>0)
                    {
                        memory.clear();
                        memory.set_int(&iId);
                        memory.set_int2(&i);
                        b=m_thread.set_work(WORK_ORDERING_ALL_CHANGE_DEALER, &memory);
                        if(b)
                            m_bUpdateTableOrdering=true; //update ordering table
                    }
                    //-
                    b=m_widgets.remove_row(ui->tableWidgetDealer,iId);
                    dealer_update_info();
                    dealer_update_count();
                    dealer_check_user_input();

                    //logbook
                    logbook_insert(lg);

                    //set last change in db -> another clients update table
                    s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_ORDERING_DEALER,-1);// create string
                    m_LastDbChange.write_last_change(s);//write in db
                }
            }
        }
    }
    lsOrdIds.clear();
    return b;
}


//customer--------------------------------------------------------------------------------

bool MainWindow::customer_update_info(void)
{
    if(!m_db.is_db_connect())
        return false;

    CPointerMemory memory;
    bool b;
    int iId=-1;
    QList <QTableWidgetItem*> ls=ui->tableWidgetCustomer->selectedItems();
    if(ls.count()<=0)//nothing select, fill list empty
        m_widgets.customer_update_list(ui->listWidgetCustomerData,-1);//empty list
    else if(ls.count()>0)//selected
    {
        iId=ls[4]->text().toInt(&b,10);
        if(b==true && iId>0)
        {
            memory.set_listwidget(ui->listWidgetCustomerData);
            memory.set_int(&iId);
            if(!m_thread.set_work(WORK_CUSTOMER_UPDATE_LISTWIDGET,&memory))
            {}
        }
    }
    ls.clear();
    //-
    return b;
}

bool MainWindow::customer_check_user_input(void)
{
    QList <QTableWidgetItem*> ls=ui->tableWidgetCustomer->selectedItems();
    if(ls.count()<=0)
    {//no selected
        ui->pushButtonCustomerEdit->setEnabled(false);
        ui->pushButtonCustomerDelete->setEnabled(false);
    }
    else
    {//selected
        ui->pushButtonCustomerEdit->setEnabled(true);
        ui->pushButtonCustomerDelete->setEnabled(true);
    }
    return true;
}

bool MainWindow::customer_update_count(void)
{
    int row=ui->tableWidgetCustomer->rowCount();
    int count;
    QString s=QString::fromUtf8("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_CUSTOMER_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString::fromUtf8("%1/").arg(row);
    s+=QString::fromUtf8("%1").arg(count);
    ui->labelCountCustomer->setText(s);
    //-
    return true;
}

bool MainWindow::customer_mask_edit(void)
{
    if(ui->checkBoxAutoSearchCustomer->isChecked())
        customer_mask_set();
    return true;
}

bool MainWindow::customer_checkbox_auto_clicked()
{
    if(ui->checkBoxAutoSearchCustomer->isChecked())
        ui->pushButtonMaskSetCustomer->setEnabled(false);
    else
        ui->pushButtonMaskSetCustomer->setEnabled(true);
    return true;
}

bool MainWindow::customer_widgetitem_clicked()
{
    customer_check_user_input();
    customer_update_info();
    return true;
}

bool MainWindow::customer_update_customernumber_trade(QString sOld, QString sNew)
{
    int number=1,type=TYPE_CUSTOMER_OUTGOING;
    CPointerMemory memory;
    memory.set_string(&sOld);
    memory.set_string2(&sNew);
    memory.set_int(&number);
    memory.set_int2(&type);
    //-
    bool b=m_thread.set_work(WORK_TRADE_UPDATE_ALL_INFO,&memory);
    return b;
}

bool MainWindow::customer_mask_set(void)
{
    QList<int> lsIds;
    QString sCondition,s=ui->lineEditCustomerMask->text();
    CPointerMemory memory;
    memory.set_int_list(&lsIds);
    memory.set_string(&s);
    int index=ui->comboBoxMaskCustomer->currentIndex();
    bool b;

    //search by ???
    if(s.length()<=0)//no mask
        b=m_thread.set_work(WORK_CUSTOMER_GET_ALL,&memory);
    else
    {
        if(index==0)//customer number
            sCondition=QString::fromUtf8("customer_number like '%%1%'").arg(s);
        else if(index==1)//name
            sCondition=QString::fromUtf8("name like '%%1%'").arg(s);
        else if(index==2)//first name
            sCondition=QString::fromUtf8("first_name like '%%1%'").arg(s);
        else if(index==3)//street
            sCondition=QString::fromUtf8("street like '%%1%'").arg(s);
        else if(index==4)//postcode
            sCondition=QString::fromUtf8("postcode like '%%1%'").arg(s);
        else if(index==5)//city
            sCondition=QString::fromUtf8("city like '%%1%'").arg(s);
        else if(index==6)//comment
            sCondition=QString::fromUtf8("comment like '%%1%'").arg(s);
        else if(index==7)//comment
            sCondition=QString::fromUtf8("call_numbers like '%%1%'").arg(s);
        else if(index==8)//comment
            sCondition=QString::fromUtf8("fax_numbers like '%%1%'").arg(s);
        else if(index==9)//comment
            sCondition=QString::fromUtf8("email_adress like '%%1%'").arg(s);
        //-
        memory.set_string(&sCondition);
        b=m_thread.set_work(WORK_CUSTOMER_GET_ALL,&memory);
    }
    //-
    if(!b)
    {}
    else
    {
        memory.set_tablewidget(ui->tableWidgetCustomer);
        b=m_thread.set_work(WORK_CUSTOMER_UPDATE_TABLEWIDGET,&memory);
        if(!b)
        {}
        else
            customer_update_count();
    }
    //clear
    m_widgets.customer_update_list(ui->listWidgetCustomerData,-1);//empty list
    //-
    customer_check_user_input();
    return b;
}

bool MainWindow::customer_new(void)
{
    if(!m_db.is_db_connect())
        return false;
    bool b=true;
    CLogbook lg;
    QString s;
    CCustomer cu;
    CInputDialogCustomer dlg;
    dlg.set_db(&m_thread);
    dlg.setWindowTitle("neuer Kunde");
    dlg.customer_number_nomination();
    if(dlg.exec())
    {//pressed ok
        dlg.get_data(cu);// get input data
        if(!m_db.customer_add(cu))
            b=false;
        else
        {//ok?
            m_widgets.customer_insert_row(ui->tableWidgetCustomer,cu,ITEM_POSITION_BOTTOM,true);
            customer_update_info();
            customer_update_count();

            //logbook
            if(m_db.logbook_create_new(cu,lg))
                logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_CUSTOMER,cu.get_id());// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return b;
}

bool MainWindow::customer_edit(void)
{
    if(!m_db.is_db_connect())
        return false;
    CCustomer cu;
    CLogbook lg;
    CInputDialogCustomer dlg;
    bool b=true;
    int iId=-1;
    QString s,sMark;

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetCustomer,iId))//select?
    {
        if(!m_db.customer_get(iId,cu))//get customer data
            b=false;
        else
        {
            //mark customernumber
            sMark=cu.get_customernumber();

            //logbook - mark before
            m_db.logbook_create_edit(cu,lg,true);
            //-
            dlg.set_data(cu);
            dlg.set_db(&m_thread);
            dlg.setWindowTitle("Kunde bearbeiten");
            if(dlg.exec())
            {
                dlg.get_data(cu);
                if(!m_db.customer_update(cu))
                    b=false;
                else
                {
                    //customernumber update?
                    if(sMark!=cu.get_customernumber())
                    {//update all trade info by customer
                        customer_update_customernumber_trade(sMark,cu.get_customernumber());
                        trade_widgetitem_clicked();//update trade info
                    }

                    //-
                    b=m_widgets.customer_update_row(ui->tableWidgetCustomer,cu,true);//update table item
                    customer_update_info();
                }

                //logbook after
                if(m_db.logbook_create_edit(cu,lg,false))
                    logbook_insert(lg);

                //set last change in db -> another clients update table
                s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_CUSTOMER,cu.get_id());// create string
                m_LastDbChange.write_last_change(s);//write in db
            }
        }
    }

    //check delete by another client & user will not created new
    if(cu.get_id()>0)
    {
        s=QString::fromUtf8("id = %1").arg(cu.get_id());
        if(m_db.customer_get_count(s)<=0)
            customer_mask_set();//update table
    }
    //-
    return b;
}

bool MainWindow::customer_delete(void)
{
    if(!m_db.is_db_connect())
        return false;

    int iId;
    bool b=false;
    CLogbook lg;
    CCustomer cu;
    QString sMark,s;
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    //-
    m_widgets.get_selected_table_item_value(ui->tableWidgetCustomer,iId);//get id
    if(m_db.customer_get(iId,cu))//get customer
    {
        sMark=cu.get_customernumber();//mark number
        m_db.logbook_create_remove(cu,lg);//create logbook

        s=QString::fromUtf8("Soll der Kunde '%1,%2' wirklich gelöscht werden?").arg(cu.get_name(),cu.get_first_name());
        msg.setText(s);
        //-
        msg.exec();
        if(msg.clickedButton()==yesButton)//remove?
        {
            if(!m_db.customer_remove(iId))//remove in db ok?
                b=false;
            else
            {
                b=m_widgets.remove_row(ui->tableWidgetCustomer,iId);
                customer_update_count();
                customer_update_info();
                customer_check_user_input();

                //logbook after
                logbook_insert(lg);

                //update trade info by customer
                customer_update_customernumber_trade(sMark,QString::fromUtf8("Kunde gelöscht"));
                trade_widgetitem_clicked();//update trade info

                //set last change in db -> another clients update table
                s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_CUSTOMER,cu.get_id());// create string
                m_LastDbChange.write_last_change(s);//write in db
            }
        }
    }
    return b;
}


//-article---------------------------------------------------------------------------------

bool MainWindow::article_check_user_input(void)
{
    bool b=true;
    QList <QTableWidgetItem*> ls=ui->tableWidgetArticle->selectedItems();
    if(ls.count()==0)
        b=false;//no selected -> buttons false
    ui->pushButtonArticleEdit->setEnabled(b);
    ui->pushButtonArticleCopy->setEnabled(b);
    ui->pushButtonArticleDelete->setEnabled(b);
    ls.clear();
    return true;
}

bool MainWindow::article_update_count(void)
{
    int row=ui->tableWidgetArticle->rowCount();
    int count;
    QString s=QString::fromUtf8("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString::fromUtf8("%1/").arg(row);
    s+=QString::fromUtf8("%1").arg(count);
    ui->labelCountArticle->setText(s);
    //-
    return true;
}

bool MainWindow::article_mask_edit(void)
{
    if(ui->checkBoxAutoSearchArticle->isChecked())
        article_mask_set();
    return true;
}

bool MainWindow::article_checkbox_auto_clicked()
{
    if(ui->checkBoxAutoSearchArticle->isChecked())
        ui->pushButtonMaskSetArticle->setEnabled(false);
    else
        ui->pushButtonMaskSetArticle->setEnabled(true);
    return true;
}

bool MainWindow::article_widgetitem_clicked()
{
    article_check_user_input();

    int iId=-1;
    //select same article in inventory table
    if(m_widgets.get_selected_table_item_value(ui->tableWidgetArticle,iId))
         m_widgets.find_and_select_row(ui->tableWidgetInventory,iId);
    return true;
}

bool MainWindow::article_mask_set(void)
{
    int iFormatType=FORMAT_ONE;
    QList<int> lsIds;
    QString sCondition,s=ui->lineEditArticleMask->text();
    CPointerMemory memory;
    memory.set_int(&iFormatType);
    memory.set_int_list(&lsIds);
    memory.set_string(&s);
    int index=ui->comboBoxMaskArticle->currentIndex();
    float fInput;
    bool b=true;

    //search by ???

    if(s.length()<=0)//no mask
        b=m_thread.set_work(WORK_ARTICLE_GET_ALL,&memory);
    else if(index==3||index==4)
    {
        if(index==3)//by maker
            b=m_thread.set_work(WORK_ARTICLE_GET_ALL_BY_MAKER,&memory);
        else if(index==4)//by waregroup
            b=m_thread.set_work(WORK_ARTICLE_GET_ALL_BY_WAREGROUP,&memory);
    }
    else
    {
        if(index==0)//name
            sCondition=QString::fromUtf8("name like '%%1%'").arg(s);
        else if(index==1)//articlenumber
            sCondition=QString::fromUtf8("articlenumber like '%%1%'").arg(s);
        else if(index==2)//2.articlenumber
            sCondition=QString::fromUtf8("articlenumber2 like '%%1%'").arg(s);
        else if(index==9)//location
            sCondition=QString::fromUtf8("location like '%%1%'").arg(s);
        else if(index==10)//unit
            sCondition=QString::fromUtf8("unit like '%%1%'").arg(s);
        else if(index==11)//valuta
            sCondition=QString::fromUtf8("valuta like '%%1%'").arg(s);
        else if(index==12)//comment
            sCondition=QString::fromUtf8("comment like '%%1%'").arg(s);
        else if(index>4 && index<9) //basprice /salesprice
        {
            fInput=s.toFloat(&b);
            if(b)
            {
                if(index==5)//basep. <
                    sCondition=QString::fromUtf8("baseprice < %1").arg(fInput);
                else if(index==6)//basep. >
                    sCondition=QString::fromUtf8("baseprice > %1").arg(fInput);
                else if(index==7)//salesp.inv. <
                    sCondition=QString::fromUtf8("salesprice < %1").arg(fInput);
                else if(index==8)//salesp..inv. >
                    sCondition=QString::fromUtf8("salesprice > %1").arg(fInput);
            }
            else{}
        }
        //-
        memory.set_string(&sCondition);
        b=m_thread.set_work(WORK_ARTICLE_GET_ALL,&memory);
    }
    //-
    if(!b)
    {}
    else
    {
        memory.set_tablewidget(ui->tableWidgetArticle);
        b=m_thread.set_work(WORK_ARTICLE_UPDATE_TABLEWIDGET,&memory);
        if(!b)
        {}
        else
            article_update_count();
    }
    //-
    article_check_user_input();
    //-
    lsIds.clear();
    return b;
}

bool MainWindow::article_new(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    int iType=LC_ACTION_UPDATE_ARTICLE_INV;
    CArticle ar;
    CLogbook lg;
    CMaker mk;
    QString s;
    CInputDialogArticle dlg;
    dlg.set_thread(&m_thread);//set db object
    dlg.init();
    dlg.setWindowTitle("neuer Artikel");
    if(dlg.exec())
    {//pressed ok
        dlg.get_data(ar);
        b=m_db.article_add(ar);// add in db
        if(!b)
        {}
        else
        {
            if(dlg.m_iNewMakerId>0)//new maker add?
            {
                if(m_db.maker_get(dlg.m_iNewMakerId,mk))//get new maker
                {
                    m_widgets.maker_insert_row(ui->tableWidgetMaker,mk,ITEM_POSITION_BOTTOM,true);
                    maker_update_count();
                    maker_update_info();
                    maker_check_user_input();
                }
                iType=LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG;
            }
            if(dlg.m_iNewWaregroupId>0)//new waregroup add?
            {
                waregroup_update_tree(dlg.m_iNewWaregroupId);
                waregroup_update_count();
                waregroup_check_user_input();
                iType=LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG;
            }
            m_widgets.article_insert_row(ui->tableWidgetArticle,ar,FORMAT_ONE,ITEM_POSITION_BOTTOM,true);
            m_widgets.inventory_insert_row(ui->tableWidgetInventory,ar,ITEM_POSITION_BOTTOM,true);
            article_update_count();
            inventory_update_count();
            inventory_check_article_under_warnlimit();//set icon to tab, if articles under limit

            //logbook
            if(m_db.logbook_create_new(ar,lg))
                logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(iType,-1);// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return b;
}

bool MainWindow::article_edit(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    QString s;
    CArticle ar;
    CMaker mk;
    CLogbook lg;
    CInputDialogArticle dlg;
    int iId=-1;

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetArticle,iId))//select?
    {
        if(!m_db.article_get(iId,ar))//get article data
            b=false;
        else
        {
            //logbook - mark before
            m_db.logbook_create_edit(ar,lg,true);
            //-
            dlg.set_thread(&m_thread);//set thread
            dlg.init(ar.get_waregroup_id());
            dlg.set_data(ar);
            dlg.setWindowTitle("Artikel bearbeiten");
            if(dlg.exec())
            {
                dlg.get_data(ar);
                b=m_db.article_update(ar);
                if(b)
                {
                    if(dlg.m_iNewMakerId>0)//new maker add?
                    {
                        if(m_db.maker_get(dlg.m_iNewMakerId,mk))//get new maker
                        {
                            m_widgets.maker_insert_row(ui->tableWidgetMaker,mk,ITEM_POSITION_BOTTOM,true);
                            maker_update_count();
                            maker_update_info();
                            maker_check_user_input();
                        }
                    }
                    if(dlg.m_iNewWaregroupId>0)//new waregroup add?
                    {
                        waregroup_update_tree(dlg.m_iNewWaregroupId);
                        waregroup_update_count();
                        waregroup_check_user_input();
                    }
                    m_widgets.article_update_row(ui->tableWidgetArticle,ar,FORMAT_ONE,true);
                    m_widgets.inventory_update_row(ui->tableWidgetInventory,ar,true);
                    //-
                    m_bUpdateTableOrdering=true;//update ordering table
                    m_bUpdateTableOrderingWarelist=true;//update ordering articlelist
                    m_bUpdateTableTrade=true;//update trade table
                    m_bUpdateTableTradeWarelist=true;//update trade articlelist

                    inventory_check_article_under_warnlimit();//set icon to tab, if articles under limit

                    //logbook after
                    if(m_db.logbook_create_edit(ar,lg,false))
                        logbook_insert(lg);

                    //set last change in db -> another clients update table
                    s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG,-1);// create string
                    m_LastDbChange.write_last_change(s);//write in db
                }
            }
        }
    }

    //check delete by another client & user will not created new
    if(ar.get_id()>0)
    {
        s=QString::fromUtf8("id = %1").arg(ar.get_id());
        if(m_db.article_get_count(s)<=0)
            article_mask_set();//update table
    }
    return b;
}

bool MainWindow::article_copy(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    CLogbook lg;
    int iReturn,count=0;
    QString s,sCondition;
    CPointerMemory memory;
    memory.set_string(&sCondition);
    memory.set_int(&count);
    QMessageBox msg(this);
    msg.setWindowTitle(QString::fromUtf8("Fehler"));
    msg.setText(QString::fromUtf8("Der Artikel der als Quelle dienen sollte wurde zwischenzeitlich von einem anderen client gelöscht."));
    QInputDialog dlg(this);
    dlg.setWindowTitle("Artikel kopieren");
    dlg.setOkButtonText(QString::fromUtf8("Kopie erstellen"));
    CArticle ar;
    int iId=-1;

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetArticle,iId))//select?
    {
        if(!m_db.article_get(iId,ar))//get article data
            b=false;
        else
        {
            s=QString::fromUtf8("Artikelbezeichnung der Kopie von \"%1\":").arg(ar.get_name());
            dlg.setLabelText(s);
            s=ar.get_name()+QString::fromUtf8(" Kopie");
            dlg.setTextValue(s);
            //-
            do
            {
                iReturn=dlg.exec();
                sCondition=QString::fromUtf8("name='%1'").arg(dlg.textValue());
                m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory);
                if(count>0 && iReturn!=QDialog::Rejected)
                    QMessageBox::warning(this,"Fehler!","Ein Artikel mit dieser Beschreibung existiert bereits!");
            }while(iReturn!=QDialog::Rejected && count>0);
            //-
            if(iReturn==QDialog::Accepted)
            {//ok? -> copy

                //check delete by another client (	interim )
                s=QString::fromUtf8("id = %1").arg(ar.get_id());
                if(m_db.article_get_count(s)<=0)
                {//error , is delete
                    msg.exec();
                    article_mask_set();//update table
                }
                else
                {
                    m_db.logbook_create_copy(ar.get_name(),dlg.textValue(),lg);//create logbook
                    ar.set_name(dlg.textValue());//set copy name
                    if(m_db.article_add(ar))//add in db
                    {
                        //update displays
                        m_widgets.article_insert_row(ui->tableWidgetArticle,ar,FORMAT_ONE,ITEM_POSITION_BOTTOM,true);
                        m_widgets.inventory_insert_row(ui->tableWidgetInventory,ar,ITEM_POSITION_BOTTOM,true);
                        article_update_count();
                        inventory_update_count();

                        //logbook
                        logbook_insert(lg);

                        //set last change in db -> another clients update table
                        s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_ARTICLE_INV,-1);// create string
                        m_LastDbChange.write_last_change(s);//write in db
                    }
                }
            }
        }
    }
    memory.clear();
    return b;
}

bool MainWindow::article_delete(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true,bGiveItTrades=false;
    CLogbook lg;
    CArticle ar;
    //-
    int i,iId=-1;
    QString s;
    QMessageBox msg;
    CPointerMemory memory;
    memory.set_int(&iId);
    //-
    if(m_widgets.get_selected_table_item_value(ui->tableWidgetArticle,iId))//select?
    {
        m_db.article_get(iId,ar);
        m_db.logbook_create_remove(ar,lg);//create logbook

        //check , give it trades with this article?
        bGiveItTrades=m_thread.set_work(WORK_TRADE_GIVE_IT_WITH_ARTICLE,&memory);

        //find count of ordering with this article
        s=QString::fromUtf8("wares like '%x%1|%'").arg(iId);
        memory.clear();
        memory.set_string(&s);
        memory.set_int(&i);
        m_thread.set_work(WORK_ORDERING_GET_COUNT,&memory);
        if(i>0)//orderings with this article?
        {
            msg.setWindowTitle("!");
            s=QString::fromUtf8("Es gibt %1 Bestellung(en) mit diesem Artikel, ändern Sie die Bestellung(en) bevor dieser Artikel gelöscht werden kann!").arg(i);
            msg.setText(s);
            msg.exec();
            b=false;
        }
        else
        {
            QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
            msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
            msg.setWindowTitle("?");
            s=QString::fromUtf8("Soll der Artikel '%1' wirklich gelöscht werden?").arg(ar.get_name());
            if(bGiveItTrades)
                s+=QString::fromUtf8("\n(Es existieren Buchungen mit diesen Artikel!)");
            msg.setText(s);
            //-
            msg.exec();
            if(msg.clickedButton()==yesButton)
            {
                if(!bGiveItTrades)
                    b=m_db.article_remove(iId);//no trades with this article->remove really
                else
                    b=m_db.article_set_delete(iId);//trades with this article->only set delete

                if(b)
                {
                    m_widgets.remove_row(ui->tableWidgetArticle,iId);
                    m_widgets.remove_row(ui->tableWidgetInventory,iId);

                    article_check_user_input();
                    article_update_count();
                    inventory_update_count();

                    //logbook after
                    logbook_insert(lg);

                    //set last change in db -> another clients update table
                    s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG,-1);// create string
                    m_LastDbChange.write_last_change(s);//write in db
                }
            }
        }
    }
    return b;
}


//ordering---------------------------------------------------------------------------------------------------------------

bool MainWindow::ordering_check_user_input(void)
{
    bool b;
    QList<QTableWidgetItem*> ls=ui->tableWidgetOrdering->selectedItems();
    if(ls.count()>0)//seleted
        b=true;
    else
        b=false;
    ui->pushButtonOrderingEdit->setEnabled(b);
    ui->pushButtonOrderingDelete->setEnabled(b);
    ls.clear();
    return true;
}

bool MainWindow::ordering_update_count(void)
{
    int row=ui->tableWidgetOrdering->rowCount();
    int count;
    QString s=QString::fromUtf8("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_ORDERING_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString::fromUtf8("%1/").arg(row);
    s+=QString::fromUtf8("%1").arg(count);
    ui->labelCountOrdering->setText(s);
    //-
    return true;
}

bool MainWindow::ordering_update_wares_list(void)
{
    CPointerMemory memory;
    bool b=true;
    int iId=-1;
    int iFormatType=FORMAT_ONE;
    if(m_widgets.get_selected_table_item_value(ui->tableWidgetOrdering,iId))//select?
    {
        memory.set_tablewidget(ui->tableWidgetOrderingWareslist);
        memory.set_int(&iId);
        memory.set_int2(&iFormatType);
        b=m_thread.set_work(WORK_ORDERING_UPDATE_TABLEWIDGET_WARES_LIST,&memory);
    }
    else
        m_widgets.remove_all_rows(ui->tableWidgetOrderingWareslist);//clear
    //-
    return b;
}

bool MainWindow::ordering_mask_edit(void)
{
    if(ui->checkBoxAutoSearchOrdering->isChecked())
        ordering_mask_set();
    return true;
}

bool MainWindow::ordering_checkbox_auto_clicked()
{
    if(ui->checkBoxAutoSearchOrdering->isChecked())
        ui->pushButtonMaskSetOrdering->setEnabled(false);
    else
        ui->pushButtonMaskSetOrdering->setEnabled(true);
    return true;
}

bool MainWindow::ordering_widgetitem_clicked()
{
    ordering_check_user_input();
    ordering_update_wares_list();
    return true;
}

bool MainWindow::ordering_mask_set(void)
{
    bool bArticlenumber;
    int iWork=WORK_ORDERING_GET_ALL;
    QList<int> lsIds;
    QStringList ls;
    QString sCondition,s2;
    int index=ui->comboBoxMaskOrdering->currentIndex();
    int iFormatType=FORMAT_ONE;
    CPointerMemory memory;
    //-
    QString s=ui->lineEditOrderingMask->text();
    sCondition=s;
    if(s.length()<=0)//no mask
    {}
    else if(index==0)//dealer
        iWork=WORK_ORDERING_GET_ALL_BY_DEALER;
    else if(index==5)//order number
        sCondition=QString::fromUtf8("order_number like '%1%'").arg(s);
    else if(index==6)//article
        iWork=WORK_ORDERING_GET_ALL_BY_ARTICLE;
    else if(index==7)//maker
        iWork=WORK_ORDERING_GET_ALL_BY_MAKER;
    else if(index==8)//waregroup
        iWork=WORK_ORDERING_GET_ALL_BY_WAREGROUP;
    else if(index==9)//articlenumber 1
    {
        bArticlenumber=true;
        iWork=WORK_ORDERING_GET_ALL_BY_ARTICLENUMBER;
    }
    else if(index==10)//articlenumber 2
    {
        bArticlenumber=false;
        iWork=WORK_ORDERING_GET_ALL_BY_ARTICLENUMBER;
    }
    else if(index==11)//comment
        sCondition=QString::fromUtf8("comment like '%%1%'").arg(s);
    //-date
    else if(index==1)//date from - to
    {
        ls=s.split("-");
        if(ls.count()==2)//two dates (format dd.mm.yyyy-dd.mm.yyyy)
        {
            s=m_widgets.check_date(ls[0]);
            if(s.length()>0)//date 1 right
            {
                s2=m_widgets.check_date(ls[1]);
                if(s2.length()>0)// date 2 right
                    sCondition=QString::fromUtf8("date >= '%1' AND date <= '%2'").arg(s,s2);
                else{}
            }
            else{}
        }
        else{}
    }
    else if(index >=2 && index <=4)
    {
        s2=m_widgets.check_date(s);
        if(s2.length()>0)//date right
        {
            if(index==2)// date <
                sCondition=QString::fromUtf8("date < '%1'").arg(s2);
            else if(index==3)//date =
                sCondition=QString::fromUtf8("date = '%1'").arg(s2);
            else if(index==4)//date >
                sCondition=QString::fromUtf8("date > '%1'").arg(s2);
        }
    }
    //-update table-
    memory.set_bool(&bArticlenumber);
    memory.set_int_list(&lsIds);
    memory.set_tablewidget(ui->tableWidgetOrdering);
    memory.set_string(&sCondition);
    memory.set_int(&iFormatType);

    if(!m_thread.set_work(iWork,&memory))
    {}
    if(!m_thread.set_work(WORK_ORDERING_UPDATE_TABLEWIDGET,&memory))
    {}
    else
    {
        ordering_check_user_input();
        ordering_update_wares_list();
        ordering_update_count();
    }
    //-
    ordering_check_user_input();
    //-
    return true;
}

bool MainWindow::ordering_new(void)
{
    if(!m_db.is_db_connect())
        return false;

    int iType=LC_ACTION_UPDATE_ORDERING;
    bool b=true;
    QString s;
    CLogbook lg;
    CDealer de;
    COrdering ord;
    CInputDialogOrdering dlg;
    dlg.setWindowTitle("neue Bestellung");
    dlg.set_thread(&m_thread);
    if(dlg.exec())
    {
        //new dealer created?
        if(dlg.m_iNewDealerId>0)
        {
            if(m_db.dealer_get(dlg.m_iNewDealerId,de))
            {
                m_widgets.dealer_insert_row(ui->tableWidgetDealer,de,ITEM_POSITION_BOTTOM,true);
                dealer_update_info();
                dealer_update_count();
                dealer_check_user_input();
                iType=LC_ACTION_UPDATE_ORDERING_DEALER;
            }
        }
        //-
        dlg.get_data(ord);// get input data
        if(!m_db.ordering_add(ord))
            b=false;
        else
        {//ok?
            m_widgets.ordering_insert_row(ui->tableWidgetOrdering,ord,FORMAT_ONE,ITEM_POSITION_BOTTOM,true);
            ordering_update_wares_list();
            ordering_update_count();
            inventory_mask_set();//update inventory table

            //logbook
            if(m_db.logbook_create_new(ord,lg))
                logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(iType,-1);// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return b;
}

bool MainWindow::ordering_edit(void)
{
    if(!m_db.is_db_connect())
        return false;

    int iType=LC_ACTION_UPDATE_ORDERING;
    QString s;
    bool b=true;
    COrdering ord;
    CLogbook lg;
    CDealer de;
    CInputDialogOrdering dlg;
    int iId=-1;

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetOrdering,iId))//select?
    {
        if(!m_db.ordering_get(iId,ord))//get ordering data
            b=false;
        else
        {
            //logbook - mark before
            m_db.logbook_create_edit(ord,lg,true);
            //-
            dlg.set_thread(&m_thread);//set thread
            dlg.set_data(ord);
            dlg.setWindowTitle("Bestellung bearbeiten");
            if(dlg.exec())
            {
                dlg.get_data(ord);
                b=m_db.ordering_update(ord);
                if(b)//error?
                {
                    if(dlg.m_iNewDealerId>0)//new dealer add?
                    {
                        if(m_db.dealer_get(dlg.m_iNewDealerId,de))//get new dealer
                        {
                            m_widgets.dealer_insert_row(ui->tableWidgetDealer,de,ITEM_POSITION_BOTTOM,true);
                            dealer_update_info();
                            dealer_update_count();
                            dealer_check_user_input();
                            iType=LC_ACTION_UPDATE_ORDERING_DEALER;
                        }
                    }
                    m_widgets.ordering_update_row(ui->tableWidgetOrdering,ord,FORMAT_ONE,true);
                    ordering_update_wares_list();//update warelist
                    m_bUpdateTableInventory=true;//update inv table

                    //logbook after
                    if(m_db.logbook_create_edit(ord,lg,false))
                        logbook_insert(lg);

                    //set last change in db -> another clients update table
                    s=m_LastDbChange.create_last_change_string(iType,-1);// create string
                    m_LastDbChange.write_last_change(s);//write in db
                }
            }
        }
    }

    //check delete by another client
    if(ord.get_id()>0)
    {
        s=QString::fromUtf8("id = %1").arg(ord.get_id());
        if(m_db.ordering_get_count(s)<=0)
            ordering_mask_set();//update table
    }
    return b;
}

bool MainWindow::ordering_delete(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=false;
    COrdering ord;
    CLogbook lg;
    QString s;
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    //-
    int iId;
    m_widgets.get_selected_table_item_value(ui->tableWidgetOrdering,iId);//get id
    if(m_db.ordering_get(iId,ord))//get ordering
    {
        m_db.logbook_create_remove(ord,lg);//create logbook

        s=QString::fromUtf8("Soll die Bestellung Nummer '%1' wirklich gelöscht werden?").arg(ord.get_ordernumber());
        msg.setText(s);
        msg.exec();
        if(msg.clickedButton()==yesButton)
        {
            if(m_db.ordering_remove(iId))
            {
                b=true;
                m_widgets.remove_row(ui->tableWidgetOrdering,iId);
                ordering_update_count();
                ordering_update_wares_list();//update warelist
                m_bUpdateTableInventory=true;//update inv table
                ordering_check_user_input();

                //logbook after
                logbook_insert(lg);

                //set last change in db -> another clients update table
                s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_ORDERING,-1);// create string
                m_LastDbChange.write_last_change(s);//write in db
            }
        }
    }
    return b;
}

bool MainWindow::ordering_table_warelist_doubleclick(void)
{
    open_picture_view(ui->tableWidgetOrderingWareslist);
    return true;
}


//trade----------------------------------------------------------------------------------------------------------------------------------

int MainWindow::trade_get_selected_year(void)
{
    int year=0;
    //-
    QString s=ui->comboBoxTradeMaskYear->itemText(ui->comboBoxTradeMaskYear->currentIndex());
    bool b;
    int i=s.toInt(&b,10);
    if(b)
        year=i;
    //-
    return year;
}

bool MainWindow::trade_info_hide_status(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=false;
    CTrade tr;
    if(trade_get_selected(tr))
    {
        if(tr.get_type()==TYPE_ORDERING_INCOMING || tr.get_type()==TYPE_CUSTOMER_OUTGOING)
            b=false;
        else
            b=true;
    }
    //-
    return b;
}

bool MainWindow::trade_get_selected(CTrade & trade)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=false;
    QString sBookingNumber;
    if(m_widgets.get_selected_table_item_value(ui->tableWidgetTrade,sBookingNumber,2))//select?
    {
        if(sBookingNumber.length()>0)
        {
            if(m_db.trade_get(sBookingNumber,trade))
                b=true;
        }
    }
    return b;
}

bool MainWindow::trade_update_wares_list(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=false;
    int iFormatType=FORMAT_ONE;
    CPointerMemory memory;
    memory.set_int(&iFormatType);
    QString sBookingNumber;
    if(m_widgets.get_selected_table_item_value(ui->tableWidgetTrade,sBookingNumber,2))//select?
    {
        if(sBookingNumber.length()>0)
        {
            memory.set_tablewidget(ui->tableWidgetTradeWareslist);
            memory.set_string(&sBookingNumber);
            b=m_thread.set_work(WORK_TRADE_UPDATE_TABLEWIDGET_WARES_LIST,&memory);
        }
    }
    return b;
}

bool MainWindow::trade_check_user_input(void)
{
    CTrade tr;
    QString sBookingNumber;
    bool b=true;

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetTrade,sBookingNumber,2))//select?
    {
        if(sBookingNumber.length()>0)
        {
            if(!m_db.trade_get(sBookingNumber,tr))
                b=false;//trade not found
            else
            {
                if(tr.get_canceled()==true)//was canceled?
                    b=false;
                else
                    b=true;
            }
        }
    }
    ui->pushButtonTradeCancel->setEnabled(b);
    return b;
}

bool MainWindow::trade_update_count(void)
{
    if(!m_db.is_db_connect())
        return false;

    int row=ui->tableWidgetTrade->rowCount();
    int count;
    QString s=QString::fromUtf8("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_TRADE_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString::fromUtf8("%1/").arg(row);
    s+=QString::fromUtf8("%1").arg(count);
    ui->labelCountTrade->setText(s);
    //-
    return true;
}

bool MainWindow::trade_hide_info_list(bool bHide)
{
    bool bQ=ui->listWidgetTradeInfo->isHidden();
    if(bHide==true && bQ==false)
    {
        ui->tableWidgetTradeWareslist->setFixedWidth(ui->tabWidget->width()-300);
        ui->listWidgetTradeInfo->hide();
        ui->labelTradeInfo->hide();
    }
    else if(bHide==false && bQ==true)
    {
        ui->tableWidgetTradeWareslist->setFixedWidth(ui->tabWidget->width()-560);
        ui->listWidgetTradeInfo->show();
        ui->labelTradeInfo->show();
    }
    return true;
}

bool MainWindow::trade_refresh_datelist(void)
{
    if(!m_db.is_db_connect())
        return false;

    CPointerMemory memory;
    int indexMask=ui->comboBoxMaskTrade->currentIndex();
    int indexDate=ui->comboBoxTradeMaskDate->currentIndex();
    QString s=ui->lineEditTradeMask->text();
    memory.set_int(&indexDate);
    memory.set_int2(&indexMask);
    memory.set_string(&s);
    memory.set_listwidget(ui->listWidgetTradeMaskDate);
    bool  b=m_thread.set_work(WORK_TRADE_REFRESH_DATE_LIST,&memory);
    return b;
}

bool MainWindow::trade_update_datelist(QDate dtSelect,int iYear)
{
    if(!m_db.is_db_connect())
        return false;

    //clear
    ui->listWidgetTradeMaskDate->clear();

    CPointerMemory memory;
    int indexMask=ui->comboBoxMaskTrade->currentIndex();
    int indexDate=ui->comboBoxTradeMaskDate->currentIndex();
    QString s=ui->lineEditTradeMask->text();
    memory.set_int(&indexDate);
    memory.set_int2(&indexMask);
    memory.set_string(&s);
    memory.set_listwidget(ui->listWidgetTradeMaskDate);
    memory.set_date(&dtSelect);
    memory.set_int3(&iYear);
    bool b=m_thread.set_work(WORK_TRADE_UPDATE_DATE_LIST,&memory);
    return b;
}

bool MainWindow::trade_check_diff_ordering(int iOrdId, CTrade & tr)
{
    if(!m_db.is_db_connect())
        return false;

    COrdering ord;
    if(!m_db.ordering_get(iOrdId,ord))
        return false;

    int i,j,id,id2,countO,countT,diff;
    bool bFound,b=true;
    QString s;
    QList<QString> lsO;
    QList<QString> lsT;

    //get
    ord.get_wares(lsO);
    tr.get_wares(lsT);

    //diff
    for(i=0;i<lsO.count();i++)
    {
        if(m_widgets.cast_count_and_id(lsO[i],countO,id))
        {
            for(j=0,bFound=false;j<lsT.count();j++)//search in trade(by id)
            {
                if(m_widgets.cast_count_and_id(lsT[j],countT,id2))
                {
                    if(id==id2)
                    {
                        lsT.removeAt(j);
                        bFound=true;
                        break;
                    }
                }
            }
            //-
            if(!bFound)//not found, no article (by id)
                countT=0;

            //set diff
            diff=countO-countT;
            if(diff>0)
            {
                s=QString::fromUtf8("%1x").arg(diff);//count(diff)
                s+=QString::fromUtf8("%1").arg(id);//a.id
                lsO[i]=s;//set diff.
             }
             else
             {
                 lsO.removeAt(i);//count zero or null -> remove
                 i--;
             }
         }
    }
    //update db + tables
    if(lsO.count()>0)
    {
        ord.set_wares(lsO);
        if(!m_db.ordering_update(ord))
            b=false;
        else
            ordering_update_wares_list();//update a.list
    }
    //-
    lsT.clear();
    lsO.clear();
    return b;
}

bool MainWindow::trade_mask_edit(void)
{
    if(ui->checkBoxAutoSearchTrade->isChecked())
        trade_mask_set();
    return true;
}

bool MainWindow::trade_checkbox_auto_clicked()
{
    if(ui->checkBoxAutoSearchTrade->isChecked())
        ui->pushButtonMaskSetTrade->setEnabled(false);
    else
        ui->pushButtonMaskSetTrade->setEnabled(true);
    return true;
}

bool MainWindow::trade_mask_date_changed(void)
{
    //update trade date mask list & middle the last selected dates(from->to)
    static int last_index=0;
    //-
    QList<QListWidgetItem*> ls=ui->listWidgetTradeMaskDate->selectedItems();
    if(ls.count()<0)//nothing select?->error
        return false;
    QString s=ls[0]->text();
    int index=ui->comboBoxTradeMaskDate->currentIndex();
    int diffDays,year=trade_get_selected_year();
    QDate dt,dtF,dtT;
    if(!m_widgets.trade_get_selecteted_mask_date(s,last_index,dtF,dtT))
        return false;
    //-
    dtF.setDate(year,dtF.month(),dtF.day());
    if(dtF>QDate().currentDate())
        dtF=QDate().currentDate();
    if(dtF.day()>dtF.daysInMonth())
        dtF.setDate(dtF.year(),dtF.month(),dtF.daysInMonth());
    dtT.setDate(year,dtT.month(),dtT.day());
    if(dtT>QDate().currentDate())
        dtT=QDate().currentDate();
    if(dtF.day()>dtF.daysInMonth())
        dtF.setDate(dtF.year(),dtF.month(),dtF.daysInMonth());
    //-
    if(dtF==dtT)
        dt=dtF;
    else
    {
        diffDays=dtF.daysTo(dtT);
        diffDays/=2;
        dt=dtF.addDays(diffDays);
    }
    //-
    if(year>0)
    {
        trade_update_datelist(dt,year);
        trade_mask_set(false);
    }
    //-
    last_index=index;
    //-
    return true;
}

bool MainWindow::trade_widgetitem_clicked()
{
    if(!m_db.is_db_connect())
        return false;

    trade_check_user_input();
    trade_update_wares_list();
    CTrade tr;
    if(trade_get_selected(tr))
    {
        if(!trade_info_hide_status())
        {
            trade_hide_info_list(false);
            m_widgets.trade_update_list_info(ui->listWidgetTradeInfo,tr);
        }
        else
        {
            trade_hide_info_list(true);
            ui->listWidgetTradeInfo->clear();
        }
    }
    else
        ui->listWidgetTradeInfo->clear();
    return true;
}

bool MainWindow::trade_date_mask_select_changed(void)
{
    QList<QListWidgetItem*>  ls=ui->listWidgetTradeMaskDate->selectedItems();
    if(ls.count()<=0)
        return false;
    trade_mask_set(false);
    return true;
}

bool MainWindow::trade_mask_set(bool bRefreshDateList)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=false;
    int iWork=WORK_TRADE_GET_ALL;
    bool bFirst=true;
    int index_rules=ui->comboBoxMaskTrade->currentIndex();
    QString sRules=ui->lineEditTradeMask->text();
    int year,index=ui->comboBoxTradeMaskDate->currentIndex();
    QDate dtFrom,dtTo;
    QString s,sCondition,si;
    QList<QString> lsTrade;
    CPointerMemory memory;
    //-
    if(ui->listWidgetTradeMaskDate->currentRow()>=0 && ui->listWidgetTradeMaskDate->currentRow()<ui->listWidgetTradeMaskDate->count())//date selected
    {//get mask date
        si=ui->listWidgetTradeMaskDate->item(ui->listWidgetTradeMaskDate->currentRow())->text();
        b=m_widgets.trade_get_selecteted_mask_date(si,index,dtFrom,dtTo);
    }
    //-
    if(!b)
    {
        year=trade_get_selected_year();
        if(year>0)
        {
            dtFrom=QDate(year,1,1);
            dtTo=QDate(year,12,31);
        }
    }
    //dates
    sCondition=QString::fromUtf8("date >= '%1' AND date <= '%2'").arg(dtFrom.toString(QString::fromUtf8("yyyy-MM-dd")),dtTo.toString(QString::fromUtf8("yyyy-MM-dd")));

    //onther rules
    if(sRules.length()>0)
    {
        if(index_rules==0)//bookingnumber
        {
            s=QString::fromUtf8("booking_number like '%%1%'").arg(sRules);
        }
        if(index_rules==1)//comment
        {
            s=QString::fromUtf8("comment like '%%1%'").arg(sRules);
        }
        if(index_rules==2)//article name
        {
            iWork=WORK_TRADE_GET_ALL_BY_ARTICLE;
        }
        if(index_rules==3)//maker(article)
        {
            iWork=WORK_TRADE_GET_ALL_BY_MAKER;
        }
        if(index_rules==4)//waregroup(article)
        {
            iWork=WORK_TRADE_GET_ALL_BY_WAREGROUP;
        }
        if(index_rules==5)//1.article number
        {
            iWork=WORK_TRADE_GET_ALL_BY_ARTICLENUMBER;
        }
        if(index_rules==6)//2.article number
        {
            iWork=WORK_TRADE_GET_ALL_BY_ARTICLENUMBER;
            bFirst=false;
        }
        if(index_rules==7)//dealer
        {
            s=QString::fromUtf8("info_2 like '%%1%'").arg(sRules);
            s+=QString::fromUtf8(" AND type = %1").arg(TYPE_ORDERING_INCOMING);
        }
        if(index_rules==8)//ordering number
        {
            s=QString::fromUtf8("info_1 like '%%1%'").arg(sRules);
            s+=QString::fromUtf8(" AND type = %1").arg(TYPE_ORDERING_INCOMING);
        }
        if(index_rules==9)//customer name
        {
            s=QString::fromUtf8("info_2 like '%%1%'").arg(sRules);
            s+=QString::fromUtf8(" AND type = %1").arg(TYPE_CUSTOMER_OUTGOING);
        }
        if(index_rules==10)//ordering number
        {
            s=QString::fromUtf8("info_1 like '%%1%'").arg(sRules);
            s+=QString::fromUtf8(" AND type = %1").arg(TYPE_CUSTOMER_OUTGOING);
        }
        //-
        if(index_rules<2 || index_rules>6)//booking_number or comment or dealer or customer
            sCondition+=QString::fromUtf8(" AND %1").arg(s);
    }

    //get records
    memory.set_string_list(&lsTrade);
    memory.set_string(&sCondition);
    memory.set_string2(&sRules);
    memory.set_bool(&bFirst);

    s=QString::fromUtf8("");
    if(!m_thread.set_work(iWork,&memory))
    {}
    else
    {
        //update table
        memory.set_tablewidget(ui->tableWidgetTrade);
        if(!m_thread.set_work(WORK_TRADE_UPDATE_TABLEWIDGET,&memory))
        {}
        else
        {
            trade_update_wares_list();//clear article info
            if(bRefreshDateList)
                trade_refresh_datelist();//refresh date list
            ui->listWidgetTradeInfo->clear();//clear info list
            trade_update_count();
            trade_check_user_input();
            m_widgets.remove_all_rows(ui->tableWidgetTradeWareslist);
        }
    }
    //-
    return true;
}

bool MainWindow::trade_incoming(void)
{
    if(!m_db.is_db_connect())
        return false;

    //get date mask
    QString s,si;
    si=ui->listWidgetTradeMaskDate->item(ui->listWidgetTradeMaskDate->currentRow())->text();
    QDate dtFrom,dtTo;
    if(!m_widgets.trade_get_selecteted_mask_date(si,ui->comboBoxTradeMaskDate->currentIndex(),dtFrom,dtTo))
        return false;
    //-
    CLogbook lg;
    int type;
    QList<QString> ls;
    CPointerMemory memory;
    memory.set_int(&type);
    memory.set_string_list(&ls);
    //-
    CTrade tr;
    CInputDlgTrade dlg;
    dlg.set(&m_thread,false);
    dlg.setWindowTitle("Wareneingang");
    dlg.set_booking_number_nomination();
    if(dlg.exec())
    {
        dlg.get_data(tr);
        tr.set_type(TYPE_INCOMING);
        m_db.trade_add(tr);

        //if trade date in mask date
        if(tr.get_date()>=dtFrom && tr.get_date()<=dtTo)
            m_widgets.trade_insert_row(ui->tableWidgetTrade,tr,ITEM_POSITION_TOP,true);//insert item at table

        //-update article counts
        tr.get_wares(ls);
        type=tr.get_type();
        if(m_thread.set_work(WORK_ARTICLE_UPDATE_ALL_COUNT,&memory))
        {
            m_bUpdateTableInventory=true;//update inv table
            trade_refresh_datelist();
            trade_update_count();
            inventory_check_article_under_warnlimit();//set icon to tab, if articles under limit
            //logbook
            if(m_db.logbook_create_new(tr,lg))
                logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INVENTORY_TRADE,-1);// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return true;
}

bool MainWindow::trade_outgoing(void)
{
    if(!m_db.is_db_connect())
        return false;

    //get date mask
    QString s,si;
    si=ui->listWidgetTradeMaskDate->item(ui->listWidgetTradeMaskDate->currentRow())->text();
    QDate dtFrom,dtTo;
    if(!m_widgets.trade_get_selecteted_mask_date(si,ui->comboBoxTradeMaskDate->currentIndex(),dtFrom,dtTo))
        return false;
    //-
    CLogbook lg;
    int type;
    QList<QString> ls;
    CPointerMemory memory;
    memory.set_int(&type);
    memory.set_string_list(&ls);
    //-
    CTrade tr;
    CInputDlgTrade dlg;
    dlg.set(&m_thread,true);
    dlg.setWindowTitle("Warenausgang");
    dlg.set_booking_number_nomination();
    if(dlg.exec())
    {
        dlg.get_data(tr);
        tr.set_type(TYPE_OUTGOING);
        m_db.trade_add(tr);

        //if trade date in mask date
        if(tr.get_date()>=dtFrom && tr.get_date()<=dtTo)
            m_widgets.trade_insert_row(ui->tableWidgetTrade,tr,ITEM_POSITION_TOP,true);//insert item at table

        //-update article counts
        tr.get_wares(ls);
        type=tr.get_type();
        if(m_thread.set_work(WORK_ARTICLE_UPDATE_ALL_COUNT,&memory))
        {
            m_bUpdateTableInventory=true;//update inv table
            trade_refresh_datelist();
            trade_update_count();
            inventory_check_article_under_warnlimit();//set icon to tab, if articles under limit

            //logbook
            if(m_db.logbook_create_new(tr,lg))
                logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INVENTORY_TRADE,-1);// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return true;
}

bool MainWindow::trade_ordering_incoming(void)
{
    if(!m_db.is_db_connect())
        return false;

    //get date mask
    QString si;
    si=ui->listWidgetTradeMaskDate->item(ui->listWidgetTradeMaskDate->currentRow())->text();
    QDate dtFrom,dtTo;
    if(!m_widgets.trade_get_selecteted_mask_date(si,ui->comboBoxTradeMaskDate->currentIndex(),dtFrom,dtTo))
        return false;
    //-

    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * noButton=msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
    msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msg.setWindowTitle("?");
    QString s=QString::fromUtf8("Die Bestellung ist nicht vollständig eingegangen, soll die Bestellung trotzdem gelöscht werden?\n(bei 'nein' wird die Differenz in der Bestellung gespeichert)");
    msg.setText(s);
    //-
    CLogbook lg;
    bool bOrdDelete=true;
    bool bCompleteIncoming;
    int iOrderingId,type;
    CTrade tr;
    QList<QString> ls;
    CPointerMemory memory;
    memory.set_int(&type);
    memory.set_string_list(&ls);
    CInputDlgOrderingIncoming dlg;
    dlg.setWindowTitle(QString::fromUtf8("Wareneingang(Bestellung)"));
    dlg.set_thread(&m_thread);
    dlg.init();//fill tables
    if(dlg.exec())
    {//pressed ok?
        if(dlg.get_data(tr,iOrderingId,bCompleteIncoming)) // get data
        {
            //complete?
            if(!bCompleteIncoming)
            {
                msg.exec();
                if(msg.clickedButton()==noButton)
                {
                    trade_check_diff_ordering(iOrderingId,tr);
                    bOrdDelete=false;
                }
            }
            if(bOrdDelete)
            {
                if(m_db.ordering_remove(iOrderingId))
                    m_widgets.remove_row(ui->tableWidgetOrdering,iOrderingId);
            }
            m_bUpdateTableOrdering=true;//update ordering table
            //-
            if(m_db.trade_add(tr))
            {
                if(tr.get_date()>=dtFrom && tr.get_date()<=dtTo)
                    m_widgets.trade_insert_row(ui->tableWidgetTrade,tr,ITEM_POSITION_TOP,true);
            }
            //-update article counts
            tr.get_wares(ls);
            type=tr.get_type();
            if(m_thread.set_work(WORK_ARTICLE_UPDATE_ALL_COUNT,&memory))
            {
                m_bUpdateTableInventory=true;//update inv table
                trade_refresh_datelist();
                trade_update_count();
                inventory_check_article_under_warnlimit();//set icon to tab, if articles under limit

                //logbook
                if(m_db.logbook_create_new(tr,lg))
                    logbook_insert(lg);

                //set last change in db -> another clients update table
                s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INVENTORY_TRADE_ORDERING,-1);// create string
                m_LastDbChange.write_last_change(s);//write in db
            }
        }
    }
    return true;
}

bool MainWindow::trade_outgoing_customer(void)
{
    if(!m_db.is_db_connect())
        return false;

    //get date mask
    QString si;
    si=ui->listWidgetTradeMaskDate->item(ui->listWidgetTradeMaskDate->currentRow())->text();
    QDate dtFrom,dtTo;
    if(!m_widgets.trade_get_selecteted_mask_date(si,ui->comboBoxTradeMaskDate->currentIndex(),dtFrom,dtTo))
        return false;
    //-
    QString s;
    CLogbook lg;
    int type;
    QList<QString> ls;
    CPointerMemory memory;
    memory.set_int(&type);
    memory.set_string_list(&ls);
    //-
    CTrade tr;
    CInputDlgOutgoingCustomer dlg;
    dlg.set_thread(&m_thread);
    dlg.setWindowTitle("Warenausgang(Kunden)");
    dlg.init();
    if(dlg.exec())
    {
        dlg.get_data(tr);
        tr.set_type(TYPE_CUSTOMER_OUTGOING);
        m_db.trade_add(tr);

        //if trade date in mask date
        if(tr.get_date()>=dtFrom && tr.get_date()<=dtTo)
            m_widgets.trade_insert_row(ui->tableWidgetTrade,tr,ITEM_POSITION_TOP,true);//insert item at table

        //-update article counts
        tr.get_wares(ls);
        type=tr.get_type();
        if(m_thread.set_work(WORK_ARTICLE_UPDATE_ALL_COUNT,&memory))
        {
            m_bUpdateTableInventory=true;//update inv table
            trade_refresh_datelist();
            trade_update_count();
            inventory_check_article_under_warnlimit();//set icon to tab, if articles under limit

            //logbook
            if(m_db.logbook_create_new(tr,lg))
                logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INVENTORY_TRADE,-1);// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return true;
}

bool MainWindow::trade_canceled(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=false;
    int iSwitchType;
    QMessageBox msg;
    QMessageBox msg2(QMessageBox::Information,"","");
    CTrade tr;
    CLogbook lg;
    QList<QString> lsS;
    QString s,sErr,sBookingNumber;
    CPointerMemory memory;
    memory.set_string(&sBookingNumber);
    memory.set_string2(&sErr);

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetTrade,sBookingNumber,2))//get selected booking number
    {
        if(sBookingNumber.length()>0)//booking number set?
        {
            if(m_db.trade_get(sBookingNumber,tr))//get trade
            {
                if(!tr.get_canceled())//trade not canceled already?
                {
                    b=m_thread.set_work(WORK_TRADE_CHECK_CANCEL_POSSIBLE,&memory);//check trade possible
                    if(!b)
                    {//canceled is not possible
                        s=QString::fromUtf8("Die Buchung kann nicht storniert werden da es zwischenzeitlich zu negativen Bestand kommen würde,"
                                  "beachten Sie die chronologische Reihenfolge!");
                        if(sErr.length()>0)
                            s+=QString::fromUtf8("\n(%1)").arg(sErr);
                        msg.setWindowTitle("!");
                        msg.setText(s);
                        msg.exec();
                    }
                    if(b)
                    {//canceled is possible
                        s=QString::fromUtf8("Möchten Sie wirklich die Buchnung mit der Nummer '%1' stornieren?").arg(sBookingNumber);
                        QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
                        msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
                        msg.setWindowTitle("?");
                        msg.setText(s);
                        msg.exec();
                        if(msg.clickedButton()==yesButton)
                        {
                            //another client cancel the trade in the interim?
                            if(m_db.trade_is_canceled(sBookingNumber))
                            {
                                msg2.setWindowTitle("!");
                                msg2.setText("Der Warengang wurde von einer anderen Programminstanz bereits storniert!");
                                msg2.exec();
                            }
                            else
                            {
                                tr.set_canceled(true);
                                m_db.trade_set_canceled(sBookingNumber);//set canceled in db
                                m_widgets.trade_update_row(ui->tableWidgetTrade,tr,true);//update table item

                                //update articles counts
                                if(tr.get_type()==TYPE_INCOMING || tr.get_type()==TYPE_ORDERING_INCOMING)
                                    iSwitchType=TYPE_OUTGOING;
                                else
                                    iSwitchType=TYPE_INCOMING;
                                tr.get_wares(lsS);
                                memory.clear();
                                memory.set_string_list(&lsS);
                                memory.set_int(&iSwitchType);
                                if(!m_thread.set_work(WORK_ARTICLE_UPDATE_ALL_COUNT,&memory))
                                    b=false;//error
                                else
                                {
                                    inventory_mask_set();//update inventory table

                                    //logbook
                                    if(m_db.logbook_create_canceled(tr,lg))
                                        logbook_insert(lg);

                                    //set last change in db -> another clients update table
                                    s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_INVENTORY_TRADE,-1);// create string
                                    m_LastDbChange.write_last_change(s);//write in db
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //-
    lsS.clear();
    memory.clear();
    return b;
}

bool MainWindow::trade_table_warelist_doubleclick(void)
{
    open_picture_view(ui->tableWidgetTradeWareslist);
    return true;
}

//inventory--------------------------------------

bool MainWindow::inventory_update_count(void)
{
    if(!m_db.is_db_connect())
        return false;

    int row=ui->tableWidgetInventory->rowCount();
    int count;
    QString s=QString::fromUtf8("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString::fromUtf8("%1/").arg(row);
    s+=QString::fromUtf8("%1").arg(count);
    ui->labelCountInventory->setText(s);
    //-
    return true;
}

int MainWindow::inventory_get_count_of_arctile_under_limit(void)
{
    if(!m_db.is_db_connect())
        return 0;

    int count=0;
    QString sCondition=QString::fromUtf8("article.inventory < article.warning_limit");
    CPointerMemory memory;
    memory.set_int(&count);
    memory.set_string(&sCondition);
    m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory);
    if(count<0)
        count=0;
    return count;
}

bool MainWindow::inventory_check_article_under_warnlimit(void)
{
    //inventory-warning
    int i=ui->tabWidget->indexOf(ui->tabInventory);
    if(i<0||i>=ui->tabWidget->count())
        i=0;
    if(inventory_get_count_of_arctile_under_limit()>0)
        ui->tabWidget->setTabIcon(i,m_widgets.get_warning_icon());
    else
        ui->tabWidget->setTabIcon(i,QIcon());
    return true;
}

bool MainWindow::inventory_widgetitem_clicked()
{
    int iId=-1;

    //select same article in article table
    if(m_widgets.get_selected_table_item_value(ui->tableWidgetInventory,iId))
         m_widgets.find_and_select_row(ui->tableWidgetArticle,iId);
    return true;
}

bool MainWindow::inventory_mask_edit(void)
{
    if(ui->checkBoxAutoSearchInventory->isChecked())
        inventory_mask_set();
    return true;
}

bool MainWindow::inventory_checkbox_auto_clicked()
{
    if(ui->checkBoxAutoSearchInventory->isChecked())
        ui->pushButtonMaskSetInventory->setEnabled(false);
    else
        ui->pushButtonMaskSetInventory->setEnabled(true);
    return true;
}

bool MainWindow::inventory_mask_set(void)
{
    if(!m_db.is_db_connect())
        return false;

    QList<int> lsIds;
    QString sCondition,s=ui->lineEditInventoryMask->text();
    CPointerMemory memory;
    memory.set_int_list(&lsIds);
    memory.set_string(&s);
    int input=-1,index=ui->comboBoxMaskInventory->currentIndex();
    bool b=true,b2;


    //search by ???

    if(s.length()<=0)//no mask
        b=m_thread.set_work(WORK_ARTICLE_GET_ALL,&memory);
    else if(index==4||index==5||index==9||index==10)
    {
        if(index==4||index==5)//by percent
        {
            input=s.toInt(&b,10);//its number?
            if(!b)
            {}
            else
            {
                memory.set_int(&input);
                if(index==4)// % <
                    b2=true;
                else if(index==5)// % >
                    b2=false;
                memory.set_bool(&b2);
                b=m_thread.set_work(WORK_ARTICLE_GET_ALL_BY_PERCENT,&memory);
            }
        }
        else if(index==9)//by maker
            b=m_thread.set_work(WORK_ARTICLE_GET_ALL_BY_MAKER,&memory);
        else if(index==10)//by waregroup
            b=m_thread.set_work(WORK_ARTICLE_GET_ALL_BY_WAREGROUP,&memory);
    }
    else
    {
        if(index==6)//name
            sCondition=QString::fromUtf8("name like '%%1%'").arg(s);
        else if(index==7)//articlenumber
            sCondition=QString::fromUtf8("articlenumber like '%%1%'").arg(s);
        else if(index==8)//2.articlenumber
            sCondition=QString::fromUtf8("articlenumber2 like '%%1%'").arg(s);
        else if(index==11)//location
            sCondition=QString::fromUtf8("location like '%%1%'").arg(s);
        else if(index==12)//unit
            sCondition=QString::fromUtf8("unit like '%%1%'").arg(s);
        else if(index==13)//comment
            sCondition=QString::fromUtf8("comment like '%%1%'").arg(s);
        else if(index>=0 && index<4)//inventory / max.inventory
        {
            input=s.toInt(&b,10);
            if(b)//is number
            {
                if(index==0)//inv. <
                    sCondition=QString::fromUtf8("inventory < %1").arg(input);
                else if(index==1)//inv. >
                    sCondition=QString::fromUtf8("inventory > %1").arg(input);
                else if(index==2)//max.inv. <
                    sCondition=QString::fromUtf8("max_inventory < %1").arg(input);
                else if(index==3)//max.inv. >
                    sCondition=QString::fromUtf8("max_inventory > %1").arg(input);
            }
            else{}
        }
        //-
        memory.set_string(&sCondition);
        b=m_thread.set_work(WORK_ARTICLE_GET_ALL,&memory);
    }
    //-
    if(!b)
    {}
    else
    {
        memory.set_tablewidget(ui->tableWidgetInventory);
        b=m_thread.set_work(WORK_INVENTORY_UPDATE_TABLEWIDGET,&memory);
        if(!b)
        {}
        else
            inventory_update_count();
    }
    return b;
}

bool MainWindow::inventory_table_doubleclick(void)
{
    open_picture_view(ui->tableWidgetInventory);
    return true;
}

//logbook------------------------------------------------------------------------------------------------------

QDate MainWindow::logbook_get_selected_date(void)
{
    QDate dt;
    QListWidgetItem * pItem=ui->listWidgetLogbookMaskDate->currentItem();
    if(pItem!=NULL)
    {
        if(!m_widgets.logbook_get_selecteted_mask_date(pItem->text(),dt))
            dt=QDate(0,0,0);
    }
    return dt;
}

int MainWindow::logbook_get_selected_year(void)
{
    int year=0;
    //-
    QString s=ui->comboBoxLogbookMaskYear->itemText(ui->comboBoxLogbookMaskYear->currentIndex());
    bool b;
    int i=s.toInt(&b,10);
    if(b)
        year=i;
    //-
    return year;
}

bool MainWindow::logbook_update_count(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    QDate dt=logbook_get_selected_date();
    int count,count2;
    QString s=QString::fromUtf8("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count2);
    //get all
    m_thread.set_work(WORK_LOGBOOK_GET_COUNT,&memory);
    if(count2<0)
        b=false;
    //by mask
    if(b)
    {
        memory.set_int(&count);
        s=QString::fromUtf8("date_time like '%1%'").arg(dt.toString("yyyy-MM-dd"));
        m_thread.set_work(WORK_LOGBOOK_GET_COUNT,&memory);
        if(count<0)
            b=false;
        else
        {
            s=QString::fromUtf8("");
            if(count!=count2)
                s=QString::fromUtf8("%1/").arg(count);
            s+=QString::fromUtf8("%1").arg(count2);
            ui->labelCountLogbook->setText(s);
        }
    }
    //-
    return b;
}

bool MainWindow::logbook_insert(CLogbook & lg)
{
    QString s;
    bool b,bMask=true;
    int index=ui->comboBoxMaskLogbook->currentIndex();
    QDate dt2,dt=logbook_get_selected_date();
    QDateTime dt_ti=lg.get_date_time();
    dt2=dt_ti.date();

    //username set? -> add in logbook text
    if(m_sUser.length()>0)//user name set?
    {
        s=QString::fromUtf8("user: %1 - %2").arg(m_sUser,lg.get_text());
        lg.set_text(s);
    }

    //-
    if(index==1)
    {
        if(lg.get_type()<LOGBOOK_TYPE_TRADE_IN || lg.get_type()>LOGBOOK_TYPE_TRADE_CUS_OUT)//trade
            bMask=false;
    }
    if(index==2)
    {
        if(lg.get_type()<LOGBOOK_TYPE_ORDERING_NEW || lg.get_type()>LOGBOOK_TYPE_ORDERING_REMOVE)//ordering
            bMask=false;
    }
    if(index==3 )
    {
        if(lg.get_type()<LOGBOOK_TYPE_ARTICLE_NEW || lg.get_type()>LOGBOOK_TYPE_ARTICLE_REMOVE)//article
            bMask=false;
    }
    if(index==4)
    {
        if(lg.get_type()<LOGBOOK_TYPE_WAREGROUP_NEW || lg.get_type()>LOGBOOK_TYPE_WAREGROUP_REMOVE)//waregroup
            bMask=false;
    }
    if(index==5)
    {
        if(lg.get_type()<LOGBOOK_TYPE_MAKER_NEW || lg.get_type()>LOGBOOK_TYPE_MAKER_REMOVE)//maker
            bMask=false;
    }
    if(index==6)
    {
        if(lg.get_type()<LOGBOOK_TYPE_DEALER_NEW || lg.get_type()>LOGBOOK_TYPE_DEALER_REMOVE)//dealer
            bMask=false;
    }
    if(index==7)
    {
        if(lg.get_type()<LOGBOOK_TYPE_CUSTOMER_NEW || lg.get_type()>LOGBOOK_TYPE_CUSTOMER_REMOVE)//customer
            bMask=false;
    }
    //write in db
    b=m_db.logbook_add(lg);
    if(!b)
    {}
    //remove item over count
    if(logbook_check_items_over_count())
        logbook_mask_set();//over count removed?->update table
    else
    {
        //-insert item at list or update list
        if(dt==dt2 && bMask==true)//current date + mask select?
            ui->listWidgetLogbook->insertItem(0,lg.get_output_string());//insert first row
    }
    //refresh list
    logbook_refresh_datelist();
    //count
    logbook_update_count();
    //-
    return b;
}

bool MainWindow::logbook_refresh_datelist(void)
{
    if(!m_db.is_db_connect())
        return false;

    CPointerMemory memory;
    int index=ui->comboBoxMaskLogbook->currentIndex();
    memory.set_int(&index);
    memory.set_listwidget(ui->listWidgetLogbookMaskDate);
    bool  b=m_thread.set_work(WORK_LOGBOOK_REFRESH_DATE_LIST,&memory);
    return b;
}

bool MainWindow::logbook_update_datelist(QDate dtSelect,int iYear)
{
    if(!m_db.is_db_connect())
        return false;

    //clear
    ui->listWidgetLogbookMaskDate->clear();

    int index=ui->comboBoxMaskLogbook->currentIndex();
    CPointerMemory memory;
    memory.set_listwidget(ui->listWidgetLogbookMaskDate);
    memory.set_date(&dtSelect);
    memory.set_int(&iYear);
    memory.set_int2(&index);
    bool b=m_thread.set_work(WORK_LOGBOOK_UPDATE_DATE_LIST,&memory);
    return b;
}

bool MainWindow::logbook_check_items_over_count(void)
{
    if(!m_db.is_db_connect())
        return false;

    QList<QDateTime> ls;
    QString s2,s,sOrderBy("date_time DESC");
    bool b,b2=false;
    int log_count;
    CPointerMemory memory;
    memory.set_int(&log_count);
    memory.set_string(&s);
    memory.set_string2(&sOrderBy);
    s=ui->lineEditCountItemLog->text();
    int max_count=s.toInt(&b,10);//get max count
    if(b)
    {
        s=QString::fromUtf8("");
        m_thread.set_work(WORK_LOGBOOK_GET_COUNT,&memory);//get item count logbook
        if(log_count>max_count)//over?
        {
            memory.set_date_time_list(&ls);
            if(m_thread.set_work(WORK_LOGBOOK_GET_ALL,&memory))
            {
                if(max_count<ls.count())
                {
                    memory.clear();
                    s2=ls[max_count].toString("yyyy-MM-dd hh:mm:ss");
                    s=QString::fromUtf8("date_time <= '%1'").arg(s2);
                    memory.set_string(&s);
                    if(m_thread.set_work(WORK_LOGBOOK_REMOVE_ALL,&memory))
                        b2=true;//found/remove -> update widgets
                }
            }
        }
    }
    //-
    ls.clear();
    //-
    return b2;
}

bool MainWindow::logbook_date_mask_year_changed(void)
{
    if(!m_db.is_db_connect())
        return false;

    int year=logbook_get_selected_year();
    QDate dt=logbook_get_selected_date();
    dt.setDate(year,dt.month(),dt.day());
    QDate dtCurrent=QDate().currentDate();
    if(dt>dtCurrent)
        dt=dtCurrent;
    if(dt.day()>dt.daysInMonth())
        dt.setDate(dt.year(),dt.month(),dt.daysInMonth());
    if(year>0)
    {
        logbook_update_datelist(dt,year);
        logbook_mask_set();
    }
    //-
    return true;
}

bool MainWindow::logbook_combo_mask_changed(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=logbook_refresh_datelist();
    if(b)
        b=logbook_mask_set();
    return b;
}

bool MainWindow::logbook_mask_set(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    QString sCondition,sOrderBy;
    QList<QDateTime> lsDateTime;
    CPointerMemory memory;
    memory.set_date_time_list(&lsDateTime);
    memory.set_string(&sCondition);
    memory.set_string2(&sOrderBy);
    QDate dt=logbook_get_selected_date();
    QDateTime dt_tiSelect;
    int index=ui->comboBoxMaskLogbook->currentIndex();
    //-
    b=m_widgets.logbook_get_mask_condition(dt,index,sCondition);
    if(b)
    {
        b=m_thread.set_work(WORK_LOGBOOK_GET_ALL,&memory);
        if(!b)
        {}
        else
        {
            memory.set_listwidget(ui->listWidgetLogbook);
            memory.set_date_time(&dt_tiSelect);
            b=m_thread.set_work(WORK_LOGBOOK_UPDATE_LISTWIDGET,&memory);
            if(!b)
            {}
            else
                logbook_update_count();
        }
    }
    return b;
}

bool MainWindow::logbook_change_count_of_item(void)
{
    bool b;
    CLogbook lg;
    QString s=ui->lineEditCountItemLog->text();
    int new_count,cur_count=s.toInt(&b,10);
    if(b)
    {
        new_count=QInputDialog::getInt(this,QString::fromUtf8("Anzahl ändern"),QString::fromUtf8("neue Anzahl(min.10):"),cur_count,10,9999,1,&b);
        if(b)
        {//pressed ok
            if(cur_count!=new_count)
            {//changed?
                s=QString::fromUtf8("%1").arg(new_count);
                ui->lineEditCountItemLog->setText(s);//set new count

                //write to db
                m_db.write_logbook_count(new_count);

                //logbook
                lg.set_type(LOGBOOK_TYPE_OTHER);
                lg.set_date_time(QDateTime::currentDateTime());
                s=QString::fromUtf8("Anzahl der letzten zu speichernden Logbucheinträge von %1 auf ").arg(cur_count);
                s+=QString::fromUtf8("%1 geändert").arg(new_count);
                lg.set_text(s);
                logbook_insert(lg);

                if(logbook_check_items_over_count())//check itemcount- remove over
                {
                    logbook_mask_set();//over-> update list
                    logbook_refresh_datelist();//update list
                }

                //set last change in db -> another clients update count
                s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_LOGBOOK_COUNT,-1);// create string
                m_LastDbChange.write_last_change(s);//write in db
            }
        }
    }
    return b;
}

bool MainWindow::logbook_clear(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=false;
    QString s=QString::fromUtf8("");
    CLogbook lg;
    CPointerMemory memory;
    memory.set_string(&s);

    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle(QString::fromUtf8("Achtung!"));
    msg.setText(QString::fromUtf8("Möchten Sie wirklich alle Logbucheinträge löschen ?"));
    msg.exec();
    if(msg.clickedButton()==yesButton)
    {
        b=m_thread.set_work(WORK_LOGBOOK_REMOVE_ALL,&memory);
        if(b)//remove ok?
        {
            ui->listWidgetLogbook->clear();

            //logbook
            lg.set_type(LOGBOOK_TYPE_OTHER);
            lg.set_date_time(QDateTime::currentDateTime());
            lg.set_text(QString::fromUtf8("alle Logbucheinträge gelöscht"));
            logbook_insert(lg);

            //set last change in db -> another clients update table
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_LOGBOOK,-1);// create string
            m_LastDbChange.write_last_change(s);//write in db
        }
    }
    return b;
}


//menu--------------------------------------------------------------------------------------------------------------------
bool MainWindow::menu_db_backup_create(void)
{
    if(!m_db.is_db_connect())
        return false;

    QFile file(m_sDbPath);
    QFileInfo file_info;
    CLogbook lg;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("Fehler"));
    QDateTime dtTi=QDateTime::currentDateTime();


    //load backup-path
    QDir dir;
    CSettings settings;
    QString s,s2,sBackupPath;
    if(settings.load(QString::fromUtf8("BACKUP_PATH"),s))//load backup path
    {
        if(s.length()>0)
        {
            dir.setPath(s);
            if(dir.exists())
                sBackupPath=s;
        }
    }

    //get db path folder
    QStringList ls;
    QString sDbPathFolder;
    ls=m_sDbPath.split("/");
    for(int i=0;i<ls.count()-1;i++)
        sDbPathFolder+=ls[i]+QString::fromUtf8("/");
    ls.clear();

    //set default path+name(backup file)
    QString sWriteFile;
    if(sBackupPath.length()>0)//backup path load?
        sWriteFile=sBackupPath+QString::fromUtf8("/%1.sqlite").arg(dtTi.toString(QString::fromUtf8("hh-mm-ss_dd-MM-yyyy")));
    else//default path
        sWriteFile=sDbPathFolder+QString::fromUtf8("/%1.sqlite").arg(dtTi.toString(QString::fromUtf8("hh-mm-ss_dd-MM-yyyy")));

    //open file dialog
    sWriteFile=QFileDialog::getSaveFileName(this,QString::fromUtf8("Sicherung erstellen"),sWriteFile);
    if(sWriteFile.length()>0)//file dialog finish with 'save'?
    {
        //file info (permission)
        file_info.setFile(sDbPathFolder);
        if(!file_info.permission(QFile::WriteUser))
        {
            msg.setText(QString::fromUtf8("Sie haben keine Schreibrechte an diesem Ort!"));
            msg.exec();
        }
        else
        {
            //user overwrite current db? ->error
            if(m_sDbPath==sWriteFile)
            {
                s=QString::fromUtf8("Sie können nicht die aktuelle Datenbankdatei überschreiben!");
                msg.setText(s);
                msg.exec();
            }
            else
            {
                //save backup path
                settings.write(QString::fromUtf8("BACKUP_PATH"),file_info.filePath());

                //check give it file?
                if(QFile(sWriteFile).exists())
                    QFile::remove(sWriteFile);//delete old file

                //copy
                if(!file.copy(sWriteFile))
                    s2=QString::fromUtf8("(errorcode: %1)").arg(file.error());

                //exists create file?
                if(!QFile(sWriteFile).exists())
                {
                    s=QString::fromUtf8("Fehler beim Erstellen der Sicherung, evtl. fehlen Ihnen die Schreibrechte für diesen Ort!");
                    if(s2.length()>0)
                        s+=s2;
                    else{}
                    msg.setText(s);
                    msg.exec();
                }
                else
                {
                    //logbook
                    lg.set_type(LOGBOOK_TYPE_OTHER);
                    lg.set_date_time(QDateTime::currentDateTime());
                    s=QString::fromUtf8("Sicherung \"%1 \" erstellt").arg(sWriteFile);
                    lg.set_text(QString(s));
                    logbook_insert(lg);

                    //set last change in db -> another clients update table
                    s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_LOGBOOK,-1);// create string
                    m_LastDbChange.write_last_change(s);//write in db
                }
            }
        }
    }
    file.close();
    ls.clear();
    return true;
}

bool MainWindow::menu_db_backup_load(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b;
    QString s;
    QStringList ls;
    QString sError,sReadFile;
    QString sWriteTempFile;
    QString sWriteFile=m_sDbPath;
    QMessageBox msgQuest(QMessageBox::Question,"","");
    msgQuest.setText(QString::fromUtf8("Achtung, beim Laden einer Sicherung gehen alle aktuellen Daten unwiderruflich verloren."
                             " Ausserdem werden alle anderen Programminstanzen die auf diese Datenbank zugreifen aufgeforderten, sich zu beenden.\nMöchten Sie wirklich eine Sicherung laden?"));
    QMessageBox msgCritical(QMessageBox::Critical,"","");
    msgCritical.setWindowTitle(QString::fromUtf8("Fehler"));
    msgQuest.setWindowTitle(QString::fromUtf8("?"));
    QPushButton * yesButton=msgQuest.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msgQuest.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);


    //get db path folder
    QString sDbPathFolder;
    ls=m_sDbPath.split("/");
    for(int i=0;i<ls.count()-1;i++)
        sDbPathFolder+=ls[i]+QString::fromUtf8("/");
    ls.clear();
    sWriteTempFile=sDbPathFolder+QString::fromUtf8("lava_temp.sqlite");//set temp file path

    //load backup-path
    QDir dir;
    CSettings settings;
    QString sPath=sDbPathFolder;
    if(settings.load(QString::fromUtf8("BACKUP_PATH"),s))//load backup path
    {
        if(s.length()>0)
        {
            dir.setPath(s);
            if(dir.exists())
                sPath=s;
        }
    }   

    //stop timer
    if(m_iTimerId!=-1)
    {
        killTimer(m_iTimerId);//stop timer
        m_iTimerId=-1;
    }

    //check permission db-path
    QFileInfo file_info(sDbPathFolder);
    if(!file_info.permission(QFile::WriteUser))
        sError=QString::fromUtf8("Sie haben keine Schreibrechte für den  Ordner \"%1\"!").arg(sDbPathFolder);//error
    else
    {
        msgQuest.exec();//quest really overwrite db-file?
        if(msgQuest.clickedButton()==yesButton)
        {
            //send exit-order to another clients
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_ORDER_QUIT,-1);// create string
            m_LastDbChange.write_last_change(s);//write in db

            //close current db connection
            m_db.close_db();

            //-
            sReadFile=QFileDialog::getOpenFileName(this,QString::fromUtf8("Sicherung laden"),sPath);
            if(sReadFile.length()>0)
            {//load button pressed?

                if(sReadFile==m_sDbPath)
                    sError=QString::fromUtf8("Sie benutzen zur Zeit die Datenbankdatei \"%1\"!").arg(m_sDbPath);//error
                else
                {
                    if(!QFile::exists(sReadFile))//check give it read file?
                        sError=QString::fromUtf8("Die angegebene Datei existiert nicht!");//error
                    else
                    {
                        if(!m_db.open_db(sReadFile))//open db(read file) ok?
                            sError=QString::fromUtf8("Die angegebene Datei konnte nicht geöffnet werden!");//error
                        else
                        {
                            b=m_db.check_database_file(); // is the read file a lava2 sqlite db file?
                            m_db.close_db();//close read db
                            if(!b)
                                sError=QString::fromUtf8("Die angegebene Datei ist keine LaVa2-Datenbankdatei!");//error
                            else
                            {
                                //save current db-file
                                if(QFile(sWriteTempFile).exists())//exists temp-file ? ->remove it
                                    QFile::remove(sWriteTempFile);
                                if(!QFile::rename(sWriteFile,sWriteTempFile))//rename current db to temp
                                    sError=QString::fromUtf8("Fehler beim umbenennen der Datenbank-Datei!");
                                else
                                {
                                    //copy file
                                    b=QFile::copy(sReadFile,sWriteFile);//copy ok?
                                    if(!b || !QFile::exists(sWriteFile))
                                    {//copy error
                                        sError=QString::fromUtf8("Fehler beim kopieren der Datenbank-Datei!");
                                        QFile::rename(sWriteTempFile,sWriteFile);//rename back
                                    }
                                    else//copy ok
                                        QFile::remove(sWriteTempFile);//delete temp file(old db-file)

                                    //save backup path
                                    s=QString::fromUtf8("");
                                    ls=sReadFile.split("/");
                                    for(int i=0;i<ls.count()-1;i++)
                                        s+=ls[i]+QString::fromUtf8("/");
                                    settings.write(QString::fromUtf8("BACKUP_PATH"),s);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //check error after copy
    if(sError.length()>0)
    {
        msgCritical.setText(sError);
        msgCritical.exec();
        sError=QString::fromUtf8("");
    }


    //start all
    if(!m_db.open_db(sWriteFile))
        sError=QString::fromUtf8("Fehler beim Öffnen der Datenbank,");
    else
    {
        //check + update db-file
        if(!m_db.check_and_update_db_version())
            sError=QString::fromUtf8("Fehler beim update der Datenbankdatei auf Version %1,").arg(CURRENT_DB_VERSION);
        else
        {
            //update widgets
            fill_date_lists();
            fill_all_table(true);
            fill_logbook_count();

            //set last change in db -> another clients update db
            s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_ALL,-1);// create string
            m_LastDbChange.write_last_change(s);//write in db
            //-
            startTimer(3000);
        }
    }


    //check error after start
    if(sError.length()>0)
    {
        sError+=QString::fromUtf8("\ndas Programm hat keine Datenbankverbindung mehr!!!");
        msgCritical.setText(sError);
        msgCritical.exec();
    }
    //-
    ls.clear();
    return true;
}

bool MainWindow::menu_db_clear(void)
{
    if(!m_db.is_db_connect())
        return false;

    QMessageBox msg(QMessageBox::Question,"","");
    msg.setWindowTitle(QString::fromUtf8("?"));
    QPushButton * yesButton=msg.addButton(QString::fromUtf8("Ja"),QMessageBox::YesRole);
    msg.addButton(QString::fromUtf8("Nein"),QMessageBox::NoRole);
    QString s=QString::fromUtf8("Möchten Sie wirklich alle Daten der Datenbank löschen?\n(die Daten sind unwiderruflich verloren)");
    msg.setText(s);
    msg.exec();
    if(msg.clickedButton()==yesButton)
    {
        m_db.clear();//clear db

        fill_date_lists();
        fill_all_table();
        fill_logbook_count();

        //set last change in db -> another clients update db
        s=m_LastDbChange.create_last_change_string(LC_ACTION_UPDATE_ALL,-1);// create string
        m_LastDbChange.write_last_change(s);//write in db
    }
    return true;
}

bool MainWindow::menu_tool_close(void)
{
    close();
    return true;
}

bool MainWindow::menu_help(void)
{
    QString s;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("Fehler"));
    if(!QFile::exists(QCoreApplication::applicationDirPath()+QString::fromUtf8("/doc/help_de.pdf")))//help file not found
    {
        s=QString::fromUtf8("Die Datei \"%1\" ist nicht vorhanden!").arg(QCoreApplication::applicationDirPath()+QString::fromUtf8("/doc/help_de.pdf"));
        msg.setText(s);
        msg.exec();
    }
    else
    {
        QUrl url=QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+QString::fromUtf8("/doc/help_de.pdf"));
        QDesktopServices::openUrl(url);
    }
    return true;
}

bool MainWindow::menu_about(void)
{
    QString s=QString::fromUtf8("L A ger V erwaltungs A ssistent 2"
                      "\n(inventory management assistant)"
                      "\n------------------------------------------------------------------------"
                      "\ngeschrieben von/written by Robert Ewert - germany(Berlin)"
                      "\nrobert.ewert@gmail.com - www.robert.ewert.de.vu"
                      "\n------------------------------------------------------------------------"
                      "\nLizenz/licence: GNU GPL version 3"
                      "\nwritten in QT 4.8 & QtCreator (C++)"
                      "\n------------------------------------------------------------------------");
    s+=QString::fromUtf8("\n\n%1").arg(VERSION);
    QMessageBox msg(QMessageBox::Information,"","");
    msg.setWindowTitle("information:");
    msg.setText(s);
    msg.exec();
    return true;
}

bool MainWindow::menu_balance_list(void)
{
    CDlgBalanceList dlg;
    dlg.setWindowTitle(QString::fromUtf8("Artikelsaldenliste"));
    dlg.set_thread(&m_thread);
    dlg.exec();
    return true;
}

bool MainWindow::menu_article_under_warnlimit(void)
{
    QList<QString> ls;
    ls.push_back(QString::fromUtf8("Artikelbezeichnung"));
    ls.push_back(QString::fromUtf8("1.Artikelnummer"));
    ls.push_back(QString::fromUtf8("2.Artikelnummer"));
    ls.push_back(QString::fromUtf8("Hersteller"));
    ls.push_back(QString::fromUtf8("Warengruppe"));
    ls.push_back(QString::fromUtf8("Bestand"));
    ls.push_back(QString::fromUtf8("Warnlimit"));

    CDlgUniList dlg;
    dlg.setWindowTitle(QString::fromUtf8("Artikel unter Warnlimit"));
    dlg.set_thread(&m_thread);
    dlg.set_type(TYPE_WARN_LIST);
    dlg.create_table_columns(ls);
    dlg.update_table();
    dlg.exec();
    ls.clear();
    return true;
}

bool MainWindow::menu_inventorys_on_date(void)
{
    QList<QString> ls;
    ls.push_back(QString::fromUtf8("Bestand"));
    ls.push_back(QString::fromUtf8("Artikelbezeichnung"));
    ls.push_back(QString::fromUtf8("1.Artikelnummer"));
    ls.push_back(QString::fromUtf8("2.Artikelnummer"));
    ls.push_back(QString::fromUtf8("Hersteller"));
    ls.push_back(QString::fromUtf8("Warengruppe"));

    CDlgUniList dlg;
    dlg.setWindowTitle(QString::fromUtf8("Artikelbestände am folgenden Datum (Tagesabschluss)"));
    dlg.set_thread(&m_thread);
    dlg.set_type(TYPE_INVENTORYS_ON_DATE);
    dlg.create_table_columns(ls);
    dlg.update_table();
    dlg.exec();
    ls.clear();
    return true;
}

bool MainWindow::menu_inventorys_list(void)
{
    QList<QString> ls;
    ls.push_back(QString::fromUtf8("Bst./max.Kap."));
    ls.push_back(QString::fromUtf8("Artikelbez."));
    ls.push_back(QString::fromUtf8("1/2.Artikelnr."));
    ls.push_back(QString::fromUtf8("Hersteller"));
    ls.push_back(QString::fromUtf8("Warengruppe"));
    ls.push_back(QString::fromUtf8("Standort/Kommentar"));
    ls.push_back(QString::fromUtf8("akt.Bestand"));

    CDlgUniList dlg;
    dlg.setWindowTitle(QString::fromUtf8("Inventurliste"));
    dlg.set_thread(&m_thread);
    dlg.set_type(TYPE_INVENTORY_LIST);
    dlg.create_table_columns(ls);
    dlg.update_table();
    dlg.exec();
    ls.clear();
    return true;
}

bool MainWindow::menu_list_value_of_goods(void)
{
    QList<QString> ls;
    ls.push_back(QString::fromUtf8("Artikelbez."));
    ls.push_back(QString::fromUtf8("Hersteller"));
    ls.push_back(QString::fromUtf8("Warengruppe"));
    ls.push_back(QString::fromUtf8("akt.Bestand"));
    ls.push_back(QString::fromUtf8("EK / VK"));
    ls.push_back(QString::fromUtf8("Wert (EK)"));
    ls.push_back(QString::fromUtf8("Wert (VK)"));
    ls.push_back(QString::fromUtf8("Marge"));

    CDlgUniList dlg;
    dlg.setWindowTitle(QString::fromUtf8("Warenwerte auf Lager"));
    dlg.set_thread(&m_thread);
    dlg.set_type(TYPE_LIST_VALUE_OF_GOODS);
    dlg.create_table_columns(ls);
    dlg.update_table();
    dlg.exec();
    ls.clear();
    return true;
}

bool MainWindow::menu_print_logbook(void)
{
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("!"));
    msg.setText(QString::fromUtf8("Es gibt an dem ausgewähltem Datum keine Logbucheinträge!"));
    //-
    QList<QListWidgetItem*> ls=ui->listWidgetLogbookMaskDate->selectedItems();
    QString s2,s=QString::fromUtf8("Logbucheinträge vom ");
    QStringList lS;
    //-
    if(ls.count()>0)
    {
        s2=ls[0]->text();
        lS=s2.split("|");
        if(lS.count()>0)
            s+=lS[0];
    }
    ls.clear();
    //-
    if(ui->listWidgetLogbook->count()<=0)
        msg.exec();//error - no logbook items in list
    else
    {
        m_print_job.m_memory.clear();
        m_print_job.set_type(PRINT_JOB_LIST);
        m_print_job.m_memory.set_listwidget(ui->listWidgetLogbook);
        print_open_preview(s);
    }
    return true;
}

bool MainWindow::menu_print_maker_selection(void)
{
    QString s=QString::fromUtf8("Herstellerinformation:");
    QString sErr=QString::fromUtf8("Es wurde kein Hersteller ausgewählt!");
    print_list(s,sErr,ui->listWidgetMakerData,ui->tableWidgetMaker);
    return true;
}

bool MainWindow::menu_print_maker_overview(void)
{
    QString s=QString::fromUtf8("Herstellerübersicht:");
    QString sErr=QString::fromUtf8("Die Tabelle mit den Herstellern ist leer!");
    print_table(s,sErr,ui->tableWidgetMaker);
    return true;
}

bool MainWindow::menu_print_dealer_selection(void)
{
    QString s=QString::fromUtf8("Händlerinformation:");
    QString sErr=QString::fromUtf8("Es wurde kein Händler ausgewählt!");
    print_list(s,sErr,ui->listWidgetDealerData,ui->tableWidgetDealer);
    return true;
}

bool MainWindow::menu_print_dealer_overview(void)
{
    QString s=QString::fromUtf8("Händlerübersicht:");
    QString sErr=QString::fromUtf8("Die Tabelle mit den Händlern ist leer!");
    print_table(s,sErr,ui->tableWidgetDealer);
    return true;
}

bool MainWindow::menu_print_customer_selection(void)
{
    QString s=QString::fromUtf8("Kundeninformation:");
    QString sErr=QString::fromUtf8("Es wurde kein Kunde ausgewählt!");
    print_list(s,sErr,ui->listWidgetCustomerData,ui->tableWidgetCustomer);
    return true;
}

bool MainWindow::menu_print_customer_overview(void)
{
    QString s=QString::fromUtf8("Kundenübersicht:");
    QString sErr=QString::fromUtf8("Die Tabelle mit den Kunden ist leer!");
    print_table(s,sErr,ui->tableWidgetCustomer);
    return true;
}

bool MainWindow::menu_print_article_overview(void)
{
    bool bAllColumns=true;
    QString s=QString::fromUtf8("Artikelübersicht:");
    QString sErr=QString::fromUtf8("Die Tabelle mit den Artrikeln ist leer!");
    print_table(s,sErr,ui->tableWidgetArticle,&bAllColumns);
    return true;
}

bool MainWindow::menu_print_inventory_overview(void)
{
    bool bAllColumns=true;
    QString s=QString::fromUtf8("Lagerbestandsübersicht:");
    QString sErr=QString::fromUtf8("Die Tabelle mit dem Lagerbestand ist leer!");
    print_table(s,sErr,ui->tableWidgetInventory,&bAllColumns);
    return true;
}

bool MainWindow::menu_print_ordering_overview(void)
{
    QString s=QString::fromUtf8("Bestellungen(Übersicht):");
    QString sErr=QString::fromUtf8("Die Tabelle mit den Bestellungen ist leer!");
    print_table(s,sErr,ui->tableWidgetOrdering);
    return true;
}

bool MainWindow::menu_print_ordering_selection(void)
{
    QString s=QString::fromUtf8("Bestellung(Information):");
    QString sErr=QString::fromUtf8("Es wurde keine Bestellung ausgewählt!");
    print_table_and_list(s,sErr,ui->tableWidgetOrderingWareslist,ui->tableWidgetOrdering);
    return true;
}

bool MainWindow::menu_print_trade_overview(void)
{
    //date from/to string
    QString sDates;
    QList<QListWidgetItem*> ls=ui->listWidgetTradeMaskDate->selectedItems();
    if(ls.count()>0)
        sDates=ls[0]->text();

    bool bAllColumns=true;
    QString s=QString::fromUtf8("Warenein-/ausgänge (%1):").arg(sDates);
    QString sErr=QString::fromUtf8("Die Tabelle mit den Warenein-/ausgängen ist leer!");
    print_table(s,sErr,ui->tableWidgetTrade,&bAllColumns);
    return true;
}

bool MainWindow::menu_print_trade_selection(void)
{
    bool bAllColumns=true;
    QListWidget * pList=NULL;
    if(!trade_info_hide_status())
        pList=ui->listWidgetTradeInfo;
    QString s=QString::fromUtf8("Warenein-/ausgang(Information):");
    QString sErr=QString::fromUtf8("Es wurde kein Warenein-/ausgang ausgewählt!");
    print_table_and_list(s,sErr,ui->tableWidgetTradeWareslist,ui->tableWidgetTrade,pList,&bAllColumns);
    return true;
}

bool MainWindow::menu_print_waregroup(void)
{
    QString s=QString::fromUtf8("Warengruppen(Übersicht):");
    QString sErr=QString::fromUtf8("Die Warengruppenübersicht ist leer!");
    print_tree(s,sErr,ui->treeWidgetWaregroup);
    return true;
}

bool MainWindow::menu_barcode_overview(void)
{
    QString s=QString::fromUtf8("Artikelübersicht(Barcode):");

    m_print_job.m_memory.clear();
    m_print_job.set_type(PRINT_JOB_BARCODE_OVERVIEW);
    print_open_preview(s);
    return true;
}

//table settings
bool MainWindow::menu_table_setting_inventory(void)
{
    bool bReturn=false;
    CTableColumnsData tcd=m_widgets.m_tcInventory;
    CTableColumnsData tcdNew=tcd;
    QList<QString> lsHeaderNames;
    CInputDialogTableSetting dlg;
    dlg.setWindowTitle(QString::fromUtf8("Tabelleneinstellung - Lagerbestand - Übersicht"));
    dlg.set_data(tcd);

    //dlg
    if(dlg.exec())//ok
    {
        dlg.get_data(tcdNew,lsHeaderNames);
        if(tcd!=tcdNew)
        {//settings updated?
            m_widgets.m_tcInventory=tcdNew;//set new columnsdata
            if(m_widgets.update_table_by_tablecolumnsdata(tcd,tcdNew,lsHeaderNames))//update table settings
            {
                inventory_mask_set();//update table
                bReturn=true;
            }
        }
    }
    lsHeaderNames.clear();
    return bReturn;
}

bool MainWindow::menu_table_setting_article(void)
{
    bool bReturn=false;
    CTableColumnsData tcd=m_widgets.m_tcArticle;
    CTableColumnsData tcdNew=tcd;
    QList<QString> lsHeaderNames;
    CInputDialogTableSetting dlg;
    dlg.setWindowTitle(QString::fromUtf8("Tabelleneinstellung - Artikel - Übersicht"));
    dlg.set_data(tcd);

    //dlg
    if(dlg.exec())//ok
    {
        dlg.get_data(tcdNew,lsHeaderNames);
        if(tcd!=tcdNew)
        {//settings updated?
            m_widgets.m_tcArticle=tcdNew;//set new columnsdata
            if(m_widgets.update_table_by_tablecolumnsdata(tcd,tcdNew,lsHeaderNames))//update table settings
            {
                article_mask_set();//update table
                bReturn=true;
            }
        }
    }
    lsHeaderNames.clear();
    return bReturn;
}

bool MainWindow::menu_export_inventory(void)
{
    CExportCSV exportCSV;
    return exportCSV.write_data_inventory(this,ui->tableWidgetInventory,m_db,m_widgets,QString::fromUtf8("Lagerbestände"),QString::fromUtf8(""));
}

bool MainWindow::menu_export_trade(void)
{
    QString si=QString::fromUtf8("");

    //dates
    if(ui->listWidgetTradeMaskDate->currentRow()>=0 && ui->listWidgetTradeMaskDate->currentRow()<ui->listWidgetTradeMaskDate->count())//date selected
        si=ui->listWidgetTradeMaskDate->item(ui->listWidgetTradeMaskDate->currentRow())->text();

    CExportCSV exportCSV;
    return exportCSV.write_data_trade(this,ui->tableWidgetTrade,m_db,m_widgets,QString::fromUtf8("Warengänge"),QString::fromUtf8(""));
}

bool MainWindow::menu_export_ordering(void)
{
    CExportCSV exportCSV;
    return exportCSV.write_data_ordering(this,ui->tableWidgetOrdering,m_db,m_widgets,QString::fromUtf8("Bestellungen"),QString::fromUtf8(""));
}

bool MainWindow::menu_export_article(void)
{
    CExportCSV exportCSV;
    return exportCSV.write_data_article(this,ui->tableWidgetArticle,m_db,QString::fromUtf8("Artikel"),QString::fromUtf8(""));
}

bool MainWindow::menu_export_waregroup(void)
{
    CExportCSV exportCSV;
    return exportCSV.write_data_tree(this,ui->treeWidgetWaregroup,QString::fromUtf8("Warengruppen"),QString::fromUtf8(""));
}

bool MainWindow::menu_export_maker(void)
{
    CExportCSV exportCSV;
    return exportCSV.write_data_maker(this,ui->tableWidgetMaker,m_db,QString::fromUtf8("Hersteller"),QString::fromUtf8(""));
}

bool MainWindow::menu_export_dealer(void)
{
    CExportCSV exportCSV;
    return exportCSV.write_data_dealer(this,ui->tableWidgetDealer,m_db,QString::fromUtf8("Händler"),QString::fromUtf8(""));
}

bool MainWindow::menu_export_customer(void)
{
    CExportCSV exportCSV;
    return exportCSV.write_data_customer(this,ui->tableWidgetCustomer,m_db,QString::fromUtf8("Kunden"),QString::fromUtf8(""));
}

bool MainWindow::menu_export_logbook(void)
{
    bool b=false;
    CExportCSV exportCSV;
    QString s,sFile;

    //get mask datetext
    QList<QListWidgetItem *> ls=ui->listWidgetLogbookMaskDate->selectedItems();
    if(ls.count()>0)
    {
        s=ls[0]->text();
        s.resize(10);//copy only 10 char (dd.mm.yyyy)
        sFile=QString::fromUtf8("logbuch-%1").arg(s);
        exportCSV.write_data_list(this,ui->listWidgetLogbook,sFile,QString::fromUtf8(""));
        b=true;
    }
    //-
    ls.clear();
    return b;
}

bool MainWindow::menu_import_from_csv(void)
{
    int type=0;
    QString s;
    CLogbook lg;
    CInputDialogImport dlg;
    dlg.set_thread(&m_thread);
    if(dlg.exec())
    {
        //update table
        switch(dlg.get_data_type())
        {
            case 0://article
                article_mask_set();
                maker_mask_set();
                waregroup_update_tree();
                inventory_mask_set();
                type=LC_ACTION_UPDATE_INV_ORD_TRADE_ARTICLE_MK_WG;
                break;
            case 1://dealer
                dealer_mask_set();
                type=LC_ACTION_UPDATE_DEALER;
                break;
            case 2://maker
                maker_mask_set();
                type=LC_ACTION_UPDATE_MAKER;
                break;
            case 3://customer
                customer_mask_set();
                type=LC_ACTION_UPDATE_CUSTOMER;
                break;
            case 4://waregroup
                waregroup_update_tree();
                type=LC_ACTION_UPDATE_WAREGROUP;
                break;
            default:
                break;
        }

        //logbook
        lg.set_type(LOGBOOK_TYPE_OTHER);
        lg.set_date_time(QDateTime::currentDateTime());
        s=QString::fromUtf8("Daten aus der Datei \"%1\" in die Datenbank importiert").arg(dlg.get_source_file());
        lg.set_text(s);
        logbook_insert(lg);

        //set last change in db -> another clients update table
        s=m_LastDbChange.create_last_change_string(type,-1);// create string
        m_LastDbChange.write_last_change(s);//write in db
    }
    return true;
}


//print ---------------------------------------------------------------------------------------------------------------------------------
bool MainWindow::print_open_preview(QString sFirstRow)
{
    bool b2,b=true;
    QStringList ls;
    QList<int> lsInt;
    qreal margin_left=15,margin_right=10,margin_top=10,margin_bottom=10;
    CSettings settings;
    QPrinter printer(QPrinter::PrinterResolution);
    QString sDocName,sOrientation,sMargins,sOriUpdate,sMarUpdate,sPrinter,sPriUpdate;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("Fehler"));
    msg.setText(QString::fromUtf8("Dem Betriebssystem ist kein Drucker bekannt."));


    m_print_job.m_sFirstRow=sFirstRow;

    //doc-name
    ls=sFirstRow.split(QString(" "));
    if(ls.count()>0)
        sDocName=QString("LaVa 2 - %1").arg(ls[0]);
    else
        sDocName=QString("LaVa 2");
    sDocName.replace(QString(":"),QString(""));
    ls.clear();

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

    //---
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

        if(b)
        {
            //orientation
            if(sOrientation==QString::fromUtf8("0"))
                printer.setOrientation(QPrinter::Portrait);
            else
                printer.setOrientation(QPrinter::Landscape);

            //margins
            printer.setPageMargins(margin_left ,margin_top ,margin_right ,margin_bottom ,QPrinter::Millimeter);

            //show
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
    }
    //-
    lsInt.clear();
    return b;
}

bool MainWindow::print(QPrinter * pPrinter)
{
    bool b=false;
    int type=-1;
    if(pPrinter!=NULL)
    {
        b=true;
        m_print_job.m_pPrinter=pPrinter;
        type=m_print_job.get_type();
        //-
        if(type==PRINT_JOB_LIST)//print list widget
            b=m_print_job.print_list();
        if(type==PRINT_JOB_TABLE)//print table widget
            b=m_print_job.print_table();
        if(type==PRINT_JOB_TREE)//print tree
            b=m_print_job.print_tree();
        if(type==PRINT_JOB_TABLE_AND_LIST)//print table + list
            b=m_print_job.print_table_and_list();
        if(type==PRINT_JOB_BARCODE_OVERVIEW)//barcode overview
            b=m_print_job.print_barcode_overview();
    }
    return b;
}

bool MainWindow::print_table(QString sPageTitle, QString sError, QTableWidget * pTable, bool * pAllColumns)
{
    if(pTable==NULL)
        return false;
    //-
    bool ok;
    QString s;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("!"));
    msg.setText(sError);
    //-
    if(pTable->rowCount()<=0)
        msg.exec();// error nothing in table
    else
    {
        //document title change
        s=QInputDialog::getText(this,QString::fromUtf8("Überschrift ändern"),QString::fromUtf8("Überschrift:\t\t"),QLineEdit::Normal,sPageTitle,&ok);
        if(ok)
        {
            sPageTitle=s;
            m_print_job.m_memory.clear();
            m_print_job.set_type(PRINT_JOB_TABLE);
            m_print_job.m_memory.set_tablewidget(pTable);
            m_print_job.m_memory.set_bool(pAllColumns);
            print_open_preview(sPageTitle);
        }
    }
    return true;
}

bool MainWindow::print_list(QString sPageTitle, QString sError, QListWidget * pList, QTableWidget * pTable)
{
    if(pList==NULL || pTable==NULL)
        return false;
    //-
    bool ok;
    QString s;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("!"));
    msg.setText(sError);
    //-
    QList<QTableWidgetItem*> ls=pTable->selectedItems();
    if(ls.count()<=0)
        msg.exec();// error nothing selected
    else
    {
        //document title change
        s=QInputDialog::getText(this,QString::fromUtf8("Überschrift ändern"),QString::fromUtf8("Überschrift:\t\t"),QLineEdit::Normal,sPageTitle,&ok);
        if(ok)
        {
            sPageTitle=s;
            m_print_job.m_memory.clear();
            m_print_job.set_type(PRINT_JOB_LIST);
            m_print_job.m_memory.set_listwidget(pList);
            print_open_preview(sPageTitle);
        }
    }
        ls.clear();
    return true;
}

bool MainWindow::print_tree(QString sPageTitle, QString sError, QTreeWidget * pTree)
{
    if(pTree==NULL)
        return false;
    //-
    bool ok;
    QString s;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("!"));
    msg.setText(sError);
    //-
    if(pTree->topLevelItemCount()<=0)
        msg.exec();// error nothing items at tree
    else
    {
        //document title change
        s=QInputDialog::getText(this,QString::fromUtf8("Überschrift ändern"),QString::fromUtf8("Überschrift:\t\t"),QLineEdit::Normal,sPageTitle,&ok);
        if(ok)
        {
            sPageTitle=s;
            m_print_job.m_memory.clear();
            m_print_job.set_type(PRINT_JOB_TREE);
            m_print_job.m_memory.set_treewidget(pTree);
            print_open_preview(sPageTitle);
        }
    }
    return true;
}

bool MainWindow::print_table_and_list(QString sPageTitle, QString sError, QTableWidget * pTableData, QTableWidget * pTableInfo, QListWidget * pList, bool * pAllColumns)
{
    if(pTableData==NULL || pTableInfo==NULL)
        return false;
    //-
    bool ok;
    QString s;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("!"));
    msg.setText(sError);
    //-
    if(pTableData->rowCount()<=0)
        msg.exec();// error nothing in table
    else
    {
        //document title change
        s=QInputDialog::getText(this,QString::fromUtf8("Überschrift ändern"),QString::fromUtf8("Überschrift:\t\t"),QLineEdit::Normal,sPageTitle,&ok);
        if(ok)
        {
            sPageTitle=s;
            m_print_job.m_memory.clear();
            m_print_job.set_type(PRINT_JOB_TABLE_AND_LIST);
            m_print_job.m_memory.set_tablewidget(pTableData);
            m_print_job.m_memory.set_tablewidget2(pTableInfo);
            m_print_job.m_memory.set_bool(pAllColumns);
            m_print_job.m_memory.set_listwidget(pList);
            print_open_preview(sPageTitle);
        }
    }
    return true;
}
