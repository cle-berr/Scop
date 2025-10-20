#include "../include/material.hpp"

void Material::apply() const {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, _ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, _diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, _specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, _shininess);
}

