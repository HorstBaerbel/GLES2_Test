#include "Group2D.h"


Group2D::Group2D()
	: Object2D()
{
}

void Group2D::addChild(std::shared_ptr<Object2D> & child)
{
	children.push_back(child);
}

std::shared_ptr<Object2D> & Group2D::getChild(size_t index)
{
	return children[index];
}

void Group2D::update(float timeDelta)
{
	std::vector<std::shared_ptr<Object2D>>::iterator oit = children.begin();
	while (oit != children.cend()) {
		(*oit)->update(timeDelta);
		++oit;
	}
}

void Group2D::render()
{
	std::vector<std::shared_ptr<Object2D>>::iterator oit = children.begin();
	while (oit != children.cend()) {
		(*oit)->render();
		++oit;
	}
}
