int Jsi_OptionsChanged(Jsi_Interp *interp, Jsi_OptionSpec *spec, const char *pattern, ...)
{
    va_list argList;
    Jsi_OptionSpec *specPtr;
    const char *option = pattern;
    int cnt = 0;
    
    va_start(argList, pattern);
    spec = jsi_GetCachedOptionSpecs(interp, spec);
    do  {
        for (specPtr = spec; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END; specPtr++) {
            if ((Jsi_GlobMatch(option, specPtr->name, 0)) &&
                    (specPtr->flags & JSI_OPT_IS_SPECIFIED)) {
                cnt++;
            }
        }
        assert(specPtr->id == JSI_OPTION_END);

    } while ((option = va_arg(argList, char *)) != NULL);
    va_end(argList);
    return cnt;
}