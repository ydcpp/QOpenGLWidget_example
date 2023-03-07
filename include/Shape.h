#ifndef SHAPE_H
#define SHAPE_H

#include "Mesh.h"
#include "Material.h"
#include <QMatrix4x4>
#include <QString>

#include <memory>

class Shape
{
public:
    virtual ~Shape() = default;
    virtual std::shared_ptr<Mesh> getMesh() = 0;
    virtual std::shared_ptr<Material> getMaterial() = 0;
    virtual QMatrix4x4 &getTransformation() = 0;
    virtual QString ID() const = 0;
};

#endif    // SHAPE_H
