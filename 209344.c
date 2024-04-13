char *jsi_utf_tocase(const char *cp, int upper, Jsi_DString *dsPtr)
{
    char unibuf[10];
    while (*cp) {
#if JSI__UTF8
        if (*cp&0x80) {
            int32_t c;
            Jsi_UtfToUniChar(cp, &c);
            int n = Jsi_UniCharToUtf(c, unibuf);
            unibuf[n] = 0;
            cp += n;
        } else
#endif
        {
            unibuf[0] = (upper?toupper(*cp):tolower(*cp));
            unibuf[1] = 0;
            cp++;
        }
        if (upper==2) //totile
            upper = 0;
        Jsi_DSAppend(dsPtr, unibuf, NULL);
    }
    return Jsi_DSValue(dsPtr);
}