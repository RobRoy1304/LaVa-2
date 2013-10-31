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

#ifndef CINPUTDIALOGDEALER_H
#define CINPUTDIALOGDEALER_H

#include <QtGui/QDialog>
#include "cworkthread.h"

namespace Ui {
    class CInputDialogDealer;
}

class CInputDialogDealer : public QDialog
{
    Q_OBJECT

private:
    Ui::CInputDialogDealer *ui;
    CWorkThread * m_pThread;

    //only for edit
    int m_iMarkId;
    QString m_sMarkName;

public:
    explicit CInputDialogDealer(QWidget *parent = 0);
    ~CInputDialogDealer();
    bool get_data(CDealer & de);
    bool set_data(CDealer & de);
    void set_thread(CWorkThread * pThread){m_pThread=pThread;}

public slots:
    bool check_user_input(QString s);
    void press_ok(void);
    void press_cancel(void);
};

#endif // CINPUTDIALOGDEALER_H
