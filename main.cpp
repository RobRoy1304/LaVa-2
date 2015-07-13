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
    if(QFile::exists(a.applicationDirPath() + "/plugins"))
        a.addLibraryPath( a.applicationDirPath() + "/plugins" );

    //languages
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));//for german spezial letter
    QTranslator translator;
    if(translator.load("qt_de.qm", "")) // German for common qt widgets
        a.installTranslator(&translator); //for standard widgets texts

    //icon
    QIcon ico(QString::fromUtf8(":/images/res/icon.ico"));
    if(!ico.isNull())
        a.setWindowIcon(ico);

    //start
    CSettings settings;
    if(w.check_first_start())//first start? -> agreement yes?
    {
        if(!w.check_run_the_program())//don't running?
        {
            //logo
            logo_dlg.set_db_interface(&w.m_db);//set db-interface
            logo_dlg.exec();
            if(!logo_dlg.get_start())//start?
                settings.remove_line(QString::fromUtf8("PROGRAM_RUNNING"));
            else
            {
                //user name
                w.set_user(logo_dlg.get_user());
                w.set_db_path(logo_dlg.get_db_path());

                //init
                w.init();

                //fill lists
                w.fill_date_lists();

                //load settings
                w.settings(false);

                //fill tables
                w.fill_all_table(true);

                //fill logbook count
                w.fill_logbook_count();

                //warnlinit
                w.inventory_check_article_under_warnlimit();

                //timer
                w.startTimer(3000);//every 3sec.

                //show
                w.showMaximized();

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
