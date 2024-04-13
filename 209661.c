static Jsi_StructSpec* dbLookupSpecFromName(Jsi_StructSpec *specs, const char *name) {
    Jsi_StructSpec *specPtr = NULL;
    for (specPtr = specs; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END; specPtr++) {
        if  (specPtr->flags&JSI_OPT_DB_IGNORE)
            continue;
        const char *cname = specPtr->name;
        if (cname[0] == name[0] && !Jsi_Strncasecmp(cname, name, -1))
            return specPtr;
    }
    return NULL;
}