#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QRegExp>
#include <QColor>
#include <QListIterator>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // list of all possible ui where we show data
    swiftUIs << "UIButton" << "UILabel" << "UIImageView" << "UITextFied" << "UITextView";

    // list of all swift data type
    swiftDataTypes << "Double" << "Int" << "Float" << "String" << "NSNumber" << "Bool" << "UInt";

    selectedModel = Model();
    selectedStructure = Structure();
    selectedViewModel = ViewModel();

    models = QList<Model>();
    structures = QList<Structure>();
    viewModels = QList<ViewModel>();
}

MainWindow::~MainWindow() {
    delete ui;
}

// TOP LEVEL
//........................TOP LEVEL.....TOP LEVEL..........TOP LEVEL.................
// Events

// models
void MainWindow::on_pushButtonAddModelFile_clicked() {
    this->openFileAndExtractAttributes();
}

void MainWindow::on_pushButtonRemoveModel_clicked() {
    // get selected index, remove items from models and ui
    QModelIndexList selectedIndex = ui->listWidgetAllModels->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndex);
    while (i.hasNext()) {
        int index = i.next().row();
        models.removeAt(index);
        ui->listWidgetAllModels->reset();
        delete ui->listWidgetAllModels->takeItem(index);
    }

    if (models.length() > 0) {
        populateSelectedModelAttributes(0);
    }else {
        clearSelectedModelAttributes();
    }
}

void MainWindow::on_listWidgetAllModels_currentRowChanged(int currentRow) {
    this->populateSelectedModelAttributes(currentRow);
}

void MainWindow::on_pushButtonCreateNewModel_clicked() {

    if (!isModuleNameValid()) {
        return;
    }

    QString modelName = ui->lineEditNewModelName->text();
    modelName.replace(QRegularExpression("[\\s\\n\\r]+"), "");

    if (!validateName(modelName, "Model name")) {
        return;
    }

    if (!isModelUnique(modelName)) {
        this->showError("Duplicate name.");
        return;
    }

    Model model = Model();
    model.name = modelName;
    model.attributes = QHash<QString, QString>();
    models.append(model);

    ui->listWidgetAllModels->addItem(model.name);

    if (models.length() == 1) {
        populateSelectedModelAttributes(0);
    }
}

// structures

void MainWindow::on_pushButtonCreateStructureForSelectedModels_clicked() {
    if (!isModuleNameValid()) {
        return;
    }

    // get all selected models and create structures for them
    ui->listWidgetAllStructures->reset();

    QModelIndexList selectedIndex = ui->listWidgetAllModels->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndex);
    while (i.hasNext()) {
        int index = i.next().row();
        Model model = models[index];
        Structure structure = Structure();
        structure.name = getModuleName() + model.name + "Structure";
        structure.attributes = QHash<QString, QHash<QString, QString>>();
        structure.attributes[model.name] = model.attributes;

        int structIndex = indexOfStructure(structure.name);
        if (structIndex >= 0) {
            structures.removeAt(structIndex);
            delete ui->listWidgetAllStructures->takeItem(structIndex);
        }
        structures.append(structure);
        ui->listWidgetAllStructures->addItem(structure.name);
    }

    if (structures.length() > 0) {
        populateSelectedStructureAttributes(0);
    }
}

void MainWindow::on_listWidgetAllStructures_currentRowChanged(int currentRow) {
    this->populateSelectedStructureAttributes(currentRow);
}

void MainWindow::on_pushButtonRemoveStructure_clicked() {
    // get selected index, remove items from structures and ui
    QModelIndexList selectedIndex = ui->listWidgetAllStructures->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndex);
    while (i.hasNext()) {
        int index = i.next().row();
        structures.removeAt(index);
        ui->listWidgetAllStructures->reset();
        delete ui->listWidgetAllStructures->takeItem(index);
    }

    if (structures.length() > 0) {
        populateSelectedStructureAttributes(0);
    }else {
        clearSelectedStructureAttributes();
    }
}

