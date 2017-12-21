#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QTextEdit>

struct UIComponents {
   QTextEdit *model;
   QTextEdit *structure;
   QTextEdit *viewModel;
   QLabel *modelName;
   QLabel *structureName;
   QLabel *viewModelName;

    UIComponents (QTextEdit *model,
                  QLabel *modelName,
                  QTextEdit *structure,
                  QLabel *structureName,
                  QTextEdit *viewModel,
                  QLabel *viewModelName) {
        this->model = model;
        this->modelName = modelName;
        this->structure = structure;
        this->structureName = structureName;
        this->viewModel = viewModel;
        this->viewModelName = viewModelName;
    }
};
