#include "Material.h"

#include <QRandomGenerator>

Material::Material()
{
    std::uniform_real_distribution randColor(0.0, 1.0);
    for (int i = 0; i < MATERIAL_COLOR_COUNT; i++) {
        Color[i] = QVector4D(randColor(*QRandomGenerator::global()), randColor(*QRandomGenerator::global()),
                             randColor(*QRandomGenerator::global()), 1.0f);
    }
}