void MainWindow::on_pushButtonCreateNewStructure_clicked() {
    if (!isModuleNameValid()) {
        return;
    }

    QString structureName = ui->lineEditNewStructureName->text();
    structureName.replace(QRegularExpression("[\\s\\n\\r]+"), "");

    if (!validateName(structureName, "Structure name")) {
        return;
    }

    if (!isStructureUnique(structureName)) {
        this->showError("Duplicate name.");
        return;
    }

    Structure structure = Structure();
    structure.name = getModuleName() + structureName;
    structure.attributes = QHash<QString, QHash<QString, QString>>();
    structures.append(structure);

    ui->listWidgetAllStructures->addItem(structure.name);

    if (structures.length() == 1) {
        populateSelectedStructureAttributes(0);
    }
}

// view models

void MainWindow::on_pushButtonCreateViewModelForSelectedStructures_clicked() {
    if (!isModuleNameValid()) {
        return;
    }

    // get all selected structures and create viewModels for them
    ui->listWidgetAllViewModels->reset();

    QModelIndexList selectedIndex = ui->listWidgetAllStructures->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndex);
    while (i.hasNext()) {
        int index = i.next().row();
        Structure structure = structures[index];
        ViewModel viewModel = ViewModel();

        QString viewModelName = structure.name;
        viewModelName.replace("Structure", "ViewModel");

        viewModel.name = viewModelName;
        viewModel.attributes = QHash<QString, QHash<QString, QString>>();

        // get all attributes of structure
        QHash<QString, QString> allAttributes = QHash<QString, QString>();
        QHashIterator<QString, QHash<QString, QString>> i(structure.attributes);
        while (i.hasNext()) {
            i.next();
            QHash<QString, QString> attributes = i.value();
            QHashIterator<QString, QString> j(attributes);
            while(j.hasNext()) {
                j.next();
                allAttributes[j.key()] = j.value();
            }
        }
        viewModel.attributes[structure.name] = allAttributes;

        int viewModelIndex = indexOfStructure(viewModel.name);
        if (viewModelIndex >= 0) {
            structures.removeAt(viewModelIndex);
            delete ui->listWidgetAllViewModels->takeItem(viewModelIndex);
        }
        viewModels.append(viewModel);
        ui->listWidgetAllViewModels->addItem(viewModel.name);
    }

    if (structures.length() > 0) {
        populateSelectedViewModelAttributes(0);
    }
}

void MainWindow::on_pushButtonRemoveViewModel_clicked() {
    // get selected index, remove items from view models and ui
    QModelIndexList selectedIndex = ui->listWidgetAllViewModels->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndex);
    while (i.hasNext()) {
        int index = i.next().row();
        viewModels.removeAt(index);
        ui->listWidgetAllViewModels->reset();
        delete ui->listWidgetAllViewModels->takeItem(index);
    }

    if (viewModels.length() > 0) {
        populateSelectedViewModelAttributes(0);
    }else {
        clearSelectedViewModelAttributes();
    }
}

void MainWindow::on_listWidgetAllViewModels_currentRowChanged(int currentRow) {
    this->populateSelectedViewModelAttributes(currentRow);
}

void MainWindow::on_pushButtonCreateNewViewModel_clicked() {
    if (!isModuleNameValid()) {
        return;
    }

    QString viewModelName = ui->lineEditNewViewModelName->text();
    viewModelName.replace(QRegularExpression("[\\s\\n\\r]+"), "");

    if (!validateName(viewModelName, "View model name")) {
        return;
    }

    if (!isViewModelUnique(viewModelName)) {
        this->showError("Duplicate name.");
        return;
    }

    ViewModel viewModel = ViewModel();
    viewModel.name = getModuleName() + viewModelName;
    viewModel.attributes = QHash<QString, QHash<QString, QString>>();
    viewModels.append(viewModel);

    ui->listWidgetAllViewModels->addItem(viewModel.name);

    if (viewModels.length() == 1) {
        populateSelectedViewModelAttributes(0);
    }
}


// ACTIONS

// Models

