#include <raytracing/model.h>

namespace rtmodel {
    std::vector<std::string> split(std::string str, char c) {
        std::string part;
        std::vector<std::string> parts;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == c) {
                parts.push_back(part);
                part = "";
                continue;
            }
            part += str[i];
        }
        parts.push_back(part);
        return parts;
    }
    
    std::vector<rt::Object> loadObj(std::string path, glm::vec3 position, rt::Material material) {
        std::vector<rt::Object> objList;
        std::ifstream file(path.c_str());
        std::vector<glm::vec3> vertices;
        for( std::string line; std::getline(file, line); ) {
            std::vector<std::string> parts = split(line, ' ');
            if (parts[0].c_str()[0] == 'v') {
                vertices.push_back(glm::vec3(std::stof(parts[1]), std::stof(parts[2]), std::stof(parts[3])) + position);
            }
            else if (parts[0].c_str()[0] == 'f') {
                objList.push_back(rt::createTriangle(vertices[std::stoi(split(parts[1], '/')[0]) - 1], vertices[std::stoi(split(parts[2], '/')[0]) - 1], vertices[std::stoi(split(parts[3], '/')[0]) - 1], material));
            }
        }
        return objList;
    }
};