#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector4D>

#define MATERIAL_COLOR_COUNT 6

struct Material {
    Material();
    QVector4D Color[MATERIAL_COLOR_COUNT];
};

#endif    // MATERIAL_H