void MainWindow::populateSelectedModelAttributes(int index) {
    ui->listWidgetSelectedModelAttributes->reset();
    ui->listWidgetSelectedModelAttributes->clear();

    Model model = models[index];
    QHashIterator<QString, QString> i(model.attributes);
    while (i.hasNext()) {
        i.next();
        QString text = i.key() + ": " + i.value();
        ui->listWidgetSelectedModelAttributes->addItem(text);
    }
    ui->labelSelectedModelName->setText(model.name);
    selectedModel = model;
}

//void MainWindow::populateSelectedModelAttributes(Model model) {

//}

void MainWindow::clearSelectedModelAttributes() {
    ui->labelSelectedModelName->setText("");
    ui->listWidgetSelectedModelAttributes->clear();
    ui->lineEditSelectedModelAttributeName->setText("");
    selectedModel = Model();
}

// Structures

void MainWindow::populateSelectedStructureAttributes(int index) {
    ui->listWidgetSelectedStructureAttributes->clear();
    Structure structure = structures[index];
    QHashIterator<QString, QHash<QString, QString>> i(structure.attributes);
    while (i.hasNext()) {
        i.next();
        QHash<QString, QString> attributes = i.value();
        QHashIterator<QString, QString> j(attributes);
        while(j.hasNext()) {
            j.next();
            QString text = i.key() + " -> " + j.key() + ": " + j.value();
            ui->listWidgetSelectedStructureAttributes->addItem(text);
        }
    }

    ui->labelSelectedStructureName->setText(structure.name);
    selectedStructure = structure;
}

void MainWindow::clearSelectedStructureAttributes() {
    ui->labelSelectedStructureName->setText("");
    ui->listWidgetSelectedStructureAttributes->clear();
    selectedStructure = Structure();
}

// View Models

void MainWindow::populateSelectedViewModelAttributes(int index) {
    ui->listWidgetSelectedViewModelAttributes->clear();
    ViewModel viewModel = viewModels[index];
    QHashIterator<QString, QHash<QString, QString>> i(viewModel.attributes);
    while (i.hasNext()) {
        i.next();
        QHash<QString, QString> attributes = i.value();
        QHashIterator<QString, QString> j(attributes);
        while(j.hasNext()) {
            j.next();
            QString text = i.key() + " -> " + j.key() + ": " + j.value();
            ui->listWidgetSelectedViewModelAttributes->addItem(text);
        }
    }

    ui->labelSelectedViewModelName->setText(viewModel.name);
    selectedViewModel = viewModel;
}

void MainWindow::clearSelectedViewModelAttributes() {
    ui->labelSelectedViewModelName->setText("");
    ui->listWidgetSelectedViewModelAttributes->clear();
    selectedViewModel = ViewModel();
}

// SECOND LEVEL
//.........SECOND LEVEL..........SECOND LEVEL..............SECOND LEVEL...................

// EVENTS

// Models
void MainWindow::on_listWidgetSelectedModelAttributes_currentRowChanged(int currentRow) {
    QString text = ui->listWidgetSelectedModelAttributes->currentItem()->text();
    ui->lineEditSelectedModelAttributeName->setText(text);
}

void MainWindow::on_pushButtonAddUpdateModelAttribute_clicked() {

    if (selectedModel.filePath.length() > 0) {
        showError("This model is created from file.");
        return;
    }

    QString text = ui->lineEditSelectedModelAttributeName->text();
    if (text.trimmed().length() == 0) {
        return;
    }
    int index = indexOfModel(selectedModel.name);
    if (index >= 0) {
        if (ui->listWidgetSelectedModelAttributes->currentRow() >= 0) {
            QString oldText = ui->listWidgetSelectedModelAttributes->currentItem()->text();
            KeyVal oldAttributes = parseAttribute(oldText);
            selectedModel.attributes.remove(oldAttributes.key);
        }

        KeyVal attribute = parseAttribute(text);
        selectedModel.attributes[attribute.key] = attribute.value;
        models[index] = selectedModel;
        populateSelectedModelAttributes(index);
    }
}

