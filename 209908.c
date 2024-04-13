static void jsi_SuboptFree(Jsi_Interp *interp, Jsi_OptionSpec* spec, void *ptr)
{
    Jsi_OptionSpec *subspec = (Jsi_OptionSpec *)spec->data;
    Jsi_OptionsFree(interp, subspec, ptr, 0);
}