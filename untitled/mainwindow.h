#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QStandardItemModel>
#include<QItemSelectionModel>
#include<QLabel>
#define FixedColumnCount 6

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void on_currentChanged(const QModelIndex &current, const QModelIndex &previous);
private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionDelete_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_actionAppend_triggered();

    void on_actionView_triggered();

    void on_actionInsert_triggered();

private:
    Ui::MainWindow *ui;

    QStandardItemModel *model;
    QItemSelectionModel *selection;

    QLabel *LabCurFile;
    QLabel *LabCellPos;
    QLabel *LabCellText;

    void iniModelFromStringList(QStringList& aFileContent);
};

#endif // MAINWINDOW_H
