#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>

namespace Ui {
class Preferences_Dialog;
}

class Preferences_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences_Dialog(QWidget *parent = nullptr);
    ~Preferences_Dialog();

private:
    Ui::Preferences_Dialog *ui;

private slots:
    void on_accept();

};

#endif // PREFERENCES_DIALOG_H
