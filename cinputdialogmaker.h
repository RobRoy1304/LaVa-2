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

#ifndef CINPUTDIALOGMAKER_H
#define CINPUTDIALOGMAKER_H

#include <QtGui/QDialog>
#include "cworkthread.h"

namespace Ui {
    class CInputDialogMaker;
}

class CInputDialogMaker : public QDialog
{
    Q_OBJECT

private:
    Ui::CInputDialogMaker *ui;
    QString m_sMarkName;
    CWorkThread * m_pThread;

public:
    explicit CInputDialogMaker(QWidget *parent = 0);
    ~CInputDialogMaker();
    bool get_data(CMaker & mk);
    bool set_data(CMaker & mk);
    void set_thread(CWorkThread * pThread){m_pThread=pThread;}


protected:
    void changeEvent(QEvent *e);

public slots:
    bool check_user_input(QString s);


};

#endif // CINPUTDIALOGMAKER_H
