Jsi_RC Jsi_HashConf(Jsi_Hash *hashPtr, Jsi_MapOpts *opts, bool set)
{
    if (set)
        hashPtr->opts = *opts;
    else
        *opts = hashPtr->opts;
    return JSI_OK;
}