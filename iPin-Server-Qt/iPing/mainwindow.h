#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void SetLineText(QString str);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QLineEdit *line;
};

#endif // MAINWINDOW_H
