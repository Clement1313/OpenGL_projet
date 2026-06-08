#define _USE_MATH_DeFINES

#include "TorchRenderer.hh"
#include "../include/TorchRenderer.hh"

#include <GL/glew.h>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

string TorchRenderer::loadShader(const string path) const
{
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

unsigned int TorchRenderer::compileShader(unsigned int type,
                                         const char* source) const
{
  const unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  int success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    char log[512] = { 0 };
    glGetShaderInfoLog(shader, 512, nullptr, log);
    std::cerr << "Shader compilation failed: " << log << std::endl;
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}


unsigned int TorchRenderer::createProgram(const char* vertexSource,
                                         const char* fragmentSource) const
{
  const unsigned int vertexShader =
      compileShader(GL_VERTEX_SHADER, vertexSource);
  const unsigned int fragmentShader =
      compileShader(GL_FRAGMENT_SHADER, fragmentSource);

  if (vertexShader == 0 || fragmentShader == 0)
  {
    if (vertexShader != 0)
    {
      glDeleteShader(vertexShader);
    }
    if (fragmentShader != 0)
    {
      glDeleteShader(fragmentShader);
    }
    return 0;
  }

  const unsigned int program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  int success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success)
  {
    char log[512] = { 0 };
    glGetProgramInfoLog(program, 512, nullptr, log);
    std::cerr << "Program linking failed: " << log << std::endl;
    glDeleteProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return 0;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}



TorchRenderer::TorchRenderer(Vector3 position, float hauteur_manche,float rayon_manche,std::vector<Vector3> chemin, size_t numberPrisme): m_position_(position),
m_hauteur_manche_(hauteur_manche), m_rayon_manche_(rayon_manche),chemin_(chemin),m_numberPrisme_(numberPrisme),m_intensite_fire_(1.0f) {
  m_hauteur_bout_ = hauteur_manche * 0.4;
  for (int i = 0; i < 16; ++i)
  {
    m_viewMatrix[i] = 0.0f;
    m_projectionMatrix[i] = 0.0f;
  }

  m_viewMatrix[0] = 1.0f;
  m_viewMatrix[5] = 1.0f;
  m_viewMatrix[10] = 1.0f;
  m_viewMatrix[15] = 1.0f;

  m_projectionMatrix[0] = 1.0f;
  m_projectionMatrix[5] = 1.0f;
  m_projectionMatrix[10] = 1.0f;
  m_projectionMatrix[15] = 1.0f;
}


void genCircle(std::vector<Vector3>& circlePoint,size_t count) {

  float step =  2 * M_PI / count;
  for (size_t i = 0; i < count; i++) {
    float tetha = step * i;
    circlePoint.push_back({cos(tetha), 0.f , sin(tetha)});
  }
}


void TorchRenderer::getVertex(const std::vector<Vector3>& circlePoint,float rayon, Vector3 origin, Vector3 direction, Vector3 normal, std::vector<Vertex>& vertex,float verticalTextCoord) {

  size_t sizeCircle = circlePoint.size();
  Vector3 direction_normal = normalize(direction);
  Vector3 normal_normalized = normalize(normal);

  Vector3 start_up = {0.f,1.f,0.f};
  if (std::abs(dot(direction_normal,start_up)) >= 0.99f) {
    start_up = {1.f,0.f,0.f};
  }
  Vector3 right = normalize(cross(direction_normal,start_up));
  Vector3 up =normalize(cross(right,direction_normal));


  float inclinaison = dot(direction_normal,normal_normalized);
  for (int j = 0; j < sizeCircle; j++) {
    Vertex v;
    Vector3 point = {circlePoint[j].x * rayon, 0.0f,circlePoint[j].z * rayon};
    float t = 0.0f;
    Vector3 point_position = {right.x * point.x + up.x * point.z,right.y * point.x + up.y *point.z,right.z * point.x + up.z * point.z};

    if (std::abs(inclinaison) > 1e-5f) {
      // Vector3 vecteur_origin_point = sub(origin,point);
      t = - dot(normal_normalized,point_position);
      t /= inclinaison;

    }
    Vector3 vecteur_projection = multiply(direction_normal,t);


    /*Vector3 point_projete = add(point,vecteur_projection); // projection du points sur le plan
    Vector3 direction_projete_origin = sub(point_projete,origin);
    direction_projete_origin = normalize(direction_projete_origin);

    direction_projete_origin = multiply(direction_projete_origin,rayon);
*/
    v.position = add(origin,point_position);
    v.position = add(v.position,vecteur_projection);
    Vector3 center = add(origin,vecteur_projection);
    Vector3 normal_du_plan = sub(v.position,center);
    v.normal = normalize(normal_du_plan);
    v.uv.x = ((float)j)/((float)sizeCircle);
    v.uv.y = verticalTextCoord;

    vertex.push_back(v);

  }

}

/*
void TorchRenderer::getVertex(const std::vector<Vector3>& circlePoint,float rayon, Vector3 origin, Vector3 direction, Vector3 normal, std::vector<Vertex>& vertex,float verticalTextCoord) {
  size_t sizeCircle = circlePoint.size();

  Vector3 start_up = {0.f,1.f,0.f};
  Vector3 right = normalize(cross(direction,start_up));
  Vector3 up =normalize(cross(right,direction));

  for (int j = 0; j < sizeCircle; j++) {
    Vertex v;
    Vector3 point  = {circlePoint[j].x  * rayon, 0.0f,circlePoint[j].z * rayon};

    v.position = {
    origin.x + right.x * point.x * rayon + up.x * point.z * rayon,
      origin.y + right.y * point.x * rayon + up.y * point.z * rayon,
      origin.z + right.z * point.x * rayon + up.z * point.z * rayon
    };
    v.normal = {right.x * point.x + up.x * point.z,right.y * point.x + up.y *point.z,right.z * point.x + up.z * point.z};
    v.normal = normalize(v.normal);
    /*
    float intersection = dot(normal, direction);
    float t = 0.0f;
    if (std::abs(intersection) > 1e-5f) {
      Vector3 vecteur_origin_point = sub(origin,point);
      t = dot(normal,vecteur_origin_point);
      t /= intersection;
    }
    Vector3 vecteur_projection = multiply(direction,t);

    Vector3 point_projete = add(point,vecteur_projection); // projection du points sur le plan

    v.position = point_projete;
    Vector3 normal_du_plan = sub(point_projete,origin);
    v.normal = normalize(normal_du_plan);
    v.uv.x = ((float)j)/((float)sizeCircle);
    v.uv.y = verticalTextCoord;
    vertex.push_back(v);
  }
}

*/

/*
void genVertex(std::vector<Vector3>& circlePoint,float rayon, float hauteur, Vector3& origin,std::vector<Vertex>& vertex) {
  getVertex(circlePoint, rayon, hauteur , origin, vertex,1);
  getVertex(circlePoint, rayon, 0, origin, vertex,0);
}
*/

void getVertexBout(std::vector<Vector3>& circlePoint,float rayon, float hauteur, Vector3& origin,std::vector<Vertex>& vertex,float verticalTextCoord,float pente) {
  Vertex v;
  size_t sizeCircle = circlePoint.size();
  for (int j = 0; j < sizeCircle; j++) {
    v.position.x = (circlePoint[j].x * rayon)  +  origin.x;
    v.position.y = hauteur + origin.y;
    v.position.z = (circlePoint[j].z * rayon)  +  origin.z;

    v.normal.x =circlePoint[j].x;
    v.normal.y = pente;
    v.normal.z =circlePoint[j].z;
    float longueur = std::sqrt(v.normal.x*v.normal.x + v.normal.y * v.normal.y + v.normal.z * v.normal.z);
    v.normal.x /= longueur;
    v.normal.y /= longueur;
    v.normal.z /= longueur;
    v.uv.x = ((float)j)/((float)sizeCircle);
    v.uv.y = verticalTextCoord;
    vertex.push_back(v);
  }
}

void genVertexBout(std::vector<Vector3>& circlePoint, Vector3& origin,float rayon_bas,float rayon_haut, float hauteur, std::vector<Vertex>& vertex) {
  float pente = (rayon_bas - rayon_haut) / hauteur;
  getVertexBout(circlePoint, rayon_bas, 0, origin , vertex,0,pente);
  //Vector3 otherpoint = {origin.x , origin.y + hauteur , origin.z};
  getVertexBout(circlePoint,rayon_haut,hauteur ,origin,vertex,1,pente);
}

/*
void genVertices(std::vector<Vector3>& circlePoint, float rayon, float hauteur,std::vector<Vector3>& cheminManche,std::vector<Vertex>& result) {
  for (auto & point : cheminManche) {
    genVertex(circlePoint, rayon, hauteur, point,result);
  }
}
*/


void genIndices(size_t nombre_element, size_t prismeNumber,std::vector<TriangleIndices>& indices) {

  for (unsigned int i = 0; i < nombre_element -1; i++) {
    unsigned int indiceFirstVertexChemin = i * prismeNumber;
    unsigned int indiceFirstVertexNextChemin = (i + 1) * prismeNumber;
    for (unsigned int j = 0; j < prismeNumber; j++) {
        unsigned int j_next = (j + 1) % prismeNumber;
        indices.push_back({indiceFirstVertexChemin + j, indiceFirstVertexNextChemin + j,indiceFirstVertexNextChemin + j_next});
        indices.push_back({indiceFirstVertexChemin + j, indiceFirstVertexNextChemin + j_next, indiceFirstVertexChemin+ j_next});
    }
  }
}

void genVertexDisque(std::vector<Vertex>& vertex,std::vector<Vector3>& circlePoint,float rayon, Vector3& origin,float hauteur, bool high) {
  Vertex center;
  center.position = {origin.x,origin.y + hauteur,origin.z};
  float value  =(high)? 1:-1;
  center.normal = {0.f,value,0.f};
  center.uv = {0.5f,0.5f};

  vertex.push_back(center);

  for (int j = 0; j < circlePoint.size(); j++) {
    Vertex newVertex;
    newVertex.position = {origin.x + circlePoint[j].x * rayon,
      origin.y + hauteur,origin.z + circlePoint[j].z * rayon
    };
    newVertex.normal = {0.0f,value,0.0f};
    newVertex.uv = {circlePoint.at(j).x * 0.5f +0.5f,circlePoint.at(j).z * 0.5f + 0.5f};
    vertex.push_back(newVertex);
  }

}



void genIndiceDisque(unsigned int  index,size_t numberPrisme,bool high,std::vector<TriangleIndices>& indices) {
  for (int i = 0; i < numberPrisme; i++) {
    int i_next = (i + 1) % numberPrisme;
    if (high)
    {
      indices.push_back({index, index + 1  + i_next, index + 1 +  i});
    }
    else {
     indices.push_back({index, index + 1  + i, index + 1 +  i_next});
    }
  }
}


void TorchRenderer::genManche(std::vector<Vertex>& vertex, std::vector<TriangleIndices>& indices) {
  if (chemin_.size() < 2) {
    return;
  }

  std::vector<Vector3> circlePoint;
  genCircle(circlePoint,m_numberPrisme_);

  Vector3 direction = normalize(sub(chemin_[1], chemin_[0]));
  getVertex(circlePoint,m_rayon_manche_,chemin_[0],direction,direction,vertex,0.0f);

  size_t len = chemin_.size();
  for (size_t i = 1; i < len; i++) {
    Vector3 distance1 = sub(chemin_[i], chemin_[i -1]);
    Vector3 distance2;
    if (i == len - 1) {
      distance2 = sub(chemin_[i], chemin_[i -  1]);
    }
    else {
      distance2 = sub(chemin_[i + 1], chemin_[i]);
    }
    Vector3 normal= add(distance1,distance2);
    normal = normalize(normal);
    float UV_coord = (float) i / (float) (len - 1);
    getVertex(circlePoint,m_rayon_manche_,chemin_[i],distance1,normal,vertex,UV_coord);
  }
   genIndices(chemin_.size(),m_numberPrisme_,indices);
}

void TorchRenderer::genBout(std::vector<Vertex>& vertex,std::vector<TriangleIndices>& indices) {
  if (chemin_.empty()) {
    return;
  }

  std::vector<Vector3> circlePoint;
  genCircle(circlePoint,m_numberPrisme_);

  Vector3 boutStart = chemin_.back();
  float rayon_haut = m_rayon_manche_ * 1.5f;
  genVertexBout(circlePoint,boutStart,m_rayon_manche_,rayon_haut,m_hauteur_bout_,vertex);
  Vector3 element = {boutStart.x,boutStart.y + m_hauteur_bout_,boutStart.z};
  std::vector<Vector3> chemin_bout = {boutStart, element};
  genIndices(chemin_bout.size(),m_numberPrisme_,indices);
  unsigned int vertexElement  = vertex.size();
  genVertexDisque(vertex,circlePoint,rayon_haut,boutStart,m_hauteur_bout_,true);
  genIndiceDisque(vertexElement,m_numberPrisme_,true,indices);
}

bool TorchRenderer::initialize() {

  const string vertexCode = loadShader("src/vertex_torch.glsl");
  static const char* vertexShaderSource = vertexCode.c_str();

  const string fragmentCode = loadShader("src/fragment_torch.glsl");
  static const char* fragmentShaderSource = fragmentCode.c_str();

  m_program = createProgram(vertexShaderSource, fragmentShaderSource);
  if (m_program == 0)
  {
    return false;
  }

  // Partie Génération du tube
  std::vector<Vertex> vertex;
  std::vector<TriangleIndices> indices;
  genManche(vertex,indices);

  // Partie Génération du bout du tube
  std::vector<Vertex> vertex_bout;
  std::vector<TriangleIndices> indices_bout;
  genBout(vertex_bout,indices_bout);

  // Partie VA0, VB0 et EB0
  glGenVertexArrays(1, &m_vao_manche_);
  glGenBuffers(1, &m_vbo_manche_);
  glGenBuffers(1, &m_ebo_manche_);
  glBindVertexArray(m_vao_manche_);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_manche_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex.size(), vertex.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_manche_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(TriangleIndices), indices.data(), GL_STATIC_DRAW);

  const GLsizei stride = static_cast<GLsizei>(sizeof(Vertex));

  // Position
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, stride,
      reinterpret_cast<void*>(offsetof(Vertex, position)));
  glEnableVertexAttribArray(0);

  // normale
  glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, stride,
      reinterpret_cast<void*>(offsetof(Vertex, normal)));
  glEnableVertexAttribArray(1);

  // uv
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(Vertex, uv)));
  glEnableVertexAttribArray(2);


  glGenVertexArrays(1, &m_vao_bout_);
  glGenBuffers(1, &m_vbo_bout_);
  glGenBuffers(1, &m_ebo_bout_);
  glBindVertexArray(m_vao_bout_);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bout_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex_bout.size(), vertex_bout.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_bout_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_bout.size() * sizeof(TriangleIndices), indices_bout.data(), GL_STATIC_DRAW);

  // Position
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, stride,
      reinterpret_cast<void*>(offsetof(Vertex, position)));
  glEnableVertexAttribArray(0);

  // normale
  glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, stride,
      reinterpret_cast<void*>(offsetof(Vertex, normal)));
  glEnableVertexAttribArray(1);

  // uv
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(Vertex, uv)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

  vec3 originFire = {chemin_.back().x, chemin_.back().y + m_hauteur_bout_, chemin_.back().z};
  fire_renderer_.setOrigin(originFire);

  m_indice_count = indices.size() * 3;
  m_indice_count_bout_ = indices_bout.size() * 3;
  fire_renderer_.setRayonBase(m_rayon_manche_ * 1.5f );
  return  fire_renderer_.initialize();
}

