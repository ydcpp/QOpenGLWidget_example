#include "Cube.h"

#include <QRandomGenerator>
#include <QQuaternion>

Cube::Cube(const QString &id) :
    m_id(id)
{
    m_material = std::make_shared<Material>();

    // clang-format off
    QVector<VerticeInfo> vertices = {
        // Vertex data for face 0
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)}, // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), m_material->Color[0]}, // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), m_material->Color[0]}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), m_material->Color[1]}, // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), m_material->Color[1]}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)}, // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), m_material->Color[2]}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), m_material->Color[2]}, // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector4D(1.0f, 1.0f, 1.0f, 1.0f)}, // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), m_material->Color[3]}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), m_material->Color[3]}, // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector4D(0.5f, 0.5f, 0.5f, 1.0f)}, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector4D(0.5f, 0.5f, 0.5f, 1.0f)}, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector4D(0.5f, 0.5f, 0.5f, 1.0f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector4D(0.5f, 0.5f, 0.5f, 1.0f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f), m_material->Color[5]}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), m_material->Color[5]}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), m_material->Color[5]}, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), m_material->Color[5]}  // v23
    };

    QVector<GLushort> indices = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };
    // clang-format on
    m_mesh = std::make_shared<Mesh>(vertices, indices);

    std::uniform_real_distribution rand(-10.0, 10.0);

    QVector3D pos(rand(*QRandomGenerator::global()), rand(*QRandomGenerator::global()), rand(*QRandomGenerator::global()));
    m_transformation.setToIdentity();
    m_transformation.translate(pos);
}

std::shared_ptr<Mesh> Cube::getMesh()
{
    return m_mesh;
}

std::shared_ptr<Material> Cube::getMaterial()
{
    return m_material;
}

QMatrix4x4 &Cube::getTransformation()
{
    return m_transformation;
}

QString Cube::ID() const
{
    return m_id;
}
