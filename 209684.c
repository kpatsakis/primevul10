const Jsi_OptionTypedef* Jsi_OptionsStr2Type(const char *str, bool cName) {
    int typ;
    for (typ=JSI_OPTION_BOOL; typ < JSI_OPTION_END; typ++) {
        const char *snam = (cName?jsi_OptTypeInfo[typ].cName:jsi_OptTypeInfo[typ].idName);
        if (snam && snam[0] && !Jsi_Strcmp(str, snam))
            return jsi_OptTypeInfo+typ;
    }
    return NULL;
}