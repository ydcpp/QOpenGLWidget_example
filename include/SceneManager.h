#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QString>

#include <unordered_map>

class Shape;

enum class CameraState { NONE, ROTATE, PAN, ZOOM };

struct Camera {
    QVector3D Position;
    QVector3D LookAt;
    QVector3D Up;
    QVector3D Right;
    float FOV;
    CameraState State;
};

namespace Ui
{
class SceneManager;
}

class SceneManager : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit SceneManager(QWidget *parent = nullptr);
    ~SceneManager();

signals:
    void UpdateStatusLabel(const QString &msg);

public slots:
    void onCreateCube();
    void onPanToggled(bool checked);
    void onRotateToggled(bool checked);
    void onZoomToggled(bool checked);
protected slots:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *event) override;
    void PrintLoggedMessage(const QOpenGLDebugMessage &debugMessage);

protected:
    void InitalizeShaders();
    void InitalizeBuffers();
    void initializeGL() override;
    void paintGL() override;

private:
    Ui::SceneManager *ui;
    QOpenGLDebugLogger m_logger;
    QOpenGLShaderProgram m_program;
    std::unordered_map<QString, Shape *> m_shapes;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QOpenGLBuffer m_arrayBuf;
    QOpenGLBuffer m_indexBuf;
    Camera m_camera;
    Shape *m_selected_shape;

    const float m_near_z = 2.0f;
    const float m_far_z = 200.0f;
    const float m_default_fov = 30.0f;
    const float m_rotation_speed_scalar = 2.0f;

    void renderAll();
    Shape *pickShape(int x, int y);
    Shape *createShape(const QString &type, QString &id);
    void PanViewport(int key);
    void ZoomViewport(int key);
    void RotateViewport(int key);

    void CastRayFromScreenToWorld(int mouseX, int mouseY, QVector3D &out_origin, QVector3D &out_direction);
    bool RayIntersectionTest(Shape *shape, const QVector3D &ray_origin, const QVector3D &ray_direction);
};

#endif    // SCENEMANAGER_H
