#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include "camera.h"
#include "context.h"
#include "gl_helper.h"
#include "model.h"
#include "opengl_context.h"
#include "program.h"
#include "utils.h"

void initOpenGL();
void resizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int, int action, int);

Context ctx;

Material mFlatwhite;
Material mShinyred;
Material mClearblue;

void loadMaterial() {
  mFlatwhite.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
  mFlatwhite.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
  mFlatwhite.specular = glm::vec3(0.0f, 0.0f, 0.0f);
  mFlatwhite.shininess = 10;

  mShinyred.ambient = glm::vec3(0.1985f, 0.0000f, 0.0000f);
  mShinyred.diffuse = glm::vec3(0.5921f, 0.0167f, 0.0000f);
  mShinyred.specular = glm::vec3(0.5973f, 0.2083f, 0.2083f);
  mShinyred.shininess = 100.0f;

  mClearblue.ambient = glm::vec3(0.0394f, 0.0394f, 0.3300f);
  mClearblue.diffuse = glm::vec3(0.1420f, 0.1420f, 0.9500f);
  mClearblue.specular = glm::vec3(0.1420f, 0.1420f, 0.9500f);
  mClearblue.shininess = 10;
}

void loadPrograms() {
  ctx.programs.push_back(new ExampleProgram(&ctx));
  ctx.programs.push_back(new LightProgram(&ctx));
  //ctx.programs.push_back(new BasicProgram(&ctx));

  for (auto iter = ctx.programs.begin(); iter != ctx.programs.end(); iter++) {
    if (!(*iter)->load()) {
      std::cout << "Load program fail, force terminate" << std::endl;
      exit(1);
    }
  }
  glUseProgram(0);
}

Model* createDice() {
  /* TODO#1-1: Add the dice model
   *           1. Create a model by reading the model file "../assets/models/cube/cube.obj" with the object loader(Model::fromObjectFile()) you write.
   *           2. Add the texture "../assets/models/cube/dice.jpg" to the model.
   *           3. Do transform(rotation & scale) to the model.
   *           
   * Note:
   *           You should finish implement the object loader(Model::fromObjectFile()) first.
   *           You can refer to the Model class structure in model.h.
   * Hint:
   *           Model* m = Model::fromObjectFile();
   *           m->textures.push_back();
   *           m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.4f, 0.4f, 0.4f));
   *           ctx.models.push_back(m);
   */

  /*** Create a new Model Object ***/
  Model* m = new Model();
  /*** Read the obj file ***/
  m = Model::fromObjectFile("../assets/models/cube/cube.obj");
  /*** Add the texture to the model ***/
  m->textures.push_back(createTexture("../assets/models/cube/dice.jpg"));
  /*** Resize the object ***/
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.4f, 0.4f, 0.4f));

  return m;
}

Model* createBottle() {
  /* TODO#1-2: Add the bottle model
   *           1. Create a model by reading the model file "../assets/models/bottle/bottle.obj" with the object loader(Model::fromObjectFile()) you write.
   *           2. Add the texture "../assets/models/bottle/bottle.jpg" to the model.
   *           3. Do transform(rotation & scale) to the model.
   *           4. Set the drawMode for this model
   * Note:
   *           You should finish implement the object loader(Model::fromObjectFile()) first.
   *           You can refer to the Model class structure in model.h.
   * Hint:
   *           Model* m = Model::fromObjectFile();
   *           m->textures.push_back();
   *           m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.05f, 0.05f, 0.05f));
   *           m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
   *           m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
   *           m->drawMode = 
   */

  /*** Create a new Model Object ***/
  Model* m = new Model();
  /*** Read the onj file ***/
  m = Model::fromObjectFile("../assets/models/bottle/bottle.obj");
  /*** Add the texture to the model ***/
  m->textures.push_back(createTexture("../assets/models/bottle/bottle.jpg"));
  /*** Do scale and rotation ***/
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.05f, 0.05f, 0.05f));
  m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  /*** Use GL_QUADS to draw it ***/
  m->drawMode = GL_QUADS;

  return m;
}

Model* createMug(std::string texture) {

  /*** Specific the path of texture ***/
  std::string texture_path = "../assets/models/Mugs/Textures/Mug_" + texture + ".png" ;
  /*** Create a new Model Object ***/
  Model *m = new Model();
  /*** Read the onj file ***/
  m = Model::fromObjectFile("../assets/models/Mugs/Models/Mug_obj.obj");
  /*** Add the texture to the model ***/
  m->textures.push_back(createTexture(texture_path.c_str()));
  /*** Scale the mug ***/
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(6.0f, 6.0f, 6.0f));
  /*** Rotate the mug to flip it ***/
  m->modelMatrix = glm::rotate(m->modelMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  return m;
}

