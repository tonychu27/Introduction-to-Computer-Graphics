#include <algorithm>
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <cstdlib>
#include <ctime>    

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include "camera.h"
#include "opengl_context.h"
#include "utils.h"

#define ANGLE_TO_RADIAN(x) (float)((x)*M_PI / 180.0f)
#define RADIAN_TO_ANGLE(x) (float)((x)*180.0f / M_PI)

#define CIRCLE_SEGMENT 64
#define SECTOR 36
#define STACK 18

#define ROTATE_SPEED 1.0f
#define MOVING_SPEED ROTATE_SPEED / 20.f
#define BALL_MOVING_SPEED 0.05f
#define SWING_SPEED 2.0f
#define ROLLING_SPEED 5.0f

#define HEIGHT 6
#define CONSTRAIN_ANGLE -90
#define ANGLE_MAX 0

#define MASS 0.45f
#define GRAVITY 9.81f
#define FRICTION 0.3f

#define GOLD 0.83, 0.69, 0.22
#define CYAN 0, 1, 1
#define PURPLE 1, 0, 1
#define RED 1, 0, 0
#define SILVER 0.75, 0.75, 0.75
#define BRONZE 0.55, 0.47, 0.14
#define PINK 1, 0, 1

#define TOLERATE 0.75f
#define DISTANCE_TOLERANCE 3
using namespace std;

enum class angle { NONE = 0, CLOCKWISE = -1, COUNTERCLOCKWISE = 1 };
enum class scalar { NONE = 0, MINUS = -1, PLUS = 1 };

// to check the putter is swing or not
bool isSwing = false;
// to check the ball is hit or not
bool isHit = false;

// the scalar for the putter translation
scalar delta_xzpos = scalar::NONE;
// the scalar for the ball translation
scalar delta_ballpos = scalar::NONE;
// the scalar for the ball rotation
angle delta_ball_rotate = angle::NONE;
// the scalar for the putter swing
angle delta_x_rotate = angle::NONE;
// the scalar for the putter yaw
angle delta_y_rotate = angle::NONE;

// the angle for the ball rotation
float ball_rotate = 0.0f;
// the angle for the putter swing
float x_rotate = 0.0f;
// the angle for the yaw rotation
float y_rotate = 0.0f;

// the position of the putter
glm::vec3 xzpos(0.0f, 0.0f, 0.0f);
// the forward vector for putter
glm::vec3 forward_vector(0.0f, 0.0f, 1.0f);
// the forward vector for the ball translation
glm::vec3 ball_forward(0.0f, 0.0f, 1.0f);
// x,y,z coordinate for ball rotation
glm::vec3 ball_rotate_normal(1, 0, 0);
// the position of the ball
glm::vec3 ballpos(2.0f, 0.25f, 2.0f);
// the position of the ball when it is hit at the start
glm::vec3 startpos(0, 0, 0);

// all the former rotations for the ball
glm::mat4 currentRotation = glm::identity<glm::mat4>();

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
  if (action == GLFW_REPEAT) return;

  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  /* TODO#4-1: Detect key-events, perform all the controls for the golfing
   *       1. Use switch && case to find the key you want.
   *       2. Define and modify some global variable to trigger update in rendering loop
   * Hint:
   *       glfw3's key list (https://www.glfw.org/docs/3.3/group__keys.html)
   *       glfw3's action codes (https://www.glfw.org/docs/3.3/group__input.html#gada11d965c4da13090ad336e030e4d11f)
   * Note:
   *       You should finish rendering your putter and golfball first.
   *       Otherwise you will spend a lot of time debugging this with a black screen.
   */

  switch (key) {
    /*** If the key is up, move the putter foreward ***/
    case GLFW_KEY_UP:
      if (action == GLFW_PRESS) delta_xzpos = scalar::PLUS;
      else if (action == GLFW_RELEASE) delta_xzpos = scalar::NONE; 
      break;
    /*** If the key is down, move the putter backward ***/
    case GLFW_KEY_DOWN:
      if (action == GLFW_PRESS) delta_xzpos = scalar::MINUS;
      else if (action == GLFW_RELEASE) delta_xzpos = scalar::NONE; 
      break;
    /*** If the key is left, rotate the putter clockwise ***/
    case GLFW_KEY_LEFT:
      if (action == GLFW_PRESS) delta_y_rotate = angle::CLOCKWISE;
      else if (action == GLFW_RELEASE) delta_y_rotate = angle::NONE; 
      break;
    /*** If the key is right, rotate the putter counterclockwise ***/
    case GLFW_KEY_RIGHT:
      if (action == GLFW_PRESS) delta_y_rotate = angle::COUNTERCLOCKWISE;
      else if (action == GLFW_RELEASE) delta_y_rotate = angle::NONE;
      break;
    /*** If the key is space, swing the putter ***/
    case GLFW_KEY_SPACE:
      if (action == GLFW_PRESS) isSwing = true; 
      else if (action == GLFW_RELEASE) isSwing = false;
      break;

    default:
      break;
  }
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  // OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW1 - `your student id`"
   *        Ex. HW1 - 311550000
   */
  glfwSetWindowTitle(window, "HW1 - 110700045");
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}

