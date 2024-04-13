int parse_namespace(const xmlChar *inval, char **value, char **namespace)
{
	char *found = strrchr((char*)inval, ':');

	if (found != NULL && found != (char*)inval) {
		(*namespace) = estrndup((char*)inval, found - (char*)inval);
		(*value) = estrdup(++found);
	} else {
		(*value) = estrdup((char*)inval);
		(*namespace) = NULL;
	}

	return FALSE;
}