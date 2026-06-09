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



TorchRenderer::TorchRenderer(Vector3 position, float hauteur_manche,float rayon_manche,float rayon_bout_,float hauteur_bout_,std::vector<Vector3> chemin,size_t numberPrisme): m_position_(position),
m_hauteur_manche_(hauteur_manche), m_rayon_manche_(rayon_manche),chemin_(chemin),m_numberPrisme_(numberPrisme),m_intensite_fire_(1.0f) {
  m_hauteur_bout_ = hauteur_manche;
  m_rayon_bout_ = rayon_bout_;
  m_hauteur_bout_ = hauteur_bout_;
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


void TorchRenderer::getVertex(std::vector<Vector3>& BeforeCirclePoint, Plan& actualPlan, Droite& actualDroite, std::vector<Vertex>& vertex,float verticalTextCoord) {
  size_t sizeCircle = BeforeCirclePoint.size();
  for (unsigned int j = 0; j < sizeCircle; j++) {
    Vertex v;
    actualDroite.point = BeforeCirclePoint[j];
    v.position = intersectionPlanDroite(actualPlan,actualDroite);
    BeforeCirclePoint[j] = v.position;
    v.normal = sub(v.normal,actualDroite.point);
    v.normal = normalize(v.normal);  // à revoir pour correction
    v.uv.x = ((float)j)/((float)sizeCircle);
    v.uv.y = verticalTextCoord;
    vertex.push_back(v);

  }

}

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
    v.normal = normalize(v.normal);
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

void generateFirstCircle(std::vector<Vector3>&  circlePoint,float rayon,Vector3& origin) {
  for (unsigned int i = 0; i < circlePoint.size(); i++) {
    circlePoint[i].x = circlePoint[i].x * rayon  + origin.x;
    circlePoint[i].y = origin.y;
    circlePoint[i].z = circlePoint[i].z * rayon  + origin.z;
  }
}
void TorchRenderer::genManche(std::vector<Vertex>& vertex, std::vector<TriangleIndices>& indices) {
  if (chemin_.size() < 2) {
    return;
  }
  // cas pour le premier point (chemin_[0])
  std::vector<Vector3> circlePoint;
  genCircle(circlePoint,m_numberPrisme_);



  // cas pour les autres points
  size_t len = chemin_.size();
  for (size_t i = 0; i < len; i++) {
    Vector3 before;
    Vector3 after;
    Vector3 actual = chemin_[i];
    if (!i) {
      before = {chemin_[0].x,chemin_[0].y - 1.f,chemin_[0].z};
      after = chemin_[1];
      generateFirstCircle(circlePoint,m_rayon_manche_,before);
    }
    else if (i == len - 1) {
      before = chemin_[i - 1];
      after = chemin_[i - 1];
    }
    else {
      before = chemin_[i - 1];
      after = chemin_[i + 1];
    }
    Vector3 v1 = sub(actual,before);
    Vector3 v2 = sub(after,actual);
    if (i ==len - 1) {
      v2 = v1;
    }
    v1 = normalize(v1);
    v2 = normalize(v2);

    Vector3 n = add(v1,v2);
    n  = normalize(n);
    Plan pointCheminPlan{n,-dot(n,actual)};
    Droite droite {v1,{0,0,0}};
    float UV_coord = (float) i / (float) (len - 1);
    getVertex(circlePoint, pointCheminPlan,droite,vertex,UV_coord);
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
  float rayon_haut = m_rayon_bout_;
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
  fire_renderer_.setRayonBase(m_rayon_bout_ );
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

void TorchRenderer::updateChemin(std::vector<Vector3>& newChemin,float rayon_manche,float rayon_bout, float hauteur_bout,float * color) {
  chemin_ = newChemin;
  m_materialColor[0] = color[0];
  m_materialColor[1] = color[1];
  m_materialColor[2] = color[2];
  Vector3 new_Vector3 = {chemin_.back().x, chemin_.back().y + 0.2f, chemin_.back().z};
  chemin_.push_back(new_Vector3);
  m_rayon_manche_ = rayon_manche;
  m_rayon_bout_ = rayon_bout;
  m_hauteur_bout_ = hauteur_bout;
  std::vector<Vertex> vertex;
  std::vector<TriangleIndices> indices;
  genManche(vertex,indices);
  m_indice_count = indices.size() * 3;

  std::vector<Vertex> vertex_bout;
  std::vector<TriangleIndices> indices_bout;
  genBout(vertex_bout,indices_bout);
  m_indice_count_bout_ = indices_bout.size() * 3;

  const int materialColorLoc = glGetUniformLocation(m_program, "uMaterialColor");
  glUniform3fv(materialColorLoc,1,m_materialColor);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_manche_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex.size(), vertex.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_manche_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(TriangleIndices), indices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bout_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex_bout.size(), vertex_bout.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_bout_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_bout.size() * sizeof(TriangleIndices), indices_bout.data(), GL_DYNAMIC_DRAW);

  vec3 originFire = {newChemin.back().x, newChemin.back().y + m_hauteur_bout_ + 0.2f, newChemin.back().z};
  fire_renderer_.setOrigin(originFire);
  fire_renderer_.setRayonBase(m_rayon_bout_);
  fire_renderer_.update();
  glBindVertexArray(0);
}