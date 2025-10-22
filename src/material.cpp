#include "../include/material.hpp"

Material::Material()
{
    _ambient[0] = 0.2f; _ambient[1] = 0.2f; _ambient[2] = 0.2f; _ambient[3] = 1.0f;
    _diffuse[0] = 0.8f; _diffuse[1] = 0.8f; _diffuse[2] = 0.8f; _diffuse[3] = 1.0f;
    _specular[0] = 0.0f; _specular[1] = 0.0f; _specular[2] = 0.0f; _specular[3] = 1.0f;
    _shininess = 0.0f;
    _opacity = 1.0f;

    // Fur runtime params
    _hasFur = false;
    _furLayers = 0;
    _furLength = 0.0f;
    _furOpacity = 0.0f;
    _furColor[0] = 0.0f; _furColor[1] = 0.0f; _furColor[2] = 0.0f;

    // Initialiser les buffers temporaires
    __ambient[0] = 0.2f; __ambient[1] = 0.2f; __ambient[2] = 0.2f; __ambient[3] = 1.0f;
    __diffuse[0] = 0.8f; __diffuse[1] = 0.8f; __diffuse[2] = 0.8f; __diffuse[3] = 1.0f;
    __specular[0] = 0.0f; __specular[1] = 0.0f; __specular[2] = 0.0f; __specular[3] = 1.0f;
    __shininess = 0.0f;
    __opacity = 1.0f;

    // Fur parsing buffers (defaults off)
    __hasFur = false;
    __furLayers = 0;
    __furLength = 0.0f;
    __furOpacity = 0.0f;
    __furColor[0] = 0.0f; __furColor[1] = 0.0f; __furColor[2] = 0.0f;
}

Material::~Material() {}

void Material::apply() const {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, _ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, _diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, _specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, _shininess);
}

void Material::setvalue() {
    for (int i = 0; i < 4; ++i) {
        _ambient[i] = __ambient[i];
        _diffuse[i] = __diffuse[i];
        _specular[i] = __specular[i];
    }
    _shininess = __shininess;
    _opacity = __opacity;

    // Commit fur params
    _hasFur = __hasFur && __furLayers > 0 && __furLength > 0.0f && __furOpacity > 0.0f;
    _furLayers = __furLayers;
    _furLength = __furLength;
    _furOpacity = __furOpacity;
    _furColor[0] = __furColor[0];
    _furColor[1] = __furColor[1];
    _furColor[2] = __furColor[2];
}

bool Material::loadFromMTL(const std::string &filename, const std::string &materialName) 
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open MTL file: " << filename << std::endl;
        return false;
    }
    std::string line;
    bool materialFound = false;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "newmtl") {
            std::string name;
            iss >> name;
            materialFound = (name == materialName);
        } else if (materialFound) {
            if (prefix == "Ka") {
                iss >> __ambient[0] >> __ambient[1] >> __ambient[2];
                __ambient[3] = 1.0f;
            } else if (prefix == "Kd") {
                iss >> __diffuse[0] >> __diffuse[1] >> __diffuse[2];
                __diffuse[3] = 1.0f;
            } else if (prefix == "Ks") {
                iss >> __specular[0] >> __specular[1] >> __specular[2];
                __specular[3] = 1.0f;
            } else if (prefix == "Ns") {
                iss >> __shininess;
            } else if (prefix == "d") {
                iss >> __opacity;
            } else if (prefix == "fur_layers") {
                iss >> __furLayers;
                __hasFur = true;
            } else if (prefix == "fur_length") {
                iss >> __furLength;
                __hasFur = true;
            } else if (prefix == "fur_opacity") {
                iss >> __furOpacity;
                __hasFur = true;
            } else if (prefix == "fur_color") {
                iss >> __furColor[0] >> __furColor[1] >> __furColor[2];
            } else if (prefix == "Tr") {
                float transparency;
                iss >> transparency;
                __opacity = 1.0f - transparency;
            } else if (prefix == "newmtl") {
                break;
            }
        }
    }
    return true;
}

void Material::reset() 
{
    _ambient[0] = 0.2f; _ambient[1] = 0.2f; _ambient[2] = 0.2f; _ambient[3] = 1.0f;
    _diffuse[0] = 0.8f; _diffuse[1] = 0.8f; _diffuse[2] = 0.8f; _diffuse[3] = 1.0f;
    _specular[0] = 0.0f; _specular[1] = 0.0f; _specular[2] = 0.0f; _specular[3] = 1.0f;
    _shininess = 0.0f;
    _opacity = 1.0f;

    _hasFur = false; _furLayers = 0; _furLength = 0.0f; _furOpacity = 0.0f;
    _furColor[0] = 0.0f; _furColor[1] = 0.0f; _furColor[2] = 0.0f;
}

float Material::getFurColor(int index) const
{
    if (index < 0 || index > 2) return 0.0f;
    return _furColor[index];
}