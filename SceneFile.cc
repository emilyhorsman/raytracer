#include <iostream>
#include <fstream>
#include <map>

#include "Material.h"
#include "Scene.h"
#include "SceneFile.h"


#define ASSIGN_VEC3F(key, prop, map, i) i = map.find(key); if (i != map.end() && i->second.size() == 3) prop = mapToVec3f(i->second);
#define ASSIGN_FLOAT(key, prop, map, i) i = map.find(key); if (i != map.end() && i->second.size() == 1) prop = i->second.at(0);


Vec3f mapToVec3f(std::vector<float> v) {
    Vec3f a;
    std::copy_n(v.begin(), 3, a.begin());
    return a;
}


std::shared_ptr<Material> getMaterial(MaterialProperties properties, std::string materialType) {
    std::shared_ptr<Material> mat = NULL;
    auto i = properties.begin();
    if (materialType == "CheckerboardMaterial") {
        auto m = std::make_shared<CheckerboardMaterial>(
            Vec3f({ 1, 1, 1 }),
            Vec3f({ 0, 0, 0 }),
            0, 1, 0, 0, 1, 0.5f
        );

        ASSIGN_VEC3F("odd", m->oddColor, properties, i);
        ASSIGN_FLOAT("grain", m->size, properties, i);
        mat = m;
    } else if (materialType == "Material") {
        mat = std::make_shared<Material>(
            Vec3f({ 0, 0, 0 }),
            0, 1, 0, 0, 1
        );
    }

    ASSIGN_VEC3F("color", mat->color, properties, i);
    ASSIGN_FLOAT("ambient", mat->ambient, properties, i);
    ASSIGN_FLOAT("diffuse", mat->diffuse, properties, i);
    ASSIGN_FLOAT("specular", mat->specular, properties, i);
    ASSIGN_FLOAT("transmission", mat->transmission, properties, i);
    ASSIGN_FLOAT("refractiveIndex", mat->refractiveIndex, properties, i);

    return mat;
}


std::vector<std::string> split(std::string s, std::string delimiter) {
    std::vector<std::string> parts;
    std::string::size_type index = 0;
    while ((index = s.find(delimiter)) != std::string::npos) {
        parts.push_back(s.substr(0, index));
        s.erase(0, index + delimiter.length());
    }
    parts.push_back(s);

    return parts;
}


void parseProperty(std::string line, std::string &key, std::vector<float> &value) {
    std::string::size_type index = line.rfind(":");
    if (index == std::string::npos) {
        throw "Property must be of form `key: value`";
    }

    key = line.substr(0, index);
    auto parts = split(line.substr(index + 1), ",");
    for (auto s : parts) {
        if (s.empty()) {
            throw "Empty value.";
        }
        value.push_back(std::stof(s));
    }
}


bool parseMaterial(Materials &materials, std::istream &stream, std::string line, std::string materialType) {
    if (line.rfind(materialType, 0) != 0) {
        return false;
    }

    std::string materialId = line.substr(materialType.length() + 1);
    if (materialId.empty()) {
        throw "No material ID for material.";
    }
    std::cout << "Found " << materialType << " with ID " << materialId << std::endl;

    std::string row;
    MaterialProperties properties;
    while (true) {
        std::getline(stream, row);
        if (row.empty()) {
            break;
        }

        std::string key;
        std::vector<float> value;
        parseProperty(row, key, value);
        properties[key] = value;
    }

    auto mat = getMaterial(properties, materialType);
    materials[materialId] = mat;
    return true;
}


bool loadSceneFile(Scene &scene, std::string file) {
    std::ifstream f(file);
    if (!f.is_open()) {
        std::cout << "Failed to open " << file << ". The scene was not loaded.\n" << std::endl;
        return false;
    }

    Materials materials;
    while (f.peek() != EOF) {
        std::string line;
        std::getline(f, line);
        bool result = parseMaterial(materials, f, line, "CheckerboardMaterial");
        if (!result) {
            result = parseMaterial(materials, f, line, "Material");
        }
    }

    f.close();

    return true;
}
