#pragma once

#include "Math.h"


class Transform2
{
	bool changed;
	vec2 translate;
	float rotate;
	vec2 scale;
	mat3 matrix;

public:
	Transform2(const vec2 & translation = vec2(0.0f, 0.0f), float rotation = 0.0f, const vec2 & scaleFactor = vec2(1.0f, 1.0f));

	const vec2 & getTranslation() const;
	void setTranslation(const vec2 & translation);

	const float getRotation() const;
	void setRotation(float rotation);

	const vec2 & getScaleFactor() const;
	void setScaleFactor(const vec2 & scaleFactor);

	void update();
};