Model* createMug3(std::string texture) {

  /*** Specific the path of texture ***/
  std::string texture_path = "../assets/models/Mugs/Textures/Mug_" + texture + ".png" ;
  /*** Create a new Model Object ***/
  Model *m = new Model();
  /*** Read the onj file ***/
  m = Model::fromObjectFile("../assets/models/Mugs/Models/Mug_obj3.obj");
  /*** Add the texture to the model ***/
  m->textures.push_back(createTexture(texture_path.c_str()));
  /*** Scale the mug ***/
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(6.0f, 6.0f, 6.0f));

  return m;
}

void addVertex(Model *m, float x, float y, float z) {
  m->positions.push_back(x);
  m->positions.push_back(y);
  m->positions.push_back(z); 
}

void addNormal(Model *m, float nx, float ny, float nz) {
  m->normals.push_back(nx);
  m->normals.push_back(ny);
  m->normals.push_back(nz);
}

void addTexCoords(Model* m, float startX, float startY, float endX, float endY) {
  m->texcoords.insert(m->texcoords.end(), {startX, startY, endX, startY, endX, endY, startX, endY});
}

auto addTexCoords(Model* m, float u, float v) {
    m->texcoords.push_back(u);
    m->texcoords.push_back(v);
}

Model* createPlane() {
  /* TODO#1-3: Add a plane model
   *           1. Create a model and manually set plane positions, normals, texcoords
   *           2. Add texure "../assets/models/Wood_maps/AT_Wood.jpg"
   *           3. Set m->numVertex, m->drawMode
   * Note:
   *           GL_TEXTURE_WRAP is set to GL_REPEAT in createTexture, you may need to know
   *           what this means to set m->textures correctly
   */
  Model* m = new Model();

  /*** Set the size of the plane based on spec ***/
  float X = 8.192f;
  float Y = 0.5f;
  float sizeZ = 5.12f;

  /*** Top ***/
  addVertex(m, -X / 2, 0, sizeZ / 2);
  addVertex(m, X / 2, 0, sizeZ / 2);
  addVertex(m, X / 2, 0, -sizeZ / 2);
  addVertex(m, -X / 2, 0, -sizeZ / 2);

  /*** Buttom ***/
  addVertex(m, -X / 2, -Y / 2, -sizeZ / 2);
  addVertex(m, X / 2, -Y / 2, -sizeZ / 2);
  addVertex(m, X / 2, -Y / 2, sizeZ / 2);
  addVertex(m, -X / 2, -Y / 2, sizeZ / 2);

  /*** Front ***/
  addVertex(m, -X / 2, -Y / 2, sizeZ / 2);
  addVertex(m, X / 2, -Y / 2, sizeZ / 2);
  addVertex(m, X / 2, 0, sizeZ / 2);
  addVertex(m, -X / 2, 0, sizeZ / 2);
  
  /*** Back ***/
  addVertex(m, -X / 2, -Y / 2, -sizeZ / 2);
  addVertex(m, -X / 2, 0, -sizeZ / 2);
  addVertex(m, X / 2, 0, -sizeZ / 2);
  addVertex(m, X / 2, -Y / 2, -sizeZ / 2);

  /*** Right ***/
  addVertex(m, X / 2, -Y / 2, sizeZ / 2);
  addVertex(m, X / 2, -Y / 2, -sizeZ / 2);
  addVertex(m, X / 2, 0, -sizeZ / 2);
  addVertex(m, X / 2, 0, sizeZ / 2);

  /*** Left ***/
  addVertex(m, -X / 2, -Y / 2, -sizeZ / 2);
  addVertex(m, -X / 2, -Y / 2, sizeZ / 2);
  addVertex(m, -X / 2, 0, sizeZ / 2);
  addVertex(m, -X / 2, 0, -sizeZ / 2);

  m->normals = {
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
  };

  float textureX = 4.096f;
  float textureY = 2.56f;

  /*** Top ***/
  addTexCoords(m, 0.0f, 0.0f, X / textureX, Y / textureY);

  /*** Buttom ***/
  addTexCoords(m, 0.0f, 0.0f, X / textureX, Y / textureY);

  /*** Front ***/
  addTexCoords(m, 0.0f, 0.0f, X / textureX, Y / textureY);

  /*** Back ***/
  addTexCoords(m, 0.0f, 0.0f, X / textureX, Y / textureY);

  /*** Right ***/
  addTexCoords(m, 0.0f, 0.0f, sizeZ / textureX, Y / textureY);

  /*** Left ***/
  addTexCoords(m, 0.0f, 0.0f, sizeZ / textureX, Y / textureY);

  m->textures.push_back(createTexture("../assets/models/Wood_maps/AT_Wood.jpg"));
  m->numVertex = 24;
  m->drawMode = GL_QUADS;

  return m;
}

