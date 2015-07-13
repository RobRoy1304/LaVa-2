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

#ifndef CINPUTDLGTRADE_H
#define CINPUTDLGTRADE_H

#include <QtGui/QDialog>
#include "cworkthread.h"
#include "cinputdialogarticleallowance.h"
#include "csettings.h"
#include "clastdbchange.h"
#include "cinputdlgbarcode.h"

namespace Ui {
    class CInputDlgTrade;
}

class CInputDlgTrade : public QDialog {
    Q_OBJECT
private:
    Ui::CInputDlgTrade *ui;
    QMenu * m_pContextMenu; //table context menu
    QDate m_dtMark;//to check is booking number handmake or not
    int m_iTimerId;
    CLastDbChange m_LastDbChange;//class to check-> db change from another client
    CWorkThread * m_pThread;
    bool m_bOutgoing;

public:
    CInputDlgTrade(QWidget *parent = 0);
    ~CInputDlgTrade();
    virtual void keyPressEvent(QKeyEvent * event);
    void timerEvent(QTimerEvent *event);
    void set(CWorkThread * pThread,bool bOutgoing);
    bool insert_ware_at_table(QString sData,bool bEdit, bool bSelect);
    bool get_data(CTrade & tr);
    bool check_article_counts_on_date(void);
    bool settings(bool bUpdate=false);
    bool update_wares_table(void);

public slots:
    //context menu
    bool open_context_menu();
    bool receiv_context_menu(QAction * pAction);

    //-
    bool add_ware(void);
    bool add_ware_barcode(void);
    bool delete_ware(void);
    bool edit_ware(void);
    bool check_user_input(void);
    bool date_changed(QDate dtNew);
    bool set_booking_number_nomination(void);
    void press_ok(void);
    void press_cancel(void);
};

#endif // CINPUTDLGTRADE_H
