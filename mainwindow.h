#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_beginButton_clicked();

    void on_endButton_clicked();

private:
    Ui::MainWindow *ui;
    bool videoFlag;
    QStandardItemModel *model = new QStandardItemModel();
    void TEST_CAM();
};
#endif // MAINWINDOW_H