float bezier(float t, float p0, float p1, float p2, float p3) {
  float u = 1 - t;
  return u * u * u * p0 + 3 * u * u * t * p1 + 3 * u * t * t * p2 + t * t * t * p3;
}

Model* createBezierVaseModel() {
  const int segments = 36;           // Circular segments
  const int height_segments = 2500;  // Height segments
  float height = 3.0f;             // Vase Height

  // Control points for the Bezier curve (you can try adjusting these to shape the vase)
  float p0 = 1;     // Radius at base
  float p1 = 1.5f;  // Control point 1
  float p2 = 1.2f;  // Control point 2
  float p3 = 0.5f;  // Radius at neck

  /* TODO#1-3: Add a vase outer surface model
   *           1. Create a model and manually set vase positions, normals, texcoords
   *           2. Add texure "../assets/models/Vase/Vase.jpg"
   *           3. Set m->numVertex, m->drawMode
   * Note:
   *           You should refer to the cubic bezier curve function bezier().
   */
  Model* vase = new Model();

  /*** Draw the vase surface by using height segments and segments to draw ***/
  for (int i = 0; i < height_segments; i++) {
    /*** The height step ***/
    float t = static_cast<float>(i) / height_segments;
    /*** Radius at this point ***/
    float radius = bezier(t, p0, p1, p2, p3);
    /*** The y position ***/
    float y = height * t;

    for (int j = 0; j <= segments; ++j) {
      /*** Theta at this step ***/
      float theta = 2.0f * M_PI * j / segments;
      /*** The x position ***/
      float x = radius * cos(theta);
      /*** The z position ***/
      float z = radius * sin(theta);

      addVertex(vase, x, y, z);

      glm::vec3 normal = glm::normalize(glm::vec3(x, radius, z));
      addNormal(vase, normal.x, normal.y, normal.z);

      /*** Calculate the UV coordinate ***/
      float u = static_cast<float>(j) / segments;
      float v = static_cast<float>(i) / height_segments;
      addTexCoords(vase, u, v);
    }
  }

  vase->numVertex = height_segments * segments * 3;
  vase->drawMode = GL_TRIANGLE_STRIP; 

  vase->textures.push_back(createTexture("../assets/models/Vase/Vase.jpg"));

  return vase;
}

/*** The logic is same as createBezierVaseModel() ***/
Model* createBezierVaseInnerModel() {
  const int segments = 36;         // Circular segments
  const int height_segments = 2500;  // Height segments
  float height = 3.0f;             // Vase Height

  // Control points for the Bezier curve (adjust these to shape the vase)
  float p0 = 1;     // Radius at base
  float p1 = 1.4f;  // Control point 1
  float p2 = 1.1f;  // Control point 2
  float p3 = 0.45f;  // Radius at neck

  /* TODO#1-4: Add a vase inner surface model
   *           1. Create a model and manually set vase positions, normals, texcoords
   *           2. Add texure "../assets/models/Vase/Vase2.jpg"
   *           3. Set m->numVertex, m->drawMode
   * Note:
   *           You should refer to the cubic bezier curve function bezier().
   */
  Model* vase = new Model();

  for (int i = 0; i < height_segments; i++) {
    float t = static_cast<float>(i) / height_segments;
    float radius = bezier(t, p0, p1, p2, p3);
    float y = height * t;

    for (int j = 0; j <= segments; j++) {
      float theta = 2.0f * M_PI * j / segments;
      float x = radius * cos(theta);
      float z = radius * sin(theta);

      addVertex(vase, x, y, z);

      glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
      addNormal(vase, normal.x, normal.y, normal.z);

      float u = static_cast<float>(j) / segments;
      float v = static_cast<float>(i) / height_segments;
      addTexCoords(vase, u, v);
    }
  }

  vase->numVertex = height_segments * segments * 3;
  vase->drawMode = GL_TRIANGLE_STRIP; 

  vase->textures.push_back(createTexture("../assets/models/Vase/Vase2.jpg"));
  return vase;
}

