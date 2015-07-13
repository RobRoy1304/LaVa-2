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

#include "clogodialog.h"
#include "ui_clogodialog.h"

CLogoDialog::CLogoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLogoDialog)
{
    ui->setupUi(this);
    m_pDb=NULL;
    m_bStart=false;
    m_image.load(":/images/res/logo.png");
    fill_users();
    fill_db_path();
    setFixedSize(m_image.width(),m_image.height());

    //white label text
    QPalette palette(ui->label_user->palette());
    palette.setColor(QPalette::WindowText,Qt::white);
    ui->label_user->setPalette(palette);
    ui->label_path->setPalette(palette);

    //setWindowFlags(Qt::FramelessWindowHint);//no border
    //setWindowFlags(Qt::SplashScreen);

    connect(ui->pushButtonStart,SIGNAL(clicked()),this,SLOT(push_start()));
    connect(ui->pushButtonBrowseFile,SIGNAL(clicked()),this,SLOT(browse_file()));
    connect(ui->pushButtonHelp,SIGNAL(clicked()),this,SLOT(help()));
}

CLogoDialog::~CLogoDialog()
{
    delete ui;
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
            painter.drawText(20,r.height()-140,VERSION);
        }
    }
}

bool CLogoDialog::push_start(void)
{
    if(m_pDb==NULL)
    {//error no database interface
        close();
        return false;
    }

    int i;
    QFileInfo file_info;
    QDateTime dtTiLastSession;
    QDateTime dtTi=QDateTime().currentDateTime();
    QString s,sPath,sError;
    QString curSel=ui->comboBoxUser->currentText();
    QStringList sls;
    CSettings settings;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(tr("Fehler!"));

    //database path------------------------check & open db
    sPath=ui->lineEdit->text();
    if(!QFile::exists(sPath))
        sError=QString::fromUtf8("Die angegebene Datei \"%1\" existiert nicht!").arg(sPath);
    else
    {
        file_info.setFile(sPath);
        if(!file_info.permission(QFile::WriteUser))
            sError=QString::fromUtf8("Sie haben keine Schreibrechte an dem Ort \"%1\"!").arg(sPath);
        else
        {
            if(!m_pDb->open_db(sPath,QString::fromUtf8("localhost")))//open db?
            {
                s=m_pDb->last_error();
                sError=QString::fromUtf8("Die Datei \"%1\" konnte nicht geöffnet werden!\n(error: %2)").arg(sPath,s);
            }
            else
            {
                if(!m_pDb->check_and_update_db_version())//database old version? -> update
                    sError=QString::fromUtf8("Fehler beim update der Datenbankdatei auf Version %1!").arg(CURRENT_DB_VERSION);
                else
                {
                    if(!m_pDb->check_database_file(true))
                        sError=QString::fromUtf8("Die Datei \"%1\" ist beschädigt oder nicht kompatibel!").arg(sPath);
                    else
                    {
                        //save path->settings
                        settings.write(QString::fromUtf8("DB_PATH"),sPath);//save setting

                        //mark path
                        m_sDbPath=sPath;

                        //check last session(time) and current
                        dtTiLastSession=m_pDb->get_last_session();
                        if(dtTiLastSession>QDateTime(QDate(0,0,0),QTime(0,0,0)))//set?
                        {
                            if(dtTiLastSession.date()>dtTi.date())
                            {
                                s=QString::fromUtf8("Bitte prüfen Sie Ihre Systemzeit-/datumseinstellungen Ihres Betriebssystems. Das Datum Ihrer letzten Sitzung "
                                           "ist später als das aktuelle Datum. Falsche Einstellungen können die chronologische Reihenfolge der Artikelmengen"
                                           " beim Warenausgang beeinflussen.");
                                msg.setWindowTitle(QString::fromUtf8("Achtung!"));
                                msg.setText(s);
                                msg.exec();
                            }
                        }
                    }
                }
            }
        }
    }
    //-------------------------------------


    if(sError.length()>0)
    {
        msg.setText(sError);
        msg.exec();
        m_pDb->close_db();
    }
    else
    {
        //user name-----------------------------
        //mark user name
        if(settings.load(QString::fromUtf8("USERS"),s))
        {
            if(s.length()>0)
                sls=s.split(",");
        }

        //add current selection text in front
        sls.push_front(curSel);

        //remove dupl.
        sls.removeDuplicates();

        //max mark user = 10
        while(sls.count()>10)
            sls.removeLast();

        //save user setting
        for(i=0,s=QString::fromUtf8("");i<sls.count();i++)
        {
            if(sls[i]!=QString::fromUtf8(""))
            {
                s+=sls[i];
                if(i+1<sls.count())//not last
                    s+=QString::fromUtf8(",");
            }
        }
        settings.write(QString::fromUtf8("USERS"),s);//save setting
        sls.clear();
        //-------------------------------------------

        m_sUser=curSel;
        m_bStart=true;
        close();
    }
    return true;
}

bool CLogoDialog::fill_users(void)
{
    QString s;
    CSettings settings;
    QStringList sls;
    QString CurUserName;

    //user name
    CurUserName=qgetenv( "USER" );//current user name (by OS)
    if(CurUserName==QString::fromUtf8(""))//OS no support qgetenv
    {
        QDir dir(QDir::home());
        CurUserName=dir.dirName();
    }

    if(settings.load(QString::fromUtf8("USERS"),s))
    {
        if(s.length()>0)
            sls=s.split(",");
    }
    if(sls.count()<=0)//empty?
        ui->comboBoxUser->addItem(CurUserName);//add user name fom OS
    else
        ui->comboBoxUser->addItems(sls);//add lastest users
    //-
    sls.clear();
    return true;
}

bool CLogoDialog::fill_db_path(void)
{
    QString s,sDbPath;
    CSettings settings;

    //default database path's
    if(QFile::exists(QDir::homePath()+QString::fromUtf8("/lava2/")))//aplication(lava2) installed?
        sDbPath=QDir::homePath()+QString::fromUtf8("/lava2/lava.sqlite");
    else
        sDbPath=QDir::currentPath()+QString::fromUtf8("/lava.sqlite");//not installed

    //setting
    if(settings.load(QString::fromUtf8("DB_PATH"),s))
    {
        if(s.length()>0)
            sDbPath=s;//load last path
    }

    //set
    ui->lineEdit->setText(sDbPath);
    return true;
}

bool CLogoDialog::browse_file(void)
{
    QString sOldPath=ui->lineEdit->text();
    QString sFile=QFileDialog::getOpenFileName(this,QString::fromUtf8("Datenbankdatei wählen"),sOldPath);
    if(sFile.length()>0)
    {//load button pressed?
        ui->lineEdit->setText(sFile);
    }
    return true;
}

bool CLogoDialog::help(void)
{
    QString s;
    QMessageBox msg(QMessageBox::Critical,"","");
    msg.setWindowTitle(QString::fromUtf8("Fehler"));
    if(!QFile::exists(QCoreApplication::applicationDirPath()+QString::fromUtf8("/doc/help_de.pdf")))//help file not found
    {
        s=QString::fromUtf8("Die Datei \"%1\" ist nicht vorhanden!").arg(QCoreApplication::applicationDirPath()+QString::fromUtf8("/doc/help_de.pdf"));
        msg.setText(s);
        msg.exec();
    }
    else
    {
        QUrl url=QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+QString::fromUtf8("/doc/help_de.pdf"));
        QDesktopServices::openUrl(url);
    }
    return true;
}
