#pragma once

#include "../math/Math.h"
#include "../gl/GLBase.h"


class Material : public INamedObject, public IChangeableObject, public IBindableObject
{
public:
    enum LightModel {FLAT, GOURAUD, PHONG};

private:
    //std::string library; //for ModelObj
    col4 ambient;
    col4 diffuse;
    col4 specular;
    float alpha;
    float shininess;
    LightModel lightModel;

public:
    /*!
    Material constructor.
    \param[in] name Material name.
    \param[in] a Ambient color in RGBA.
    \param[in] d Diffuse color in RGBA.
    \param[in] s Specular color in RGBA.
    \param[in] opacity Alpha value [0.0-1.0].
    \param[in] sh Shininess value of material. Exponent of power function for specular color.
    \param[in] model Light model. Flat shading applies material per face, Gouraud per vertex, Phong per pixel.
    */
    Material(const std::string & name = "", const col4 & a = col4(0.2f, 0.2f, 0.2f, 1.0f), const col4 & d = col4(0.8f, 0.8f, 0.8f, 1.0f), const col4 & s = col4(1.0, 1.0f, 1.0f, 1.0f), float opacity = 1.0f, float sh = 16.0f, LightModel model = GOURAUD);

    virtual bool bind(std::shared_ptr<ParameterBase> parameter = nullptr) override;
	virtual bool unbind(std::shared_ptr<ParameterBase> parameter = nullptr) override;

    virtual bool fromFile(const std::string & fileName);
    virtual bool fromString(const std::string & data);
    //virtual bool fromRaw(const char * data, const size_t size);

    virtual bool toFile(const std::string & fileName);
    virtual const std::string toString();
};