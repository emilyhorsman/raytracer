/**
 * This parsing code is fairly rudimentary and fairly unpleasant. It lets many
 * erroenous scene files off the hook.
 */
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <utility>

#include "Material.h"
#include "PointLight.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneFile.h"


#define ASSIGN_VEC3F(key, prop, map, i) i = map.find(key); if (i != map.end() && i->second.size() == 3) prop = mapToVec3f(i->second);
#define ASSIGN_FLOAT(key, prop, map, i) i = map.find(key); if (i != map.end() && i->second.size() == 1) prop = i->second.at(0);


Vec3f mapToVec3f(std::vector<float> v) {
    Vec3f a;
    std::copy_n(v.begin(), 3, a.begin());
    return a;
}


std::shared_ptr<Material> defaultMaterial = std::make_shared<Material>(
    Vec3f({ 1, 0, 0 }),
    0, 1, 0, 0, 1
);


std::shared_ptr<Material> getMaterial(FloatProperties properties, std::string materialType) {
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
        mat = std::dynamic_pointer_cast<Material>(m);
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


std::string trimString(std::string s) {
    std::string::size_type index = s.find_first_not_of(" ");
    if (index == std::string::npos) {
        return "";
    }
    return s.substr(index);
}


std::pair<std::string, std::string> parseKey(std::string line) {
    std::string::size_type index = line.rfind(":");
    if (index == std::string::npos) {
        throw "Property must be of form `key: value`";
    }

    return std::make_pair(line.substr(0, index), line.substr(index + 1));
}


void parseProperty(std::string line, std::string &key, std::vector<float> &value, std::string &stringValue) {
    std::string::size_type index = line.rfind(":");
    if (index == std::string::npos) {
        throw "Property must be of form `key: value`";
    }

    auto t = parseKey(line);
    std::string v;
    std::tie(key, v) = t;
    if (key == "material") {
        stringValue = trimString(line.substr(index + 1));
        return;
    }

    auto parts = split(v, ",");
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

    std::string row;
    FloatProperties properties;
    while (true) {
        std::getline(stream, row);
        if (row.empty()) {
            break;
        }

        std::string key, _v;
        std::vector<float> value;
        parseProperty(row, key, value, _v);
        properties[key] = value;
    }

    auto mat = getMaterial(properties, materialType);
    if (mat == NULL) {
        return false;
    }

    materials[materialId] = mat;
    return true;
}


std::shared_ptr<PointLight> getLight(FloatProperties properties) {
    std::shared_ptr<PointLight> light = std::make_shared<PointLight>(
        Vec3f({ 0, 0, 1 }),
        0.1,
        0
    );
    auto i = properties.begin();

    ASSIGN_VEC3F("position", light->mPosition, properties, i);
    ASSIGN_FLOAT("intensity", light->mIntensity, properties, i);
    ASSIGN_FLOAT("radius", light->mRadius, properties, i);

    return light;
}


bool parseLight(std::vector<std::shared_ptr<PointLight>> &lights, std::istream &stream, std::string line, std::string lightType) {
    if (line != lightType) {
        return false;
    }

    std::string row;
    FloatProperties properties;
    while (true) {
        std::getline(stream, row);
        if (row.empty()) {
            break;
        }

        std::string key, _v;
        std::vector<float> value;
        parseProperty(row, key, value, _v);
        properties[key] = value;
    }

    auto light = getLight(properties);
    if (light == NULL) {
        return false;
    }

    lights.push_back(light);
    return true;
}


std::shared_ptr<SceneObject> getObject(FloatProperties properties, Materials &materials, std::string materialId, std::string objectType) {
    std::shared_ptr<SceneObject> obj = NULL;
    auto i = properties.begin();

    if (objectType == "Sphere") {
        auto o = std::make_shared<Sphere>(
            defaultMaterial,
            Vec3f({ 0, 0, -1 }),
            0.25f
        );

        ASSIGN_VEC3F("origin", o->mOrigin, properties, i);
        ASSIGN_FLOAT("radius", o->mRadius, properties, i);
        obj = o;
    } else if (objectType == "Plane") {
        auto o = std::make_shared<Plane>(
            defaultMaterial,
            Vec3f({ 0, -1, 0 }),
            Vec3f({ 0, 1, 0 })
        );

        ASSIGN_VEC3F("point", o->mPoint, properties, i);
        ASSIGN_VEC3F("normal", o->mNormal, properties, i);
        obj = o;
    } else if (objectType == "Disk") {
        auto o = std::make_shared<Disk>(
            defaultMaterial,
            Vec3f({ 0, -1, 0}),
            Vec3f({ 0, 1, 0 }),
            0.25f
        );

        ASSIGN_VEC3F("origin", o->mOrigin, properties, i);
        ASSIGN_VEC3F("normal", o->mNormal, properties, i);
        ASSIGN_FLOAT("radius", o->mRadius, properties, i);
        obj = o;
    }

    auto m = materials.find(materialId);
    if (m != materials.end()) {
        obj->mMaterial = m->second;
    }

    return obj;
}


bool parseObject(std::vector<std::shared_ptr<SceneObject>> &objects, Materials &materials, std::istream &stream, std::string line, std::string objectType) {
    if (line != objectType) {
        return false;
    }

    std::string row;
    FloatProperties properties;
    std::string materialId;
    while (true) {
        std::getline(stream, row);
        if (row.empty()) {
            break;
        }

        std::string key;
        std::vector<float> value;
        parseProperty(row, key, value, materialId);
        properties[key] = value;
    }

    auto object = getObject(properties, materials, materialId, objectType);
    if (object == NULL) {
        return false;
    }

    objects.push_back(object);
    return true;
}


bool parseRenderer(Renderer &renderer, std::istream &stream, std::string line, std::string tag) {
    if (line != tag) {
        return false;
    }

    std::string row;
    FloatProperties properties;
    std::string materialId;
    while (true) {
        std::getline(stream, row);
        if (row.empty()) {
            break;
        }

        std::string key, value;
        std::tie(key, value) = parseKey(row);
        value = trimString(value);
        if (key == "width") {
            renderer.mWidth = std::stoi(value);
        } else if (key == "height") {
            renderer.mHeight = std::stoi(value);
        } else if (key == "maxDepth") {
            renderer.mMaxDepth = std::stoi(value);
        } else if (key == "antiAliasing") {
            renderer.mAntiAliasing = std::stoi(value);
        } else if (key == "iterations") {
            renderer.mNoiseReduction = std::stoi(value);
        } else if (key == "threads") {
            renderer.mNumThreads = std::stoi(value);
        } else if (key == "samplingMethod") {
            if (value == "regular") {
                renderer.mAntiAliasingMethod = REGULAR;
            } else if (value == "random") {
                renderer.mAntiAliasingMethod = RANDOM;
            } else {
                std::cout << "Invalid anti-aliasing method. Must be 'regular' or 'random'." << std::endl;
                throw "Invalid anti-aliasing method. Must be 'regular' or 'random'.";
            }
        } else if (key == "useSoftShadows") {
            if (value == "true") {
                renderer.mEnableSoftShadows = true;
            } else if (value == "false") {
                renderer.mEnableSoftShadows = false;
            } else {
                std::cout << "Invalid useSoftShadows. Must be 'true' or 'false'." << std::endl;
                throw "Invalid useSoftShadows. Must be 'true' or 'false'.";
            }
        } else {
            std::cout << "Invalid Renderer key: " << key << std::endl;
            throw "Invalid renderer key.";
        }
    }

    return true;
}


bool parseCamera(Camera &camera, std::istream &stream, std::string line, std::string tag) {
    if (line != tag) {
        return false;
    }

    std::string row;
    FloatProperties properties;
    std::string materialId;
    while (true) {
        std::getline(stream, row);
        if (row.empty()) {
            break;
        }

        std::string key, value;
        std::tie(key, value) = parseKey(row);
        value = trimString(value);
        if (key == "fieldOfViewDegrees") {
            camera.mFieldOfViewRadians = std::stof(value) * M_PI / 180.0f;
        } else if (key == "eye" || key == "lookAt") {
            auto parts = split(value, ",");
            std::vector<float> v;
            for (auto s : parts) {
                if (s.empty()) {
                    throw "Empty value.";
                }
                v.push_back(std::stof(s));
            }
            if (v.size() != 3) {
                std::cout << "Invalid value for " << key << ", must have 3 comma-separated floats." << std::endl;
                throw "Invalid camera value.";
            }

            Vec3f vec = mapToVec3f(v);
            if (key == "eye") {
                camera.mPosition = vec;
            } else if (key == "lookAt") {
                camera.mLookAt = vec;
            }
        } else if (key == "apertureRadius") {
            camera.mApertureRadius = std::stof(value);
        } else {
            std::cout << "Invalid Camera key: " << key << std::endl;
            throw "Invalid camera key.";
        }
    }

    return true;
}


bool loadSceneFile(Renderer &renderer, Scene &scene, std::string file) {
    std::ifstream f(file);
    if (!f.is_open()) {
        std::cout << "Failed to open " << file << ". The scene was not loaded.\n" << std::endl;
        return false;
    }

    Materials materials;
    while (f.peek() != EOF) {
        std::string line;
        std::getline(f, line);
        // This is so bad
        bool result = parseMaterial(materials, f, line, "CheckerboardMaterial");
        if (!result) {
            result = parseMaterial(materials, f, line, "Material");
        }
        if (!result) {
            result = parseLight(scene.mPointLights, f, line, "PointLight");
        }
        if (!result) {
            result = parseObject(scene.mObjects, materials, f, line, "Plane");
        }
        if (!result) {
            result = parseObject(scene.mObjects, materials, f, line, "Sphere");
        }
        if (!result) {
            result = parseObject(scene.mObjects, materials, f, line, "Disk");
        }
        if (!result) {
            result = parseRenderer(renderer, f, line, "Renderer");
        }
        if (!result) {
            result = parseCamera(scene.mCamera, f, line, "Camera");
        }
    }

    f.close();

    return true;
}
