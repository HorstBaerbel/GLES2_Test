#include "Base.h"


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

template <typename TYPE>
Parameter<TYPE>::Parameter(const TYPE & newValue)
	: IChangeableObject()
{
}

template <typename TYPE>
Parameter<TYPE>::Parameter(const Parameter<TYPE> & newValue)
	: IChangeableObject()
{
	*this = newValue;
}

template <typename TYPE>
const Parameter<TYPE> & Parameter<TYPE>::operator=(const Parameter<TYPE> & newValue)
{
	value = newValue.value;
	return this;
}

template <typename TYPE>
const Parameter<TYPE> & Parameter<TYPE>::operator=(const TYPE & newValue)
{
	value = newValue;
	return this;
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