#ifndef CPICTUREVIEWDIALOG_H
#define CPICTUREVIEWDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QDesktopWidget>

namespace Ui {
class CPictureViewDialog;
}

class CPictureViewDialog : public QDialog
{
    Q_OBJECT

private:
    QImage m_image;

public:
    explicit CPictureViewDialog(QWidget *parent = 0);
    ~CPictureViewDialog();
    bool set_data(QString sPath);

private:
    void paintEvent(QPaintEvent * event);

private:
    Ui::CPictureViewDialog *ui;
};

#endif // CPICTUREVIEWDIALOG_H
