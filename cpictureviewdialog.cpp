#include "cpictureviewdialog.h"
#include "ui_cpictureviewdialog.h"

CPictureViewDialog::CPictureViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPictureViewDialog)
{
    ui->setupUi(this);

    //black background
    QPalette pal;
    pal.setColor(QPalette::Background,Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);
}

CPictureViewDialog::~CPictureViewDialog()
{
    delete ui;
}

void CPictureViewDialog::paintEvent(QPaintEvent * event)
{
    int x=0,y=0;
    QImage img;
    QRect r=rect();
    QPainter painter(this);

    if(event!=NULL && m_image.isNull()==false)
    {
        img=m_image.scaled(r.width(),r.height(),Qt::KeepAspectRatio);
        if(img.width() < r.width())
            x=(r.width()-img.width())/2;
        if(img.height() < r.height())
            y=(r.height()-img.height())/2;
        painter.drawImage(x,y,img);
    }
}

bool CPictureViewDialog::set_data(QString sPath)
{
    QImage img;
    QSize sz,szWindow;
    bool bReturn=false;
    QDesktopWidget desktop;
    QRect r=desktop.availableGeometry();
    QFile file(sPath);
    QString s;
    QMessageBox msg(this);
    msg.setWindowTitle(QString("!"));
    //-
    if(!file.exists())//path wrong
    {
        s=QString::fromUtf8("Die Bilddatei unter \"%1\" wurde nicht gefunden!").arg(sPath);
        msg.setText(s);
        msg.exec();
    }
    else if(!m_image.load(sPath))
    {
        s=QString::fromUtf8("Die Datei unter \"%1\" ist keine Bilddatei!").arg(sPath);
        msg.setText(s);
        msg.exec();
    }
    else
    {
        //size
        sz=m_image.size();

        //picture size & screen resoltution
        if(sz.width()>=r.width())
            szWindow.setWidth(r.width());
        else
            szWindow.setWidth(sz.width());

        if(sz.height()>=r.height())
            szWindow.setHeight(r.height());
        else
            szWindow.setHeight(sz.height());

        //resize window (scaled size image)
        img=m_image.scaled(szWindow.width(),szWindow.height(),Qt::KeepAspectRatio);
        resize(img.width(),img.height());

        //set window min size
        setMinimumSize(100,100);

        //set path as title
        setWindowTitle(sPath);
        //-
        bReturn=true;
    }
    return bReturn;
}
