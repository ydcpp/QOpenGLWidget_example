#include "Mesh.h"

Mesh::Mesh(const QVector<VerticeInfo> &vertices, const QVector<GLushort> &indices) :
    m_vertices(vertices),
    m_indices(indices)
{
}

const QVector<VerticeInfo> &Mesh::getVertices()
{
    return m_vertices;
}

const QVector<GLushort> &Mesh::getIndices()
{
    return m_indices;
}
