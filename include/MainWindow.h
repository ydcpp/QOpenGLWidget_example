#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_rotate_toggled(bool checked);

    void on_pushButton_pan_toggled(bool checked);

    void on_pushButton_zoom_toggled(bool checked);

    void UpdateStatusLabel(const QString &msg);

private:
    Ui::MainWindow *ui;
};
#endif    // MAINWINDOW_H
