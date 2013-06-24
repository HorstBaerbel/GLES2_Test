#include "GLBase.h"
#include "ContextBase.h"


IGLObject::IGLObject(std::shared_ptr<ContextBase> context, const std::string & name)
	: INamedObject(name), glContext(context), valid(false)
{
}

bool IGLObject::isValid() const
{
	return valid;
}

IGLObject::~IGLObject()
{
	valid = false;
}

//------------------------------------------------------------------------------------------------------

GLException::GLException(const char * errorString) throw()
	: InternalException(errorString)
{
}

GLException::GLException(const std::string & errorString) throw()
	: InternalException(errorString)
{
}