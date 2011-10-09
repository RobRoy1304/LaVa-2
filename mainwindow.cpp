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

#include "mainwindow.h"
#include "ui_mainwindow.h"

//---basic-------------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //-register-
    qRegisterMetaType<Qt::SortOrder>("Qt::SortOrder");
    qRegisterMetaType<QItemSelection>("QItemSelection");

    //basic
    ui->setupUi(this);

    //hide statusbar
    ui->statusBar->hide();

    //database path
    m_sDbPath=QDir::homePath()+QString("/lava2/");

    //hide columns from tree widget
    ui->treeWidgetWaregroup->hideColumn(2);//id column
    ui->treeWidgetWaregroup->hideColumn(3);//parent_id column

    //year's combo box's
    QString s;
    QDate dt=QDate::currentDate();
    for(int year=dt.year();year>=2009;year--)//fill year combo trade+logbook
    {
        s=QString("%1").arg(year);
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
    m_pContextMenuDefault=new QMenu(QString("context_default"),this);
    if(m_pContextMenuDefault!=NULL)
    {
        m_pContextMenuDefault->addAction(QString("neu"));
        m_pContextMenuDefault->addAction(QString("bearbeiten"));
        m_pContextMenuDefault->addAction(QString("löschen"));
    }
    m_pContextMenuTrade=new QMenu(QString("context_trade"),this);
    if(m_pContextMenuTrade!=NULL)
    {
        m_pContextMenuTrade->addAction(QString("Wareneingang"));
        m_pContextMenuTrade->addAction(QString("Warenausgang"));
        m_pContextMenuTrade->addAction(QString("Wareneingang(Bestellung)"));
        m_pContextMenuTrade->addAction(QString("Warenausgang(Kunde)"));
        m_pContextMenuTrade->addAction(QString("stornieren"));
    }
    m_pContextMenuArticle=new QMenu(QString("context_article"),this);
    if(m_pContextMenuArticle!=NULL)
    {
        m_pContextMenuArticle->addAction(QString("neu"));
        m_pContextMenuArticle->addAction(QString("bearbeiten"));
        m_pContextMenuArticle->addAction(QString("kopieren"));
        m_pContextMenuArticle->addAction(QString("löschen"));
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
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tab_switched()));

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

    //inventory
    connect(ui->pushButtonMaskSetInventory,SIGNAL(clicked()),this,SLOT(inventory_mask_set()));
    connect(ui->checkBoxAutoSearchInventory,SIGNAL(clicked()),this,SLOT(inventory_checkbox_auto_clicked()));
    connect(ui->lineEditInventoryMask,SIGNAL(textChanged(QString)),this,SLOT(inventory_mask_edit()));
    connect(ui->comboBoxMaskInventory,SIGNAL(currentIndexChanged(QString)),this,SLOT(inventory_mask_edit()));
    connect(ui->tableWidgetInventory,SIGNAL(itemSelectionChanged()),this,SLOT(inventory_widgetitem_clicked()));

    //logbook
    connect(ui->comboBoxLogbookMaskYear,SIGNAL(currentIndexChanged(QString)),this,SLOT(logbook_date_mask_year_changed()));
    connect(ui->comboBoxMaskLogbook,SIGNAL(currentIndexChanged(QString)),this,SLOT(logbook_combo_mask_changed()));
    connect(ui->listWidgetLogbookMaskDate,SIGNAL(itemSelectionChanged()),this,SLOT(logbook_mask_set()));
    connect(ui->pushButtonLogbookChangeCount,SIGNAL(clicked()),this,SLOT(logbook_change_count_of_item()));
    connect(ui->pushButtonLogbookClear,SIGNAL(clicked()),this,SLOT(logbook_clear()));

    //menu
    connect(ui->menuDbBackupCreate,SIGNAL(triggered()),this,SLOT(menu_db_backup_create()));
    connect(ui->menuDbBackupLoad,SIGNAL(triggered()),this,SLOT(menu_db_backup_load()));
    connect(ui->menuDbImportFromLava1,SIGNAL(triggered()),this,SLOT(menu_db_import_from_lava1()));
    connect(ui->menuDbClear,SIGNAL(triggered()),this,SLOT(menu_db_clear()));
    connect(ui->menuQuit,SIGNAL(triggered()),this,SLOT(menu_tool_close()));
    connect(ui->menuAbout,SIGNAL(triggered()),this,SLOT(menu_about()));
    connect(ui->menuHelp,SIGNAL(triggered()),this,SLOT(menu_help()));
    connect(ui->menuListBalanceList,SIGNAL(triggered()),this,SLOT(menu_balance_list()));
    connect(ui->menuListUnderWarnLimit,SIGNAL(triggered()),this,SLOT(menu_article_under_warnlimit()));
    connect(ui->menuListInventoryDate,SIGNAL(triggered()),this,SLOT(menu_inventorys_on_date()));
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
}

MainWindow::~MainWindow()
{
    //check article inv under warning limit
    QMessageBox msg(QMessageBox::Warning,"","");
    msg.setWindowTitle(QString("Achtung!"));
    QString s;
    int count=inventory_get_count_of_arctile_under_limit();
    if(count>0)
    {
        s=QString("Es gibt %1 Artikel deren Lagerbestand unter dem Warnungslimit liegt.").arg(count);
        msg.setText(s);
        msg.exec();
    }

    //write(save) settings
    settings(true);

    //set last session in db-
    QDateTime dtTi=QDateTime().currentDateTime();
    if(m_db.is_db_connect())
        m_db.set_last_session(dtTi);

    //context menu's
    if(m_pContextMenuDefault!=NULL)
        delete m_pContextMenuDefault;
    if(m_pContextMenuTrade!=NULL)
        delete m_pContextMenuTrade;
    if(m_pContextMenuArticle!=NULL)
        delete m_pContextMenuArticle;

    //clear
    killTimer(m_iTimerId);
    m_thread.stop_thread();
    m_db.close_db();
    delete ui;
}

