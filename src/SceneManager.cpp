#include "SceneManager.h"
#include "ui_SceneManager.h"

#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QRandomGenerator>
#include <QUuid>
#include <QVector3D>
#include <QVector4D>
#include <QDebug>
#include "Cube.h"

SceneManager::SceneManager(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::SceneManager),
    m_arrayBuf(QOpenGLBuffer::VertexBuffer),
    m_indexBuf(QOpenGLBuffer::IndexBuffer),
    m_selected_shape(nullptr)
{
    ui->setupUi(this);
    connect(&m_logger, &QOpenGLDebugLogger::messageLogged, this, &SceneManager::PrintLoggedMessage);
    m_camera.Position = QVector3D(-30.0f, 30.0f, 40.0f);
    m_camera.LookAt = QVector3D(0.0f, 0.0f, 0.0f);
    QVector3D dir = (m_camera.Position - m_camera.LookAt).normalized();
    m_camera.Right = QVector3D::crossProduct(QVector3D(0.0f, 1.0f, 0.0f), dir).normalized();
    m_camera.Up = QVector3D::crossProduct(dir, m_camera.Right).normalized();
    m_camera.FOV = m_default_fov;
    m_camera.State = CameraState::NONE;
}

SceneManager::~SceneManager()
{
    makeCurrent();
    m_logger.stopLogging();
    m_arrayBuf.destroy();
    m_indexBuf.destroy();
    m_program.release();
    doneCurrent();
    for (const auto &shape : m_shapes) {
        if (shape.second) {
            delete shape.second;
        }
    }
    delete ui;
}

void SceneManager::renderAll()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto &shape : m_shapes) {
        Shape *cube = shape.second;
        auto vertices = cube->getMesh()->getVertices();
        auto indices = cube->getMesh()->getIndices();

        // Update vertex and index buffers
        m_arrayBuf.write(0, vertices.data(), (int)vertices.size() * sizeof(VerticeInfo));
        m_indexBuf.write(0, indices.data(), (int)indices.size() * sizeof(GLushort));

        // Model View Projection
        static const QVector3D up(0.0f, 1.0f, 0.0f);
        QVector3D dir = (m_camera.Position - m_camera.LookAt).normalized();
        m_camera.Right = QVector3D::crossProduct(up, dir).normalized();
        m_camera.Up = QVector3D::crossProduct(dir, m_camera.Right).normalized();
        m_view.setToIdentity();
        m_view.lookAt(m_camera.Position, m_camera.LookAt, m_camera.Up);

        // Let GPU do the calculation of the final mvp
        m_program.setUniformValue("u_proj", m_projection);
        m_program.setUniformValue("u_view", m_view);
        m_program.setUniformValue("u_trans", cube->getTransformation());

        int offset = 0;
        // Vertex positions
        int vertexLocation = m_program.attributeLocation("a_position");
        Q_ASSERT(vertexLocation != -1);
        m_program.enableAttributeArray(vertexLocation);
        m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(VerticeInfo));

        offset += (int)sizeof(QVector3D);

        // Colors for the surfaces
        int colorLocation = m_program.attributeLocation("a_color");
        Q_ASSERT(colorLocation != -1);
        m_program.enableAttributeArray(colorLocation);
        m_program.setAttributeBuffer(colorLocation, GL_FLOAT, offset, 4, sizeof(VerticeInfo));

        // Draw the shape
        glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_SHORT, nullptr);
    }

    if (m_selected_shape) {
        // Draw x-y-z axes of the selected shape from its center
        m_program.setUniformValue("u_trans", m_selected_shape->getTransformation());

        static const QVector<VerticeInfo> vertices = { { QVector3D(0.0f, 0.0f, 0.0f), QVector4D(1.0f, 0.0f, 0.0f, 1.0f) },
                                                       { QVector3D(6.0f, 0.0f, 0.0f), QVector4D(1.0f, 0.0f, 0.0f, 1.0f) },
                                                       { QVector3D(0.0f, 0.0f, 0.0f), QVector4D(0.0f, 1.0f, 0.0f, 1.0f) },
                                                       { QVector3D(0.0f, 6.0f, 0.0f), QVector4D(0.0f, 1.0f, 0.0f, 1.0f) },
                                                       { QVector3D(0.0f, 0.0f, 0.0f), QVector4D(0.0f, 0.0f, 1.0f, 1.0f) },
                                                       { QVector3D(0.0f, 0.0f, 6.0f), QVector4D(0.0f, 0.0f, 1.0f, 1.0f) } };

        m_arrayBuf.write(0, vertices.data(), (int)vertices.size() * sizeof(VerticeInfo));

        glDrawArrays(GL_LINES, 0, vertices.size());
    }
}

