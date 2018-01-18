#ifndef DATA_H
#define DATA_H

#endif // DATA_H

struct Model {
    QString name;
    QString filePath;
    QHash<QString, QString> attributes; //field and type
};

struct Structure {
    QString name;
    QHash<QString, QHash<QString, QString>> attributes; // origin model, field and type
};

struct ViewModel {
    QString name;
    QHash<QString, QHash<QString, QString>> attributes; // origin structure, field and type
};

struct KeyVal {
    QString key;
    QString value;
};

// UIVIEWS
struct View {
    QString name;
    QString type;
    QHash<QString, QHash<QString, KeyVal>> attributes; // Origin ViewModel, field, datatype and displaying UIView type
};

//struct Path {
//     QString modulePath;

//     init(QString modulePath) {
//         this->modulePath = modulePath;
//     }


//};
