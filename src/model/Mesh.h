#pragma once

#include <stdint.h>
#include <vector>

#include "../gl/GLVertexBuffer.h"


class Mesh : public IGLObject, public IRenderableObject
{
	std::vector<std::shared_ptr<GLVertexBuffer>> vertexBuffers;

public:
    enum PrimitiveType {POINTS = 0, LINES = 1, LINE_STRIP = 3, TRIANGLES = 4, QUADS = 7, POLYGON = 9};

    Mesh(std::shared_ptr<ContextBase> context, const std::string & name = "");

	/*!
	Add a vertex buffers to the mesh.
	\param[in] buffer Vertex buffer to be used when rendering the mesh.
	*/
	void addVertexBuffer(std::shared_ptr<GLVertexBuffer> & buffer);

	/*!
	Generate normals for a mesh
	\param[in] perVertex Optional. Pass true to generate per-vertex normals, else per-face normals are generated.
	*/
    //virtual void generateNormals(bool perVertex = true);

	/*!
	Generate a set of tangents and bi-tangents for the mesh.
	*/
	//virtual void generateTangents();

	virtual bool prepareRender(std::shared_ptr<ParameterBase> parameter = nullptr) override;
	virtual bool render(std::shared_ptr<ParameterBase> parameter = nullptr) override;
	virtual bool finishRender(std::shared_ptr<ParameterBase> parameter = nullptr) override;

    //Stream stuff
    friend std::ostream & operator<<(std::ostream & os, const Mesh & mesh);
    friend std::istream & operator>>(std::istream & is, Mesh & mesh);

    virtual bool fromString(const std::string & data);
    virtual const std::string toString();

    ~Mesh();
};