Shape *SceneManager::pickShape(int mouse_x, int mouse_y)
{
    QVector3D ray_origin;
    QVector3D ray_direction;
    CastRayFromScreenToWorld(mouse_x, this->height() - mouse_y, ray_origin, ray_direction);

    for (const auto &shape : m_shapes) {
        if (RayIntersectionTest(shape.second, ray_origin, ray_direction)) {
            return shape.second;
        }
    }

    return nullptr;
}

Shape *SceneManager::createShape(const QString &type, QString &id)
{
    if (type == "Cube") {
        id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        Shape *newShape = new Cube(id);
        return newShape;
    }
    return nullptr;
}

void SceneManager::onCreateCube()
{
    QString id;
    Shape *newShape = createShape("Cube", id);
    if (newShape) {
        m_shapes[id] = newShape;
        qDebug() << " new cube id = " << id;
    }
    update();
}

void SceneManager::onPanToggled(bool checked)
{
    m_camera.State = checked ? CameraState::PAN : CameraState::NONE;
}

void SceneManager::onRotateToggled(bool checked)
{
    m_camera.State = checked ? CameraState::ROTATE : CameraState::NONE;
}

void SceneManager::onZoomToggled(bool checked)
{
    m_camera.State = checked ? CameraState::ZOOM : CameraState::NONE;
}

void SceneManager::keyPressEvent(QKeyEvent *event)
{
    auto key = event->key();
    switch (m_camera.State) {
    case CameraState::PAN:
        PanViewport(key);
        break;
    case CameraState::ZOOM:
        ZoomViewport(key);
        break;
    case CameraState::ROTATE:
        RotateViewport(key);
        break;
    default:
        break;
    }
}

void SceneManager::mousePressEvent(QMouseEvent *e)
{
    Shape *cube = pickShape(e->pos().x(), e->pos().y());
    if (!cube) {
        m_selected_shape = nullptr;
        emit UpdateStatusLabel("Nothing is selected.");
    } else {
        m_selected_shape = cube;
        emit UpdateStatusLabel("Cube is selected.");
    }
    update();
}

void SceneManager::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        // Zoom in
        ZoomViewport(Qt::Key_Up);
    } else if (event->angleDelta().y() < 0) {
        // Zoom out
        ZoomViewport(Qt::Key_Down);
    }
}

void SceneManager::InitalizeShaders()
{
    // Compile vertex shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.glsl")) {
        qDebug() << "SceneManager::InitalizeShaders: Failed to compile vertex shader!";
        close();
    }

    // Compile fragment shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragment.glsl")) {
        qDebug() << "SceneManager::InitalizeShaders: Failed to compile fragment shader!";
        close();
    }

    // Link shader pipeline
    if (!m_program.link()) {
        qDebug() << "SceneManager::InitalizeShaders: Failed to link shaders!";
        close();
    }

    // Bind shader pipeline for use
    if (!m_program.bind()) {
        qDebug() << "SceneManager::InitalizeShaders: Failed to bind shader program!";
        close();
    }
}

void SceneManager::InitalizeBuffers()
{
    if (!m_arrayBuf.create())
        close();
    if (!m_arrayBuf.bind())
        close();
    if (!m_indexBuf.create())
        close();
    if (!m_indexBuf.bind())
        close();

    m_arrayBuf.allocate(nullptr, 24 * sizeof(VerticeInfo));
    m_indexBuf.allocate(nullptr, 34 * sizeof(GLushort));
}

