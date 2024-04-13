static void jsi_CArrayFree(Jsi_Interp *interp, Jsi_OptionSpec* spec, void *ptr)
{
    /*Jsi_OptionSpec *subSpec = spec->init.ini.OPT_CARRAY; // TODO: ???
    if (!subSpec) {
        Jsi_Value **v = (Jsi_Value**)ptr;
        if (v)
            Jsi_DecrRefCount(interp, *v);
    }
    int i, isize, size = spec->asize;
    if ((isize=subSpec->size)<=0)
        return;
    isize = isize/size;
    uchar *s = (uchar*)ptr;
    for (i=0; i<size; i++) {
        Jsi_OptionsFree(interp, subSpec, s, 0);
        s += isize;
    }*/
}