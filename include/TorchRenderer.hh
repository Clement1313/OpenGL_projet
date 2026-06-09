#ifndef TORCHRENDERER_HH
#define TORCHRENDERER_HH
#include <memory>
#include <vector>
#include "FireRenderer.hh"
#include "Vec.hh"

#include <cmath>
using namespace std;

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };
struct Vertex {
  Vector3 position;
  Vector3 normal;
  Vector2 uv;
};
struct Plan {
  Vector3 normal;
  float d;
};
struct Droite {
  Vector3 v;
  Vector3 point;
};





inline Vector3 sub(const Vector3& v1,const  Vector3& v2) {
  return {v1.x - v2.x,v1.y - v2.y,v1.z - v2.z};
}

inline Vector3 cross(const Vector3& v1, const Vector3& v2) {
  return {v1.y * v2.z - v1.z * v2.y,v1.z * v2.x - v1.x * v2.z,v1.x * v2.y - v1.y * v2.x};
}


inline Vector3 multiply(const Vector3& v1, float alpha) {
  return {v1.x * alpha,v1.y * alpha,v1.z * alpha};
}

inline Vector3 normalize(const Vector3 v) {
  float longeur = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (longeur == 0.0f) {
    return {v.x,v.y,v.z};
  }
  return  {v.x / longeur, v.y / longeur, v.z / longeur};
}

inline Vector3 add(const Vector3& v1,const  Vector3& v2) {
  return {v1.x + v2.x,v1.y + v2.y,v1.z + v2.z};
}

inline float dot(const Vector3& v1, const Vector3& v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vector3 intersectionPlanDroite(const Plan& plan,const Droite& droite) {
  float alpha =0.f;

  float produit_scalaireNormalDirection = dot(plan.normal,droite.v);
  float produit_scalaireNormalPoint = dot(plan.normal,droite.point);
  if (std::abs(produit_scalaireNormalDirection) < 0.0001f) {
    return {0.0f,0.0f,0.0f}; // problème car la normale du plan et le vecteur directeur sont orthogonal
  }
  alpha = ((produit_scalaireNormalPoint + plan.d)/ produit_scalaireNormalDirection) * -1.0f;

  Vector3 translation = multiply(droite.v,alpha);
  return add(droite.point,translation);

}


struct TriangleIndices {unsigned int x, y, z; };




class TorchRenderer {
public:
  TorchRenderer(Vector3 position, float hauteur_manche,float rayon_manche,std::vector<Vector3> chemin,size_t numberPrisme = 30);
  bool initialize();
  void setCameraMatrices(const float* viewMatrix, const float* projectionMatrix, const float * cameraPosition);
  void render(float timeSeconds) const;
  void updateChemin(std::vector<Vector3>& newChemin,float rayon_manche,float rayon_bout, float hauteur_bout);
  void cleanup();
private:
  void genBout();
  string loadShader(const string path) const;
  unsigned int compileShader(unsigned int type, const char* source) const;
  unsigned int createProgram(const char* vertexSource,
                                 const char* fragmentSource) const;
  FireRenderer fire_renderer_;

  unsigned int m_program;
//  void getVertex(const std::vector<Vector3>& circlePoint,float rayon, Vector3 origin, Vector3 distance, Vector3 normal_plan, std::vector<Vertex>& vertex,float verticalTextCoord);
//  void getVertex(const std::vector<Vector3>& circlePoint,float rayon,float hauteur, Vector3 origin, std::vector<Vertex>& vertex,float verticalTextCoord);
  void getVertex(std::vector<Vector3>& BeforeCirclePoint, Plan& actualPlan, Droite& actualDroite, std::vector<Vertex>& vertex,float verticalTextCoord);

  void genManche(std::vector<Vertex>& vertex,
  std::vector<TriangleIndices>& indices);

  void genBout(std::vector<Vertex>& vertex,
std::vector<TriangleIndices>& indices);

  // information du manche
  std::vector<Vector3> chemin_;
  unsigned int m_indice_count;
  unsigned int m_vao_manche_;
  unsigned int m_vbo_manche_;
  unsigned int m_ebo_manche_;
  float m_hauteur_manche_;
  float m_rayon_manche_;
  size_t m_numberPrisme_;

  // information sur le haut de la torche
  unsigned int m_vao_bout_;
  unsigned int m_vbo_bout_;
  unsigned int m_ebo_bout_;
  float m_hauteur_bout_;
  float m_rayon_bout_;
  unsigned int m_indice_count_bout_;


  float m_cameraPosition_[3] = {0.0f,0.0f,0.0f};
  Vector3 m_position_;

  // Phong
  unsigned int phong_torche;
  float m_intensite_fire_;
  float m_viewMatrix[16];
  float m_projectionMatrix[16];
  float m_modelMatrix[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
   0.0f, 1.0f, 0.0f, 0.0f,
   0.0f, 0.0f, 1.0f, 0.0f,
   0.0f, 0.0f, 0.0f, 1.0f
  };
  float m_materialColor[3] = {0.3f,0.2f,0.5f};
};
#endif //TORCHRENDERER_HH