void MainWindow::on_pushButtonRemoveSelectedAttributes_clicked() {

    if (selectedModel.filePath.length() > 0) {
        showError("This model is created from file.");
        return;
    }

    // get selected index, remove items from attributes of selected models and ui
    QModelIndexList selectedIndexes = ui->listWidgetSelectedModelAttributes->selectionModel()->selectedIndexes();
    ui->listWidgetSelectedModelAttributes->reset();

    foreach (QModelIndex index, selectedIndexes) {
        QString text = index.data(Qt::DisplayRole).toString();
        selectedModel.attributes.remove(parseAttribute(text).key);
        delete ui->listWidgetSelectedModelAttributes->takeItem(index.row());
    }
    int index = indexOfModel(selectedModel.name);
    if (index >= 0) {
        models[index] = selectedModel;
    }
}

void MainWindow::on_pushButtonSendSelectedModelAttribsToSelectedStructure_clicked() {
    if (selectedStructure.name.length() == 0) {
        showError("No structure selected.");
    }

    QModelIndexList selectedIndexes = ui->listWidgetSelectedModelAttributes->selectionModel()->selectedIndexes();
    foreach (QModelIndex index, selectedIndexes) {
        QString text = index.data(Qt::DisplayRole).toString();
        KeyVal parsed = parseAttribute(text);
        QString modelName = ui->labelSelectedModelName->text();
        selectedStructure.attributes[modelName][parsed.key] = parsed.value;
    }
    int index = indexOfStructure(selectedStructure.name);
    if (index >= 0) {
        structures[index] = selectedStructure;
        populateSelectedStructureAttributes(index);
    }
}

void MainWindow::on_pushButtonClearSelectedModelAttribSelection_clicked() {
    ui->listWidgetSelectedModelAttributes->reset();
}

// Structure

void MainWindow::on_pushButtonClearSelectedStructureAttribSelection_clicked() {
    ui->listWidgetSelectedStructureAttributes->reset();
}

void MainWindow::on_pushButtonSendSelectedStructureAttribsToSelectedViewModel_clicked() {
    if (selectedViewModel.name.length() == 0) {
        showError("No view model selected.");
    }

    QModelIndexList selectedIndexes = ui->listWidgetSelectedStructureAttributes->selectionModel()->selectedIndexes();
    foreach (QModelIndex index, selectedIndexes) {
        QString text = index.data(Qt::DisplayRole).toString().split(" -> ").last();
        KeyVal parsed = parseAttribute(text);
        QString structureName = ui->labelSelectedStructureName->text();
        selectedViewModel.attributes[structureName][parsed.key] = parsed.value;
    }
    int index = indexOfViewModel(selectedViewModel.name);
    if (index >= 0) {
        viewModels[index] = selectedViewModel;
        populateSelectedViewModelAttributes(index);
    }
}

void MainWindow::on_pushButtonRemoveStructureSelectedAttributes_clicked() {

    // get selected index, remove items from attributes of selected structure and ui
    QModelIndexList selectedIndexes = ui->listWidgetSelectedStructureAttributes->selectionModel()->selectedIndexes();
    ui->listWidgetSelectedStructureAttributes->reset();

    foreach (QModelIndex index, selectedIndexes) {
        QStringList splitted = index.data(Qt::DisplayRole).toString().split(" -> ");
        QString modelName = splitted.first();
        QString text = splitted.last();
        selectedStructure.attributes[modelName].remove(parseAttribute(text).key);
        delete ui->listWidgetSelectedStructureAttributes->takeItem(index.row());
    }
    int index = indexOfStructure(selectedStructure.name);
    if (index >= 0) {
        structures[index] = selectedStructure;
    }
}


// Others

void MainWindow::showError(QString text) {
    QMessageBox messageBox;
    messageBox.critical(0,"Error",text);
    messageBox.setFixedSize(500,200);
}

bool MainWindow::isModelUnique(QString modelName) {
    QListIterator<Model> i(models);
    while (i.hasNext()) {
        if ( modelName == i.next().name) {
            return false;
        }
    }
    return true;
}

