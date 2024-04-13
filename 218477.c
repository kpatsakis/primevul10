void *php_stream_xmlIO_open_wrapper(const char *filename)
{
	TSRMLS_FETCH();
	return php_stream_open_wrapper((char*)filename, "rb", ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);
}