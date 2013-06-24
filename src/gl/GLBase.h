#pragma once

#include <string>

#include "../Base.h"
#include "GLIncludes.h"

class ContextBase;

class IGLObject : public INamedObject
{
	IGLObject();

protected:
	bool valid;
	std::shared_ptr<ContextBase> glContext;

	IGLObject(std::shared_ptr<ContextBase> context, const std::string & name = "");

public:
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