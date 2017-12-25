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
    models = QHash<QString, QList<Model>>();
}

MainWindow::~MainWindow() {
    delete ui;
}

// Events
void MainWindow::on_pushButtonAddModelFile_clicked() {
    this->openFileAndExtractAttributes();
}

void MainWindow::on_listWidgetAllModels_currentRowChanged(int currentRow) {
    this->populateSelectedModelAttributes(currentRow);
}

// Actions

void MainWindow::populateSelectedModelAttributes(int index) {
    Model model = models["models"][index];
    QHashIterator<QString, QString> i(model.attributes);
    while (i.hasNext()) {
        i.next();
        QString text = i.key() + ": " + i.value();
        ui->listWidgetSelectedModelAttributes->addItem(text);
    }
    ui->labelSelectedModelName->setText(model.name);
}

void MainWindow::showError(QString text) {
    QMessageBox messageBox;
    messageBox.critical(0,"Error",text);
    messageBox.setFixedSize(500,200);
}



// Private functions: Handle model file and model attributes
void MainWindow::openFileAndExtractAttributes() {

    QString str = ui->moduleName->text();
    QString fileName = "/Projects/B2BOrderingiOS/B2BOrdering/Model/Profile.swift";
    //    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Model"), "/home", tr("Image Files (*.Swift)"));

    QString moduleName = ui->moduleName->text();
    QString modelName = this->getModelName(fileName);

    QListIterator<Model> i(models["models"]);
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

    if (models["models"].length() == 0) {
        models["models"] = QList<Model>();
        models["models"].append(model);
        this->populateSelectedModelAttributes(0);
    }else {
        models["models"].append(model);
    }

    ui->listWidgetAllModels->addItem(modelName);
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