/*** The logic is similar to createBezierVaseModel(), the difference is I draw a small cylinder as the bottom of the vase ***/
Model* createBezierVaseBottomModel() {
  /* TODO#1-5: Add a vase bottom surface model
   *           1. Create a model and manually set vase positions, normals, texcoords
   *           2. Add texure "../assets/models/Vase/Vase2.jpg"
   *           3. Set m->numVertex, m->drawMode
   * Note:
   *           You should refer to the cubic bezier curve function bezier().
   */

  int segments = 36;         // Circular segments
  int height_segments = 50;  // Height segments
  float height = 0.1f;             // Vase Height

  /* TODO#1-4: Add a vase inner surface model
   *           1. Create a model and manually set vase positions, normals, texcoords
   *           2. Add texure "../assets/models/Vase/Vase2.jpg"
   *           3. Set m->numVertex, m->drawMode
   * Note:
   *           You should refer to the cubic bezier curve function bezier().
   */
  Model* vase = new Model();

  for(int r = 1000; r >= 0; r--) {
    float radius = r / 1000.;
    float y = 1;

    for (int j = 0; j <= segments; ++j) {
      float theta = 2.0f * M_PI * j / segments;
      float x = radius * cos(theta);
      float z = radius * sin(theta);

      addVertex(vase, x, y, z);

      glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
      addNormal(vase, normal.x, normal.y, normal.z);

      float u = static_cast<float>(j) / segments;
      float v = static_cast<float>(r) / height_segments;
      addTexCoords(vase, u, v);
    }

  }

  vase->numVertex = segments * 1000;
  vase->drawMode = GL_TRIANGLE_STRIP; 

  vase->textures.push_back(createTexture("../assets/models/Vase/Vase2.jpg"));
  return vase;
}

void loadModels() {
  /* TODO#2-2: Push the model to ctx.models
   *        1. Comment out this example models
   *        2. Push your model to ctx.models
   * Note:
   *    You can refer to the context class in context.h and model class in model.h
   * Hint:
        ctx.models.push_back();
   */
  
  // Model* m = new Model();
  // float pos[] = {-1, 0, -1, -1, 0, 1, 1, 0, 1, 1, 0, -1};
  // float nor[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};
  // float tx[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f};
  // for (int i = 0; i < 12; i++) {
  //   m->positions.push_back(pos[i]);
  //   m->normals.push_back(nor[i]);
  //   if (i < 8) m->texcoords.push_back(tx[i]);
  // }
  // m->textures.push_back(createTexture("../assets/models/Vase/Vase2.jpg"));
  // m->numVertex = 4;
  // m->drawMode = GL_QUADS;
  
  // ctx.models.push_back(m);

  /*** Load all the created model to ctx ***/
  Model *dice = createDice();
  Model *bottle = createBottle();
  Model *plane = createPlane();

  Model *inner = createBezierVaseInnerModel();
  Model *bottom = createBezierVaseBottomModel();
  Model *vase = createBezierVaseModel();

  Model *coffee_mug = createMug("C");
  Model *tea_mug = createMug3("T");
  Model *normal = createMug("Normal");
  Model *AO = createMug3("AO");
  Model *rough = createMug("Roughness");

  ctx.models.push_back(dice);
  ctx.models.push_back(bottle);
  ctx.models.push_back(vase);
  ctx.models.push_back(inner);
  ctx.models.push_back(bottom);
  ctx.models.push_back(coffee_mug);
  ctx.models.push_back(tea_mug);
  ctx.models.push_back(normal);
  ctx.models.push_back(AO);
  ctx.models.push_back(rough);
  ctx.models.push_back(plane);
  
}