/*** This function is used to draw the golf ball ***/
void drawUnitSphere() {
  /* TODO#2-1: Render a unit sphere
   * Hint:
   *       glBegin/glEnd (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glVertex3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glVertex.xml)
   *       glNormal (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glNormal.xml)
   *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
   * Note:
   *       You can refer to ppt "Draw Sphere" page and `SECTOR` and `STACK`
   *       You can use color `CYAN` and `PURPLE`
   *       You should set normal for lighting
   *       You should use GL_TRIANGLES_STRIP
   */

  /*** First of All, calculate how many steps to do ***/
  float sectorStep = 2 * M_PI / SECTOR;
  float stackStep = M_PI / STACK;
  float radius = 1.0f;

  /*** Secondly, use a nested for loop to draw the vertex and connect them ***/
  /*** Draw the longtitude ***/
  for(int i = 0; i < STACK; i++) {
    /*** Consider two angles together ***/
    float stackAngle1 = M_PI / 2 - i * stackStep;
    float stackAngle2 = M_PI / 2 - (i + 1) * stackStep;

    /*** Use TRIANGLE STRIP Mode to draw the latitude ***/
    glBegin(GL_TRIANGLE_STRIP);
    for(int k = 0; k <= SECTOR; k++) {
      float sectorAngle = k * sectorStep;

      /*** Find the vertex's position ***/
      float x1 = radius * cos(stackAngle1) * cos(sectorAngle);
      float y1 = radius * sin(stackAngle1) + radius;
      float z1 = radius * cos(stackAngle1) * sin(sectorAngle);

      /*** For two colors ***/
      i >= STACK / 2? glColor3f(CYAN): glColor3f(PURPLE);

      glNormal3f(x1, y1, z1);
      glVertex3f(x1, y1, z1);

      float x2 = radius * cos(stackAngle2) * cos(sectorAngle);
      float y2 = radius * sin(stackAngle2) + radius;
      float z2 = radius * cos(stackAngle2) * sin(sectorAngle);

      glNormal3f(x2, y2, z2);
      glVertex3f(x2, y2, z2);
    }
    glEnd();

  }
}

/*** This function is used to draw the rod of the putter ***/
void drawUnitCylinder(float radius = 0.15f, float height = 6.0f) {
  /*** Calculate how many steps to do ***/
  float angleStep = 2 * M_PI / CIRCLE_SEGMENT;

  /*** Render it as gold color ***/
  glColor3f(GOLD); 
  /*** Draw the side face of the cylinder, using GL_TRIANGLES mode ***/
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < CIRCLE_SEGMENT; i++) {
    float angle1 = i * angleStep;
    float angle2 = (i + 1) * angleStep;

    /*** Find the vertex's position ***/
    float x1 = radius * cos(angle1);
    float z1 = radius * sin(angle1);
    float x2 = radius * cos(angle2);
    float z2 = radius * sin(angle2);

    glNormal3f(x1, 0.0f, z1);
    glVertex3f(x1, 0.0f, z1);
    glNormal3f(x2, 0.0f, z2);
    glVertex3f(x2, 0.0f, z2);
    glNormal3f(x1, 0.0f, z1);
    glVertex3f(x1, height, z1);

    glNormal3f(x1, 0.0f, z1);
    glVertex3f(x1, height, z1);
    glNormal3f(x2, 0.0f, z2);
    glVertex3f(x2, 0.0f, z2);
    glNormal3f(x2, 0.0f, z2);
    glVertex3f(x2, height, z2);
  }
  glEnd();

  /*** Draw the button cap of the cylinder, using GL_TRIANGLES mode ***/
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < CIRCLE_SEGMENT; ++i) {
    float angle1 = i * angleStep;
    float angle2 = (i + 1) * angleStep;

    /*** Vertices for bottom cap ***/
    float x1 = radius * cos(angle1);
    float z1 = radius * sin(angle1);
    float x2 = radius * cos(angle2);
    float z2 = radius * sin(angle2);

    /*** Normal facing down for bottom cap ***/
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(x2, 0.0f, z2);
    glVertex3f(x1, 0.0f, z1);
  }
  glEnd();

  /*** Draw the top cap of the cylinder, using GL_TRIANGLES mode ***/
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < CIRCLE_SEGMENT; i++) {
    float angle1 = i * angleStep;
    float angle2 = (i + 1) * angleStep;

    /*** Vertices for top cap ***/
    float x1 = radius * cos(angle1);
    float z1 = radius * sin(angle1);
    float x2 = radius * cos(angle2);
    float z2 = radius * sin(angle2);

    /*** Normal facing up for top cap ***/
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(x1, height, z1);
    glVertex3f(x2, height, z2);
  }
  glEnd();
}

