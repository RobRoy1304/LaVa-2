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

#ifndef CINPUTDIALOGEXPORT_H
#define CINPUTDIALOGEXPORT_H

#include <QtGui/QDialog>
#include "csettings.h"

namespace Ui {
class CInputDialogExport;
}

class CInputDialogExport : public QDialog
{
    Q_OBJECT

private:
    Ui::CInputDialogExport *ui;

public:
    explicit CInputDialogExport(QWidget *parent = 0);
    ~CInputDialogExport();
    bool settings(bool bUpdate=false);
    bool get_data(QString & sSplitChar, QString & sSplitText, int & iCodecType);

public slots:
    bool update_data_examble(void);
};

#endif // CINPUTDIALOGEXPORT_H
