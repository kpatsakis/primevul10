static Jsi_OptionSpec * jsi_GetCachedOptionSpecs(Jsi_Interp *interp, const Jsi_OptionSpec *staticSpecs)
{
    Jsi_OptionSpec *cachedSpecs;
    Jsi_Hash *specCacheTablePtr;
    Jsi_HashEntry *entryPtr;
    bool isNew;

    specCacheTablePtr = (Jsi_Hash*)Jsi_InterpGetData(interp, "jsi:OptionSpec", NULL);
    if (specCacheTablePtr == NULL) {
        specCacheTablePtr = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, 0);
        Jsi_InterpSetData(interp, "jsi:OptionSpec", specCacheTablePtr, jsi_DeleteSpecCacheTable);
    }
    
    entryPtr = Jsi_HashEntryNew(specCacheTablePtr, (char *) staticSpecs, &isNew);
    if (isNew) {
        unsigned int entrySpace = sizeof(Jsi_OptionSpec);
        const Jsi_OptionSpec *staticSpecPtr;

        for (staticSpecPtr=staticSpecs; staticSpecPtr->id>=JSI_OPTION_BOOL && staticSpecPtr->id!=JSI_OPTION_END; staticSpecPtr++)
            entrySpace += sizeof(Jsi_OptionSpec);
        assert(staticSpecPtr->id==JSI_OPTION_END);

        cachedSpecs = (Jsi_OptionSpec *) Jsi_Malloc(entrySpace);
        memcpy((void *) cachedSpecs, (void *) staticSpecs, entrySpace);
        Jsi_HashValueSet(entryPtr, cachedSpecs);

    } else {
        cachedSpecs = (Jsi_OptionSpec *) Jsi_HashValueGet(entryPtr);
    }

    return cachedSpecs;
}