void MainWindow::resizeEvent  ( QResizeEvent * event )
{
    widgets_position(event->size());
    QMainWindow::resizeEvent(event);
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
    fill_all_table();//fill tables
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

bool MainWindow::open_db(void)
{
    QDateTime dtTiLastSession;
    QDateTime dtTi=QDateTime().currentDateTime();
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(tr("Fehler!"));
    QString s,s2,sFile=m_sDbPath+QString("lava.sqlite");
    bool b=false;
    //-
    if(!QFile::exists(sFile))
    {
        s=QString("Die Datenbank-Datei \"lava.sqlite\" befindet sich nicht in dem Ordner \"%1\"!").arg(m_sDbPath);
        msg.setText(s);
        msg.exec();
    }
    else
    {
        if(!m_db.open_db(sFile,QString("localhost")))//open db?
        {
            s=m_db.last_error();
            s2=QString("Die Datenbank-Datei \"lava.sqlite\" konnte nicht geöffnet werden!\nerror:%1").arg(s);
            msg.setText(s2);
            msg.exec();
        }
        else
        {
            if(!m_db.check_database_file(true))
            {
                s2=QString("Die Datenbank-Datei \"lava.sqlite\" ist beschädigt oder nicht kompatibel.");
                msg.setText(s2);
                msg.exec();
            }
            else
            {
                b=true;
                //check last session(time) and current
                dtTiLastSession=m_db.get_last_session();
                if(dtTiLastSession>QDateTime(QDate(0,0,0),QTime(0,0,0)))//set?
                {
                    if(dtTiLastSession.date()>dtTi.date())
                    {
                        s2=QString("Bitte prüfen Sie Ihre Systemzeit-/datumseinstellungen Ihres Betriebssystems. Das Datum Ihrer letzten Sitzung "
                                   "ist grösser als das aktuelle Datum. Falsche Einstellungen können die chronologische Reihenfolge der Artikelmengen"
                                   " beim Warenausgang beeinflussen.");
                        msg.setText(s2);
                        msg.exec();
                    }
                }
            }
        }
    }
    //-
    if(!b)
        m_db.close_db();
    //-
    return b;
}

bool MainWindow::fill_all_table(void)
{
    //waregroup
    waregroup_update_tree();

    //maker
    maker_mask_set();

    //dealer
    dealer_mask_set();

    //customer
    customer_mask_set();

    //article
    article_mask_set();

    //ordering
    ordering_mask_set();

    //trade
    QDate dt=QDate().currentDate();
    if(dt.year()>=2009)
        trade_update_datelist(dt,dt.year());
    trade_mask_set(false);

    //inventory
    inventory_mask_set();

    //logbook
    if(dt.year()>=2009)
        logbook_update_datelist(dt,dt.year());
    logbook_mask_set();
    //-
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
    lsSType.push_back(QString("TAB_ORDER"));
    //-
    lsSType.push_back(QString("INVENTORY_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString("SORT_ORDER_INVENTORY"));
    lsSType.push_back(QString("AUTO_CHECKBOX_MAIN_INV"));
    lsSType.push_back(QString("MASK_SELECTION_INVENTORY"));
    //-
    lsSType.push_back(QString("TRADE_TABLE_COLUMNS_WIDTHS_OVIEW"));
    lsSType.push_back(QString("SORT_ORDER_TRADE_OVIEW"));
    lsSType.push_back(QString("TRADE_TABLE_COLUMNS_WIDTHS_WARES"));
    lsSType.push_back(QString("SORT_ORDER_TRADE_WARES"));
    lsSType.push_back(QString("AUTO_CHECKBOX_MAIN_TRADE"));
    lsSType.push_back(QString("MASK_SELECTION_TRADE"));
    lsSType.push_back(QString("MASK_SELECTION_TRADE_PERIOD"));
    //-
    lsSType.push_back(QString("ORDERING_TABLE_COLUMNS_WIDTHS_OVIEW"));
    lsSType.push_back(QString("SORT_ORDER_ORDERING_OVIEW"));
    lsSType.push_back(QString("ORDERING_TABLE_COLUMNS_WIDTHS_WARES"));
    lsSType.push_back(QString("SORT_ORDER_ORDERING_WARES"));
    lsSType.push_back(QString("AUTO_CHECKBOX_MAIN_ORDERING"));
    lsSType.push_back(QString("MASK_SELECTION_ORDERING"));
    //-
    lsSType.push_back(QString("ARTICLE_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString("SORT_ORDER_ARTICLE"));
    lsSType.push_back(QString("AUTO_CHECKBOX_MAIN_ARTICLE"));
    lsSType.push_back(QString("MASK_SELECTION_ARTICLE"));
    //-
    lsSType.push_back(QString("WAREGROUP_TREE_COLUMNS_WIDTHS"));
    //-
    lsSType.push_back(QString("MAKER_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString("SORT_ORDER_MAKER"));
    lsSType.push_back(QString("AUTO_CHECKBOX_MAIN_MAKER"));
    lsSType.push_back(QString("MASK_SELECTION_MAKER"));
    //-
    lsSType.push_back(QString("DEALER_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString("SORT_ORDER_DEALER"));
    lsSType.push_back(QString("AUTO_CHECKBOX_MAIN_DEALER"));
    lsSType.push_back(QString("MASK_SELECTION_DEALER"));
    //-
    lsSType.push_back(QString("CUSTOMER_TABLE_COLUMNS_WIDTHS"));
    lsSType.push_back(QString("SORT_ORDER_CUSTOMER"));
    lsSType.push_back(QString("AUTO_CHECKBOX_MAIN_CUSTOMER"));
    lsSType.push_back(QString("MASK_SELECTION_CUSTOMER"));
    //-
    lsSType.push_back(QString("MASK_SELECTION_LOGBOOK"));
    lsSType.push_back(QString("LOGBOOK_COUNT"));

    //load
    int count;
    QString s;
    bool b2,b=settings.load(lsSType,lsSValues);
    if(b && lsSValues.count()==lsSType.count())
    {
        if(!bUpdate)//set
        {
            //--tab-order--
            settings.set_tab_order(ui->tabWidget,lsSValues[0]);
            ui->tabWidget->setCurrentWidget(ui->tabInventory);

            //--inventory--
            settings.set_table_width(ui->tableWidgetInventory,lsSValues[1],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetInventory,lsSValues[2],0);
            settings.set_checkbox(ui->checkBoxAutoSearchInventory,lsSValues[3],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskInventory,lsSValues[4],0);//mask selection

            //--trade--
            settings.set_table_width(ui->tableWidgetTrade,lsSValues[5],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetTrade,lsSValues[6],3);
            settings.set_table_width(ui->tableWidgetTradeWareslist,lsSValues[7],200);//table 2 width+sort
            settings.set_table_sort(ui->tableWidgetTradeWareslist,lsSValues[8],1);
            settings.set_checkbox(ui->checkBoxAutoSearchTrade,lsSValues[9],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskTrade,lsSValues[10],0);//mask selection
            settings.set_combobox(ui->comboBoxTradeMaskDate,lsSValues[11],0);//mask 2 selection

            //--ordering--
            settings.set_table_width(ui->tableWidgetOrdering,lsSValues[12],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetOrdering,lsSValues[13],0);
            settings.set_table_width(ui->tableWidgetOrderingWareslist,lsSValues[14],200);//table 2 width+sort
            settings.set_table_sort(ui->tableWidgetOrderingWareslist,lsSValues[15],1);
            settings.set_checkbox(ui->checkBoxAutoSearchOrdering,lsSValues[16],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskOrdering,lsSValues[17],0);//mask selection

            //--article--
            settings.set_table_width(ui->tableWidgetArticle,lsSValues[18],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetArticle,lsSValues[19],0);
            settings.set_checkbox(ui->checkBoxAutoSearchArticle,lsSValues[20],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskArticle,lsSValues[21],0);//mask selection

            //--waregroup--
            settings.set_tree_width(ui->treeWidgetWaregroup,lsSValues[22],500);//tree width

            //--maker--
            settings.set_table_width(ui->tableWidgetMaker,lsSValues[23],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetMaker,lsSValues[24],0);
            settings.set_checkbox(ui->checkBoxAutoSearchMaker,lsSValues[25],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskMaker,lsSValues[26],0);//mask selection

            //--dealer--
            settings.set_table_width(ui->tableWidgetDealer,lsSValues[27],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetDealer,lsSValues[28],0);
            settings.set_checkbox(ui->checkBoxAutoSearchDealer,lsSValues[29],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskDealer,lsSValues[30],0);//mask selection

            //--customer--
            settings.set_table_width(ui->tableWidgetCustomer,lsSValues[31],200);//table width+sort
            settings.set_table_sort(ui->tableWidgetCustomer,lsSValues[32],1);
            settings.set_checkbox(ui->checkBoxAutoSearchCustomer,lsSValues[33],false);//auto checkbox
            settings.set_combobox(ui->comboBoxMaskCustomer,lsSValues[34],0);//mask selection

            //--logbook--
            settings.set_combobox(ui->comboBoxMaskLogbook,lsSValues[35],0);//mask selection
            //count
            count=lsSValues[36].toInt(&b2,10);
            if(!b2)
                count=100;
            else
            {
                if(count<0 || count>9999)
                    count=100;
            }
            s=QString("%1").arg(count);
            ui->lineEditCountItemLog->setText(s);
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
            if(settings.get_table_width(ui->tableWidgetInventory,lsSValues[1]))//table width+sort
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

            //--trade--
            if(settings.get_table_width(ui->tableWidgetTrade,lsSValues[5]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[5]);
                lsSUpdateValue.push_back(lsSValues[5]);
            }
            if(settings.get_table_sort(ui->tableWidgetTrade,lsSValues[6]))
            {
                lsSUpdateType.push_back(lsSType[6]);
                lsSUpdateValue.push_back(lsSValues[6]);
            }
            if(settings.get_table_width(ui->tableWidgetTradeWareslist,lsSValues[7]))//table 2 width+sort
            {
                lsSUpdateType.push_back(lsSType[7]);
                lsSUpdateValue.push_back(lsSValues[7]);
            }
            if(settings.get_table_sort(ui->tableWidgetTradeWareslist,lsSValues[8]))
            {
                lsSUpdateType.push_back(lsSType[8]);
                lsSUpdateValue.push_back(lsSValues[8]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchTrade,lsSValues[9]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[9]);
                lsSUpdateValue.push_back(lsSValues[9]);
            }
            if(settings.get_combobox(ui->comboBoxMaskTrade,lsSValues[10]))//combobox
            {
                lsSUpdateType.push_back(lsSType[10]);
                lsSUpdateValue.push_back(lsSValues[10]);
            }
            if(settings.get_combobox(ui->comboBoxTradeMaskDate,lsSValues[11]))//combobox 2
            {
                lsSUpdateType.push_back(lsSType[11]);
                lsSUpdateValue.push_back(lsSValues[11]);
            }

            //--ordering--
            if(settings.get_table_width(ui->tableWidgetOrdering,lsSValues[12]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[12]);
                lsSUpdateValue.push_back(lsSValues[12]);
            }
            if(settings.get_table_sort(ui->tableWidgetOrdering,lsSValues[13]))
            {
                lsSUpdateType.push_back(lsSType[13]);
                lsSUpdateValue.push_back(lsSValues[13]);
            }
            if(settings.get_table_width(ui->tableWidgetOrderingWareslist,lsSValues[14]))//table 2 width+sort
            {
                lsSUpdateType.push_back(lsSType[14]);
                lsSUpdateValue.push_back(lsSValues[14]);
            }
            if(settings.get_table_sort(ui->tableWidgetOrderingWareslist,lsSValues[15]))
            {
                lsSUpdateType.push_back(lsSType[15]);
                lsSUpdateValue.push_back(lsSValues[15]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchOrdering,lsSValues[16]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[16]);
                lsSUpdateValue.push_back(lsSValues[16]);
            }
            if(settings.get_combobox(ui->comboBoxMaskOrdering,lsSValues[17]))//combobox
            {
                lsSUpdateType.push_back(lsSType[17]);
                lsSUpdateValue.push_back(lsSValues[17]);
            }

            //--article--
            if(settings.get_table_width(ui->tableWidgetArticle,lsSValues[18]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[18]);
                lsSUpdateValue.push_back(lsSValues[18]);
            }
            if(settings.get_table_sort(ui->tableWidgetArticle,lsSValues[19]))
            {
                lsSUpdateType.push_back(lsSType[19]);
                lsSUpdateValue.push_back(lsSValues[19]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchArticle,lsSValues[20]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[20]);
                lsSUpdateValue.push_back(lsSValues[20]);
            }
            if(settings.get_combobox(ui->comboBoxMaskArticle,lsSValues[21]))//combobox
            {
                lsSUpdateType.push_back(lsSType[21]);
                lsSUpdateValue.push_back(lsSValues[21]);
            }

            //--waregroup--
            if(settings.get_tree_width(ui->treeWidgetWaregroup,lsSValues[22]))//combobox
            {
                lsSUpdateType.push_back(lsSType[22]);
                lsSUpdateValue.push_back(lsSValues[22]);
            }

            //--maker--
            if(settings.get_table_width(ui->tableWidgetMaker,lsSValues[23]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[23]);
                lsSUpdateValue.push_back(lsSValues[23]);
            }
            if(settings.get_table_sort(ui->tableWidgetMaker,lsSValues[24]))
            {
                lsSUpdateType.push_back(lsSType[24]);
                lsSUpdateValue.push_back(lsSValues[24]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchMaker,lsSValues[25]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[25]);
                lsSUpdateValue.push_back(lsSValues[25]);
            }
            if(settings.get_combobox(ui->comboBoxMaskMaker,lsSValues[26]))//combobox
            {
                lsSUpdateType.push_back(lsSType[26]);
                lsSUpdateValue.push_back(lsSValues[26]);
            }

            //--dealer--
            if(settings.get_table_width(ui->tableWidgetDealer,lsSValues[27]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[27]);
                lsSUpdateValue.push_back(lsSValues[27]);
            }
            if(settings.get_table_sort(ui->tableWidgetDealer,lsSValues[28]))
            {
                lsSUpdateType.push_back(lsSType[28]);
                lsSUpdateValue.push_back(lsSValues[28]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchDealer,lsSValues[29]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[29]);
                lsSUpdateValue.push_back(lsSValues[29]);
            }
            if(settings.get_combobox(ui->comboBoxMaskDealer,lsSValues[30]))//combobox
            {
                lsSUpdateType.push_back(lsSType[30]);
                lsSUpdateValue.push_back(lsSValues[30]);
            }

            //--customer--
            if(settings.get_table_width(ui->tableWidgetCustomer,lsSValues[31]))//table width+sort
            {
                lsSUpdateType.push_back(lsSType[31]);
                lsSUpdateValue.push_back(lsSValues[31]);
            }
            if(settings.get_table_sort(ui->tableWidgetCustomer,lsSValues[32]))
            {
                lsSUpdateType.push_back(lsSType[32]);
                lsSUpdateValue.push_back(lsSValues[32]);
            }
            if(settings.get_checkbox(ui->checkBoxAutoSearchCustomer,lsSValues[33]))//checkbox
            {
                lsSUpdateType.push_back(lsSType[33]);
                lsSUpdateValue.push_back(lsSValues[33]);
            }
            if(settings.get_combobox(ui->comboBoxMaskCustomer,lsSValues[34]))//combobox
            {
                lsSUpdateType.push_back(lsSType[34]);
                lsSUpdateValue.push_back(lsSValues[34]);
            }

            //--logbook--
            if(settings.get_combobox(ui->comboBoxMaskLogbook,lsSValues[35]))//combobox
            {
                lsSUpdateType.push_back(lsSType[35]);
                lsSUpdateValue.push_back(lsSValues[35]);
            }
            //count
            s=ui->lineEditCountItemLog->text();
            if(s!=lsSValues[36])//update?
            {
                lsSUpdateType.push_back(lsSType[36]);
                lsSUpdateValue.push_back(lsSValues[36]);
            }


            //----write----
            if(lsSUpdateType.count()>0 && lsSUpdateType.count()==lsSUpdateValue.count())
                b=settings.write(lsSUpdateType,lsSUpdateValue);

            //remove flag 'programm_running'
            settings.remove_line(QString("PROGRAM_RUNNING"));
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
        msgAgreement.setWindowTitle(QString("Nutzungsbedingungen:"));
        s=QString("Dieses Programm(LaVa 2) steht unter der open-source-Lizenz GNU-GPL v3. Es entstehen für die Nutzung "
                  "keine Kosten. Der Autor entbindet sich von jeglichen Haftungsansprüchen, gleich welcher Art. "
                  "Das Programm befindet sich momentan noch im Beta-Status, und ist somit noch nicht für "
                  "den produktiven Einsatz gedacht.\n\nSind Sie mit diesen Bedingungen einverstanden?");
        msgAgreement.setText(s);
        msgAgreement.addButton(QString("Ja"),QMessageBox::YesRole);
        noButton=msgAgreement.addButton(QString("Nein"),QMessageBox::NoRole);
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
        msgWarning.setWindowTitle(QString("Warnung"));
        s=QString("Das Programm läuft bereits oder wurde das letzte Mal gewaltsam beendet. Bitte überprüfen Sie das!");
        msgWarning.setText(s);
        msgWarning.exec();
        settings.remove_line(QString("PROGRAM_RUNNING"));
        bReturn=true;
    }
    else
    {
        s=QString("PROGRAM_RUNNING");//set program running
        settings.write(s,QString(""));
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

    if(pAction->text()==QString("Wareneingang"))
        trade_incoming();
    if(pAction->text()==QString("Warenausgang"))
        trade_outgoing();
    if(pAction->text()==QString("Wareneingang(Bestellung)"))
        trade_ordering_incoming();
    if(pAction->text()==QString("Warenausgang(Kunde)"))
        trade_outgoing_customer();
    if(pAction->text()==QString("stornieren"))
        trade_canceled();
    if(pAction->text()==QString("neu"))
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
    if(pAction->text()==QString("bearbeiten"))
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
    if(pAction->text()==QString("löschen"))
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
    if(pAction->text()==QString("kopieren"))
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

void MainWindow::tab_switched(void)
{
    ui->lineEditInventoryMask->clearFocus();
    ui->lineEditTradeMask->clearFocus();
    ui->lineEditOrderingMask->clearFocus();
    ui->lineEditArticleMask->clearFocus();
    ui->lineEditMakerMask->clearFocus();
    ui->lineEditDealerMask->clearFocus();
    ui->lineEditCustomerMask->clearFocus();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    static QDate mark_date=QDate(0,0,0);
    static bool mark_messagebox_error_open=false;

    QMessageBox msg(QMessageBox::Critical,"","");
    QString s,s2;
    QDate dt=QDate().currentDate();
    int i;

    //check db-error
    s=m_db.last_error();
    if(s.length()>3)//error db?
    {
        if(!mark_messagebox_error_open)//msg not open ?
        {
            mark_messagebox_error_open=true;
            s2=msg.text();
            s2+=QString("\n%1").arg(s);
            msg.setWindowTitle("error - database");
            msg.exec();
            mark_messagebox_error_open=false;
        }
    }

    //inventory-warning
    i=ui->tabWidget->indexOf(ui->tabInventory);
    if(i<0||i>=ui->tabWidget->count())
        i=0;
    if(inventory_get_count_of_arctile_under_limit()>0)
        ui->tabWidget->setTabIcon(i,m_widgets.get_warning_icon());
    else
        ui->tabWidget->setTabIcon(i,QIcon());

    //new date?
    if(mark_date==QDate(0,0,0))
        mark_date=dt;
    else if(dt>mark_date)
    {//new day?->update mask trade date list

        if(dt.year()>mark_date.year() && mark_date!=QDate(0,0,0))//new year?
        {
            s=QString("%1").arg(dt.year());
            ui->comboBoxTradeMaskYear->insertItem(0,s);
        }
        //-
        trade_mask_date_changed();
        mark_date=dt;
    }
    //-
    m_iTimerId=event->timerId();
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
    QString s=QString("%1").arg(i);
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
                    inventory_mask_set();//update inventory table
                    ordering_update_wares_list();//update ordering articlelist
                    trade_update_wares_list();//update trade articlelist
                    article_mask_set();//update article table

                    //logbook after
                    if(m_db.logbook_create_edit(wg,lg,false))
                        logbook_insert(lg);
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
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");

    //-how many article with this waregroup??
    s=QString("waregroup_id=%1").arg(iId);
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    if(!m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory))
        return false;
    //-
    if(count>0)//article with this waregroup found?
    {
        s=QString("Es gibt %1 Artikel in der Warengruppe ").arg(count);
        s+=QString("'%1' !!!").arg(ls[0]->text(0));
        s+=QString("\nSoll die Warengruppe '%1' wirklich gelöscht werden?").arg(ls[0]->text(0));
        s+=QString("\n(Artikel werden in die übergeordnete Gruppe verschoben)");
        msg.setText(s);
    }
    else
    {
        s=QString("Soll die Warengruppe '%1' wirklich gelöscht werden?").arg(ls[0]->text(0));
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
                    inventory_mask_set();//update inventory table
                    ordering_update_wares_list();//update ordering articlelist
                    trade_update_wares_list();//update trade articlelist
                    article_mask_set();//update article table
                }
            }
            //logbook
            logbook_insert(lg);
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
    QString s=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_MAKER_GET_COUNT,&memory);
    if(count<0)//error
        return false;
    //-
    if(row!=count)
        s=QString("%1/").arg(row);
    s+=QString("%1").arg(count);
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
        sCondition=QString("");
    else if(iIndex==0)//name
        sCondition=QString("name like '%%1%'").arg(s);
    else if(iIndex==1)//adress
        sCondition=QString("adress like '%%1%'").arg(s);
    else if(iIndex==2)//call number
        sCondition=QString("call_numbers like '%%1%'").arg(s);
    else if(iIndex==3)//fax
        sCondition=QString("fax_numbers like '%%1%'").arg(s);
    else if(iIndex==4)//email
        sCondition=QString("email_adress like '%%1%'").arg(s);
    else if(iIndex==5)//homepage
        sCondition=QString("homepage like '%%1%'").arg(s);
    else if(iIndex==6)//contakt pers.
        sCondition=QString("cotakt_person like '%%1%'").arg(s);
    else if(iIndex==7)//comment
        sCondition=QString("comment like '%%1%'").arg(s);
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
                    inventory_mask_set();//update inventory table
                    ordering_update_wares_list();//update ordering articlelist
                    trade_update_wares_list();//update trade articlelist
                    article_mask_set();//update article table

                    //logbook after
                    if(m_db.logbook_create_edit(mk,lg,false))
                        logbook_insert(lg);
                }
            }
        }
    }
    //-
    return b;
}

bool MainWindow::maker_delete(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    int i=-1,iId=-1;
    QList <QTableWidgetItem*> ls=ui->tableWidgetMaker->selectedItems();
    if(ls.count()<3)
       return false;
    //-
    iId=ls[2]->text().toInt(&b,10);//get id
    if(!b)
        return false;
    //-
    CLogbook lg;
    CMaker mk;
    m_db.maker_get(iId,mk);
    m_db.logbook_create_remove(mk,lg);//create logbook
    //-
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    CPointerMemory memory;
    QList <int> lsArIds;
    QString s=QString("maker_id=%1").arg(iId);
    memory.set_string(&s);
    memory.set_int_list(&lsArIds);
    b=m_thread.set_work(WORK_ARTICLE_GET_ALL,&memory);
    if(!b)
        return false;

    if(lsArIds.count()>0)//article with this maker found?
    {
        s=QString("Es gibt %1 Artikel von dem Hersteller ").arg(lsArIds.count());
        s+=QString("'%1' !!!").arg(ls[0]->text());
        s+=QString("\nSoll der Hersteller wirklich gelöscht werden?");
        msg.setText(s);
    }
    else
    {
        s=QString("Soll der Hersteller '%1' wirklich gelöscht werden?").arg(ls[0]->text());
        msg.setText(s);
    }
    //-
    msg.exec();
    if(msg.clickedButton()==yesButton)
    {
        if(!m_db.maker_remove(iId))//remove in db ok?
            b=false;
        else
        {
            //update all article with this maker
            if(lsArIds.count()>0)
            {
                memory.clear();
                memory.set_int(&iId);
                memory.set_int2(&i);
                b=m_thread.set_work(WORK_ARTICLE_ALL_CHANGE_MAKER,&memory);
                if(!b)
                {}
                else
                {
                    inventory_mask_set();//update inventory table
                    ordering_update_wares_list();//update ordering articlelist
                    trade_update_wares_list();//update trade articlelist
                    article_mask_set();//update article table
                }
            }
            //-
            b=m_widgets.remove_row(ui->tableWidgetMaker,iId);
            maker_update_info();
            maker_update_count();
            maker_check_user_input();

            //logbook insert
            logbook_insert(lg);
        }
    }
    ls.clear();
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
    QString s=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_DEALER_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString("%1/").arg(row);
    s+=QString("%1").arg(count);
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
        sCondition=QString("");
    else if(iIndex==0)//name
        sCondition=QString("name like '%%1%'").arg(s);
    else if(iIndex==1)//customer_number
        sCondition=QString("customer_number like '%%1%'").arg(s);
    else if(iIndex==2)//adress
        sCondition=QString("adress like '%%1%'").arg(s);
    else if(iIndex==3)//call number
        sCondition=QString("call_numbers like '%%1%'").arg(s);
    else if(iIndex==4)//fax
        sCondition=QString("fax_numbers like '%%1%'").arg(s);
    else if(iIndex==5)//email
        sCondition=QString("email_adress like '%%1%'").arg(s);
    else if(iIndex==6)//homepage
        sCondition=QString("homepage like '%%1%'").arg(s);
    else if(iIndex==7)//contakt pers.
        sCondition=QString("cotakt_person like '%%1%'").arg(s);
    else if(iIndex==8)//comment
        sCondition=QString("comment like '%%1%'").arg(s);
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
    CLogbook lg;
    CInputDialogDealer dlg;
    dlg.set_thread(&m_thread);
    dlg.setWindowTitle("neuer Händler");
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
        }
    }
    return b;
}

bool MainWindow::dealer_edit(void)
{
    if(!m_db.is_db_connect())
        return false;
    CDealer de;
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
            dlg.setWindowTitle("Händler bearbeiten");
            if(dlg.exec())
            {
                dlg.get_data(de);
                if(!m_db.dealer_update(de))
                    b=false;
                else
                {
                    b=m_widgets.dealer_update_row(ui->tableWidgetDealer,de,true);//update table item
                    dealer_update_info();
                    ordering_mask_set();//update ordering table

                    //logbook after
                    if(m_db.logbook_create_edit(de,lg,false))
                        logbook_insert(lg);
                }
            }
        }
    }
    //-
    return b;
}

bool MainWindow::dealer_delete(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    int i=-1,iId=-1;
    QList <QTableWidgetItem*> ls=ui->tableWidgetDealer->selectedItems();
    if(ls.count()<3)
       return false;
    //-
    iId=ls[2]->text().toInt(&b,10);//get id
    if(!b)
        return false;
    //-
    CLogbook lg;
    CDealer de;
    m_db.dealer_get(iId,de);
    m_db.logbook_create_remove(de,lg);//create logbook
    //-
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    CPointerMemory memory;
    QList <int> lsOrdIds;
    QString s=QString("dealer_id=%1").arg(iId);
    memory.set_string(&s);
    memory.set_int_list(&lsOrdIds);
    b=m_thread.set_work(WORK_ORDERING_GET_ALL,&memory);
    if(!b)
        return false;

    if(lsOrdIds.count()>0)//ordering with dealer found?
    {
        s=QString("Es gibt %1 Bestellungen beim Händler").arg(lsOrdIds.count());
        s+=QString(" '%1'").arg(ls[0]->text());
        s+=QString("\nSoll der Händler wirklich gelöscht werden?");
        msg.setText(s);
    }
    else
    {
        s=QString("Soll der Händler '%1' wirklich gelöscht werden?").arg(ls[0]->text());
        msg.setText(s);
    }
    //-
    msg.exec();
    if(msg.clickedButton()==yesButton)
    {
        if(!m_db.dealer_remove(iId))//remove in db ok?
            b=false;
        else
        {
            //update all orderings with this dealer
            if(lsOrdIds.count()>0)
            {
                memory.clear();
                memory.set_int(&iId);
                memory.set_int2(&i);
                b=m_thread.set_work(WORK_ORDERING_ALL_CHANGE_DEALER, &memory);
                if(!b)
                {}
                else
                    ordering_mask_set();//update ordering table
            }
            //-
            b=m_widgets.remove_row(ui->tableWidgetDealer,iId);
            dealer_update_info();
            dealer_update_count();
            dealer_check_user_input();

            //logbook
            logbook_insert(lg);
        }
    }
    ls.clear();
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
    QString s=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_CUSTOMER_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString("%1/").arg(row);
    s+=QString("%1").arg(count);
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
            sCondition=QString("customer_number like '%%1%'").arg(s);
        else if(index==1)//name
            sCondition=QString("name like '%%1%'").arg(s);
        else if(index==2)//first name
            sCondition=QString("first_name like '%%1%'").arg(s);
        else if(index==3)//street
            sCondition=QString("street like '%%1%'").arg(s);
        else if(index==4)//postcode
            sCondition=QString("postcode like '%%1%'").arg(s);
        else if(index==5)//city
            sCondition=QString("city like '%%1%'").arg(s);
        else if(index==6)//comment
            sCondition=QString("comment like '%%1%'").arg(s);
        else if(index==7)//comment
            sCondition=QString("call_numbers like '%%1%'").arg(s);
        else if(index==8)//comment
            sCondition=QString("fax_numbers like '%%1%'").arg(s);
        else if(index==9)//comment
            sCondition=QString("email_adress like '%%1%'").arg(s);
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

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetCustomer,iId))//select?
    {
        if(!m_db.customer_get(iId,cu))//get customer data
            b=false;
        else
        {
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
                    b=m_widgets.customer_update_row(ui->tableWidgetCustomer,cu,true);//update table item
                    customer_update_info();
                }

                //logbook after
                if(m_db.logbook_create_edit(cu,lg,false))
                    logbook_insert(lg);
            }
        }
    }
    //-
    return b;
}

