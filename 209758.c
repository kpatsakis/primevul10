Jsi_OptionSpecsCached(Jsi_Interp *interp, const Jsi_OptionSpec *staticSpecs)
{
#ifdef NO_CACHED_SPECS
    return (Jsi_OptionSpec*)staticSpecs;
#else
    /* If we aren't master interp, need to cache due to init and modified flags if Jsi_OptionsChanged is called. */
    if (interp->mainInterp == NULL) {
        interp->mainInterp = interp;
    }
    if (interp == interp->mainInterp) {
        return staticSpecs;
    }
    return jsi_GetCachedOptionSpecs(interp, staticSpecs);
#endif
}