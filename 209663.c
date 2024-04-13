static Jsi_RC JsiCheckConversion(const char *str, const char *endptr)
{
    if (str[0] == '\0' || str == endptr) {
        return JSI_ERROR;
    }

    if (endptr[0] != '\0') {
        while (*endptr) {
            if (!isspace(UCHAR(*endptr))) {
                return JSI_ERROR;
            }
            endptr++;
        }
    }
    return JSI_OK;
}