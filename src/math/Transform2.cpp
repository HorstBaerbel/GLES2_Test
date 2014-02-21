#include "Transform2.h"

using namespace Math;


Transform2::Transform2(const vec2 & translation, float rotation, const vec2 & scaleFactor)
	: changed(true), translate(translation), rotate(rotation), scale(scaleFactor)
{
	matrix.setIdentity();
}

const vec2 & Transform2::getTranslation() const
{
	return translate;
}

void Transform2::setTranslation(const vec2 & translation)
{
	translate = translation;
	changed = true;
}

const float Transform2::getRotation() const
{
	return rotate;
}

void Transform2::setRotation(float rotation)
{
	rotate = rotation;
	changed = true;
}

const vec2 & Transform2::getScaleFactor() const
{
	return scale;
}

void Transform2::setScaleFactor(const vec2 & scaleFactor)
{
	scale = scaleFactor;
	changed = true;
}

void Transform2::update()
{
	if (changed) {
		//re-create transformation matrix. usually the order is scale * rotate * translate
		matrix(0) = scale[0] * Math::cos(rotate);
		matrix(1) = scale[1] * -Math::sin(rotate);
		matrix(2) = 0.0f;
		matrix(3) = scale[0] * Math::sin(rotate);
		matrix(4) = scale[1] * Math::cos(rotate);
		matrix(5) = 0.0f;
		matrix(6) = translate[0];
		matrix(7) = translate[1];
		matrix(8) = 1.0f;
		//transform is up to date now
		changed = false;
	}
}
