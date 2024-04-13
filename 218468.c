int php_stream_xmlIO_close(void *context)
{
	TSRMLS_FETCH();
	return php_stream_close((php_stream*)context);
}