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
#ifndef CINPUTDIALOGORDERING_H
#define CINPUTDIALOGORDERING_H

#include <QtGui/QDialog>
#include "cworkthread.h"
#include "cinputdialogarticleallowance.h"
#include "csettings.h"
#include "clastdbchange.h"
#include "cinputdlgbarcode.h"

namespace Ui {
    class CInputDialogOrdering;
}

class CInputDialogOrdering : public QDialog
{
    Q_OBJECT

private:
    Ui::CInputDialogOrdering *ui;
    CWorkThread * m_pThread;
    QMenu * m_pContextMenu; //table context menu
    int m_iTimerId;
    CLastDbChange m_LastDbChange;//class to check-> db change from another client

    //only for edit mode
    int m_iMarkId;
    QString m_sMarkWares;

public:
    int m_iNewDealerId;
    explicit CInputDialogOrdering(QWidget *parent = 0);
    ~CInputDialogOrdering();
    virtual void keyPressEvent(QKeyEvent * event);
    void timerEvent(QTimerEvent *event);
    bool fill_combo_dealer(void);
    void set_thread(CWorkThread * pThread);
    bool insert_ware_at_table(QString sData,bool bEdit,bool bSelect);
    bool get_data(COrdering & ord);
    bool set_data(COrdering & ord);
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
    bool edit_maker_combobox(QString s);
    void press_ok(void);
    void press_cancel(void);
};

#endif // CINPUTDIALOGORDERING_H
