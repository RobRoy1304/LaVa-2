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

#ifndef CINPUTDLGOUTGOINGCUSTOMER_H
#define CINPUTDLGOUTGOINGCUSTOMER_H

#include <QtGui/QDialog>
#include "cworkthread.h"
#include "cinputdialogarticleallowance.h"
#include "csettings.h"
#include "clastdbchange.h"
#include "cinputdlgbarcode.h"

namespace Ui {
    class CInputDlgOutgoingCustomer;
}

class CInputDlgOutgoingCustomer : public QDialog {
    Q_OBJECT

private:
    Ui::CInputDlgOutgoingCustomer *ui;
    QMenu * m_pContextMenu; //table context menu
    int m_iTimerId;
    CLastDbChange m_LastDbChange;//class to check-> db change from another client
    QDate m_dtMark;//to check is booking number handmake or not
    CWorkThread * m_pThread;

public:

    CInputDlgOutgoingCustomer(QWidget *parent = 0);
    ~CInputDlgOutgoingCustomer();
    virtual void keyPressEvent(QKeyEvent * event);
    void timerEvent(QTimerEvent *event);
    bool set_thread(CWorkThread * pThread);
    bool insert_ware_at_table(QString sData,bool bEdit,bool bSelect);
    bool get_data(CTrade & tr);
    bool init(void);
    bool check_article_counts_on_date(void);
    bool settings(bool bUpdate=false);
    bool update_wares_table(void);

public slots:
    //context menu
    bool open_context_menu();
    bool receiv_context_menu(QAction * pAction);

    bool update_customer_table(void);
    void checkbox_auto_clicked(void);
    void mask_edit(void);
    bool add_ware(void);
    bool add_ware_barcode(void);
    bool delete_ware(void);
    bool edit_ware(void);
    bool check_user_input(void);
    bool set_booking_number_nomination(void);
    bool date_changed(QDate dtNew);
    void press_ok(void);
    void press_cancel(void);
};

#endif // CINPUTDLGOUTGOINGCUSTOMER_H
