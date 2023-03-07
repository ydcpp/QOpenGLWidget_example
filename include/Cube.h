#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

class Cube : public Shape
{
public:
    explicit Cube(const QString &id);
    virtual ~Cube() = default;

    virtual std::shared_ptr<Mesh> getMesh() override;
    virtual std::shared_ptr<Material> getMaterial() override;
    virtual QMatrix4x4 &getTransformation() override;
    virtual QString ID() const override;

private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;
    QMatrix4x4 m_transformation;
    QString m_id;
};

#endif // CUBE_H
