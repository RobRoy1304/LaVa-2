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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QTreeWidgetItem>
#include <QMessageBox>

#include "cworkthread.h"
#include "cdbconnection.h"
#include "cwidgetinterface.h"
#include "cinputdialogwaregroup.h"
#include "cinputdialogordering.h"
#include "cinputdialogmaker.h"
#include "cinputdialogdealer.h"
#include "cinputdialogarticle.h"
#include "cinputdialogcustomer.h"
#include "cinputdlgtrade.h"
#include "cinputdlgorderingincoming.h"
#include "cinputdlgoutgoingcustomer.h"
#include "cprintjob.h"
#include "csettings.h"
#include "cdlgbalancelist.h"
#include "cdlgunilist.h"

namespace Ui
{
    class MainWindow;
}

//-register-
Q_DECLARE_METATYPE(Qt::SortOrder)
Q_DECLARE_METATYPE(QItemSelection)


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
public:
    int m_iTimerId;
    CWorkThread m_thread;//work thread
    CDbConnection m_db;//database interface
    CWidgetInterface m_widgets;//fill tables/lists/tree...
    CPrintJob m_print_job;//func's for print jobs
    QMenu * m_pContextMenuDefault; //table context menu default(new,edit,remove)
    QMenu * m_pContextMenuTrade; //table context menu (incoming,outgoing,ordering incoming,customer outgoing,canceled
    QMenu * m_pContextMenuArticle; //table context menu article(new,edit,copy,remove)
    QString m_sDbPath;//path for database-file

protected://overload's
    virtual void resizeEvent(QResizeEvent * event);
    virtual void keyPressEvent(QKeyEvent * event);

public:
    //basic
    bool init(void);
    bool widgets_position(QSize szScreen);
    bool open_db(void);
    bool fill_all_table(void);
    bool settings(bool bUpdate=false);
    bool check_first_start(void);
    bool check_run_the_program(void);
    void timerEvent(QTimerEvent *event);

    //waregroup
    bool waregroup_update_tree(int iIdSelect=-1);
    bool waregroup_check_user_input(void);
    bool waregroup_update_count(QString sCondition=QString(""));

    //maker
    bool maker_update_info(void);
    bool maker_check_user_input(void);
    bool maker_update_count(void);

    //dealer
    bool dealer_update_info(void);
    bool dealer_check_user_input(void);
    bool dealer_update_count(void);

    //customer
    bool customer_update_info(void);
    bool customer_check_user_input(void);
    bool customer_update_count(void);
    bool customer_update_customernumber_trade(QString sOld, QString sNew);

    //article
    bool article_check_user_input(void);
    bool article_update_count(void);

    //ordering
    bool ordering_check_user_input(void);
    bool ordering_update_count(void);
    bool ordering_update_wares_list(void);

    //trade
    int trade_get_selected_year(void);
    bool trade_info_hide_status(void);
    bool trade_get_selected(CTrade & trade);
    bool trade_update_wares_list(void);
    bool trade_check_user_input(void);
    bool trade_update_count(void);
    bool trade_hide_info_list(bool bHide);
    bool trade_refresh_datelist(void);
    bool trade_update_datelist(QDate dtSelect,int iYear);
    bool trade_check_diff_ordering(int iOrdId, CTrade & tr);

    //inventory
    bool inventory_update_count(void);
    int inventory_get_count_of_arctile_under_limit(void);

    //logbook
    QDate logbook_get_selected_date(void);
    int logbook_get_selected_year(void);
    bool logbook_update_count(void);
    bool logbook_insert(CLogbook & lg);
    bool logbook_refresh_datelist(void);
    bool logbook_update_datelist(QDate dtSelect,int iYear);
    bool logbook_check_items_over_count(void);

    //print
    bool print_open_preview(QString sFirstRow);
    bool print_table(QString sPageTitle, QString sError, QTableWidget * pTable, bool * pAllColumns=NULL);
    bool print_list(QString sPageTitle, QString sError, QListWidget * pList, QTableWidget * pTable);
    bool print_tree(QString sPageTitle, QString sError, QTreeWidget * pTree);
    bool print_table_and_list(QString sPageTitle, QString sError, QTableWidget * pTableData, QTableWidget * pTableInfo, QListWidget * pList=NULL, bool * pAllColumns=NULL);


