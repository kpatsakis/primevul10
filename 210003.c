Jsi_JSONQuote(Jsi_Interp *interp, const char *str, int len, Jsi_DString *dsPtr)
{
    const char *cp = str;
    int i = 0;
    if (len<0)
        len = Jsi_Strlen(str);
    char cbuf[10];
    Jsi_DSAppend(dsPtr,"\"",NULL);
    while (*cp && i++<len) {
        if (*cp == '\\' /* || *cp == '/' */ || *cp == '\"') {
            cbuf[0] = '\\';
            cbuf[1] = *cp;
            cbuf[2] = 0;
            Jsi_DSAppend(dsPtr,cbuf,NULL);
        } else if (!isprint(*cp)) {
            int ilen;
            switch (*cp) {
                case '\b': Jsi_DSAppend(dsPtr,"\\b",NULL); break;
                case '\n': Jsi_DSAppend(dsPtr,"\\n",NULL); break;
                case '\r': Jsi_DSAppend(dsPtr,"\\r",NULL); break;
                case '\f': Jsi_DSAppend(dsPtr,"\\f",NULL); break;
                case '\t': Jsi_DSAppend(dsPtr,"\\t",NULL); break;
                default:
                    if ((ilen = Jsi_UtfEncode(cp, cbuf))) {
                        Jsi_DSAppend(dsPtr,cbuf,NULL);
                        cp += (ilen-1);
                    }
            }
        } else {
            cbuf[0] = *cp;
            cbuf[1] = 0;
            Jsi_DSAppend(dsPtr,cbuf,NULL);
        }
        cp++;
    }
    
    Jsi_DSAppend(dsPtr,"\"", NULL);
    return Jsi_DSValue(dsPtr);;
}