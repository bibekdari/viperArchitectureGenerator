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
#include <QDate>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // list of all possible ui where we show data
    swiftUIs["UILabel"] = "Label";
    swiftUIs["UIButton"] = "Button";
    swiftUIs["UIImageView"] = "ImageView";
    swiftUIs["UITextFied"] = "TextField";
    swiftUIs["UITextView"] = "TextView";

    ui->comboBoxSelectedViewAttributeUIType->addItems(swiftUIs.keys());

    // list of all swift data type
    swiftDataTypes << "Double" << "Int" << "Float" << "String" << "NSNumber" << "Bool" << "UInt";

    selectedModel = Model();
    selectedStructure = Structure();
    selectedViewModel = ViewModel();
    selectedView = View();

    models = QList<Model>();
    structures = QList<Structure>();
    viewModels = QList<ViewModel>();
    views = QList<View>();

    QStringList allowedViews;
    allowedViews << "UIView" << "UICollectionViewCell" << "UITableViewCell";
    ui->comboBoxViewType->addItems(allowedViews);
    ui->listWidgetAllViews->addItem("Default-UIViewController");

    View view = View();
    view.name = "Default-UIViewController";
    view.attributes =  QHash<QString, QHash<QString, KeyVal>>();
    views.append(view);
    populateSelectedViewAttributes(0);
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

    int index = indexOfModel(selectedModel.name);
    if (index >= 0) {
        populateSelectedModelAttributes(index);
    }else if (models.count() > 0) {
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

    int index = indexOfStructure(selectedStructure.name);
    if (index >= 0) {
        populateSelectedStructureAttributes(index);
    }else if (structures.count() > 0) {
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

    int index = indexOfViewModel(selectedViewModel.name);
    if (index >= 0) {
        populateSelectedViewModelAttributes(index);
    }else if (viewModels.count() > 0) {
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
    ui->listWidgetSelectedStructureAttributes->reset();
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
    ui->listWidgetSelectedViewModelAttributes->reset();
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

void MainWindow::on_listWidgetSelectedStructureAttributes_currentRowChanged(int currentRow) {
    QString text = ui->listWidgetSelectedStructureAttributes->currentItem()->text().split(" -> ").last();
    KeyVal parsed = parseAttribute(text);
    ui->comboBoxSelectedStrctureAttributeType->setCurrentText(parsed.value);
}

void MainWindow::on_pushButtonAssignTypeToSelectedStructureAttribute_clicked() {
    // get selected index, edit item type
    QModelIndexList selectedIndexes = ui->listWidgetSelectedStructureAttributes->selectionModel()->selectedIndexes();

    foreach (QModelIndex index, selectedIndexes) {
        QStringList splitted = index.data(Qt::DisplayRole).toString().split(" -> ");
        QString modelName = splitted.first();
        KeyVal parsed = parseAttribute(splitted.last());
        selectedStructure.attributes[modelName][parsed.key] = ui->comboBoxSelectedStrctureAttributeType->currentText();
    }
    int index = indexOfStructure(selectedStructure.name);
    if (index >= 0) {
        structures[index] = selectedStructure;
        populateSelectedStructureAttributes(index);
    }
}

// View Model

void MainWindow::on_pushButtonRemoveViewModelSelectedAttributes_clicked() {

    // get selected index, remove items from attributes of selected view model and ui
    QModelIndexList selectedIndexes = ui->listWidgetSelectedViewModelAttributes->selectionModel()->selectedIndexes();
    ui->listWidgetSelectedViewModelAttributes->reset();

    foreach (QModelIndex index, selectedIndexes) {
        QStringList splitted = index.data(Qt::DisplayRole).toString().split(" -> ");
        QString structureName = splitted.first();
        QString text = splitted.last();
        selectedViewModel.attributes[structureName].remove(parseAttribute(text).key);
        delete ui->listWidgetSelectedViewModelAttributes->takeItem(index.row());
    }
    int index = indexOfViewModel(selectedViewModel.name);
    if (index >= 0) {
        viewModels[index] = selectedViewModel;
    }
}

void MainWindow::on_pushButtonSendSelectedViewModelAttribsToSelectedView_clicked() {
    // get selected index, get selected items to selected view
    QModelIndexList selectedIndexes = ui->listWidgetSelectedViewModelAttributes->selectionModel()->selectedIndexes();
    ui->listWidgetSelectedViewAttributes->reset();

    foreach (QModelIndex index, selectedIndexes) {
        QStringList splitted = index.data(Qt::DisplayRole).toString().split(" -> ");
        QString viewModelName = ui->labelSelectedViewModelName->text();
        KeyVal parsedKeyValue = parseAttribute(splitted.last());
        QHash<QString, KeyVal> attrib = selectedView.attributes[viewModelName];
        KeyVal attribAndType = KeyVal();
        attribAndType.key = parsedKeyValue.value;
        attribAndType.value = "UILabel";
        attrib[parsedKeyValue.key] = attribAndType;
        selectedView.attributes[viewModelName] = attrib;
    }
    int index = indexOfView(selectedView.name);
    if (index >= 0) {
        views[index] = selectedView;
        populateSelectedViewAttributes(index);
    }
}

void MainWindow::on_listWidgetSelectedViewModelAttributes_currentRowChanged(int currentRow) {
    QString text = ui->listWidgetSelectedViewModelAttributes->currentItem()->text().split(" -> ").last();
    qDebug() << text;
    KeyVal parsed = parseAttribute(text);
    ui->comboBoxSelectedViewModelAttributeType->setCurrentText(parsed.value);
}

void MainWindow::on_pushButtonAssignTypeToSelectedViewModelAttribute_clicked() {
    // get selected index, edit item type
    QModelIndexList selectedIndexes = ui->listWidgetSelectedViewModelAttributes->selectionModel()->selectedIndexes();

    foreach (QModelIndex index, selectedIndexes) {
        QStringList splitted = index.data(Qt::DisplayRole).toString().split(" -> ");
        QString modelName = splitted.first();
        KeyVal parsed = parseAttribute(splitted.last());
        selectedViewModel.attributes[modelName][parsed.key] = ui->comboBoxSelectedViewModelAttributeType->currentText();
    }
    int index = indexOfViewModel(selectedViewModel.name);
    if (index >= 0) {
        viewModels[index] = selectedViewModel;
        populateSelectedViewModelAttributes(index);
    }
}


//--------VIEW RELATED -------------------VIEW RELATED ------------- VIEW RELATED ------------------VIEW RELATED ----------


void MainWindow::on_pushButtonCreateNewView_clicked() {

    if (!isModuleNameValid()) {
        return;
    }

    QString name = ui->lineEditNewViewName->text();
    QString type = ui->comboBoxViewType->currentText();
    name.replace(QRegularExpression("[\\s\\n\\r]+"), "");

    if (!validateName(name, "View name")) {
        return;
    }

    if (!isViewUnique(name)) {
        showError("Duplicate name.");
        return;
    }

    View view = View();
    view.name = getModuleName() + name;
    view.type = type;
    view.attributes =  QHash<QString, QHash<QString, KeyVal>>();

    views.append(view);

    ui->listWidgetAllViews->addItem(view.name + ": " + view.type);
}

void MainWindow::on_pushButtonRemoveView_clicked() {
    // get selected index, remove selected views
    QModelIndexList selectedIndexes = ui->listWidgetAllViews->selectionModel()->selectedIndexes();
    ui->listWidgetAllViews->reset();

    foreach (QModelIndex index, selectedIndexes) {
        QString text = index.data(Qt::DisplayRole).toString();
        // Dont delete default view controller
        if (text != "Default-UIViewController") {
            views.removeAt(index.row());
            delete ui->listWidgetAllViews->takeItem(index.row());
        }
    }

    int index = indexOfView(selectedView.name);
    if (index >= 0) {
        populateSelectedViewAttributes(index);
    }else if (views.count() > 0) {
        populateSelectedViewAttributes(0);
    }else {
        clearSelectedViewAttributes();
    }
}

void MainWindow::on_listWidgetAllViews_currentRowChanged(int currentRow) {
    populateSelectedViewAttributes(currentRow);
}

void MainWindow::on_pushButtonBindSelectedViewModelsToSelectedView_clicked() {

    int viewIndex = ui->listWidgetAllViews->currentRow();
    if (viewIndex < 0) {
        return;
    }

    // get all selected structures and create viewModels for them
    ui->listWidgetSelectedViewAttributes->reset();

    QModelIndexList selectedIndex = ui->listWidgetAllViewModels->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndex);
    while (i.hasNext()) {
        int index = i.next().row();
        ViewModel viewModel = viewModels[index];

        // get all attributes of structure
        QHash<QString, KeyVal> allAttributes = QHash<QString, KeyVal>();
        QHashIterator<QString, QHash<QString, QString>> i(viewModel.attributes);
        while (i.hasNext()) {
            i.next();
            QHash<QString, QString> attributes = i.value();
            QHashIterator<QString, QString> j(attributes);
            while(j.hasNext()) {
                j.next();
                KeyVal attrib = KeyVal();
                attrib.key = j.value();
                attrib.value = "UILabel";
                allAttributes[j.key()] = attrib;
            }
        }
        selectedView.attributes[viewModel.name] = allAttributes;
    }
    views[viewIndex] = selectedView;
    populateSelectedViewAttributes(viewIndex);
}

void MainWindow::on_listWidgetSelectedViewAttributes_currentRowChanged(int currentRow) {
    QStringList list = swiftUIs.keys();
    QString text = ui->listWidgetSelectedViewAttributes->currentItem()->text().split(" -> ").last();
    KeyVal parsed = parseAttribute(text);
    ui->comboBoxSelectedViewAttributeUIType->setCurrentText(parsed.value);
}

void MainWindow::on_pushButtonAssignUITypeToSelectedAttribute_clicked() {
    QString selectedString = ui->listWidgetSelectedViewAttributes->currentItem()->text();
    QHashIterator<QString, QHash<QString, KeyVal>> i(selectedView.attributes);
    while (i.hasNext()) {
        i.next();
        QHash<QString, KeyVal> attrib = i.value();
        QHashIterator<QString, KeyVal> j(attrib);
        while(j.hasNext()) {
            j.next();
            QString suffix = swiftUIs[j.value().value];
            QString text = i.key() + " -> " + j.key() + suffix + ": " + j.value().value;
            KeyVal valAndType = j.value();
            if (text == selectedString) {
                valAndType.value = ui->comboBoxSelectedViewAttributeUIType->currentText();
            }
            attrib[j.key()] = valAndType;
        }
        selectedView.attributes[i.key()] = attrib;
    }

    int index = indexOfView(selectedView.name);
    if (index >= 0) {
        views[index] = selectedView;
        populateSelectedViewAttributes(index);
    }
}

void MainWindow::on_pushButtonSelectedViewAttribute_clicked() {
    QString selectedString = ui->listWidgetSelectedViewAttributes->currentItem()->text();
    QHashIterator<QString, QHash<QString, KeyVal>> i(selectedView.attributes);
    while (i.hasNext()) {
        i.next();
        QHash<QString, KeyVal> attrib = i.value();
        QHashIterator<QString, KeyVal> j(attrib);
        while(j.hasNext()) {
            j.next();
            QString suffix = swiftUIs[j.value().value];
            QString text = i.key() + " -> " + j.key() + suffix + ": " + j.value().value;
            KeyVal valAndType = j.value();
            if (text == selectedString) {
                attrib.remove(j.key());
            }
        }
        selectedView.attributes[i.key()] = attrib;
    }

    int index = indexOfView(selectedView.name);
    if (index >= 0) {
        views[index] = selectedView;
        populateSelectedViewAttributes(index);
    }
}

void MainWindow::populateSelectedViewAttributes(int index) {
    ui->listWidgetSelectedViewAttributes->reset();
    ui->listWidgetSelectedViewAttributes->clear();

    View view = views[index];
    QHashIterator<QString, QHash<QString, KeyVal>> i(view.attributes);
    while (i.hasNext()) {
        i.next();
        QHash<QString, KeyVal> attributes = i.value();
        QHashIterator<QString, KeyVal> j(attributes);
        while(j.hasNext()) {
            j.next();
            QString suffix = swiftUIs[j.value().value];
            QString text = i.key() + " -> " + j.key() + suffix + ": " + j.value().value;
            ui->listWidgetSelectedViewAttributes->addItem(text);
        }
    }

    ui->labelSelectedViewName->setText(view.name + ": " + view.type);
    selectedView = view;
}

void MainWindow::clearSelectedViewAttributes() {
    ui->listWidgetSelectedViewAttributes->clear();
    ui->labelSelectedViewName->setText("");
    selectedView = View();
}

bool MainWindow::isViewUnique(QString viewName) {
    QListIterator<View> i(views);
    while (i.hasNext()) {
        if ( viewName == i.next().name) {
            return false;
        }
    }
    return true;
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

int MainWindow::indexOfView(QString name) {
    for (int i=0; i<views.length(); i++) {
        if (views[i].name == name) {
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
    file.close();

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

void MainWindow::on_pushButtonGenerateModule_clicked() {
    if (!isModuleNameValid()) {
        return;
    }
    QString moduleParentFolderPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QString moduleName = this->getModuleName();
    QString moduleFolderPath = moduleParentFolderPath + "/" + moduleName;
    QDir().mkdir(moduleFolderPath);
    QDir dir = QDir(moduleFolderPath);
    dir.mkdir("Application_Logic");
    dir.mkdir("Module_Interface");
    dir.mkdir("User_Interface");

    dir.cd("Application_Logic");
    dir.mkdir("Service");
    dir.mkdir("Interactor");
    dir.cd("Service");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "Service");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "ServiceType");
    dir.cd("../Interactor");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "Interactor");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "InteractorIO");

    dir.cd("../../Module_Interface");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "ModuleInterface");

    dir.cd("../User_Interface");
    dir.mkdir("Presenter");
    dir.mkdir("View");
    dir.mkdir("Wireframe");

    dir.cd("Presenter");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "Presenter");
    dir.mkdir("Structures");
    dir.cd("Structures");
    this->createStructures(dir);
    dir.cdUp();

    dir.cd("../View");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "ViewController");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "ViewInterface");
    QString sbInputFilePath = ":/resource/Storyboard.storyboard";
    QString sbOutputFilePath = dir.absolutePath() + "/" + moduleName + ".storyboard";
    this->copyFileContent(sbInputFilePath, sbOutputFilePath);
    dir.mkdir("ViewModels");
    dir.cd("ViewModels");
    this->createViewModels(dir);
    dir.cdUp();

    dir.cd("../Wireframe");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "Wireframe");
    this->copyFileContentByCreatingPath(dir.absolutePath(), "WireframeInput");
}

