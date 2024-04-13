Jsi_RC Jsi_ListConf(Jsi_List *listPtr, Jsi_MapOpts *opts, bool set)
{
    if (set) {
        listPtr->opts = *opts;
    } else {
        *opts = listPtr->opts;
    }
    return JSI_OK;
}