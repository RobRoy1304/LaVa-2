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

#ifndef CLOGODIALOG_H
#define CLOGODIALOG_H

#include <QDialog>
#include <QtGui>
#include "data_classes.h"

namespace Ui {
    class CLogoDialog;
}

class CLogoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CLogoDialog(QWidget *parent = 0);
    ~CLogoDialog();
    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent * event);

private:
    Ui::CLogoDialog *ui;
    QImage m_image;
};

#endif // CLOGODIALOG_H
