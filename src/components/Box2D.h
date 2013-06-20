#pragma once

#include "Object2D.h"
#include "VBO2D.h"


class Box2D : public Object2D
{
	VBO2D<VertexData2D_VTC> vbo;
};
