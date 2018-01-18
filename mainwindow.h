#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <data.h>
#include <QStringList>
#include <QDir>

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
    void on_listWidgetSelectedModelAttributes_currentRowChanged(int currentRow);
    void on_pushButtonAddUpdateModelAttribute_clicked();
    void on_pushButtonRemoveSelectedAttributes_clicked();
    void on_pushButtonSendSelectedModelAttribsToSelectedStructure_clicked();
    void on_pushButtonClearSelectedModelAttribSelection_clicked();
    void on_pushButtonClearSelectedStructureAttribSelection_clicked();
    void on_pushButtonSendSelectedStructureAttribsToSelectedViewModel_clicked();
    void on_pushButtonRemoveStructureSelectedAttributes_clicked();
    void on_pushButtonRemoveViewModelSelectedAttributes_clicked();
    void on_pushButtonCreateNewView_clicked();
    void on_pushButtonRemoveView_clicked();
    void on_listWidgetAllViews_currentRowChanged(int currentRow);
    void on_pushButtonBindSelectedViewModelsToSelectedView_clicked();

    void on_listWidgetSelectedViewAttributes_currentRowChanged(int currentRow);

    void on_pushButtonAssignUITypeToSelectedAttribute_clicked();

    void on_pushButtonSelectedViewAttribute_clicked();

    void on_pushButtonSendSelectedViewModelAttribsToSelectedView_clicked();

    void on_pushButtonAssignTypeToSelectedStructureAttribute_clicked();

    void on_pushButtonAssignTypeToSelectedViewModelAttribute_clicked();

    void on_listWidgetSelectedStructureAttributes_currentRowChanged(int currentRow);

    void on_listWidgetSelectedViewModelAttributes_currentRowChanged(int currentRow);
    void on_pushButtonGenerateModule_clicked();

private:
    Ui::MainWindow *ui;
    QHash<QString, QString> swiftUIs;
    QStringList swiftDataTypes;
    QList<Model> models;
    QList<Structure> structures;
    QList<ViewModel> viewModels;
    QList<View> views;

    Model selectedModel;
    Structure selectedStructure;
    ViewModel selectedViewModel;
    View selectedView;


    KeyVal parseAttribute(QString str);
    QString getModelName(QString fileName);
    void openFileAndExtractAttributes();

    bool isModelUnique(QString modelName);
    void populateSelectedModelAttributes(int index);
//    void populateSelectedModelAttributes(Model model);
    void clearSelectedModelAttributes();
    bool isStructureUnique(QString modelName);
    void populateSelectedStructureAttributes(int index);
//    void populateSelectedStructureAttributes(Structure structure);
    void clearSelectedStructureAttributes();
    bool isViewModelUnique(QString modelName);
    void populateSelectedViewModelAttributes(int index);
//    void populateSelectedViewModelAttributes(ViewModel viewModel);
    void clearSelectedViewModelAttributes();

    bool isViewUnique(QString modelName);
    void populateSelectedViewAttributes(int index);
    void clearSelectedViewAttributes();

    bool validateName(QString name, QString paramName);
    void showError(QString text);

    int indexOfStructure(QString name);
    int indexOfModel(QString name);
    int indexOfViewModel(QString name);
    int indexOfView(QString name);
    bool isModuleNameValid();
    QString getModuleName();
    void copyFileContentByCreatingPath(QString filePath, QString fileName);
    void copyFileContentByCreatingPath(QString filePath, QString fileName, QString extension);
    void copyFileContent(QString inputFilePath, QString outputFilePath);
    void createStructures(QDir dir);
    void createViewModels(QDir dir);
};

#endif // MAINWINDOW_H
