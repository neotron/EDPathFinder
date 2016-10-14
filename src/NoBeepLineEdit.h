//
// Created by David Hedbor on 10/13/16.
//

#ifndef ELITETSP_NOBEEPLINEEDIT_H
#define ELITETSP_NOBEEPLINEEDIT_H


#include <QLineEdit>
#include <QKeyEvent>

class NoBeepLineEdit : public QLineEdit {

public:


    explicit NoBeepLineEdit(QWidget *parent) : QLineEdit(parent) {
    }

    explicit NoBeepLineEdit(const QString &string, QWidget *parent) : QLineEdit(string, parent) {
    }

    virtual ~NoBeepLineEdit() override {
    }

    void keyPressEvent(QKeyEvent *event) override {
        if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
            event->accept();
            emit editingFinished();
            clearFocus();
        } else {
            QLineEdit::keyPressEvent(event);
        }
    }
};


#endif //ELITETSP_NOBEEPLINEEDIT_H
