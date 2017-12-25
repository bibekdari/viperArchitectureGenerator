#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonModel_1_clicked();

private:
    Ui::MainWindow *ui;
    QHash<QString, QHash<QString, QString>> models;
    QHash<QString, QString> parseAttribute(QString str);
    QString getModelName(QString fileName);
};

#endif // MAINWINDOW_H
