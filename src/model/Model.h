#pragma once

#include "math.h"
#include "Mesh.h"


class Model : public IGLObject, public IRenderableObject
{
	std::vector<std::shared_ptr<Mesh>> meshes;

public:
	Model(std::shared_ptr<ContextBase> context, const std::string & name = "");

	/*!
	Add a mesh to the model.
	\param[in] mesh The mesh to add to the model.
	*/
	virtual void addMesh(std::shared_ptr<Mesh> & mesh);

	/*!
	Generate normals for a mesh.
	\param[in] perVertex Optional. Pass true to generate per-vertex normals, else per-face normals are generated
	*/
    void generateNormals(bool perVertex = true);

	/*!
	Generate a set of tangents and bi-tangents for the mesh.
    \param[in] perVertex Optional. Pass true to generate per-vertex tangents, else per-face tangents are generated
	*/
	void generateTangents(bool perVertex = true);

	virtual bool prepareRender(std::shared_ptr<ParameterBase> parameter = nullptr) override;
	virtual bool render(std::shared_ptr<ParameterBase> parameter = nullptr) override;
	virtual bool finishRender(std::shared_ptr<ParameterBase> parameter = nullptr) override;

    //Stream stuff
    friend std::ostream & operator<<(std::ostream & os, const Model & model);
    friend std::istream & operator>>(std::istream & is, Model & model);

    virtual bool fromString(const std::string & data);
    virtual const std::string toString();

    virtual ~Model();
};