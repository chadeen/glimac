#pragma once

#include <glm/glm.hpp>

class TrackballCamera {
public:
	TrackballCamera(float p_fDistance, float p_fAngleX, float p_fAngleY):
		m_fDistance(p_fDistance), m_fAngleX(p_fAngleX), m_fAngleY(p_fAngleY) {}
	TrackballCamera(): m_fDistance(-5.), m_fAngleX(0.), m_fAngleY(0.) {}
	~TrackballCamera() {}

	void moveFront(float delta) {
		m_fDistance += delta;
	}

	void rotateLeft(float degrees) {
		m_fAngleY += degrees;
	}

	void rotateUp(float degrees) {
		m_fAngleX += degrees;
	}

	void reset() {
		m_fDistance = -5.;
		m_fAngleX = 0.;
		m_fAngleY = 0.;
	}

	glm::mat4 getViewMatrix() const {
		return glm::rotate(
			glm::rotate(
				glm::translate(glm::mat4(1), glm::vec3(0, 0, m_fDistance)),
				glm::radians(m_fAngleX), glm::vec3(1, 0, 0)
			),
			glm::radians(m_fAngleY), glm::vec3(0, 1, 0)
		);
	}
private:
	float m_fDistance;
	float m_fAngleX;
	float m_fAngleY;
};