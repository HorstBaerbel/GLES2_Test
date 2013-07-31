#include "GLVertexBase.h"

//------------------------------------------------------------------------------------------------------

const std::string GLVertexAttributeBase::AttributeName[] = {"vertex0", "vertex1", "texCoord0", "texCoord1", "texCoord2", "texCoord3", "normal0", "normal1", "tangent0", "tangent1", "bitangent0", "bitangent1", "color0", "color1", "data0", "data1", "index"};

GLVertexAttributeBase::GLVertexAttributeBase(std::shared_ptr<ContextBase> & context, AttributeRole role, GLenum usagePattern)
	: IChangeableObject(), IGLObject(context), attributeRole(role), glUsage(usagePattern), elementTypeInfo(getElementTypeInfo())
{
}

GLVertexAttributeBase::AttributeRole GLVertexAttributeBase::getAttributeRole() const
{
	return attributeRole;
}

const std::string & GLVertexAttributeBase::getAttributeName() const
{
	return AttributeName[attributeRole];
}

const GLVertexAttributeBase::ElementTypeInfo & GLVertexAttributeBase::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {-1, 0, 0}; return info;
}

//------------------------------------------------------------------------------------------------------

GLVertexAttributeMap::AttributeInfo GLVertexAttributeMap::getAttributeInfo(GLVertexAttributeBase::AttributeRole role) const
{
	auto ait = attributeMap.find(role);
	if (ait != attributeMap.cend()) {
		return ait->second;
	}
	else {
		throw GLVertexAttributeException("VertexAttribMap::getAttributeIndex - No attribute with role " + GLVertexAttributeBase::AttributeName[role] + " found!");
	}
	return AttributeInfo();
}

GLVertexAttributeMap::AttributeInfo GLVertexAttributeMap::getAttributeInfo(const std::string & roleName) const
{
	for (auto aIt = attributeMap.cbegin(); aIt != attributeMap.cend(); ++aIt) {
		if (GLVertexAttributeBase::AttributeName[aIt->first] == roleName) {
			return aIt->second;
		}
	}
	//not found. try matching at least the start of the name
	for (auto aIt = attributeMap.cbegin(); aIt != attributeMap.cend(); ++aIt) {
		if (GLVertexAttributeBase::AttributeName[aIt->first].find(roleName) == 0) {
			return aIt->second;
		}
	}
	throw GLVertexAttributeException("VertexAttribMap::getAttributeIndex - No attribute with name " + roleName + " found!");
	return AttributeInfo();
}

void GLVertexAttributeMap::setAttributeIndex(GLVertexAttributeBase::AttributeRole role, const Parameter<GLuint> & index, bool enable)
{
	//check if attribute with same index, but different role already exists
	bool roleExists = false;
	for (auto aIt = attributeMap.cbegin(); aIt != attributeMap.cend(); ++aIt) {
		if (aIt->first != role && *aIt->second.index == index) {
			throw GLVertexAttributeException("VertexAttribMap::setAttributeIndex - Attribute with role " + GLVertexAttributeBase::AttributeName[role] + " is already at index " + std::to_string((long long)index) + "!");
		}
		else if (aIt->first == role) {
			roleExists = true;
		}
	}
	//if the role doesn't exist, create the shared pointer to it
	if (!roleExists) {
		attributeMap[role].index = std::make_shared<Parameter<GLuint>>(index);
	}
	else {
		*attributeMap[role].index = index;
	}
	attributeMap[role].enabled = enable;
	changed = true;
}

void GLVertexAttributeMap::setAttributeIndex(const std::string & roleName, const Parameter<GLuint> & index, bool enable)
{
	//try to find role with specified name
	for (auto aIt = attributeMap.begin(); aIt != attributeMap.end(); ++aIt) {
		if (GLVertexAttributeBase::AttributeName[aIt->first] == roleName) {
			*aIt->second.index = index;
			changed = true;
			return;
		}
	}
	//not found. try matching at least the start of the name
	for (auto aIt = attributeMap.begin(); aIt != attributeMap.end(); ++aIt) {
		if (GLVertexAttributeBase::AttributeName[aIt->first].find(roleName) == 0) {
			*aIt->second.index = index;
			aIt->second.enabled = enable;
			changed = true;
			return;
		}
	}
	throw GLVertexAttributeException("VertexAttribMap::setAttributeIndex - No attribute with name " + roleName + " found!");
}

bool GLVertexAttributeMap::hasChanged() const
{
	if (!changed) {
		for (auto aIt = attributeMap.cbegin(); aIt != attributeMap.cend(); ++aIt) {
			if (aIt->second.index->hasChanged()) {
				return true;
			}
		}
	}
	return changed;
}

//------------------------------------------------------------------------------------------------------

GLVertexAttributeException::GLVertexAttributeException(const char * errorString) throw()
	: GLException(errorString)
{
}

GLVertexAttributeException::GLVertexAttributeException(const std::string & errorString) throw()
	: GLException(errorString)
{
}
