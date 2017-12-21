#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <uicomponents.h>

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
    // QList<QHash<QString, QString>> attributes;
    QList<UIComponents> uiComponents;

    QHash<QString, QString> parseAttribute(QString str);
    void openFileAndExtractAttributes(int modelNumber);
    QString getModelName(QString fileName);
};

#endif // MAINWINDOW_H
