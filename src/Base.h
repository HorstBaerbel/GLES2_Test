#pragma once

#include <memory>
#include <string>
#include <exception>


class IChangeableObject
{
protected:
	bool changed;

	IChangeableObject();

public:
	virtual bool hasChanged() const;
	void setChanged(bool changedValue);

	virtual ~IChangeableObject();
};

//------------------------------------------------------------------------------------------------------

class ParameterBase : public IChangeableObject
{
};

template <typename TYPE>
class Parameter : public ParameterBase
{
	TYPE value;

public:
	Parameter();
	Parameter(const TYPE & newValue);
	Parameter(const Parameter<TYPE> & newValue);

	const Parameter<TYPE> & operator=(const Parameter<TYPE> & newValue);
	const Parameter<TYPE> & operator=(const TYPE & newValue);
	operator TYPE() const { return value; }

	bool operator==(const Parameter<TYPE> & b);
	bool operator!=(const Parameter<TYPE> & b);
};

template <typename TYPE>
Parameter<TYPE>::Parameter()
{
	changed = false;
}

template <typename TYPE>
Parameter<TYPE>::Parameter(const TYPE & newValue)
	: value(newValue)
{
}

template <typename TYPE>
Parameter<TYPE>::Parameter(const Parameter<TYPE> & newValue)
{
	*this = newValue;
}

template <typename TYPE>
const Parameter<TYPE> & Parameter<TYPE>::operator=(const Parameter<TYPE> & newValue)
{
	value = newValue.value;
	changed = true;
	return *this;
}

template <typename TYPE>
const Parameter<TYPE> & Parameter<TYPE>::operator=(const TYPE & newValue)
{
	value = newValue;
	changed = true;
	return *this;
}

template <typename TYPE>
bool Parameter<TYPE>::operator==(const Parameter<TYPE> & b)
{
	return value == b.value;
}

template <typename TYPE>
bool Parameter<TYPE>::operator!=(const Parameter<TYPE> & b)
{
	return value != b.value;
}

//------------------------------------------------------------------------------------------------------

class IRenderableObject
{
public:
	virtual bool prepareRender(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
	virtual bool render(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
	virtual bool finishRender(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
};

//------------------------------------------------------------------------------------------------------

class IBindableObject
{
public:
	virtual bool bind(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
	virtual bool unbind(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
};

//------------------------------------------------------------------------------------------------------

class InternalException : public std::exception
{
	std::string error;

	InternalException();

protected:
	InternalException(const char * errorString) throw();
	InternalException(const std::string & errorString) throw();

public:
	virtual const char* what() const throw();
	virtual const std::string & whatString() const throw();
};
