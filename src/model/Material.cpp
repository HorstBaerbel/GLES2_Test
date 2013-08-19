#include "Material.h"


Material::Material(const std::string & name, const col4 & a, const col4 & d, const col4 & s, float opacity, float sh, LightModel model)
    : INamedObject(name), ambient(a), diffuse(d), specular(s), alpha(opacity), shininess(sh), lightModel(model)
{
}

bool Material::fromFile(const std::string & fileName)
{
    return false;
}

bool Material::fromString(const std::string & data)
{
    return false;
}

bool Material::toFile(const std::string & fileName)
{
    return false;
}

const std::string Material::toString()
{
    return false;
}