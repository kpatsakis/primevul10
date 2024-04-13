AP_DECLARE(void) ap_get_server_revision(ap_version_t *version)
{
    version->major = AP_SERVER_MAJORVERSION_NUMBER;
    version->minor = AP_SERVER_MINORVERSION_NUMBER;
    version->patch = AP_SERVER_PATCHLEVEL_NUMBER;
    version->add_string = AP_SERVER_ADD_STRING;
}