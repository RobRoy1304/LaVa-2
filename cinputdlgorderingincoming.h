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

#ifndef CINPUTDLGORDERINGINCOMING_H
#define CINPUTDLGORDERINGINCOMING_H

#include <QtGui/QDialog>
#include "cworkthread.h"
#include "csettings.h"
#include "clastdbchange.h"

namespace Ui {
    class CInputDlgOrderingIncoming;
}

class CInputDlgOrderingIncoming : public QDialog {
    Q_OBJECT

private:
    CWorkThread * m_pThread;
    Ui::CInputDlgOrderingIncoming *ui;
    QMenu * m_pContextMenu; //table context menu
    QDate m_dtMark;//to check is booking number handmake or not
    int m_iTimerId;
    CLastDbChange m_LastDbChange;//class to check-> db change from another client

public:
    CInputDlgOrderingIncoming(QWidget *parent = 0);
    ~CInputDlgOrderingIncoming();
    virtual void keyPressEvent(QKeyEvent * event);
    void timerEvent(QTimerEvent *event);
    bool init(void);
    bool get_data(CTrade & trade, int & iOrderingId, bool & bCompleteIncoming);
    bool set_thread(CWorkThread * pThread);
    bool fill_wares_table(void);
    bool fill_ordering_table(void);
    bool settings(bool bUpdate=false);

public slots:
    //context menu
    bool open_context_menu();
    bool receiv_context_menu(QAction * pAction);
    //-
    bool ordering_table_selected(void);
    bool wares_table_selected(void);
    bool ware_count_edit(void);
    bool double_click_warelist_table(void);
    void mask_changed(void);
    void checkbox_auto_clicked(void);
    void mask_edit(void);
    bool set_booking_number_nomination(void);
    bool date_changed(QDate dtNew);
    bool check_user_input(void);
    void press_ok(void);
    void press_cancel(void);
};

#endif // CINPUTDLGORDERINGINCOMING_H