void SceneManager::initializeGL()
{
    qDebug() << Q_FUNC_INFO << ": initializing GL...";
    initializeOpenGLFunctions();
    qDebug() << "OpenGL Version: " << QOpenGLContext::currentContext()->format().majorVersion()
             << QOpenGLContext::currentContext()->format().minorVersion();

    glEnable(GL_DEBUG_OUTPUT);
    m_logger.initialize();
    m_logger.startLogging();

    InitalizeShaders();
    InitalizeBuffers();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    // Enable back face culling
    glEnable(GL_CULL_FACE);

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_projection.setToIdentity();
    m_projection.perspective(m_camera.FOV, (float)this->width() / (float)this->height(), m_near_z, m_far_z);
}

void SceneManager::paintGL()
{
    renderAll();
}

void SceneManager::PanViewport(int key)
{
    switch (key) {
    case Qt::Key_Up:
        m_camera.Position += m_camera.Up;
        m_camera.LookAt += m_camera.Up;
        break;
    case Qt::Key_Down:
        m_camera.Position -= m_camera.Up;
        m_camera.LookAt -= m_camera.Up;
        break;
    case Qt::Key_Right:
        m_camera.Position += m_camera.Right;
        m_camera.LookAt += m_camera.Right;
        break;
    case Qt::Key_Left:
        m_camera.Position -= m_camera.Right;
        m_camera.LookAt -= m_camera.Right;
        break;
    default:
        return;
    }
    update();
}

void SceneManager::ZoomViewport(int key)
{
    switch (key) {
    case Qt::Key_Up:
        m_camera.FOV -= 2.0f;
        if (m_camera.FOV < 2.0f) {
            m_camera.FOV = 2.0f;
        }
        break;
    case Qt::Key_Down:
        m_camera.FOV += 2.0f;
        if (m_camera.FOV > 178.0f) {
            m_camera.FOV = 178.0f;
        }
        break;
    default:
        return;
    }
    // Reset perspective projection
    m_projection.setToIdentity();
    m_projection.perspective(m_camera.FOV, (float)this->width() / (float)this->height(), m_near_z, m_far_z);
    update();
}

void SceneManager::RotateViewport(int key)
{
    switch (key) {
    // Make sure up and down rotations wont flip the scene
    case Qt::Key_Up:
        if (((m_camera.Position.z() > 0.0f) && ((m_camera.Position.z() + m_camera.Up.z() * m_rotation_speed_scalar) > 0.0f)) ||
            ((m_camera.Position.z() < 0.0f) && ((m_camera.Position.z() + m_camera.Up.z() * m_rotation_speed_scalar) < 0.0f))) {
            m_camera.Position += m_camera.Up * m_rotation_speed_scalar;
        }
        break;
    case Qt::Key_Down:
        if (((m_camera.Position.z() > 0.0f) && ((m_camera.Position.z() - m_camera.Up.z() * m_rotation_speed_scalar) > 0.0f)) ||
            ((m_camera.Position.z() < 0.0f) && ((m_camera.Position.z() - m_camera.Up.z() * m_rotation_speed_scalar) < 0.0f))) {
            m_camera.Position -= m_camera.Up * m_rotation_speed_scalar;
        }
        break;
    case Qt::Key_Right:
        m_camera.Position += m_camera.Right * m_rotation_speed_scalar;
        break;
    case Qt::Key_Left:
        m_camera.Position -= m_camera.Right * m_rotation_speed_scalar;
        break;
    default:
        return;
    }
    update();
}

