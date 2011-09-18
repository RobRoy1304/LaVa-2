#ifndef CINPUTDIALOGORDERING_H
#define CINPUTDIALOGORDERING_H
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

#include <QtGui/QDialog>
#include "cworkthread.h"
#include "cinputdialogarticleallowance.h"
#include "csettings.h"

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

protected://overload's
    virtual void keyPressEvent(QKeyEvent * event);
    void changeEvent(QEvent *e);
    bool fill_combo_dealer(void);

public:
    int m_iNewDealerId;

    explicit CInputDialogOrdering(QWidget *parent = 0);
    ~CInputDialogOrdering();
    void set_thread(CWorkThread * pThread);
    bool insert_ware_at_table(QString sData,bool bEdit,bool bSelect);
    bool get_data(COrdering & ord);
    bool set_data(COrdering & ord);
    bool settings(bool bUpdate=false);

public slots:
    //context menu
    bool open_context_menu();
    bool receiv_context_menu(QAction * pAction);

    //-
    bool add_ware(void);
    bool delete_ware(void);
    bool edit_ware(void);
    bool check_user_input(void);
    bool edit_maker_combobox(QString s);
    bool pressed_ok(void);
};

#endif // CINPUTDIALOGORDERING_H
