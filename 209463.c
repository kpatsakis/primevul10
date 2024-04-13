int jsi_InitTree(Jsi_Interp *interp, int release)
{
    if (release) return JSI_OK;
    TreeTest(interp);
    return JSI_OK;
}