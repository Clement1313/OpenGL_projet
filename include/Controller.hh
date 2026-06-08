#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Vec.hh"

class Controller {
public:
	Controller();

	void initialize(int width, int height);
	void update(float deltaSeconds);

	void keyDown(unsigned char key);
	void keyUp(unsigned char key);
	void specialDown(int key);
	void specialUp(int key);
	void reshape(int width, int height);

	const float* viewMatrix() const;
	const float* projectionMatrix() const;
	const float *  cameraPosition() const {
		m_cameraPosition[0] = m_position.x;
		m_cameraPosition[1] = m_position.y;
		m_cameraPosition[2] = m_position.z;
		return m_cameraPosition;

	};
private:
	void updateViewMatrix();
	void updateProjectionMatrix();

	static vec3 add(const vec3& left, const vec3& right);
	static vec3 sub(const vec3& left, const vec3& right);
	static vec3 mul(const vec3& value, float scalar);
	static float dot(const vec3& left, const vec3& right);
	static vec3 cross(const vec3& left, const vec3& right);
	static vec3 normalize(const vec3& value);
	static float radians(float degrees);

	mutable float m_cameraPosition[3];
	vec3 m_position;
	float m_yaw;
	float m_pitch;
	float m_moveSpeed;
	int m_width;
	int m_height;
	bool m_keys[256];
	bool m_specialKeys[256];
	float m_view[16];
	float m_projection[16];
};

#endif
