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
    models = QList<Model>();
    structures = QList<Structure>();
}

MainWindow::~MainWindow() {
    delete ui;
}

// Events
void MainWindow::on_pushButtonAddModelFile_clicked() {
    this->openFileAndExtractAttributes();
}

void MainWindow::on_pushButtonRemoveModel_clicked() {
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

void MainWindow::on_pushButtonCreateStructureForSelectedModels_clicked() {
    ui->listWidgetAllStructures->reset();

    QModelIndexList selectedIndex = ui->listWidgetAllModels->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndex);
    while (i.hasNext()) {
        int index = i.next().row();
        Model model = models[index];
        Structure structure = Structure();
        structure.name = model.name + "Structure";
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

// Actions

void MainWindow::populateSelectedModelAttributes(int index) {
    ui->listWidgetSelectedModelAttributes->clear();

    Model model = models[index];
    QHashIterator<QString, QString> i(model.attributes);
    while (i.hasNext()) {
        i.next();
        QString text = i.key() + ": " + i.value();
        ui->listWidgetSelectedModelAttributes->addItem(text);
    }
    ui->labelSelectedModelName->setText(model.name);
}

void MainWindow::clearSelectedModelAttributes() {
    ui->labelSelectedModelName->setText("");
    ui->listWidgetSelectedModelAttributes->clear();
}


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
}

void MainWindow::clearSelectedStructureAttributes() {
    ui->labelSelectedStructureName->setText("");
    ui->listWidgetSelectedStructureAttributes->clear();
}

void MainWindow::showError(QString text) {
    QMessageBox messageBox;
    messageBox.critical(0,"Error",text);
    messageBox.setFixedSize(500,200);
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

// Private functions: Handle model file and model attributes
void MainWindow::openFileAndExtractAttributes() {

    QString str = ui->moduleName->text();
    QString fileName = "/Projects/B2BOrderingiOS/B2BOrdering/Model/Profile.swift";
    //    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Model"), "/home", tr("Image Files (*.Swift)"));

    QString moduleName = ui->moduleName->text();
    QString modelName = this->getModelName(fileName);

    QListIterator<Model> i(models);
    while (i.hasNext()) {
        if ( modelName == i.next().name) {
            this->showError("Duplicate model name.");
            return;
        }
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
            QHash<QString, QString> parsed = this->parseAttribute(line);
            attributes.unite(parsed);
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

QHash<QString, QString> MainWindow::parseAttribute(QString str) {
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

    QHash<QString, QString> result;
    result[attributeName.trimmed()] = attributeType.trimmed();

    return result;
}

