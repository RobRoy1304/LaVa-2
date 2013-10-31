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

#include <QtGui>
#include <QDialog>
#include "csettings.h"
#include "data_classes.h"

#ifndef CINPUTDIALOGTABLESETTING_H
#define CINPUTDIALOGTABLESETTING_H

namespace Ui {
    class CInputDialogTableSetting;
}

class CInputDialogTableSetting : public QDialog
{
    Q_OBJECT

private:
    Ui::CInputDialogTableSetting *ui;
    CTableColumnsData m_tcdNewSetting;

public:
    explicit CInputDialogTableSetting(QWidget *parent = 0);
    ~CInputDialogTableSetting();
    bool set_data(CTableColumnsData & tcdData);
    bool get_data(CTableColumnsData & tcdData, QList<QString> & lsHeaderNames);
    bool fill_table(void);

public slots:
    bool check_user_input(void);
    bool button_up(void);
    bool button_down(void);
    bool button_left(void);
    bool button_center(void);
    bool button_right(void);
    bool button_visible(void);
};

#endif // CINPUTDIALOGTABLESETTING_H
