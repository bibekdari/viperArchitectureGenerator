#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>

struct UIComponents {
   QTextEdit *model;
   QTextEdit *structure;
   QTextEdit *viewModel;
   QLabel *modelName;
   QLineEdit *structureName;
   QLineEdit *viewModelName;

    UIComponents (QTextEdit *model,
                  QLabel *modelName,
                  QTextEdit *structure,
                  QLineEdit *structureName,
                  QTextEdit *viewModel,
                  QLineEdit *viewModelName) {
        this->model = model;
        this->modelName = modelName;
        this->structure = structure;
        this->structureName = structureName;
        this->viewModel = viewModel;
        this->viewModelName = viewModelName;
    }
};