void MainWindow::copyFileContentByCreatingPath(QString filePath, QString fileName) {
    this->copyFileContentByCreatingPath(filePath, fileName, ".swift");
}

void MainWindow::copyFileContentByCreatingPath(QString filePath, QString fileName, QString extension) {
    QString moduleName = this->getModuleName();
    QString inputFilePath = ":/resource/" + fileName + extension;
    QString outputFilePath = filePath + "/" + moduleName + fileName + extension;
    this->copyFileContent(inputFilePath, outputFilePath);
}

void MainWindow::copyFileContent(QString inputFilePath, QString outputFilePath) {
    QString moduleName = this->getModuleName();
    QFile inputFile(inputFilePath);
    QFile outputFile(outputFilePath);

    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    if (!outputFile.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    QTextStream stream(&outputFile);

    while (!inputFile.atEnd()) {
        QString line = inputFile.readLine();
        QString newLine = line.replace("--MODULENAME--", moduleName);
        QString now = QDate::currentDate().toString("dd/MM/yyyy");
        newLine = line.replace("--TODAY--", now);
        stream << newLine;
    }
    inputFile.close();
    outputFile.close();
}

void MainWindow::createStructures(QDir dir) {
    foreach (Structure structure, this->structures) {
        QString filePath = dir.absolutePath() + "/" + structure.name + ".swift";
        QFile outputFile(filePath);
        if (!outputFile.open(QIODevice::ReadWrite | QIODevice::Text))
            return;
        QTextStream stream(&outputFile);

        stream << "//" << endl <<
                  "//  " << structure.name << ".swift" << endl
               << "//" << endl
               << "//  Created by VIPER GENERATOR on " <<  QDate::currentDate().toString("dd/MM/yyyy") << "." << endl
               << "//" << endl << endl
               << "import Foundation" << endl << endl
               << "struct " << structure.name << " {" << endl;



        QHashIterator<QString, QHash<QString, QString>> i(structure.attributes);
        while (i.hasNext()) {
            i.next();
            QHash<QString, QString> attributes = i.value();
            QHashIterator<QString, QString> j(attributes);
            while(j.hasNext()) {
                j.next();
                QString text = "    var " + j.key() + ": " + j.value() + "\n";
                stream << text;
            }
        }

        stream << "}" << endl;

        outputFile.close();
    }
}

void MainWindow::createViewModels(QDir dir) {
    foreach (ViewModel viewModel, this->viewModels) {
        QString filePath = dir.absolutePath() + "/" + viewModel.name + ".swift";
        QFile outputFile(filePath);
        if (!outputFile.open(QIODevice::ReadWrite | QIODevice::Text))
            return;
        QTextStream stream(&outputFile);

        stream << "//" << endl <<
                  "//  " << viewModel.name << ".swift" << endl
               << "//" << endl
               << "//  Created by VIPER GENERATOR on " <<  QDate::currentDate().toString("dd/MM/yyyy") << "." << endl
               << "//" << endl << endl
               << "import Foundation" << endl << endl
               << "class " << viewModel.name << " {" << endl;

        QHashIterator<QString, QHash<QString, QString>> i(viewModel.attributes);
        while (i.hasNext()) {
            i.next();
            QHash<QString, QString> attributes = i.value();
            QHashIterator<QString, QString> j(attributes);
            while(j.hasNext()) {
                j.next();
                QString text = "    var " + j.key() + ": " + j.value() + "\n";
                stream << text;
            }
        }

        stream << "}" << endl;

        outputFile.close();
    }
}

void MainWindow::configureInteractor(QDir dir) {
    QString moduleName = this->getModuleName();
    QFile inputFile(":/resource/" + "Interactor");
    QFile outputFile(dir.absolutePath() + "/" + moduleName + "Interactor.swift");

    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    if (!outputFile.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    QTextStream stream(&outputFile);

    while (!inputFile.atEnd()) {
        QString line = inputFile.readLine();
        QString newLine = line.replace("--MODULENAME--", moduleName);
        QString now = QDate::currentDate().toString("dd/MM/yyyy");
        newLine = line.replace("--TODAY--", now);
        stream << newLine;
        if (newLine.contains("// MARK: Converting entities")) {
            /*
                private func convert(models: [HomeModel]) {
                    let model = models.first
                    self.output?.obtained(banners: self.convert(model: model))
                    self.output?.obtained(categories: self.convert(model: model))
                }
            */
            foreach (Model model, this->models) {
//               QList<QHash<QString, QString>> relatedStructures;
//               foreach (Structure structure, this->structures) {
//                   QHashIterator<QString, QHash<QString, QString>> i(structure.attributes);
//                   while (i.hasNext()) {
//                       i.next();
//                       if (i.key() == model.name) {
//                           relatedStructures.append(i.value());
//                       }
//                   }
//               }
               stream << "    private func convert(model: " << model.name << ") {" << endl << endl << "    }" << endl;

               stream << "//    private func convert(models: [" << model.name << "]) {" << endl  << endl << "    }";
            }
        }
    }
    inputFile.close();
    outputFile.close();
}
