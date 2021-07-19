#include "common.hpp"


void Transform2d::scale2d(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform2d::rotate2d(float radians)
{
	float c = std::cos(radians);
	float s = std::sin(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform2d::translate2d(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	mat = mat * T;
}

void Transform::scale(vec2 scale)
{
	mat = glm::scale(mat, vec3(scale, 1.0f));
}

void Transform::rotate(float degrees)
{
	mat = glm::rotate(mat, glm::radians(degrees), vec3(0.0f, 0.0f, 1.0f));
}

void Transform::translate(vec2 offset)
{
	mat = glm::translate(mat, glm::vec3(offset, 0.0f));
}