#pragma once

#include <vector>
#include <memory>

#include "Object2D.h"


class Group2D : public Object2D
{
	std::vector<std::shared_ptr<Object2D>> children;

public:
	Group2D();

	void addChild(std::shared_ptr<Object2D> & child);
	std::shared_ptr<Object2D> & getChild(size_t index);

	virtual void update(float timeDelta);

	virtual void render();
};