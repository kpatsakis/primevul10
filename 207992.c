static int ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep, CONTEXT *ct) THROWSPEC
{
	*ct=*ep->ContextRecord;
	return EXCEPTION_EXECUTE_HANDLER;
}