Jsi_OptionsDup(Jsi_Interp *interp, const Jsi_OptionSpec *staticSpecs)
{
    unsigned int entrySpace = sizeof(Jsi_OptionSpec);
    const Jsi_OptionSpec *staticSpecPtr;
    Jsi_OptionSpec *newSpecs;

    for (staticSpecPtr=staticSpecs; staticSpecPtr->id>=JSI_OPTION_BOOL && staticSpecPtr->id<JSI_OPTION_END && staticSpecPtr->name;
            staticSpecPtr++) {
        entrySpace += sizeof(Jsi_OptionSpec);
    }

    newSpecs = (Jsi_OptionSpec *) Jsi_Malloc(entrySpace);
    memcpy((void *) newSpecs, (void *) staticSpecs, entrySpace);
    return newSpecs;
}