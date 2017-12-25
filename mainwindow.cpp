#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QRegExp>
#include <QColor>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openFileAndExtractAttributes(int modelNumber) {

    QString str = ui->moduleName->text();
    QString fileName = "/Projects/B2BOrderingiOS/B2BOrdering/Model/Profile.swift";
    //QFileDialog::getOpenFileName(this, tr("Open Model"), "/home", tr("Image Files (*.Swift)"));

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
        }

        if (hasAttributeRecStarted && isAttribute) {
            QHash<QString, QString> parsed = this->parseAttribute(line);
            attributes.unite(parsed);
        }

        if (isAttributeStart) {
            hasAttributeRecStarted = true;
        }
    }

    //    this->attributes.append(attributes);

    QHashIterator<QString, QString> i(attributes);
    QString text = "";
    while (i.hasNext()) {
        i.next();
        text.append(i.key() + ": " + i.value());
    }

    QString moduleName = ui->moduleName->text();
    QString modelName = this->getModelName(fileName);
//    UIComponents component = this->uiComponents[modelNumber];
//    component.model->setText(text);
//    component.modelName->setText(modelName);
//    component.structureName->setText(moduleName.append(modelName).append("Structure"));
//    component.viewModelName->setText(moduleName.append(modelName).append("ViewModel"));
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

void MainWindow::on_pushButtonModel_1_clicked() {
    this->openFileAndExtractAttributes(1);
}
