#pragma once

#include "../math/Transform2.h"


class Object2D
{
	bool changed; //!< Set if the internal values have changed
	Transform2 transform; //!<The transformation in relation to the parent

protected:
	//This is protected so it can't be constructed directly, only in derived classes
	Object2D();

public:
	const Transform2 & getTransform() const;
	void setTransform(const Transform2 & newTransform);

	//we duplicate these here for convenience
	const vec2 & getTranslation() const;
	void setTranslation(const vec2 & translation);

	const float getRotation() const;
	void setRotation(float rotation);

	const vec2 & getScaleFactor() const;
	void setScaleFactor(const vec2 & scaleFactor);

	/*!
	Update object internals for rendering, e.g. rebuild VBOs, do animation
	\note Please call Object2D::update() when deriving a class to update the transform!
	*/
	virtual void update(float timeDelta);

	/*!
	Render object to current context.
	*/
	virtual void render() = 0;
};
