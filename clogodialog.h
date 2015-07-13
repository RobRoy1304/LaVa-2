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

#ifndef CLOGODIALOG_H
#define CLOGODIALOG_H

#include <QDialog>
#include <QtGui>
#include "data_classes.h"
#include "csettings.h"
#include "cdbconnection.h"

namespace Ui {
    class CLogoDialog;
}

class CLogoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CLogoDialog(QWidget *parent = 0);
    ~CLogoDialog();
    void paintEvent(QPaintEvent * event);
    bool get_start(void) {return m_bStart;}
    bool fill_users(void);
    bool fill_db_path(void);
    void set_db_interface(CDbConnection * pDb){m_pDb=pDb;}
    QString get_user(void){return m_sUser;}
    QString get_db_path(void){return m_sDbPath;}

private:
    Ui::CLogoDialog *ui;
    QImage m_image;
    bool m_bStart;
    QString m_sUser;
    QString m_sDbPath;
    CDbConnection * m_pDb;

public slots:
    bool push_start(void);
    bool browse_file(void);
    bool help(void);
};

#endif // CLOGODIALOG_H
