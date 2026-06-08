#include "../include/Controller.hh"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kPi = 3.14159265358979323846f;

void setIdentity(float matrix[16])
{
	for (int i = 0; i < 16; ++i)
	{
		matrix[i] = 0.0f;
	}
	matrix[0] = 1.0f;
	matrix[5] = 1.0f;
	matrix[10] = 1.0f;
	matrix[15] = 1.0f;
}
} // namespace

Controller::Controller()
	: m_position{ 0.0f, 0.0f, 3.0f }
	, m_yaw(-90.0f)
	, m_pitch(0.0f)
	, m_moveSpeed(3.0f)
	, m_width(800)
	, m_height(600)
{
	for (bool& key : m_keys)
	{
		key = false;
	}

	for (bool& key : m_specialKeys)
	{
		key = false;
	}

	setIdentity(m_view);
	setIdentity(m_projection);
}

void Controller::initialize(int width, int height)
{
	m_width = std::max(1, width);
	m_height = std::max(1, height);
	updateProjectionMatrix();
	updateViewMatrix();
}

void Controller::update(float deltaSeconds)
{
	const float clampedDelta = std::max(0.0f, deltaSeconds);
	const float velocity = m_moveSpeed * clampedDelta;

	const float yawRadians = radians(m_yaw);
	const float pitchRadians = radians(m_pitch);

	const vec3 front = normalize({ std::cos(yawRadians) * std::cos(pitchRadians),
								   std::sin(pitchRadians),
								   std::sin(yawRadians) * std::cos(pitchRadians) });
	const vec3 worldUp = { 0.0f, 1.0f, 0.0f };
	const vec3 right = normalize(cross(front, worldUp));

	// Movement with WASD
	if (m_keys['w'])
	{
		m_position = add(m_position, mul(front, velocity));
	}
	if (m_keys['s'])
	{
		m_position = sub(m_position, mul(front, velocity));
	}
	if (m_keys['a'])
	{
		m_position = sub(m_position, mul(right, velocity));
	}
	if (m_keys['d'])
	{
		m_position = add(m_position, mul(right, velocity));
	}

	const float rotationSpeed = 90.0f;
	const float rot = rotationSpeed * clampedDelta;
	if (m_specialKeys[100]) // LEFT
	{
		m_yaw -= rot;
	}
	if (m_specialKeys[102]) // RIGHT
	{
		m_yaw += rot;
	}
	if (m_specialKeys[101]) // UP
	{
		m_pitch += rot;
	}
	if (m_specialKeys[103]) // DOWN
	{
		m_pitch -= rot;
	}

	m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

	updateViewMatrix();
}

void Controller::keyDown(unsigned char key)
{
	m_keys[key] = true;
}

void Controller::keyUp(unsigned char key)
{
	m_keys[key] = false;
}

void Controller::specialDown(int key)
{
	if (key >= 0 && key < 256)
	{
		m_specialKeys[key] = true;
	}
}

void Controller::specialUp(int key)
{
	if (key >= 0 && key < 256)
	{
		m_specialKeys[key] = false;
	}
}


void Controller::reshape(int width, int height)
{
	m_width = std::max(1, width);
	m_height = std::max(1, height);
	updateProjectionMatrix();
}

const float* Controller::viewMatrix() const
{
	return m_view;
}

const float* Controller::projectionMatrix() const
{
	return m_projection;
}

void Controller::updateViewMatrix()
{
	const float yawRadians = radians(m_yaw);
	const float pitchRadians = radians(m_pitch);

	const vec3 front = normalize({ std::cos(yawRadians) * std::cos(pitchRadians),
								   std::sin(pitchRadians),
								   std::sin(yawRadians) * std::cos(pitchRadians) });
	const vec3 worldUp = { 0.0f, 1.0f, 0.0f };
	const vec3 center = add(m_position, front);
	const vec3 forward = normalize(sub(m_position, center));
	const vec3 right = normalize(cross(worldUp, forward));
	const vec3 up = cross(forward, right);

	setIdentity(m_view);
	m_view[0] = right.x;
	m_view[4] = right.y;
	m_view[8] = right.z;
	m_view[1] = up.x;
	m_view[5] = up.y;
	m_view[9] = up.z;
	m_view[2] = forward.x;
	m_view[6] = forward.y;
	m_view[10] = forward.z;
	m_view[12] = -dot(right, m_position);
	m_view[13] = -dot(up, m_position);
	m_view[14] = -dot(forward, m_position);
}

void Controller::updateProjectionMatrix()
{
	const float fovRadians = radians(45.0f);
	const float aspect = static_cast<float>(m_width) / static_cast<float>(m_height);
	const float nearPlane = 0.1f;
	const float farPlane = 100.0f;
	const float focalLength = 1.0f / std::tan(fovRadians / 2.0f);

	for (float& value : m_projection)
	{
		value = 0.0f;
	}

	m_projection[0] = focalLength / aspect;
	m_projection[5] = focalLength;
	m_projection[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
	m_projection[11] = -1.0f;
	m_projection[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
}

vec3 Controller::add(const vec3& left, const vec3& right)
{
	return { left.x + right.x, left.y + right.y, left.z + right.z };
}

vec3 Controller::sub(const vec3& left, const vec3& right)
{
	return { left.x - right.x, left.y - right.y, left.z - right.z };
}

vec3 Controller::mul(const vec3& value, float scalar)
{
	return { value.x * scalar, value.y * scalar, value.z * scalar };
}

float Controller::dot(const vec3& left, const vec3& right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

vec3 Controller::cross(const vec3& left, const vec3& right)
{
	return { left.y * right.z - left.z * right.y,
			 left.z * right.x - left.x * right.z,
			 left.x * right.y - left.y * right.x };
}

vec3 Controller::normalize(const vec3& value)
{
	const float length = std::sqrt(dot(value, value));
	if (length <= 0.00001f)
	{
		return { 0.0f, 0.0f, 0.0f };
	}

	return { value.x / length, value.y / length, value.z / length };
}

float Controller::radians(float degrees)
{
	return degrees * kPi / 180.0f;
}

