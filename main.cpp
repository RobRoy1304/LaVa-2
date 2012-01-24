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

#include <QtGui/QApplication>
#include "mainwindow.h"
#include "csettings.h"
#include "clogodialog.h"


int main(int argc, char *argv[])
{
    int iReturn=0;
    QApplication a(argc, argv);
    MainWindow w;
    CLogoDialog logo_dlg;

    //plugins
    a.addLibraryPath( a.applicationDirPath() + "/plugins" );

    //languages
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));//for german spezial letter
    QTranslator translator;
    if(translator.load("qt_de.qm", "")) // German for common qt widgets
        a.installTranslator(&translator); //for standard widgets texts

    //icon
    QIcon ico(QString(":/images/res/icon.ico"));
    if(!ico.isNull())
        a.setWindowIcon(ico);

    //start
    if(w.check_first_start())//first start? -> agreement yes?
    {
        if(!w.check_run_the_program())//don't running?
        {
            if(w.open_db())//open db ok?
            {
                //init
                w.init();

                //timer
                w.startTimer(3000);//every 3sec.

                //show
                w.showMaximized();

                //load settings
                w.settings(false);

                //logo
                logo_dlg.startTimer(2000);//start timer in 2 sec. close logo widget
                logo_dlg.exec();

                //start
                iReturn=a.exec();

                //save settings
                w.settings(true);
            }
        }
    }
    //-
    return iReturn;
}
