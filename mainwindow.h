#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <data.h>

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
    void on_pushButtonAddModelFile_clicked();
    void on_listWidgetAllModels_currentRowChanged(int currentRow);

    void on_pushButtonRemoveModel_clicked();

private:
    Ui::MainWindow *ui;
    QHash<QString, QList<Model>> models;

    QHash<QString, QString> parseAttribute(QString str);
    QString getModelName(QString fileName);
    void openFileAndExtractAttributes();

    void populateSelectedModelAttributes(int index);
    void clearSelectedModelAttributes();

    void showError(QString text);
};

#endif // MAINWINDOW_H