void TorchRenderer::render(float timeSeconds) const {
  glUseProgram(m_program);
  int timeLoc = glGetUniformLocation(m_program, "uTime");
  glUniform1f(timeLoc, timeSeconds);

  float positionLigth[3] = {
    chemin_.back().x,
    chemin_.back().y + m_hauteur_bout_,
    chemin_.back().z};

  const int viewLoc = glGetUniformLocation(m_program, "uView");
  const int projectionLoc = glGetUniformLocation(m_program, "uProjection");
  const int modelLoc = glGetUniformLocation(m_program, "uModel");
  const int lightPositionLoc = glGetUniformLocation(m_program, "uLightPosition");
  const int materialColorLoc = glGetUniformLocation(m_program, "uMaterialColor");
  const int cameraPositionLoc = glGetUniformLocation(m_program, "uCameraPosition");
  const int lightIntensityLoc = glGetUniformLocation(m_program, "uLightIntensity");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, m_viewMatrix);
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, m_projectionMatrix);
  glUniformMatrix4fv(modelLoc,1,GL_FALSE, m_modelMatrix);
  glUniform3fv(lightPositionLoc,1,positionLigth);
  glUniform3fv(materialColorLoc,1,m_materialColor);
  glUniform3fv(cameraPositionLoc,1,m_cameraPosition_);
  glUniform1f(lightIntensityLoc,m_intensite_fire_);

  glBindVertexArray(m_vao_manche_);
  glDrawElements(GL_TRIANGLES, m_indice_count, GL_UNSIGNED_INT, 0);

  glBindVertexArray(m_vao_bout_);
  glDrawElements(GL_TRIANGLES, m_indice_count_bout_, GL_UNSIGNED_INT, 0);

  fire_renderer_.render(timeSeconds);
  glBindVertexArray(0);
  glUseProgram(0);

}

