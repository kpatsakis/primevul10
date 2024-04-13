Jsi_RC jsi_InitTree(Jsi_Interp *interp, int release)
{
    if (release) return JSI_OK;
    /* TODO: maintain hash table of trees created per interp? */
    return JSI_OK;
}