bool MainWindow::isStructureUnique(QString modelName) {
    QListIterator<Structure> i(structures);
    while (i.hasNext()) {
        if ( modelName == i.next().name) {
            return false;
        }
    }
    return true;
}

bool MainWindow::isViewModelUnique(QString modelName) {
    QListIterator<ViewModel> i(viewModels);
    while (i.hasNext()) {
        if ( modelName == i.next().name) {
            return false;
        }
    }
    return true;
}

bool MainWindow::validateName(QString name, QString paramName) {
    if (name.length() == 0) {
        showError("Empty " + paramName);
        return false;
    }

    QRegExp exp("[a-zA-Z0-1]+");
    if (!exp.exactMatch(name)) {
        showError(paramName + "  must be alpha numeric.");
        return false;
    }

    if (name[0].isDigit()) {
        showError(paramName + " can't start with number.");
        return false;
    }
    return true;
}

// Private functions: Helpers
int MainWindow::indexOfStructure(QString name) {
    for (int i=0; i<structures.length(); i++) {
        if (structures[i].name == name) {
            return i;
        }
    }
    return -1;
}

int MainWindow::indexOfModel(QString name) {
    for (int i=0; i<models.length(); i++) {
        if (models[i].name == name) {
            return i;
        }
    }
    return -1;
}

int MainWindow::indexOfViewModel(QString name) {
    for (int i=0; i<viewModels.length(); i++) {
        if (viewModels[i].name == name) {
            return i;
        }
    }
    return -1;
}

bool MainWindow::isModuleNameValid() {
    return validateName(getModuleName(), "Module name");
}

QString MainWindow::getModuleName() {
    QString moduleName = ui->moduleName->text();
    moduleName.replace(QRegularExpression("[\\s\\n\\r]+"), "");
    return moduleName;
}

// Private functions: Handle model file and model attributes
void MainWindow::openFileAndExtractAttributes() {
    QString fileName = "/Projects/B2BOrderingiOS/B2BOrdering/Model/Profile.swift";
    //    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Model"), "/home", tr("Image Files (*.Swift)"));

    QString moduleName = getModuleName();
    QString modelName = this->getModelName(fileName);

    if (!isModelUnique(modelName)) {
        this->showError("Duplicate model name.");
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    bool hasAttributeRecStarted = false;
    QHash<QString, QString> attributes = QHash<QString, QString>();

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        bool isAttributeEnd = line.contains("// MARK: Attributes End");
        bool isAttributeStart = line.contains("// MARK: Attributes Start");
        bool isAttribute = line.contains(" var ") || line.contains(" let ");

        if (isAttributeEnd) {
            hasAttributeRecStarted = false;
            break;
        }

        if (hasAttributeRecStarted && isAttribute) {
            KeyVal parsed = this->parseAttribute(line);
            attributes[parsed.key] = parsed.value;
        }

        if (isAttributeStart) {
            hasAttributeRecStarted = true;
        }
    }

    Model model = Model();
    model.attributes = attributes;
    model.filePath = fileName;
    model.name = modelName;
    models.append(model);
    ui->listWidgetAllModels->addItem(modelName);


    if (models.length() == 1) {
        this->populateSelectedModelAttributes(0);
    }
}

QString MainWindow::getModelName(QString fileName) {
    QStringList splitted = fileName.split(".swift");
    QString leftComponent = splitted.first();
    splitted = leftComponent.split("/");
    return splitted.last();
}

KeyVal MainWindow::parseAttribute(QString str) {
    QStringList splitted = str.split(" = ");

    QString declarationPart = splitted.first();

    QString splitString = (str.contains(" var ")) ? " var " : " let ";

    splitted = declarationPart.split(splitString);

    QString attributeNameAndType = splitted.last();

    splitted = attributeNameAndType.split(": ");

    QString attributeName = splitted.first();
    QString attributeType = "";

    for(int i=1; i<splitted.length(); i++) {
        attributeType += (((i == 1) ? "" : ": ") + splitted[i]);
    }

    KeyVal result = KeyVal();
    result.key = attributeName.trimmed();
    result.value = attributeType.trimmed();

    return result;
}
