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
    attributes = QHash<QString, QString>();
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_clicked() {

    QString str = ui->moduleName->text();
    qDebug() << str;
    QString fileName = "/Projects/B2BOrderingiOS/B2BOrdering/Model/Profile.swift";
            //QFileDialog::getOpenFileName(this, tr("Open Model"), "/home", tr("Image Files (*.Swift)"));
    qDebug() << fileName;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    bool hasAttributeRecStarted = false;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
//        qDebug() << line;
        bool isAttributeEnd = line.contains("// MARK: Attributes End");
        bool isAttributeStart = line.contains("// MARK: Attributes Start");
        bool isAttribute = line.contains(" var ") || line.contains(" let ");

//        qDebug() << "attribute start " << isAttributeStart;
//        qDebug() << "attribute end " << isAttributeEnd;
//        qDebug() << "attribute " << isAttribute;

        if (isAttributeEnd) {
            hasAttributeRecStarted = false;
        }

        if (hasAttributeRecStarted && isAttribute) {
//            qDebug() << "is attribute : " << line << endl;
            QHash<QString, QString> parsed = this->parseAttribute(line);
            this->attributes.unite(parsed);
        }

        if (isAttributeStart) {
            hasAttributeRecStarted = true;
        }
    }
    qDebug() << "Printing Attributes";
    QHashIterator<QString, QString> i(attributes);
    while (i.hasNext()) {
        i.next();
        ui->textEditAttributes->append(i.key() + ": " + i.value());
//        qDebug() << i.key() << ": " << i.value() << endl;
    }


}

QHash<QString, QString> MainWindow::parseAttribute(QString str) {
    //    QString demo = "private let str: [String: [String: ([String: String], String: String)]";
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

}
