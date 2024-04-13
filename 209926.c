Jsi_RC jsi_InitOptions(Jsi_Interp *interp, int release) {
    if (release) return JSI_OK;
    assert((sizeof(jsi_OptTypeInfo)/sizeof(jsi_OptTypeInfo[0])) == (JSI_OPTION_END+1));
    int i;
    for (i=JSI_OPTION_BOOL; i<JSI_OPTION_END; i++)
        assert(jsi_OptTypeInfo[i].id == (Jsi_OptionId)i);
    return JSI_OK;
}