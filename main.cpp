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
    bool bStart=true,bDeleteSetting=false;
    QApplication a(argc, argv);
    MainWindow w;
    CLogoDialog logo_dlg;
    QMessageBox msgError(QMessageBox::Critical,"","");
    QMessageBox msgWarning(QMessageBox::Warning,"","");
    QMessageBox msgAgreement(QMessageBox::Information,"","");
    QString s;
    QDesktopWidget desktop;
    QRect r=desktop.availableGeometry();

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

    //settings
    CSettings settings;
    bool bGiveItSetting=settings.give_it_setting_file();
    if(!bGiveItSetting)//first start?
        settings.create_default_setting_file();//create default

    if(settings.is_program_running())//run the program?
    {
        msgWarning.setWindowTitle(QString("Warnung"));
        s=QString("Das Programm läuft bereits oder wurde das letzte Mal gewaltsam beendet. Bitte überprüfen Sie das!");
        msgWarning.setText(s);
        msgWarning.exec();
        settings.remove_line(QString("PROGRAM_RUNNING"));
        bStart=false;
    }
    else
    {
        s=QString("PROGRAM_RUNNING");//set program running
        settings.write(s,QString(""));

        if(r.width()<1000 ||r.height()<600)//screen size error?(min 1000x600)
        {
            msgError.setWindowTitle("Fehler!");
            s=QString("Die Bildschirmauflösung ist zu klein, das Programm setzt eine Auflösung von min. 1000x600 Px vorraus.");
            msgError.setText(s);
            msgError.exec();
            bDeleteSetting=true;
            bStart=false;
        }
        else
        {
            if(!bGiveItSetting)//first start? -> agreement
            {
                msgAgreement.setWindowTitle(QString("Nutzungsbedingungen:"));
                s=QString("Dieses Programm(LaVa 2) steht unter der open-source-Lizenz GNU-GPL v3. Es entstehen für die Nutzung "
                          "keine Kosten. Der Autor entbindet sich von jeglichen Haftungsansprüchen, gleich welcher Art. "
                          "Das Programm befindet sich momentan noch im Beta-Status, und ist somit noch nicht für "
                          "den produktiven Einsatz gedacht.\n\nSind Sie mit diesen Bedingungen einverstanden?");
                msgAgreement.setText(s);
                msgAgreement.addButton(QString("Ja"),QMessageBox::YesRole);
                QPushButton * noButton=msgAgreement.addButton(QString("Nein"),QMessageBox::NoRole);
                msgAgreement.exec();
                if(msgAgreement.clickedButton()==noButton)
                {
                    bStart=false;
                    bDeleteSetting=true;
                }
            }

            //db
            if(bStart)
            {
                if(!w.open_db())//open db ok?
                    bStart=false;
                else
                {
                    //init
                    w.init();

                    //timer
                    w.startTimer(3000);//every 3sec.

                    //show
                    w.showMaximized();

                    //load settings
                    w.settings(false);//load settings

                    //logo
                    logo_dlg.startTimer(3000);//start timer in 3 sec. close logo widget
                    logo_dlg.exec();
                }
            }
        }
    }

    if(bDeleteSetting)
        QFile::remove(QString("settings.txt"));//delete default setting file)

    int iReturn=0;
    if(bStart)//start programm?
        iReturn=a.exec();
    return iReturn;
}