bool MainWindow::customer_delete(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    int iId=-1;
    QList <QTableWidgetItem*> ls=ui->tableWidgetCustomer->selectedItems();
    if(ls.count()<5)
       return false;
    //-
    iId=ls[4]->text().toInt(&b,10);//get id
    if(!b)
        return false;
    //-
    CLogbook lg;
    CCustomer cu;
    m_db.customer_get(iId,cu);
    m_db.logbook_create_remove(cu,lg);//create logbook
    //-
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    QString s=QString("Soll der Kunde '%1,%2' wirklich gelöscht werden?").arg(ls[1]->text(),ls[2]->text());
    msg.setText(s);
    //-
    msg.exec();
    if(msg.clickedButton()==yesButton)
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
        }
    }
    ls.clear();
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
    QString s=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString("%1/").arg(row);
    s+=QString("%1").arg(count);
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
            sCondition=QString("name like '%%1%'").arg(s);
        else if(index==1)//articlenumber
            sCondition=QString("articlenumber like '%%1%'").arg(s);
        else if(index==2)//2.articlenumber
            sCondition=QString("articlenumber2 like '%%1%'").arg(s);
        else if(index==9)//location
            sCondition=QString("location like '%%1%'").arg(s);
        else if(index==10)//unit
            sCondition=QString("unit like '%%1%'").arg(s);
        else if(index==11)//valuta
            sCondition=QString("valuta like '%%1%'").arg(s);
        else if(index==12)//comment
            sCondition=QString("comment like '%%1%'").arg(s);
        else if(index>4 && index<9) //basprice /salesprice
        {
            fInput=s.toFloat(&b);
            if(b)
            {
                if(index==5)//basep. <
                    sCondition=QString("baseprice < %1").arg(fInput);
                else if(index==6)//basep. >
                    sCondition=QString("baseprice > %1").arg(fInput);
                else if(index==7)//salesp.inv. <
                    sCondition=QString("salesprice < %1").arg(fInput);
                else if(index==8)//salesp..inv. >
                    sCondition=QString("salesprice > %1").arg(fInput);
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
    return b;
}

bool MainWindow::article_new(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    CArticle ar;
    CLogbook lg;
    CMaker mk;
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
            }
            if(dlg.m_iNewWaregroupId>0)//new waregroup add?
            {
                waregroup_update_tree(dlg.m_iNewWaregroupId);
                waregroup_update_count();
                waregroup_check_user_input();
            }
            m_widgets.article_insert_row(ui->tableWidgetArticle,ar,FORMAT_ONE,ITEM_POSITION_BOTTOM,true);
            inventory_mask_set();//update inventory table
            article_update_count();

            //logbook
            if(m_db.logbook_create_new(ar,lg))
                logbook_insert(lg);
        }
    }
    return b;
}

