#ifndef DATA_H
#define DATA_H

#endif // DATA_H

struct Model {
    QString name;
    QString filePath;
    QHash<QString, QString> attributes;
};

struct Structure {
    QString name;
    QHash<QString, QHash<QString, QString>> attributes;
};

struct ViewModel {
    QString name;
    QHash<QString, QHash<QString, QString>> attributes;
};

struct KeyVal {
    QString key;
    QString value;
};

struct View {
    QString name;
    QString type;
    QHash<QString, QHash<QString, KeyVal>> attributes;
};

//struct Path {
//     QString modulePath;

//     init(QString modulePath) {
//         this->modulePath = modulePath;
//     }


//};