public slots:
    //tab
    void tab_switched(void);

    //context menu
    bool open_context_menu();
    bool receiv_context_menu(QAction * pAction);

    //waregroup
    bool waregroup_new(void);
    bool waregroup_edit(void);
    bool waregroup_delete(void);
    bool waregroup_treeitem_clicked(void);

    //maker
    bool maker_mask_edit(void);
    bool maker_checkbox_auto_clicked();
    bool maker_widgetitem_clicked();
    bool maker_mask_set(void);
    bool maker_new(void);
    bool maker_edit(void);
    bool maker_delete(void);

    //dealer
    bool dealer_mask_edit(void);
    bool dealer_checkbox_auto_clicked();
    bool dealer_widgetitem_clicked();
    bool dealer_mask_set(void);
    bool dealer_new(void);
    bool dealer_edit(void);
    bool dealer_delete(void);

    //customer
    bool customer_mask_edit(void);
    bool customer_checkbox_auto_clicked();
    bool customer_widgetitem_clicked();
    bool customer_mask_set(void);
    bool customer_new(void);
    bool customer_edit(void);
    bool customer_delete(void);

    //article
    bool article_mask_edit(void);
    bool article_checkbox_auto_clicked();
    bool article_widgetitem_clicked();
    bool article_mask_set(void);
    bool article_new(void);
    bool article_edit(void);
    bool article_copy(void);
    bool article_delete(void);

    //ordering
    bool ordering_mask_edit(void);
    bool ordering_checkbox_auto_clicked();
    bool ordering_widgetitem_clicked();
    bool ordering_mask_set(void);
    bool ordering_new(void);
    bool ordering_edit(void);
    bool ordering_delete(void);

    //trade
    bool trade_mask_edit(void);
    bool trade_checkbox_auto_clicked();
    bool trade_mask_date_changed(void);
    bool trade_mask_set(bool bRefreshDateList=true);
    bool trade_date_mask_select_changed(void);
    bool trade_widgetitem_clicked();
    bool trade_incoming(void);
    bool trade_outgoing(void);
    bool trade_ordering_incoming(void);
    bool trade_outgoing_customer(void);
    bool trade_canceled(void);

    //inventory
    bool inventory_mask_edit(void);
    bool inventory_checkbox_auto_clicked();
    bool inventory_mask_set(void);
    bool inventory_widgetitem_clicked();

    //logbook
    bool logbook_mask_set(void);
    bool logbook_date_mask_year_changed(void);
    bool logbook_combo_mask_changed(void);
    bool logbook_change_count_of_item(void);
    bool logbook_clear(void);

    //menu
    bool menu_db_backup_create(void);
    bool menu_db_backup_load(void);
    bool menu_db_import_from_lava1(void);
    bool menu_db_clear(void);
    bool menu_tool_close(void);
    bool menu_about(void);
    bool menu_help(void);
    bool menu_balance_list(void);
    bool menu_article_under_warnlimit(void);
    bool menu_inventorys_on_date(void);
    bool menu_print_logbook(void);
    bool menu_print_maker_selection(void);
    bool menu_print_maker_overview(void);
    bool menu_print_dealer_selection(void);
    bool menu_print_dealer_overview(void);
    bool menu_print_customer_selection(void);
    bool menu_print_customer_overview(void);
    bool menu_print_article_overview(void);
    bool menu_print_inventory_overview(void);
    bool menu_print_ordering_overview(void);
    bool menu_print_ordering_selection(void);
    bool menu_print_trade_overview(void);
    bool menu_print_trade_selection(void);
    bool menu_print_waregroup(void);

    //print
    bool print(QPrinter * printer);
};

#endif // MAINWINDOW_H
