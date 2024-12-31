#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>
using namespace std;

Model* Model::fromObjectFile(const char* obj_file) {
  Model* m = new Model();

  ifstream ObjFile(obj_file);

  if (!ObjFile.is_open()) {
    cout << "Can't open File !" << endl;
    return NULL;
  }

  /* TODO#1: Load model data from OBJ file
   *         You only need to handle v, vt, vn, f
   *         Other fields you can directly ignore
   *         Fill data into m->positions, m->texcoords m->normals and m->numVertex
   *         Data format:
   *           For positions and normals
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   | ...
   *         | face 1                                                       | face 2               ...
   *         | v1x  | v1y  | v1z  | v2x  | v2y  | v2z  | v3x  | v3y  | v3z  | v1x  | v1y  | v1z  | ...
   *         | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | ...
   *           For texcoords
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | ...
   *         | face 1                                  | face 2        ...
   *         | v1x  | v1y  | v2x  | v2y  | v3x  | v3y  | v1x  | v1y  | ...
   * Note:
   *        OBJ File Format (https://en.wikipedia.org/wiki/Wavefront_.obj_file)
   *        Vertex per face = 3 or 4
   */

  string line;
  string prefix;
  stringstream ss;

  vector<glm::vec3> positions;
  vector<glm::vec3> normals;
  vector<glm::vec2> textures;

  glm::vec2 tempVec2;
  glm::vec3 tempVec3;

  /*** Add a dummy element since the index starts from 1 ***/
  positions.push_back(tempVec3);
  normals.push_back(tempVec3);
  textures.push_back(tempVec2);

  GLint lint = 0;

  while (getline(ObjFile, line)) {
    ss.clear();
    ss.str(line);
    ss >> prefix;

    /*** If it is position ***/
    if (prefix == "v") {
      ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
      positions.push_back(tempVec3);
    }
    /*** If it is normal ***/
    else if (prefix == "vn") {
      ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
      normals.push_back(tempVec3);
    }
    /*** If it is texture ***/
    else if (prefix == "vt") {
      ss >> tempVec2.x >> tempVec2.y;
      textures.push_back(tempVec2);
    }
    else if (prefix == "f") {
      int cnt = 0;

      while (ss >> lint) {
        if (cnt == 0) {
          m->positions.push_back(positions[lint].x);
          m->positions.push_back(positions[lint].y);
          m->positions.push_back(positions[lint].z);
        }
        else if (cnt == 1) {
          m->texcoords.push_back(textures[lint].x);
          m->texcoords.push_back(textures[lint].y);
        }
        else if (cnt == 2) {
          m->normals.push_back(normals[lint].x);
          m->normals.push_back(normals[lint].y);
          m->normals.push_back(normals[lint].z);
        }

        /*** Bypass if the next character is '/' or ' ' ***/
        if (ss.peek() == '/') {
          cnt++;
          ss.ignore(1, '/');
        } 
        else if (ss.peek() == ' ') {
          cnt++;
          ss.ignore(1, ' ');
        }

        cnt %= 3;
      }
    }
  }

  m->numVertex = m->positions.size();

  if (ObjFile.is_open()) ObjFile.close();

  return m;
}
