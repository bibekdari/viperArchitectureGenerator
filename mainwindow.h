#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <data.h>
#include <QStringList>

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
    void on_listWidgetAllStructures_currentRowChanged(int currentRow);
    void on_pushButtonRemoveStructure_clicked();
    void on_pushButtonCreateViewModelForSelectedStructures_clicked();
    void on_pushButtonRemoveViewModel_clicked();
    void on_listWidgetAllViewModels_currentRowChanged(int currentRow);
    void on_pushButtonCreateNewModel_clicked();

    void on_pushButtonCreateNewStructure_clicked();

    void on_pushButtonCreateNewViewModel_clicked();

private:
    Ui::MainWindow *ui;
    QStringList swiftUIs;
    QStringList swiftDataTypes;
    QList<Model> models;
    QList<Structure> structures;
    QList<ViewModel> viewModels;

    QHash<QString, QString> parseAttribute(QString str);
    QString getModelName(QString fileName);
    void openFileAndExtractAttributes();

    bool isModelUnique(QString modelName);
    void populateSelectedModelAttributes(int index);
    void clearSelectedModelAttributes();
    bool isStructureUnique(QString modelName);
    void populateSelectedStructureAttributes(int index);
    void clearSelectedStructureAttributes();
    bool isViewModelUnique(QString modelName);
    void populateSelectedViewModelAttributes(int index);
    void clearSelectedViewModelAttributes();

    bool validateName(QString name, QString paramName);
    void showError(QString text);

    int indexOfStructure(QString name);
    bool isModuleNameValid();
};

#endif // MAINWINDOW_H
