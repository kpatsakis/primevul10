PHP_FUNCTION(xsl_xsltprocessor_get_security_prefs)
{
	zval *id;
	xsl_object *intern;

	DOM_GET_THIS(id);
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "") == SUCCESS) {
		intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
		RETURN_LONG(intern->securityPrefs);
	} else {
		WRONG_PARAM_COUNT;
	}
}