void TorchRenderer::cleanup() {

  // manche
  if (m_vbo_manche_ != 0)
  {
    glDeleteBuffers(1, &m_vbo_manche_);
    m_vbo_manche_ = 0;
  }
  if (m_ebo_manche_ != 0) {
    glDeleteBuffers(1, &m_ebo_manche_);
    m_ebo_manche_ = 0;
  }
  if (m_vao_manche_ != 0)
  {
    glDeleteVertexArrays(1, &m_vao_manche_);
    m_vao_manche_ = 0;
  }

  // bout
  if (m_vbo_bout_ != 0)
  {
    glDeleteBuffers(1, &m_vbo_bout_);
    m_vbo_bout_ = 0;
  }
  if (m_ebo_bout_ != 0) {
    glDeleteBuffers(1, &m_ebo_bout_);
    m_ebo_bout_ = 0;
  }
  if (m_vao_bout_ != 0)
  {
    glDeleteVertexArrays(1, &m_vao_bout_);
    m_vao_bout_ = 0;
  }

  if (m_program != 0) {
    glDeleteProgram(m_program);
    m_program = 0;
  }
  fire_renderer_.cleanup();
}

void TorchRenderer::setCameraMatrices(const float *viewMatrix,
                                      const float *projectionMatrix,
                                      const float*cameraPosition) {
  for (int i = 0; i < 16; ++i)
  {
    m_viewMatrix[i] = viewMatrix[i];
    m_projectionMatrix[i] = projectionMatrix[i];
  }
  m_cameraPosition_[0] = cameraPosition[0];
  m_cameraPosition_[1] = cameraPosition[1];
  m_cameraPosition_[2] = cameraPosition[2];
  fire_renderer_.setCameraMatrices(viewMatrix,projectionMatrix);
}
