Jsi_RC Jsi_GetBool(Jsi_Interp* interp, const char *string, bool *n)
{
    int len = Jsi_Strlen(string);
    if (len && (Jsi_Strncasecmp(string, "true", len)==0 && len<=4)) {
        *n = 1;
        return JSI_OK;
    }
    if (len && (Jsi_Strncasecmp(string, "false", len)==0 && len<=5)) {
        *n = 0;
        return JSI_OK;
    }
    return JSI_ERROR;
}