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
	std::map<GLVertexAttributeBase::AttributeRole, AttributeInfo>::const_iterator ait = attributeMap.find(role);
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
	std::map<GLVertexAttributeBase::AttributeRole, AttributeInfo>::const_iterator ait = attributeMap.cbegin();
	while (ait != attributeMap.cend()) {
		if (GLVertexAttributeBase::AttributeName[ait->first] == roleName) {
			return ait->second;
		}
		++ait;
	}
	//not found. try matching at least the start of the name
	ait = attributeMap.cbegin();
	while (ait != attributeMap.cend()) {
		if (GLVertexAttributeBase::AttributeName[ait->first].find(roleName) == 0) {
			return ait->second;
		}
		++ait;
	}
	throw GLVertexAttributeException("VertexAttribMap::getAttributeIndex - No attribute with name " + roleName + " found!");
	return AttributeInfo();
}

void GLVertexAttributeMap::setAttributeIndex(GLVertexAttributeBase::AttributeRole role, const Parameter<GLuint> & index, bool enable)
{
	//check if attribute with same index, but different role already exists
	bool roleExists = false;
	std::map<GLVertexAttributeBase::AttributeRole, AttributeInfo>::const_iterator ait = attributeMap.cbegin();
	while (ait != attributeMap.cend()) {
		if (ait->first != role && *ait->second.index == index) {
			throw GLVertexAttributeException("VertexAttribMap::setAttributeIndex - Attribute with role " + GLVertexAttributeBase::AttributeName[role] + " is already at index " + std::to_string((_ULonglong)index) + "!");
		}
		else if (ait->first == role) {
			roleExists = true;
		}
		++ait;
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
	std::map<GLVertexAttributeBase::AttributeRole, AttributeInfo>::iterator ait = attributeMap.begin();
	while (ait != attributeMap.end()) {
		if (GLVertexAttributeBase::AttributeName[ait->first] == roleName) {
			*ait->second.index = index;
			changed = true;
			return;
		}
		++ait;
	}
	//not found. try matching at least the start of the name
	ait = attributeMap.begin();
	while (ait != attributeMap.end()) {
		if (GLVertexAttributeBase::AttributeName[ait->first].find(roleName) == 0) {
			*ait->second.index = index;
			ait->second.enabled = enable;
			changed = true;
			return;
		}
		++ait;
	}
	throw GLVertexAttributeException("VertexAttribMap::setAttributeIndex - No attribute with name " + roleName + " found!");
}

bool GLVertexAttributeMap::hasChanged() const
{
	if (!changed) {
		std::map<GLVertexAttributeBase::AttributeRole, AttributeInfo>::const_iterator ait = attributeMap.cbegin();
		while (ait != attributeMap.cend()) {
			if (ait->second.index->hasChanged()) {
				return true;
			}
			++ait;
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
