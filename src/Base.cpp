#include "Base.h"


INamedObject::INamedObject(const std::string & newName)
	: name(newName)
{
}

const std::string & INamedObject::getName() const
{
	return name;
}

void INamedObject::setName(const std::string & newName)
{
	name = newName;
}

//------------------------------------------------------------------------------------------------------

IChangeableObject::IChangeableObject()
	: changed(true)
{
}

bool IChangeableObject::hasChanged() const
{
	return changed;
}

void IChangeableObject::setChanged(bool changedValue)
{
	changed = changedValue;
}

IChangeableObject::~IChangeableObject()
{
	changed = false;
}

//------------------------------------------------------------------------------------------------------

InternalException::InternalException(const char * errorString) throw()
	: exception(), error(errorString)
{
}

InternalException::InternalException(const std::string & errorString) throw()
	: exception(), error(errorString)
{
}

const char * InternalException::what() const throw()
{
	return error.c_str();
}

const std::string & InternalException::whatString() const throw()
{
	return error;
}
