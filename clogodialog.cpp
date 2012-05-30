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

#include "clogodialog.h"
#include "ui_clogodialog.h"

CLogoDialog::CLogoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLogoDialog)
{
    ui->setupUi(this);

    m_image.load(":/images/res/logo.png");

    setFixedSize(m_image.width(),m_image.height());
    setWindowFlags(Qt::FramelessWindowHint);//no border
    setWindowFlags(Qt::SplashScreen);
}

CLogoDialog::~CLogoDialog()
{
    delete ui;
}

void CLogoDialog::timerEvent(QTimerEvent *event)
{
    if(event!=NULL)
        close();
}

void CLogoDialog::paintEvent(QPaintEvent * event)
{
    if(event!=NULL)
    {
        QPainter painter(this);
        QRect r=rect();
        if(!m_image.isNull())
        {
            painter.setPen(QColor(128,128,128));
            painter.drawImage(0,0,m_image);
            painter.drawText(20,r.height()-40,VERSION);
        }
    }
}
