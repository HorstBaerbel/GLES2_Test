#include "Object2D.h"


Object2D::Object2D()
	: changed(true)
{
}

const Transform2 & Object2D::getTransform() const
{
	return transform;
}

void Object2D::setTransform(const Transform2 & newTransform)
{
	transform = newTransform;
}

const vec2 & Object2D::getTranslation() const
{
	return transform.getTranslation();
}

void Object2D::setTranslation(const vec2 & translation)
{
	transform.setTranslation(translation);
}

const float Object2D::getRotation() const
{
	return transform.getRotation();
}

void Object2D::setRotation(float rotation)
{
	transform.setRotation(rotation);
}

const vec2 & Object2D::getScaleFactor() const
{
	return transform.getScaleFactor();
}

void Object2D::setScaleFactor(const vec2 & scaleFactor)
{
	transform.setScaleFactor(scaleFactor);
}

void Object2D::update(float timeDelta)
{
	transform.update();
}
