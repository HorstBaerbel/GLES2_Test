#pragma once

#include <string>

#include "../Base.h"
#include "GLIncludes.h"

class ContextBase;

//TODO: Use namespaces for all GL or ES objects!
class IGLObject : public INamedObject
{
	IGLObject();

protected:
	bool valid; //!<True if this object is in a valid state.
	std::shared_ptr<ContextBase> glContext; //!<The context this object belongs to.

	/*!
	Constructor.
	\param[in] context The OpenGL context this object belongs to.
	\param[in] name Optional. The name of the object.
	*/
	IGLObject(std::shared_ptr<ContextBase> context, const std::string & name = "");

public:
	/*!
	Can be used to test if this object is in a valid state.
	\return Returns true if the object is in a valid state.
	*/
	virtual bool isValid() const;

	virtual ~IGLObject();
};

//------------------------------------------------------------------------------------------------------

class GLException : public InternalException
{
public:
	GLException(const char * errorString) throw();
	GLException(const std::string & errorString) throw();
};
