#ifndef MESH_H
#define MESH_H

#include <QVector3D>
#include <QVector4D>
#include <QtOpenGL>

struct VerticeInfo {
    QVector3D pos;
    QVector4D color;
};

class Mesh
{
public:
    explicit Mesh(const QVector<VerticeInfo> &vertices, const QVector<GLushort> &indices);

    const QVector<VerticeInfo> &getVertices();
    const QVector<GLushort> &getIndices();

private:
    QVector<VerticeInfo> m_vertices;
    QVector<GLushort> m_indices;
};

#endif    // MESH_H