bool MainWindow::article_edit(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
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
            dlg.m_sMarkArticleName=ar.get_name();
            dlg.set_data(ar);
            dlg.setWindowTitle("Artikel bearbeiten");
            if(dlg.exec())
            {
                dlg.get_data(ar);
                b=m_db.article_update(ar);
                if(!b)//error
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
                    }
                    if(dlg.m_iNewWaregroupId>0)//new waregroup add?
                    {
                        waregroup_update_tree(dlg.m_iNewWaregroupId);
                        waregroup_update_count();
                        waregroup_check_user_input();
                    }
                    m_widgets.article_update_row(ui->tableWidgetArticle,ar,FORMAT_ONE,true);
                    //-
                    ordering_mask_set();//update ordering table
                    inventory_mask_set();//update inventory table
                    ordering_update_wares_list();//update ordering articlelist
                    trade_update_wares_list();//update trade articlelist

                    //logbook after
                    if(m_db.logbook_create_edit(ar,lg,false))
                        logbook_insert(lg);
                }
            }
        }
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
    QInputDialog dlg(this);
    dlg.setWindowTitle("Artikel kopieren");
    dlg.setOkButtonText(QString("Kopie erstellen"));
    CArticle ar;
    int iId=-1;

    if(m_widgets.get_selected_table_item_value(ui->tableWidgetArticle,iId))//select?
    {
        if(!m_db.article_get(iId,ar))//get article data
            b=false;
        else
        {
            s=QString("Artikelbezeichnung der Kopie von \"%1\":").arg(ar.get_name());
            dlg.setLabelText(s);
            s=ar.get_name()+QString(" Kopie");
            dlg.setTextValue(s);
            //-
            do
            {
                iReturn=dlg.exec();
                sCondition=QString("name='%1'").arg(dlg.textValue());
                m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory);
                if(count>0 && iReturn!=QDialog::Rejected)
                    QMessageBox::warning(this,"Fehler!","Ein Artikel mit dieser Beschreibung existiert bereits!");
            }while(iReturn!=QDialog::Rejected && count>0);
            //-
            if(iReturn==QDialog::Accepted && count<=0)
            {//ok? -> copy
                m_db.logbook_create_copy(ar.get_name(),dlg.textValue(),lg);//create logbook
                ar.set_name(dlg.textValue());//set copy name
                ar.set_articlenumber(QString(""));//copy has another article numbers
                ar.set_articlenumber2(QString(""));
                if(m_db.article_add(ar))//add in db
                {
                    //update displays
                    m_widgets.article_insert_row(ui->tableWidgetArticle,ar,FORMAT_ONE,ITEM_POSITION_BOTTOM,true);
                    inventory_mask_set();//update inventory table
                    article_update_count();

                    //logbook
                    logbook_insert(lg);
                }
            }
        }
    }
    memory.clear();
    return false;
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
        s=QString("wares like '%x%1|%'").arg(iId);
        memory.clear();
        memory.set_string(&s);
        memory.set_int(&i);
        m_thread.set_work(WORK_ORDERING_GET_COUNT,&memory);
        if(i>0)//orderings with this article?
        {
            msg.setWindowTitle("!");
            s=QString("Es gibt %1 Bestellung(en) mit diesem Artikel, ändern Sie die Bestellung(en) bevor dieser Artikel gelöscht werden kann!").arg(i);
            msg.setText(s);
            msg.exec();
            b=false;
        }
        else
        {
            QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
            msg.addButton(QString("Nein"),QMessageBox::NoRole);
            msg.setWindowTitle("?");
            s=QString("Soll der Artikel '%1' wirklich gelöscht werden?").arg(ar.get_name());
            if(bGiveItTrades)
                s+=QString("\n(Es existieren Buchungen mit diesen Artikel!)");
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
                    inventory_mask_set();//update inventory table
                    ordering_update_wares_list();//update ordering articlelist
                    trade_update_wares_list();//update trade articlelist
                    article_check_user_input();
                    article_update_count();

                    //logbook after
                    logbook_insert(lg);
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
    QString s=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_ORDERING_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString("%1/").arg(row);
    s+=QString("%1").arg(count);
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
        sCondition=QString("order_number like '%1%'").arg(s);
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
        sCondition=QString("comment like '%%1%'").arg(s);
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
                    sCondition=QString("date >= '%1' AND date <= '%2'").arg(s,s2);
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
                sCondition=QString("date < '%1'").arg(s2);
            else if(index==3)//date =
                sCondition=QString("date = '%1'").arg(s2);
            else if(index==4)//date >
                sCondition=QString("date > '%1'").arg(s2);
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

    bool b=true;
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
        }
    }
    return b;
}

