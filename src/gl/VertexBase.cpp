#include "VertexBase.h"

//------------------------------------------------------------------------------------------------------

const std::string VertexAttributeBase::AttributeName[] = {"vertex0", "vertex1", "texCoord0", "texCoord1", "texCoord2", "texCoord3", "normal0", "normal1", "color0", "color1", "data0", "data1", "index"};

VertexAttributeBase::VertexAttributeBase(std::shared_ptr<ContextBase> & context, AttributeRole role, GLenum usagePattern)
	: IChangeableObject(), IGLObject(context), attributeRole(role), glUsage(usagePattern), elementTypeInfo(getElementTypeInfo())
{
}

VertexAttributeBase::AttributeRole VertexAttributeBase::getAttributeRole() const
{
	return attributeRole;
}

const std::string & VertexAttributeBase::getAttributeName() const
{
	return AttributeName[attributeRole];
}

const VertexAttributeBase::ElementTypeInfo & VertexAttributeBase::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {-1, 0, 0}; return info;
}

//------------------------------------------------------------------------------------------------------

VertexAttributeMap::AttributeInfo VertexAttributeMap::getAttributeInfo(VertexAttributeBase::AttributeRole role) const
{
	std::map<VertexAttributeBase::AttributeRole, AttributeInfo>::const_iterator ait = attributeMap.find(role);
	if (ait != attributeMap.cend()) {
		return ait->second;
	}
	else {
		throw VertexAttributeException("VertexAttribMap::getAttributeIndex - No attribute with role " + VertexAttributeBase::AttributeName[role] + " found!");
	}
	return AttributeInfo();
}

VertexAttributeMap::AttributeInfo VertexAttributeMap::getAttributeInfo(const std::string & roleName) const
{
	std::map<VertexAttributeBase::AttributeRole, AttributeInfo>::const_iterator ait = attributeMap.cbegin();
	while (ait != attributeMap.cend()) {
		if (VertexAttributeBase::AttributeName[ait->first] == roleName) {
			return ait->second;
		}
		++ait;
	}
	//not found. try matching at least the start of the name
	ait = attributeMap.cbegin();
	while (ait != attributeMap.cend()) {
		if (VertexAttributeBase::AttributeName[ait->first].find(roleName) == 0) {
			return ait->second;
		}
		++ait;
	}
	throw VertexAttributeException("VertexAttribMap::getAttributeIndex - No attribute with name " + roleName + " found!");
	return AttributeInfo();
}

void VertexAttributeMap::setAttributeIndex(VertexAttributeBase::AttributeRole role, std::shared_ptr<Parameter<GLuint>> & index, bool enable)
{
	//check if attribute with same role already exists
	std::map<VertexAttributeBase::AttributeRole, AttributeInfo>::const_iterator ait = attributeMap.cbegin();
	while (ait != attributeMap.cend()) {
		if (ait->first != role && ait->second.index == index) {
			throw VertexAttributeException("VertexAttribMap::setAttributeIndex - Attribute with role " + VertexAttributeBase::AttributeName[role] + " is already at index " + std::to_string((_ULonglong)*index) + "!");
		}
		++ait;
	}
	attributeMap[role].index = index;
	attributeMap[role].enabled = enable;
	changed = true;
}

void VertexAttributeMap::setAttributeIndex(const std::string & roleName, std::shared_ptr<Parameter<GLuint>> & index, bool enable)
{
	//try to find role with specified name
	std::map<VertexAttributeBase::AttributeRole, AttributeInfo>::iterator ait = attributeMap.begin();
	while (ait != attributeMap.end()) {
		if (VertexAttributeBase::AttributeName[ait->first] == roleName) {
			ait->second.index = index;
			changed = true;
			return;
		}
		++ait;
	}
	//not found. try matching at least the start of the name
	ait = attributeMap.begin();
	while (ait != attributeMap.end()) {
		if (VertexAttributeBase::AttributeName[ait->first].find(roleName) == 0) {
			ait->second.index = index;
			ait->second.enabled = enable;
			changed = true;
			return;
		}
		++ait;
	}
	throw VertexAttributeException("VertexAttribMap::setAttributeIndex - No attribute with name " + roleName + " found!");
}

//------------------------------------------------------------------------------------------------------

VertexAttributeException::VertexAttributeException(const char * errorString) throw()
	: GLException(errorString)
{
}

VertexAttributeException::VertexAttributeException(const std::string & errorString) throw()
	: GLException(errorString)
{
}
