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

#ifndef CINPUTDIALOGBROWSEWAREGROUP_H
#define CINPUTDIALOGBROWSEWAREGROUP_H

#include <QtGui/QDialog>
#include "cworkthread.h"
#include "csettings.h"

namespace Ui {
    class CInputdialogBrowseWaregroup;
}

class CInputdialogBrowseWaregroup : public QDialog {
    Q_OBJECT

public:
    CWorkThread * m_pThread;

    CInputdialogBrowseWaregroup(QWidget *parent = 0);
    ~CInputdialogBrowseWaregroup();
    bool set_thread(CWorkThread * pThread);
    bool updateWaregroup(int iIdSelect);
    bool get_selected_waregroup(QString & s, int & id);
    bool settings(bool bUpdate=false);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CInputdialogBrowseWaregroup *m_ui;
};

#endif // CINPUTDIALOGBROWSEWAREGROUP_H
