#ifndef SENDIPCHISTORY_H
#define SENDIPCHISTORY_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE
namespace Ui {
class sendipchistory;
}

class sendipchistory : public QWidget
{
    Q_OBJECT

public:
    explicit sendipchistory(QWidget *parent = 0);
    ~sendipchistory();
    void showVisual(bool visual);
    void updateInfo(QString status);
    void setinfo(const QModelIndex &idx);

private:
    Ui::sendipchistory *ui;
};

#endif // SENDIPCHISTORY_H
