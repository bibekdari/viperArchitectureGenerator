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

    void on_pushButtonCreateStructureForSelectedModels_clicked();

private:
    Ui::MainWindow *ui;
    QList<Model> models;
    QList<Structure> structures;

    QHash<QString, QString> parseAttribute(QString str);
    QString getModelName(QString fileName);
    void openFileAndExtractAttributes();

    void populateSelectedModelAttributes(int index);
    void clearSelectedModelAttributes();
    void populateSelectedStructureAttributes(int index);
    void showError(QString text);

    int indexOfStructure(QString name);
};

#endif // MAINWINDOW_H