void setupObjects() {
  /* TODO#2-2: Set up the object by the model vector
   *          1. Comment out this example object
   *          2. push your models in the ctx.models to ctx.objects
   * Note:
   *    You can refer to the context class in context.h and objects structure in model.h
   * Hint:
   *    ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(1.5, 0.4, 3))));
   *    (*ctx.objects.rbegin())->material = mFlatwhite;
   */

  // ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0, 0.0, 0.0))));

  /*** Cube ***/
  ctx.objects.push_back(new Object(0, glm::translate(glm::identity<glm::mat4>(), glm::vec3(1.5, 0.4, 2))));
  (*ctx.objects.rbegin())->material = mFlatwhite;

  /*** Bottle ***/
  ctx.objects.push_back(new Object(1, glm::translate(glm::identity<glm::mat4>(), glm::vec3(5.5, 0, 2))));
  
  /*** Vase ***/
  Object* vaseInner = new Object(3, glm::translate(glm::identity<glm::mat4>(), glm::vec3(3.5, 0, 2)));
  ctx.objects.push_back(vaseInner);
  Object* vase = new Object(2, glm::translate(glm::identity<glm::mat4>(), glm::vec3(3.5, 0, 2)));
  ctx.objects.push_back(vase);
  Object* bottom = new Object(4, glm::translate(glm::identity<glm::mat4>(), glm::vec3(3.5, -1, 2)));
  ctx.objects.push_back(bottom);
  
  /*** Mugs ***/
  ctx.objects.push_back(new Object(5, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4, 0.4, 4))));
  /*** Apple rotation to make the mug on the top of the bottle ***/
  ctx.objects.push_back(new Object(6, glm::rotate(
    glm::translate(glm::identity<glm::mat4>(), glm::vec3(5.6, 1.2, 2)),
    glm::radians(135.0f), 
    glm::vec3(0.0f, 0.0f, 1.0f)
    )
));
  ctx.objects.push_back(new Object(7, glm::translate(glm::identity<glm::mat4>(), glm::vec3(1, 0.4, 4))));
  ctx.objects.push_back(new Object(8, glm::translate(glm::identity<glm::mat4>(), glm::vec3(2, 0.4, 4))));
  ctx.objects.push_back(new Object(9, glm::translate(glm::identity<glm::mat4>(), glm::vec3(3, 0.4, 4))));

  /*** Plane ***/
  Object* planeObject = new Object(10, glm::translate(glm::identity<glm::mat4>(), glm::vec3(4.096, 0, 2.56)));
  ctx.objects.push_back(planeObject);
}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW2 - `your student id`"
   *         Ex. HW2 - 311550000
   */
  glfwSetWindowTitle(window, "HW2 - 110700045");

  // Init Camera helper
  Camera camera(glm::vec3(0, 2, 5));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbacks use
  glfwSetWindowUserPointer(window, &camera);
  ctx.camera = &camera;
  ctx.window = window;

  loadMaterial();
  loadModels();
  loadPrograms();
  setupObjects();

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);

    ctx.spotLightDirection = glm::normalize(glm::vec3(3, 0.3, 3) - ctx.spotLightPosition);
    ctx.pointLightPosition = glm::vec3(6 * glm::cos(glm::radians(ctx._pointLightPosisionDegree)), 3.0f,
                                       6 * glm::sin(glm::radians(ctx._pointLightPosisionDegree)));
    ctx.programs[ctx.currentProgram]->doMainLoop();

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
  if (action == GLFW_REPEAT) {
    switch (key) {
      case GLFW_KEY_K:
        ctx._pointLightPosisionDegree += 1.0f;
        break;
      case GLFW_KEY_L:
        ctx._pointLightPosisionDegree -= 1.0f;
        break;
      case GLFW_KEY_I:
        ctx.spotLightPosition.x = glm::clamp(ctx.spotLightPosition.x - 0.1f, -2.0f, 8.0f);
        break;
      case GLFW_KEY_O:
        ctx.spotLightPosition.x = glm::clamp(ctx.spotLightPosition.x + 0.1f, -2.0f, 8.0f);
        break;

      default:
        break;
    }
    return;
  }
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_F9: {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
          // Show the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
          // Hide the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        break;
      }
      case GLFW_KEY_1:
        ctx.currentProgram = (ctx.currentProgram + 1) % ctx.programs.size();
        break;
      //case GLFW_KEY_2:
      //  ctx.currentProgram = 1;
      //  break;
      //case GLFW_KEY_3:
      //  ctx.currentProgram = 2; 
      case GLFW_KEY_2:
        ctx.directionLightEnable = !ctx.directionLightEnable;
        break;
      case GLFW_KEY_3:
        ctx.pointLightEnable = !ctx.pointLightEnable;
        break;
      case GLFW_KEY_4:
        ctx.spotLightEnable = !ctx.spotLightEnable;
        break;
      case GLFW_KEY_H:
        ctx.pointLightColor = glm::vec3(0.8f, 0.8f, 0.8f);
        break;
      case GLFW_KEY_J:
        ctx.pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
      case GLFW_KEY_Y:
        ctx.spotLightColor = glm::vec3(0.8f, 0.8f, 0.8f);
        break;
      case GLFW_KEY_U:
        ctx.spotLightColor = glm::vec3(0.0f, 0.8f, 0.0f);
        break;
      default:
        break;
    }
  }
}

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
//  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}