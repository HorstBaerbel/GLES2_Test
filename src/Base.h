#pragma once

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

template <typename TYPE>
class Parameter : public IChangeableObject
{
	TYPE value;

public:
	Parameter(const TYPE & newValue);
	Parameter(const Parameter<TYPE> & newValue);

	const Parameter<TYPE> & operator=(const Parameter<TYPE> & newValue);
	const Parameter<TYPE> & operator=(const TYPE & newValue);
};

//------------------------------------------------------------------------------------------------------

class IRenderableObject
{
public:
	virtual void prepareRender() = 0;
	virtual void render() = 0;
	virtual void finishRender() = 0;
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
