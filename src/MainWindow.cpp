#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QSurfaceFormat>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSurfaceFormat glFormat;
    glFormat.setRenderableType(QSurfaceFormat::OpenGL);
    glFormat.setDepthBufferSize(24);
    glFormat.setOption(QSurfaceFormat::DebugContext);

    ui->scene->setFormat(glFormat);
    ui->scene->setFocus();
    connect(ui->pushButton_cube, &QPushButton::clicked, ui->scene, &SceneManager::onCreateCube);
    connect(ui->pushButton_pan, &QPushButton::toggled, ui->scene, &SceneManager::onPanToggled);
    connect(ui->pushButton_rotate, &QPushButton::toggled, ui->scene, &SceneManager::onRotateToggled);
    connect(ui->pushButton_zoom, &QPushButton::toggled, ui->scene, &SceneManager::onZoomToggled);
    connect(ui->scene, &SceneManager::UpdateStatusLabel, this, &MainWindow::UpdateStatusLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateStatusLabel(const QString &msg)
{
    ui->label_message->setText(msg);
}

void MainWindow::on_pushButton_rotate_toggled(bool checked)
{
    if (checked) {
        ui->pushButton_pan->setChecked(false);
        ui->pushButton_zoom->setChecked(false);
    }
}

void MainWindow::on_pushButton_pan_toggled(bool checked)
{
    if (checked) {
        ui->pushButton_rotate->setChecked(false);
        ui->pushButton_zoom->setChecked(false);
    }
}

void MainWindow::on_pushButton_zoom_toggled(bool checked)
{
    if (checked) {
        ui->pushButton_pan->setChecked(false);
        ui->pushButton_rotate->setChecked(false);
    }
}
