#include "Mesh.h"


Mesh::Mesh(std::shared_ptr<ContextBase> context, const std::string & name)
	: IGLObject(context, name)
{
}

void Mesh::addVertexBuffer(std::shared_ptr<GLVertexBuffer> & buffer)
{
	vertexBuffers.push_back(buffer);
}

bool Mesh::prepareRender(std::shared_ptr<ParameterBase> parameter) override
{
	bool result = true;
	std::vector<std::shared_ptr<GLVertexBuffer>> vbIt = vertexBuffers.cbegin();
	while (vbIt != vertexBuffers.cend()) {
		if (!vbIt->prepareRender(parameter)) {
			result = false;
		}
	}
	return result;
}

bool Mesh::render(std::shared_ptr<ParameterBase> parameter) override
{
	bool result = true;
	std::vector<std::shared_ptr<GLVertexBuffer>> vbIt = vertexBuffers.cbegin();
	while (vbIt != vertexBuffers.cend()) {
		if (!vbIt->render(parameter)) {
			result = false;
		}
	}
	return result;
}

bool Mesh::finishRender(std::shared_ptr<ParameterBase> parameter) override
{
	bool result = true;
	std::vector<std::shared_ptr<GLVertexBuffer>> vbIt = vertexBuffers.cbegin();
	while (vbIt != vertexBuffers.cend()) {
		if (!vbIt->finishRender(parameter)) {
			result = false;
		}
	}
	return result;
}