/*** Function to draw a circle using GL_TRIANGLE_STRIP ***/
void drawCircle(float centerX, float centerY, float centerZ, float radius) {
    glColor3f(PINK);

    glBegin(GL_TRIANGLE_STRIP);

    for (int i = 0; i <= CIRCLE_SEGMENT * 2; i++) {
        float angle = 2.0f * M_PI * i / (CIRCLE_SEGMENT * 2); 
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        /*** Add vertex at the edge of the circle (circumference) ***/
        glVertex3f(centerX + x, centerY, centerZ + z);
        glVertex3f(centerX, centerY, centerZ);
    }

    glEnd();
}

void light() {
  GLfloat light_specular[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_diffuse[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_ambient[] = {0.4, 0.4, 0.4, 1};
  GLfloat light_position[] = {50.0, 75.0, 80.0, 1.0};
  // z buffer enable
  glEnable(GL_DEPTH_TEST);
  // enable lighting
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_NORMALIZE);
  // set light property
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

int main() {

  srand(static_cast<unsigned int>(time(0))); 
  float goalx = -5.0f * 7.0 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (10.0f * 7.0)));  
  float goalz = -5.0f * 7.0 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (10.0f * 7.0)));  
  float goaly = 0.0f;

  glm::vec3 goal = glm::vec3(goalx, goaly, goalz);

  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();

  // Init Camera helper
  Camera camera(glm::vec3(0, 5, 10));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbasks use
  glfwSetWindowUserPointer(window, &camera);
  float distance_travelled = DISTANCE_TOLERANCE;
    
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
    // Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(camera.getProjectionMatrix());
    // ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(camera.getViewMatrix());

