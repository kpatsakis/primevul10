Jsi_OptionsFind(Jsi_Interp *interp, Jsi_OptionSpec *specs, const char *name, Jsi_Wide flags)
{
    Jsi_OptionSpec *specPtr;
    char c;      /* First character of current argument. */
    Jsi_OptionSpec *matchPtr; /* Matching spec, or NULL. */
    size_t length;
    const char *matStr = NULL;
    
    if (name == NULL) {
        Jsi_LogError("Null name for option");
        return NULL;
    }
    c = name[0];
    length = Jsi_Strlen(name);
    matchPtr = NULL;
    for (specPtr = specs; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END && specPtr->name; specPtr++) {

        if ((specPtr->name[0] != c)
                || (Jsi_Strncmp(specPtr->name, name, length) != 0)) {
            continue;
        }
        if (specPtr->name[length] == 0) {
            return specPtr;   /* Stop on a perfect match. */
        }
        if (matchPtr != NULL) {
            Jsi_LogError("ambiguous option \"%s\" matches both \"%s\" and \"%s\"", name, matStr, specPtr->name);
            return (Jsi_OptionSpec *) NULL;
        }
        matchPtr = specPtr;
        matStr = specPtr->name;
    }

    if (matchPtr == NULL) {
        Jsi_DString dStr = {};
        Jsi_DSAppend(&dStr, "unknown option \"", name, "\" not one of: ", NULL);
    
        for (specPtr = specs; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END && specPtr->name; specPtr++) {
            if (specPtr->name == NULL) {
                break;
            }
            if (name[0] != '?' || _JSICASTINT(specPtr->id) < 0 || specPtr->id >= JSI_OPTION_END) {
                Jsi_DSAppend(&dStr, specPtr->name, " ", NULL);
            } else {

                Jsi_DSAppend(&dStr, "?", specPtr->name, " <", jsi_OptTypeInfo[specPtr->id].idName, ">? ", NULL);
            }
        }
        assert(specPtr->id == JSI_OPTION_END);
        Jsi_LogError("%s", Jsi_DSValue(&dStr));
        Jsi_DSFree(&dStr);
        return (Jsi_OptionSpec *) NULL;
    }
    return matchPtr;
}