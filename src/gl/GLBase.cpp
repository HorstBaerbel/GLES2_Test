#include "GLBase.h"


IGLObject::IGLObject(std::shared_ptr<ContextBase> context)
	: glContext(context)
{
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