#ifndef DISABLE_LIGHT
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    light();
#endif

    /* TODO#4-1: Update putter
     *       1. Update xzpos with forward_vector.
     *       2. Update y_rotate.
     *       3. Update x_rotate(Swing). Remember that the constrain of `CONSTRAIN_ANGLE`
     *       4. Calculate forward_vector for next rendering loop.
     *
     * Hint:
     *      glm::normalize (
     * Note:
     *       You can use `ROTATE_SPEED` and `MOVING_SPEED` and `SWING_SPEED`
     * as the speed constant. If the rotate/movement/swing speed is too slow or too fast, please change `ROTATE_SPEED`
     * or `MOVING_SPEED` or `SWING_SPEED` value. You should finish keyCallback first and use variables include
     * "delta_" to update.
     */
    
    /*** Deal with the swinging, and store the heightest height ***/
    float swingHeight = 0;
    if (isSwing) {
      if (x_rotate > CONSTRAIN_ANGLE) x_rotate -= SWING_SPEED;
      else x_rotate = CONSTRAIN_ANGLE;
      swingHeight = max(swingHeight, HEIGHT * (1 - cos(glm::radians(x_rotate))));
    }
    else if (x_rotate < 0.0f) x_rotate += SWING_SPEED;
    
    /*** Deal with the y rotation of the putter ***/
    if (delta_y_rotate == angle::CLOCKWISE) y_rotate += ROTATE_SPEED;
    else if (delta_y_rotate == angle::COUNTERCLOCKWISE) y_rotate -= ROTATE_SPEED;
    
    /*** Apply the initial rotation matrix based on y_rotate ***/
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));
    /*** Forward vector in homogeneous coordinates ***/
    glm::vec4 forwardVector4 = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); 
    /*** Rotate the forward vector with the user's input ***/
    forwardVector4 = rotationMatrix * forwardVector4;

    /*** Convert back to 3D vector and normalize ***/
    forward_vector = glm::normalize(glm::vec3(forwardVector4));
    /*** The rotation vector for swinging ***/
    glm::vec3 swing_vector = forward_vector;

    /*** Apply an additional 90-degree rotation around the Y-axis ***/
    glm::mat4 rotate90Matrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    /*** Rotate the forward vector with the user's input ***/
    forwardVector4 = rotate90Matrix * glm::vec4(forward_vector, 0.0f);

    /*** Convert back to 3D vector and normalize again ***/
    forward_vector = glm::normalize(glm::vec3(forwardVector4));

    /*** Deal with the moving control ***/
    if (delta_xzpos == scalar::PLUS) xzpos += forward_vector * MOVING_SPEED;
    else if (delta_xzpos == scalar::MINUS) xzpos -= forward_vector * MOVING_SPEED;

    /* TODO#4-2: Hit detection and Update golfball
     *       1. Find the position of the hitting part and the ball to calculate distance
     *       2. Determine whether the ball is hit(distance < TOLERANCE && putter is swinging)
     *          
     *       if the ball is hit:
     *       3. Update ballpos with ball_forward.
     *       4. Update ball_rotate.
     *       5. Calculate the new ball_forward with forward_vector.
     *       6. Calculate the new ball_rotate_normal
     *       7. Calculate the new startpos
     * 
     *       Implement ball stop:
     *       8. Determine whether the ball has to stop(distance >= DISTANCE_TOLERANCE)
     *       9. Update currentRotation
     * Hint:
     *      glm::mat4
     *      glm::translate
     *      glm::rotate
     *      glm::scale
     *      glm::length
     *      glm::normalize (
     * Note:
     *       You can use `BALL_MOVING_SPEED` and `ROLLING_SPEED`
     * as the speed constant. If the rotate/movement speed is too slow or too fast, please change `ROTATE_SPEED`
     * or `BALL_MOVING_SPEED` value. You should finish keyCallback first and use variables include
     * "delta_" to update.
     */
    
    /*** Distance between the putter and the ball ***/
    glm::vec3 putterPos = xzpos;
    putterPos.z += .8;
    float distance = glm::length(ballpos - putterPos);

    /*** If the putter is swinging and their distance is smaller than TOLERATE ***/
    if (isSwing && distance < TOLERATE) {
      isHit = true;
      startpos = ballpos;
      ball_forward = forward_vector;
    }

    /*** Calculate the potential energy ***/
    float potential_energy = MASS * GRAVITY * swingHeight;
    /*** Calculate the speed of the putter when hitting the ball ***/
    float velocity = sqrt(2 * potential_energy / MASS);
    /*** Suppose the friction coefficient of the floor is 0.3, and calculate the friction force ***/
    float friction_force = FRICTION * MASS * GRAVITY;
    /*** Calculate the deceleration caused by the friction force ***/
    float friction_acceleration = friction_force / MASS;
    /*** Calculate the traveraling time of the ball ***/
    float time_to_stop = velocity / friction_acceleration;
    /*** Calculate the distance the ball will travel within the time interval ****/
    distance_travelled = velocity * time_to_stop - 0.5f * friction_acceleration * time_to_stop * time_to_stop;

    /*** Deal with the ball movement, and stop the ball when necessary ***/
    if (isHit) {
        ballpos += BALL_MOVING_SPEED * ball_forward;
        ball_rotate += ROLLING_SPEED;                 
        glm::vec3 upward(0, 1, 0);
        /*** Calculate the ball rotation ***/
        ball_rotate_normal = glm::normalize(glm::cross(ball_forward, upward));
        float traveledDistance = glm::length(ballpos - startpos);
        if (traveledDistance > distance_travelled) isHit = false;  
    }

    // Render a white plane
    glPushMatrix();
    glScalef(7, 1, 7);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-5.0f, 0.0f, -5.0f);
    glVertex3f(-5.0f, 0.0f, 5.0f);
    glVertex3f(5.0f, 0.0f, -5.0f);
    glVertex3f(5.0f, 0.0f, 5.0f);
    glEnd();
    glPopMatrix();

    /* TODO#3-1: Render the putter
     *       1. Do rotate and translate for the putter.
     *       2. Render the hitting part.
     *       3. Render the rod part.
     * Hint:
     *       glPushMatrix/glPopMatrix (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glPushMatrix.xml)
     *       glRotatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml)
     *       glTranslatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml)
     *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor.xml)
     *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
     * Note:
     *       The size of every component can refer to `Components size definition` section
     *       You may implement drawUnitCylinder first
     *       You should try and think carefully about changing the order of rotate and translate
     *       You can just use initial value for rotate and translate the whole putter before finishing TODO#3 and TODO#4
     */
    
    glPushMatrix();
    /*** First of all, translate the rotation center of x_rotate ***/
    glTranslatef(xzpos.x, HEIGHT, xzpos.z);  
    /*** Secondly, apply the rotation for the swing ***/
    glRotatef(-x_rotate, swing_vector.x, swing_vector.y, swing_vector.z);
    /*** Thrid, Translate the putter back to its original position ***/
    glTranslatef(0.0f, -HEIGHT, 0.0f);
    /*** Apply the rotation for putter ***/
    glRotatef(y_rotate, 0.0f, 1.0f, 0.0f);
    drawUnitCylinder();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(xzpos.x + 0.2f, HEIGHT + 0.4f, xzpos.z + 0.25f);
    glRotatef(-x_rotate, swing_vector.x, swing_vector.y, swing_vector.z);
    glTranslatef(-0.2f, -HEIGHT, -0.25f);
    /*** Apply the rotation for hitting part ***/
    glRotatef(y_rotate, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.2f, 0.0f, 0.25f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glScalef(1.0f, 1.0f, 0.4f);
    drawUnitCylinder(1.0f, 0.4f);
    glPopMatrix();

    
    /* TODO#3-2: Render the golf ball
     *       1. Do rotate and translate for the ball.
     * Hint:
     *       glPushMatrix/glPopMatrix (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glPushMatrix.xml)
     *       glRotatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml)
     *       glTranslatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml)
     *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor.xml)
     *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
     *       glMultMatrixf(https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glMultMatrix.xml)
     *       glm::value_ptr()
     * Note:
     *       The size of every component can refer to `Components size definition` section
     *       You may implement drawUnitSphere first
     *       You should try and think carefully about changing the order of rotate and translate
     *       You can just use initial value for rotate and translate the whole putter before finishing TODO#3 and TODO#4
     */

    glPushMatrix();
    /*** First of all, translate the ball to its position ***/
    glTranslatef(ballpos.x, ballpos.y, ballpos.z);
    /*** Secondly, apply rotation matrix for the ball movement ***/
    glRotatef(ball_rotate, ball_rotate_normal.x, ball_rotate_normal.y, ball_rotate_normal.z);
    /*** Third, scale the ball to the size we want ***/
    glScalef(0.25f, 0.25f, 0.25f);  
    drawUnitSphere();

    glPopMatrix();

    drawCircle(goalx, goaly, goalz, 0.5f);

    float wx = -2.0f, wy= 8.0f;
    float ix = -0.3f, iy = 8.0f;
    float nx = 1.0f, ny = 8.0f;
    float win = glm::length(goal - ballpos);
    
    if (win < TOLERATE + 0.25) {
      glBegin(GL_LINES);
      glVertex2f(wx, wy);          
      glVertex2f(wx + 0.5f, wy - 1.0f);  
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(wx + 0.5f, wy - 1.0f);
      glVertex2f(wx + 0.7f, wy - 0.3f);  
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(wx + 0.7f, wy - 0.3f);
      glVertex2f(wx + 0.9f, wy - 1.0f);  
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(wx + 0.9f, wy - 1.0f);
      glVertex2f(wx + 1.4f, wy);  
      glEnd();

      glBegin(GL_LINES);
      glVertex2f(ix, iy); 
      glVertex2f(ix + 0.8f, iy);
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(ix + 0.4f, iy);
      glVertex2f(ix + 0.4f, iy - 1.0f);
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(ix, iy - 1.0f); 
      glVertex2f(ix + 0.8f, iy - 1.0f);
      glEnd();

      glBegin(GL_LINES);
      glVertex2f(nx, ny);
      glVertex2f(nx, ny - 1.0f);
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(nx, ny);
      glVertex2f(nx + 0.6f, ny - 1.0f);
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(nx + 0.6f, ny - 1.0f);
      glVertex2f(nx + 0.6f, ny);
      glEnd();
    }

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }

  return 0;
}