bool MainWindow::ordering_edit(void)
{
    if(!m_db.is_db_connect())
        return false;

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
                        }
                    }
                    m_widgets.ordering_update_row(ui->tableWidgetOrdering,ord,FORMAT_ONE,true);
                    ordering_update_wares_list();
                    inventory_mask_set();//update inventory table

                    //logbook after
                    if(m_db.logbook_create_edit(ord,lg,false))
                        logbook_insert(lg);
                }
            }
        }
    }
    return b;
}

bool MainWindow::ordering_delete(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b=true;
    QList <QTableWidgetItem*> ls=ui->tableWidgetOrdering->selectedItems();
    if(ls.count()<=0)
        return false;
    //-
    COrdering ord;
    CLogbook lg;
    //-
    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle("?");
    QString s=QString("Soll die Bestellung Nummer '%1' wirklich gelöscht werden?").arg(ls[1]->text());
    msg.setText(s);
    //-
    int iId=ls[5]->text().toInt(&b,10);//get id
    if(b)//convert to int ok?
    {
        m_db.ordering_get(iId,ord);
        m_db.logbook_create_remove(ord,lg);//create logbook
        //-
        msg.exec();
        if(msg.clickedButton()==yesButton)
        {
            if(!m_db.ordering_remove(iId))
                b=false;
            else
            {
                m_widgets.remove_row(ui->tableWidgetOrdering,iId);
                ordering_update_count();
                ordering_update_wares_list();
                inventory_mask_set();//update inventory table
                ordering_check_user_input();

                //logbook after
                logbook_insert(lg);
            }
        }
    }
    ls.clear();
    return b;
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
    QString s=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_TRADE_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString("%1/").arg(row);
    s+=QString("%1").arg(count);
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
                s=QString("%1x").arg(diff);//count(diff)
                s+=QString("%1").arg(id);//a.id
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
    dtF.setYMD(year,dtF.month(),dtF.day());
    if(dtF>QDate().currentDate())
        dtF=QDate().currentDate();
    if(dtF.day()>dtF.daysInMonth())
        dtF.setDate(dtF.year(),dtF.month(),dtF.daysInMonth());
    dtT.setYMD(year,dtT.month(),dtT.day());
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
    sCondition=QString("date >= '%1' AND date <= '%2'").arg(dtFrom.toString(QString("yyyy-MM-dd")),dtTo.toString(QString("yyyy-MM-dd")));

    //onther rules
    if(sRules.length()>0)
    {
        if(index_rules==0)//bookingnumber
        {
            s=QString("booking_number like '%%1%'").arg(sRules);
        }
        if(index_rules==1)//comment
        {
            s=QString("comment like '%%1%'").arg(sRules);
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
            s=QString("info_2 like '%%1%'").arg(sRules);
            s+=QString(" AND type = %1").arg(TYPE_ORDERING_INCOMING);
        }
        if(index_rules==8)//ordering number
        {
            s=QString("info_1 like '%%1%'").arg(sRules);
            s+=QString(" AND type = %1").arg(TYPE_ORDERING_INCOMING);
        }
        if(index_rules==9)//customer name
        {
            s=QString("info_2 like '%%1%'").arg(sRules);
            s+=QString(" AND type = %1").arg(TYPE_CUSTOMER_OUTGOING);
        }
        if(index_rules==10)//ordering number
        {
            s=QString("info_1 like '%%1%'").arg(sRules);
            s+=QString(" AND type = %1").arg(TYPE_CUSTOMER_OUTGOING);
        }
        //-
        if(index_rules<2 || index_rules>6)//booking_number or comment or dealer or customer
            sCondition+=QString(" AND %1").arg(s);
    }

    //get records
    memory.set_string_list(&lsTrade);
    memory.set_string(&sCondition);
    memory.set_string2(&sRules);
    memory.set_bool(&bFirst);

    s=QString("");
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
    QString si;
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
    dlg.set_db(&m_thread);
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
            inventory_mask_set();//update inventory table
            trade_refresh_datelist();
            trade_update_count();
            //logbook
            if(m_db.logbook_create_new(tr,lg))
                logbook_insert(lg);
        }
    }
    return true;
}

bool MainWindow::trade_outgoing(void)
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
    CLogbook lg;
    int type;
    QList<QString> ls;
    CPointerMemory memory;
    memory.set_int(&type);
    memory.set_string_list(&ls);
    //-
    CTrade tr;
    CInputDlgTrade dlg;
    dlg.set_db(&m_thread);
    dlg.set_outgoing();
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
            inventory_mask_set();//update inventory table
            trade_refresh_datelist();
            trade_update_count();

            //logbook
            if(m_db.logbook_create_new(tr,lg))
                logbook_insert(lg);
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
    QPushButton * noButton=msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.setWindowTitle("?");
    QString s=QString("Die Bestellung ist nicht vollständig eingegangen, soll die Bestellung trotzdem gelöscht werden?\n(bei 'nein' wird die Differenz in der Bestellung gespeichert)");
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
    dlg.setWindowTitle(QString("Wareneingang(Bestellung)"));
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
                    ordering_mask_set();//update ordering table
                }
            }
            //-
            if(bOrdDelete)
            {
                if(m_db.ordering_remove(iOrderingId))
                    m_widgets.remove_row(ui->tableWidgetOrdering,iOrderingId);
            }
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
                inventory_mask_set();//update inventory table
                trade_refresh_datelist();
                trade_update_count();

                //logbook
                if(m_db.logbook_create_new(tr,lg))
                    logbook_insert(lg);
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
            inventory_mask_set();//update inventory table
            trade_refresh_datelist();
            trade_update_count();

            //logbook
            if(m_db.logbook_create_new(tr,lg))
                logbook_insert(lg);
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
                        s=QString("Die Buchung kann nicht storniert werden da es zwischenzeitlich zu negativen Bestand kommen würde,"
                                  "beachten Sie die chronologische Reihenfolge!");
                        if(sErr.length()>0)
                            s+=QString("\n(%1)").arg(sErr);
                        msg.setWindowTitle("!");
                        msg.setText(s);
                        msg.exec();
                    }
                    if(b)
                    {//canceled is possible
                        s=QString("Möchten Sie wirklich die Buchnung mit der Nummer '%1' stornieren?").arg(sBookingNumber);
                        QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
                        msg.addButton(QString("Nein"),QMessageBox::NoRole);
                        msg.setWindowTitle("?");
                        msg.setText(s);
                        msg.exec();
                        if(msg.clickedButton()==yesButton)
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

//inventory--------------------------------------

bool MainWindow::inventory_update_count(void)
{
    if(!m_db.is_db_connect())
        return false;

    int row=ui->tableWidgetInventory->rowCount();
    int count;
    QString s=QString("");
    CPointerMemory memory;
    memory.set_string(&s);
    memory.set_int(&count);
    m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory);
    if(count<0)
        return false;
    //-
    if(row!=count)
        s=QString("%1/").arg(row);
    s+=QString("%1").arg(count);
    ui->labelCountInventory->setText(s);
    //-
    return true;
}

