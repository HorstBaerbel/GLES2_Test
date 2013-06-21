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
	bool hasChanged() const;
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
	Parameter(const TYPE & newValue);
	Parameter(const Parameter<TYPE> & newValue);

	const Parameter<TYPE> & operator=(const Parameter<TYPE> & newValue);
	const Parameter<TYPE> & operator=(const TYPE & newValue);
	operator TYPE() { return value; }
};

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
	return this;
}

template <typename TYPE>
const Parameter<TYPE> & Parameter<TYPE>::operator=(const TYPE & newValue)
{
	value = newValue;
	return this;
}

//------------------------------------------------------------------------------------------------------

class IRenderableObject
{
public:
	virtual void prepareRender(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
	virtual void render(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
	virtual void finishRender(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
};

//------------------------------------------------------------------------------------------------------

class IBindableObject
{
public:
	virtual void bind(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
	virtual void unbind(std::shared_ptr<ParameterBase> parameter = nullptr) = 0;
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