void SceneManager::CastRayFromScreenToWorld(int mouseX, int mouseY, QVector3D &out_origin, QVector3D &out_direction)
{
    // The ray Start and End positions, in Normalized Device Coordinates
    QVector4D lRayStart_NDC(((float)mouseX / (float)this->width() - 0.5f) * 2.0f, ((float)mouseY / (float)this->height() - 0.5f) * 2.0f,
                            -1.0,    // The near plane maps to Z=-1 in Normalized Device Coordinates
                            1.0f);
    QVector4D lRayEnd_NDC(((float)mouseX / (float)this->width() - 0.5f) * 2.0f, ((float)mouseY / (float)this->height() - 0.5f) * 2.0f, 0.0,
                          1.0f);

    // The Projection matrix goes from Camera Space to NDC.
    // So inverse(ProjectionMatrix) goes from NDC to Camera Space.
    QMatrix4x4 InverseProjectionMatrix = m_projection.inverted();

    // The View Matrix goes from World Space to Camera Space.
    // So inverse(ViewMatrix) goes from Camera Space to World Space.
    QMatrix4x4 InverseViewMatrix = m_view.inverted();

    QVector4D lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;
    lRayStart_camera /= lRayStart_camera.w();
    QVector4D lRayStart_world = InverseViewMatrix * lRayStart_camera;
    lRayStart_world /= lRayStart_world.w();
    QVector4D lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;
    lRayEnd_camera /= lRayEnd_camera.w();
    QVector4D lRayEnd_world = InverseViewMatrix * lRayEnd_camera;
    lRayEnd_world /= lRayEnd_world.w();

    QVector3D lRayDir_world(lRayEnd_world - lRayStart_world);
    lRayDir_world = lRayDir_world.normalized();

    out_origin = QVector3D(lRayStart_world);
    out_direction = lRayDir_world.normalized();
}

bool SceneManager::RayIntersectionTest(Shape *shape, const QVector3D &ray_origin, const QVector3D &ray_direction)
{
    // Intersection method from Real-Time Rendering and Essential Mathematics for Games
    float tMin = 0.0f;
    float tMax = 100000.0f;

    QVector3D OBBposition_worldspace(shape->getTransformation().column(3).x(), shape->getTransformation().column(3).y(),
                                     shape->getTransformation().column(3).z());

    QVector3D delta = OBBposition_worldspace - ray_origin;
    static const QVector3D aabb_min(-1.0f, -1.0f, -1.0f);
    static const QVector3D aabb_max(1.0f, 1.0f, 1.0f);

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        QVector3D xaxis(shape->getTransformation().column(0).x(), shape->getTransformation().column(0).y(),
                        shape->getTransformation().column(0).z());
        float e = QVector3D::dotProduct(xaxis, delta);
        float f = QVector3D::dotProduct(ray_direction, xaxis);

        if (fabs(f) > 0.001f) {    // Standard case

            float t1 = (e + aabb_min.x()) / f;    // Intersection with the "left" plane
            float t2 = (e + aabb_max.x()) / f;    // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection,
            // so if it's not the case, invert t1 and t2
            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w;    // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if (t2 < tMax)
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if (t1 > tMin)
                tMin = t1;

            // If "far" is closer than "near", then there is NO intersection.
            if (tMax < tMin)
                return false;

        } else {    // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if (-e + aabb_min.x() > 0.0f || -e + aabb_max.x() < 0.0f)
                return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    {
        QVector3D yaxis(shape->getTransformation().column(1).x(), shape->getTransformation().column(1).y(),
                        shape->getTransformation().column(1).z());
        float e = QVector3D::dotProduct(yaxis, delta);
        float f = QVector3D::dotProduct(ray_direction, yaxis);

        if (fabs(f) > 0.001f) {

            float t1 = (e + aabb_min.y()) / f;
            float t2 = (e + aabb_max.y()) / f;

            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w;
            }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;

        } else {
            if (-e + aabb_min.y() > 0.0f || -e + aabb_max.y() < 0.0f)
                return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    {
        QVector3D zaxis(shape->getTransformation().column(2).x(), shape->getTransformation().column(2).y(),
                        shape->getTransformation().column(2).z());
        float e = QVector3D::dotProduct(zaxis, delta);
        float f = QVector3D::dotProduct(ray_direction, zaxis);

        if (fabs(f) > 0.001f) {

            float t1 = (e + aabb_min.z()) / f;
            float t2 = (e + aabb_max.z()) / f;

            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w;
            }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;

        } else {
            if (-e + aabb_min.z() > 0.0f || -e + aabb_max.z() < 0.0f)
                return false;
        }
    }

    return true;
}

void SceneManager::PrintLoggedMessage(const QOpenGLDebugMessage &debugMessage)
{
    qDebug() << debugMessage.message();
}