int MainWindow::inventory_get_count_of_arctile_under_limit(void)
{
    if(!m_db.is_db_connect())
        return 0;

    int count=0;
    QString sCondition=QString("article.inventory < article.warning_limit");
    CPointerMemory memory;
    memory.set_int(&count);
    memory.set_string(&sCondition);
    if(!m_thread.set_work(WORK_ARTICLE_GET_COUNT,&memory))
        count=0;
    return count;
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
            sCondition=QString("name like '%%1%'").arg(s);
        else if(index==7)//articlenumber
            sCondition=QString("articlenumber like '%%1%'").arg(s);
        else if(index==8)//2.articlenumber
            sCondition=QString("articlenumber2 like '%%1%'").arg(s);
        else if(index==11)//unit
            sCondition=QString("unit like '%%1%'").arg(s);
        else if(index==12)//comment
            sCondition=QString("comment like '%%1%'").arg(s);
        else if(index>=0 && index<4)//inventory / max.inventory
        {
            input=s.toInt(&b,10);
            if(b)//is number
            {
                if(index==0)//inv. <
                    sCondition=QString("inventory < %1").arg(input);
                else if(index==1)//inv. >
                    sCondition=QString("inventory > %1").arg(input);
                else if(index==2)//max.inv. <
                    sCondition=QString("max_inventory < %1").arg(input);
                else if(index==3)//max.inv. >
                    sCondition=QString("max_inventory > %1").arg(input);
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
    QString s=QString("");
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
        s=QString("date_time like '%1%'").arg(dt.toString("yyyy-MM-dd"));
        m_thread.set_work(WORK_LOGBOOK_GET_COUNT,&memory);
        if(count<0)
            b=false;
        else
        {
            s=QString("");
            if(count!=count2)
                s=QString("%1/").arg(count);
            s+=QString("%1").arg(count2);
            ui->labelCountLogbook->setText(s);
        }
    }
    //-
    return b;
}

bool MainWindow::logbook_insert(CLogbook & lg)
{
    bool b,bMask=true;
    int index=ui->comboBoxMaskLogbook->currentIndex();
    QDate dt2,dt=logbook_get_selected_date();
    QDateTime dt_ti=lg.get_date_time();
    dt2=dt_ti.date();
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
        s=QString("");
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
                    s=QString("date_time <= '%1'").arg(s2);
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
    dt.setYMD(year,dt.month(),dt.day());
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
    b=m_widgets.get_mask_condition_logbook(dt,index,sCondition);
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
        new_count=QInputDialog::getInt(this,QString("Anzahl ändern"),QString("neue Anzahl(min.10):"),cur_count,10,9999,1,&b);
        if(b)
        {//pressed ok
            if(cur_count!=new_count)
            {//changed?
                s=QString("%1").arg(new_count);
                ui->lineEditCountItemLog->setText(s);//set new count

                //logbook
                lg.set_type(LOGBOOK_TYPE_OTHER);
                lg.set_date_time(QDateTime::currentDateTime());
                s=QString("Anzahl der letzten zu speichernden Logbucheinträge von %1 auf ").arg(cur_count);
                s+=QString("%1 geändert").arg(new_count);
                lg.set_text(s);
                logbook_insert(lg);

                if(logbook_check_items_over_count())//check itemcount- remove over
                {
                    logbook_mask_set();//over-> update list
                    logbook_refresh_datelist();//update list
                }
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
    QString s("");
    CLogbook lg;
    CPointerMemory memory;
    memory.set_string(&s);

    QMessageBox msg(QMessageBox::Question,"","");
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    msg.setWindowTitle(QString("Achtung!"));
    msg.setText(QString("Möchten Sie wirklich alle Logbucheinträge löschen ?"));
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
            lg.set_text(QString("alle Logbucheinträge gelöscht"));
            logbook_insert(lg);
        }
    }
    return b;
}


//menu--------------------------------------------------------------------------------------------------------------------
bool MainWindow::menu_db_backup_create(void)
{
    if(!m_db.is_db_connect())
        return false;

    QFileInfo file_info;
    CLogbook lg;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString("Fehler"));
    QDateTime dtTi=QDateTime::currentDateTime();
    QString s,s2;
    QString sReadFile=m_sDbPath+QString("lava.sqlite");
    QString sWriteFile=m_sDbPath+QString("/%1.sqlite").arg(dtTi.toString(QString("hh-mm-ss_dd-MM-yyyy")));
    QFile file(sReadFile);
    sWriteFile=QFileDialog::getSaveFileName(this,QString("Sicherung erstellen"),sWriteFile);
    QStringList ls;

    if(sWriteFile.length()>0)//file dialog finish with 'save'?
    {
        //file info (permission)
        ls=sWriteFile.split("/");
        for(int i=0;i<ls.count()-1;i++)
            s+=ls[i]+QString("/");
        file_info.setFile(s);
        if(!file_info.permission(QFile::WriteUser))
        {
            msg.setText(QString("Sie haben keine Schreibrechte an diesem Ort!"));
            msg.exec();
        }
        else
        {
            //user overwrite current db? ->error
            if(sReadFile==sWriteFile)
            {
                s=QString("Sie können nicht die aktuelle Datenbankdatei überschreiben!");
                msg.setText(s);
                msg.exec();
            }
            else
            {
                //check give it file?
                if(QFile(sWriteFile).exists())
                    QFile::remove(sWriteFile);//delete old file

                //copy
                if(!file.copy(sWriteFile))
                    s2=QString("(errorcode: %1)").arg(file.error());

                //exists create file?
                if(!QFile(sWriteFile).exists())
                {
                    s=QString("Fehler beim Erstellen der Sicherung, evtl. fehlen Ihnen die Schreibrechte für diesen Ort!");
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
                    s=QString("Sicherung \"%1 \" erstellt").arg(sWriteFile);
                    lg.set_text(QString(s));
                    logbook_insert(lg);
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
    QString sError,sReadFile;
    QString sWriteFile=m_sDbPath+QString("lava.sqlite"),sWriteTempFile=m_sDbPath+QString("lava_temp.sqlite");
    QMessageBox msgQuest(QMessageBox::Question,"","");
    msgQuest.setText(QString("Achtung, beim Laden einer Sicherung gehen alle aktuellen Daten unwiderruflich verloren.\nMöchten Sie wirklich eine Sicherung laden?"));
    QMessageBox msgCritical(QMessageBox::Critical,"","");
    msgCritical.setWindowTitle(QString("Fehler"));
    msgQuest.setWindowTitle(QString("?"));
    QPushButton * yesButton=msgQuest.addButton(QString("Ja"),QMessageBox::YesRole);
    msgQuest.addButton(QString("Nein"),QMessageBox::NoRole);


    //stop db-connection & timer
    killTimer(m_iTimerId);//stop timmer
    m_db.close_db();//close current db connection


    //check permission db-path
    QFileInfo file_info(m_sDbPath);
    if(!file_info.permission(QFile::WriteUser))
        sError=QString("Sie haben keine Schreibrechte für den  Ordner \"%1\"!").arg(m_sDbPath);//error
    else
    {
        msgQuest.exec();//quest really overwrite db-file?
        if(msgQuest.clickedButton()==yesButton)
        {
            sReadFile=QFileDialog::getOpenFileName(this,QString("Sicherung laden"),m_sDbPath);
            if(sReadFile.length()>0)
            {//load button pressed?

                if(!QFile::exists(sReadFile))//check give it read file?
                    sError=QString("Die angegebene Datei existiert nicht!");//error
                else
                {
                    if(sReadFile==sWriteFile)
                        sError=QString("Sie können die aktuelle Datenbankdatei nicht als Sicherung laden!");
                    else
                    {
                        if(!m_db.open_db(sReadFile))//open db(read file) ok?
                            sError=QString("Die angegebene Datei konnte nicht geöffnet werden!");//error
                        else
                        {
                            b=m_db.check_database_file(); // is the read file a lava2 sqlite db file?
                            m_db.close_db();//close read db
                            if(!b)
                                sError=QString("Die angegebene Datei ist keine LaVa2-Datenbankdatei!");//error
                            else
                            {
                                //save current db-file
                                if(QFile(sWriteTempFile).exists())//exists temp-file ? ->remove it
                                    QFile::remove(sWriteTempFile);
                                if(!QFile::rename(sWriteFile,sWriteTempFile))//rename current db to temp
                                    sError=QString("Fehler beim umbenennen der Datenbank-Datei!");
                                else
                                {
                                    //copy file
                                    b=QFile::copy(sReadFile,sWriteFile);//copy ok?
                                    if(!b || !QFile::exists(sWriteFile))
                                    {//copy error
                                        sError=QString("Fehler beim kopieren der Datenbank-Datei!");
                                        QFile::rename(sWriteTempFile,sWriteFile);//rename back
                                    }
                                    else//copy ok
                                        QFile::remove(sWriteTempFile);//delete temp file(old db-file)
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
        sError=QString("");
    }


    //start all
    if(!m_db.open_db(sWriteFile))
        sError=QString("Fehler beim öffnen der Datenbank,");
    else
    {
        //check + update db-file
        if(!m_db.check_and_update_db_version())
            sError=QString("Fehler beim update der Datenbankdatei auf Version %1,").arg(CURRENT_DB_VERSION);
        else
        {
            //update widgets
            fill_all_table();
            startTimer(3000);
        }
    }


    //check error after start
    if(sError.length()>0)
    {
        sError+=QString("\ndas Programm hat keine Datenbankverbindung mehr!!!");
        msgCritical.setText(sError);
        msgCritical.exec();
    }

    return true;
}

bool MainWindow::menu_db_import_from_lava1(void)
{
    if(!m_db.is_db_connect())
        return false;

    bool b2;
    int i,j,k,l,x,a;
    char c=0;
    QDate dt;
    QDateTime dtTi;
    CMaker mk;
    CWaregroup wg;
    CArticle ar;
    CLogbook lg;
    COrdering ord;
    QList<QString> lsStrMaker;
    QList<QString> lsStrWaregroup;
    QList<CLogbook> lsLogbook;
    QList<CArticle> lsArticle;
    QList<COrdering> lsOrdering;
    QString s,s2,s3,s4;
    QStringList strLi;
    QStringList strLi2;
    QList<int> lsInt;
    QString sHome=QDir::homePath();
    QFile file;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString("Fehler"));
    QString sFile=QFileDialog::getOpenFileName(this,QString("Daten aus LaVa 1 impotieren"),sHome);
    if(sFile.length()>0)//press button load?
    {
        file.setFileName(sFile);
        if(file.open(QIODevice::ReadOnly))
        {
            //check file(is a lava 1 data file?)
            s=QString(file.readLine());
            for(i=0;i<4 && s.length()>=4;i++)
                c+=s[i].toAscii();
            if(c!=115)
            {
                //not a lava 1 data file?
                s=QString("Die angegebene Datei ist keine LaVa 1 Datendatei.");
                msg.setText(s);
                msg.exec();
            }
            else
            {
                //load
                s=QString(file.readLine());//only date data
                if(s.length()>0)
                {
                    s=QString(file.readLine());//begin data
                    if(s.length()>0)
                    {
                        //decode
                        for(i=0;i<s.length();i++)
                        {
                            c=s[i].toAscii();
                            c-=15;
                            s[i]=c;
                        }

                        //maker
                        for(i=0,s2=QString("");i<s.length();i++)
                        {
                            if(s[i]=='@')
                                break;
                            s2.push_back(s[i]);
                        }
                        if(i<s.length())
                        {
                            strLi=s2.split("\n");
                            if(strLi.count()>0)
                            {
                                if(strLi[0]==QString("@"))
                                    strLi.removeFirst();
                            }
                            for(j=0;j<strLi.count();j++)
                            {
                                if(strLi[j].length()>0)
                                    lsStrMaker.push_back(strLi[j]);
                            }

                            //waregroup
                            for(s2=QString("");i<s.length()-1;i++)
                            {
                                if(s[i]=='@' && s[i+1]=='@')
                                    break;
                                s2.push_back(s[i]);
                            }
                            if(i<s.length())
                            {
                                strLi.clear();
                                strLi=s2.split("\n");
                                if(strLi.count()>0)
                                {
                                    if(strLi[0]==QString("@"))
                                        strLi.removeFirst();
                                }
                                for(j=0;j<strLi.count();j++)
                                {
                                    if(strLi[j].length()>0)
                                        lsStrWaregroup.push_back(strLi[j]);
                                }
                            }

                            //article
                            for(s2=QString("");i<s.length()-2;i++)
                            {
                                if(s[i]=='@' && s[i+1]=='@' && s[i+2]=='@')
                                    break;
                                s2.push_back(s[i]);
                            }
                            if(i<s.length())
                            {
                                strLi.clear();
                                strLi=s2.split("\n");
                                if(strLi.count()>0)
                                {
                                    if(strLi[0]==QString("@@"))
                                        strLi.removeFirst();
                                }
                                for(j=0,k=0,a=0;j<strLi.count();j++,k++)
                                {
                                    if(k==0)//article number
                                        ar.set_articlenumber(strLi[j]);
                                    if(k==1)//article name
                                        ar.set_name(strLi[j]);
                                    if(k==2)//maker
                                    {
                                        for(l=0;l<lsStrMaker.count();l++)//search maker & set id
                                        {
                                            if(lsStrMaker[l]==strLi[j])
                                                break;
                                        }
                                        if(l>=lsStrMaker.count())//not found?
                                            ar.set_maker_id(-1);
                                        else
                                            ar.set_maker_id(l);
                                    }
                                    if(k==3)//waregroup
                                    {
                                        for(l=0;l<lsStrWaregroup.count();l++)//search waregroup & set id
                                        {
                                            if(lsStrWaregroup[l]==strLi[j])
                                                break;
                                        }
                                        if(l>=lsStrWaregroup.count())//not found?
                                            ar.set_waregroup_id(-1);
                                        else
                                            ar.set_waregroup_id(l);
                                    }
                                    if(k==4)//comment
                                        ar.set_comment(strLi[j]);
                                    if(k==5)//inventory + max inv
                                    {
                                        strLi2.clear();
                                        strLi2=strLi[j].split(":");
                                        if(strLi2.count()==2)//two elements inv & max inv
                                        {
                                            x=strLi2[0].toInt(&b2,10);
                                            if(b2)
                                            {//cast to int ok
                                                ar.set_inventory((unsigned int)x);//inv
                                                x=strLi2[1].toInt(&b2,10);
                                                if(b2)
                                                {//cast to int ok
                                                    ar.set_max_inventory((unsigned int)x);//max inv
                                                }
                                            }
                                        }
                                    }
                                    if(k==6)//ordering
                                    {
                                        while(strLi[j]!=QString("@@"))
                                        {
                                            strLi2.clear();
                                            strLi2=strLi[j].split(":");
                                            for(l=0;l<strLi2.count();l++)
                                            {
                                                x=strLi2[l].toInt(&b2,10);
                                                if(b2)//cast to int ok?
                                                {
                                                    if(l==0)//count
                                                    {
                                                        s3=QString("%1").arg(x);
                                                        s3+=QString("x%1|").arg(a);
                                                        ord.set_wares(s3);
                                                    }
                                                    //1,2,3 = Time from ordring
                                                    if(l>=4 && l<=6)//date
                                                    {
                                                        if(l==4)
                                                            dt.setDate(0,0,x);
                                                        if(l==5)
                                                            dt.setDate(0,x,dt.day());
                                                        if(l==6)
                                                        {
                                                            dt.setDate(x,dt.month(),dt.day());
                                                            ord.set_date(dt);
                                                        }
                                                    }
                                                }
                                            }
                                            j++;
                                            if(j<strLi.count())//comment
                                                ord.set_comment(strLi[j]);
                                            j++;
                                            if(ord.get_date().isValid()==true && ord.get_wares().length()>0)
                                                lsOrdering.push_back(ord);
                                            ord.clear();
                                            if(j>=strLi.count())
                                                break;
                                        }
                                    }
                                    //-
                                    if(k>=6)
                                    {
                                        k=-1;
                                        if(ar.get_name()!=QString(""))
                                            lsArticle.push_back(ar);
                                        ar.clear();
                                        a++;
                                    }
                                }

                                //logbook
                                for(s2=QString("");i<s.length()-1;i++)
                                    s2.push_back(s[i]);
                                strLi.clear();
                                strLi=s2.split("\n");
                                for(l=0;l<strLi.count();l++)
                                {
                                    if(strLi[l]!=QString("@@@"))
                                    {
                                        //convert time & date
                                        s4=QString("");
                                        s3=strLi[l];
                                        lsInt.clear();
                                        for(a=0;a<s3.count() && lsInt.count()<6;a++)
                                        {
                                            if(s3[a].toAscii()>='0' && s3[a].toAscii()<='9')//number?
                                            {
                                                s4.push_back(s3[a]);
                                                continue;
                                            }
                                            //-
                                            if(s4.length()>0)
                                            {
                                                x=s4.toInt(&b2,10);
                                                if(b2)
                                                    lsInt.push_back(x);
                                                s4=QString("");
                                            }
                                        }
                                        //-
                                        s3=s3.remove(0,a+2);
                                        for(a=0;a<s3.count();a++)
                                        {
                                            if(s3[a].toAscii()=='\'')
                                                s3.remove(a,1);
                                        }
                                        //-
                                        if(lsInt.count()>5)
                                        {
                                            dtTi=QDateTime(QDate(lsInt[5],lsInt[4],lsInt[3]),QTime(lsInt[0],lsInt[1],lsInt[2]));
                                            if(dtTi.isValid())//really date time?
                                            {
                                                lg.set_date_time(dtTi);
                                                lg.set_text(s3);
                                                lsLogbook.push_back(lg);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            file.close();
        }

    }

    //data import
    //maker
    for(i=0;i<lsStrMaker.count();i++)
    {
        j=0;
        s3=QString("_import_");
        s2=lsStrMaker[i];
        s=QString("name = '%1'").arg(s2);
        while(m_db.maker_get_count(s)>0)//search free name
        {
            s4=QString("%1").arg(j);
            s=QString("name = '%1%2%3'").arg(s2,s3,s4);
            j++;
        }
        if(j>0)
            s2+=s3+s4;

        //insert in db
        mk.set_name(s2);
        m_db.maker_add(mk);

        //mark
        if(lsStrMaker[i]!=s2)
            lsStrMaker[i]=s2;
    }

    //waregroup
    for(i=0;i<lsStrWaregroup.count();i++)
    {
        j=0;
        s3=QString("_import_");
        s2=lsStrWaregroup[i];
        s=QString("name = '%1' AND parent_id=-1").arg(s2);
        while(m_db.waregroup_get_count(s)>0)//search free name in root dir
        {
            s4=QString("%1").arg(j);
            s=QString("name = '%1%2%3' AND parent_id=-1").arg(s2,s3,s4);
            j++;
        }
        if(j>0)
            s2+=s3+s4;

        //insert in db
        wg.set_name(s2);
        m_db.waregroup_add(wg);

        //mark
        if(lsStrWaregroup[i]!=s2)
            lsStrWaregroup[i]=s2;
    }

    //article
    for(i=0;i<lsArticle.count();i++)
    {
        j=0;
        s3=QString("_import_");
        s2=lsArticle[i].get_name();
        s=QString("name = '%1'").arg(s2);
        while(m_db.article_get_count(s)>0)//search free name
        {
            s4=QString("%1").arg(j);
            s=QString("name = '%1%2%3'").arg(s2,s3,s4);
            j++;
        }
        if(j>0)
            s2+=s3+s4;

        //set maker id
        l=-1;
        if(lsArticle[i].get_maker_id() >=0 && lsArticle[i].get_maker_id() < lsStrMaker.count())
            l=m_db.maker_get_id(lsStrMaker[lsArticle[i].get_maker_id()]);
        if(l<=0)//not found
            lsArticle[i].set_maker_id(-1);
        else
            lsArticle[i].set_maker_id(l);

        //set waregroup id
        l=-1;
        if(lsArticle[i].get_waregroup_id() >=0 && lsArticle[i].get_waregroup_id() < lsStrWaregroup.count())
            l=m_db.waregroup_get_id(lsStrWaregroup[lsArticle[i].get_waregroup_id()],-1);
        if(l<=0)//not found
            lsArticle[i].set_waregroup_id(-1);
        else
            lsArticle[i].set_waregroup_id(l);

        //insert in db
        lsArticle[i].set_name(s2);
        m_db.article_add(lsArticle[i]);
    }

    //ordering
    for(i=0;i<lsOrdering.count();i++)
    {
        s=lsOrdering[i].get_wares();
        if(s.length()>0)
            s.remove(s.count()-1,1);
        strLi=s.split("x");
        if(strLi.count()==2)//two elements count & article id?
        {
            x=strLi[0].toInt(&b2,10);
            if(b2)//cast ok?
            {
                l=strLi[1].toInt(&b2,10);
                if(b2)//cast ok?
                {
                    s2=QString("name = '%1'").arg(lsArticle[l].get_name());
                    if(m_db.article_get_count(s2)>0)//article found?
                    {
                        a=m_db.article_get_id(lsArticle[l].get_name());
                        if(a>0)
                        {
                            s2=QString("%1").arg(x);
                            s2+=QString("x%1|").arg(a);
                            lsOrdering[i].set_wares(s2);//set really id

                            //insert in db
                            m_db.ordering_add(lsOrdering[i]);
                        }
                    }
                }
            }
        }
    }

    //logbook
    for(i=0;i<lsLogbook.count();i++)
    {
        lsLogbook[i].set_type(LOGBOOK_TYPE_IMPORT);
        m_db.logbook_add(lsLogbook[i]);
    }

    //update table's
    fill_all_table();

    //clear
    lsInt.clear();
    lsLogbook.clear();
    lsOrdering.clear();
    lsArticle.clear();
    strLi.clear();
    strLi2.clear();
    lsStrMaker.clear();
    lsStrWaregroup.clear();
    return true;
}

bool MainWindow::menu_db_clear(void)
{
    if(!m_db.is_db_connect())
        return false;

    QMessageBox msg(QMessageBox::Question,"","");
    msg.setWindowTitle(QString("?"));
    QPushButton * yesButton=msg.addButton(QString("Ja"),QMessageBox::YesRole);
    msg.addButton(QString("Nein"),QMessageBox::NoRole);
    QString s=QString("Möchten Sie wirklich alle Daten der Datenbank löschen?\n(die Daten sind unwiderruflich verloren)");
    msg.setText(s);
    msg.exec();
    if(msg.clickedButton()==yesButton)
    {
        s=QString("DELETE FROM waregroup");
        m_db.exec_sql(s);
        s=QString("DELETE FROM maker");
        m_db.exec_sql(s);
        s=QString("DELETE FROM dealer");
        m_db.exec_sql(s);
        s=QString("DELETE FROM article");
        m_db.exec_sql(s);
        s=QString("DELETE FROM logbook");
        m_db.exec_sql(s);
        s=QString("DELETE FROM customer");
        m_db.exec_sql(s);
        s=QString("DELETE FROM ordering");
        m_db.exec_sql(s);
        s=QString("DELETE FROM trade");
        m_db.exec_sql(s);
        fill_all_table();
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
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString("Fehler"));
    QFile file(QString("doc/help_de.pdf"));
    bool bGiveIt=file.open(QIODevice::ReadOnly);
    file.close();
    if(!bGiveIt)
    {
        msg.setText(QString("Die Datei 'doc/help_de.pdf' ist nicht vorhanden!"));
        msg.exec();
    }
    else
    {
        QUrl url=QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+QString("/doc/help_de.pdf"));
        QDesktopServices::openUrl(url);
    }
    return true;
}

bool MainWindow::menu_about(void)
{
    QString s=QString("L A ger V erwaltungs A ssistent 2"
                      "\n(inventory management assistant)"
                      "\n------------------------------------------------------------------------"
                      "\ngeschrieben von/written by Robert Ewert - germany(Berlin)"
                      "\nrobert.ewert@gmail.com - www.robert.ewert.de.vu"
                      "\n------------------------------------------------------------------------"
                      "\nLizenz/licence: GNU GPL version 3"
                      "\n------------------------------------------------------------------------");
    s+=QString("\n\n%1").arg(VERSION);
    QMessageBox msg(QMessageBox::Information,"","");
    msg.setWindowTitle("information:");
    msg.setText(s);
    msg.exec();
    return true;
}

bool MainWindow::menu_balance_list(void)
{
    CDlgBalanceList dlg;
    dlg.setWindowTitle(QString("Artikelsaldenliste"));
    dlg.set_thread(&m_thread);
    dlg.exec();
    return true;
}

bool MainWindow::menu_article_under_warnlimit(void)
{
    QList<QString> ls;
    ls.push_back(QString("Artikelbezeichnung"));
    ls.push_back(QString("1.Artikelnummer"));
    ls.push_back(QString("2.Artikelnummer"));
    ls.push_back(QString("Hersteller"));
    ls.push_back(QString("Warengruppe"));
    ls.push_back(QString("Bestand"));
    ls.push_back(QString("Warnlimit"));

    CDlgUniList dlg;
    dlg.setWindowTitle(QString("Artikel unter Warnlimit"));
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
    ls.push_back(QString("Bestand"));
    ls.push_back(QString("Artikelbezeichnung"));
    ls.push_back(QString("1.Artikelnummer"));
    ls.push_back(QString("2.Artikelnummer"));
    ls.push_back(QString("Hersteller"));
    ls.push_back(QString("Warengruppe"));

    CDlgUniList dlg;
    dlg.setWindowTitle(QString("Artikelbestände am folgenden Datum (Tagesabschluss)"));
    dlg.set_thread(&m_thread);
    dlg.set_type(TYPE_INVENTORYS_ON_DATE);
    dlg.create_table_columns(ls);
    dlg.update_table();
    dlg.exec();
    ls.clear();
    return true;
}

bool MainWindow::menu_print_logbook(void)
{
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString("!"));
    msg.setText(QString("Es gibt an dem ausgewähltem Datum keine Logbucheinträge!"));
    //-
    QList<QListWidgetItem*> ls=ui->listWidgetLogbookMaskDate->selectedItems();
    QString s2,s=QString("Logbucheinträge vom ");
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
        m_print_job.m_memory.set_listwidget(ui->listWidgetLogbook);
        print_open_preview(s);
    }
    return true;
}

bool MainWindow::menu_print_maker_selection(void)
{
    QString s("Herstellerinformation:");
    QString sErr("Es wurde kein Hersteller ausgewählt!");
    print_list(s,sErr,ui->listWidgetMakerData,ui->tableWidgetMaker);
    return true;
}

bool MainWindow::menu_print_maker_overview(void)
{
    QString s("Herstellerübersicht:");
    QString sErr("Die Tabelle mit den Herstellern ist leer!");
    print_table(s,sErr,ui->tableWidgetMaker);
    return true;
}

bool MainWindow::menu_print_dealer_selection(void)
{
    QString s("Händlerinformation:");
    QString sErr("Es wurde kein Händler ausgewählt!");
    print_list(s,sErr,ui->listWidgetDealerData,ui->tableWidgetDealer);
    return true;
}

bool MainWindow::menu_print_dealer_overview(void)
{
    QString s("Händlerübersicht:");
    QString sErr("Die Tabelle mit den Händlern ist leer!");
    print_table(s,sErr,ui->tableWidgetDealer);
    return true;
}

bool MainWindow::menu_print_customer_selection(void)
{
    QString s("Kundeninformation:");
    QString sErr("Es wurde kein Kunde ausgewählt!");
    print_list(s,sErr,ui->listWidgetCustomerData,ui->tableWidgetCustomer);
    return true;
}

bool MainWindow::menu_print_customer_overview(void)
{
    QString s("Kundenübersicht:");
    QString sErr("Die Tabelle mit den Kunden ist leer!");
    print_table(s,sErr,ui->tableWidgetCustomer);
    return true;
}

bool MainWindow::menu_print_article_overview(void)
{
    QString s("Artikelübersicht:");
    QString sErr("Die Tabelle mit den Artrikeln ist leer!");
    print_table(s,sErr,ui->tableWidgetArticle);
    return true;
}

bool MainWindow::menu_print_inventory_overview(void)
{
    QString s("Lagerbestandsübersicht:");
    QString sErr("Die Tabelle mit dem Lagerbestand ist leer!");
    print_table(s,sErr,ui->tableWidgetInventory);
    return true;
}

bool MainWindow::menu_print_ordering_overview(void)
{
    QString s("Bestellungen(Übersicht):");
    QString sErr("Die Tabelle mit den Bestellungen ist leer!");
    print_table(s,sErr,ui->tableWidgetOrdering);
    return true;
}

bool MainWindow::menu_print_ordering_selection(void)
{
    QString s("Bestellung(Information):");
    QString sErr("Es wurde keine Bestellung ausgewählt!");
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
    QString s=QString("Warenein-/ausgänge (%1):").arg(sDates);
    QString sErr("Die Tabelle mit den Warenein-/ausgängen ist leer!");
    print_table(s,sErr,ui->tableWidgetTrade,&bAllColumns);
    return true;
}

bool MainWindow::menu_print_trade_selection(void)
{
    bool bAllColumns=true;
    QListWidget * pList=NULL;
    if(!trade_info_hide_status())
        pList=ui->listWidgetTradeInfo;
    QString s("Warenein-/ausgang(Information):");
    QString sErr("Es wurde kein Warenein-/ausgang ausgewählt!");
    print_table_and_list(s,sErr,ui->tableWidgetTradeWareslist,ui->tableWidgetTrade,pList,&bAllColumns);
    return true;
}

bool MainWindow::menu_print_waregroup(void)
{
    QString s("Warengruppen(Übersicht):");
    QString sErr("Die Warengruppenübersicht ist leer!");
    print_tree(s,sErr,ui->treeWidgetWaregroup);
    return true;
}


//print ---------------------------------------------------------------------------------------------------------------------------------
bool MainWindow::print_open_preview(QString sFirstRow)
{
    m_print_job.m_sFirstRow=sFirstRow;
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
    }
    return b;
}

bool MainWindow::print_table(QString sPageTitle, QString sError, QTableWidget * pTable, bool * pAllColumns)
{
    if(pTable==NULL)
        return false;

    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString("!"));
    msg.setText(sError);
    //-
    if(pTable->rowCount()<=0)
        msg.exec();// error nothing in table
    else
    {
        m_print_job.m_memory.clear();
        m_print_job.m_memory.set_tablewidget(pTable);
        m_print_job.m_memory.set_bool(pAllColumns);
        print_open_preview(sPageTitle);
    }
    return true;
}

bool MainWindow::print_list(QString sPageTitle, QString sError, QListWidget * pList, QTableWidget * pTable)
{
    if(pList==NULL || pTable==NULL)
        return false;

    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString("!"));
    msg.setText(sError);
    //-
    QList<QTableWidgetItem*> ls=pTable->selectedItems();
    if(ls.count()<=0)
        msg.exec();// error nothing selected
    else
    {
        ls.clear();
        m_print_job.m_memory.clear();
        m_print_job.m_memory.set_listwidget(pList);
        print_open_preview(sPageTitle);
    }
    return true;
}

bool MainWindow::print_tree(QString sPageTitle, QString sError, QTreeWidget * pTree)
{
    if(pTree==NULL)
        return false;

    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString("!"));
    msg.setText(sError);
    //-
    if(pTree->topLevelItemCount()<=0)
        msg.exec();// error nothing items at tree
    else
    {
        m_print_job.m_memory.clear();
        m_print_job.m_memory.set_treewidget(pTree);
        print_open_preview(sPageTitle);
    }
    return true;
}

bool MainWindow::print_table_and_list(QString sPageTitle, QString sError, QTableWidget * pTableData, QTableWidget * pTableInfo, QListWidget * pList, bool * pAllColumns)
{
    if(pTableData==NULL || pTableInfo==NULL)
        return false;

    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString("!"));
    msg.setText(sError);
    //-
    if(pTableData->rowCount()<=0)
        msg.exec();// error nothing in table
    else
    {
        m_print_job.m_memory.clear();
        m_print_job.m_memory.set_tablewidget(pTableData);
        m_print_job.m_memory.set_tablewidget2(pTableInfo);
        m_print_job.m_memory.set_bool(pAllColumns);
        m_print_job.m_memory.set_listwidget(pList);
        print_open_preview(sPageTitle);
    }
    return true;
}
