void  Jsi_ValueFromDS(Jsi_Interp *interp, Jsi_DString *dsPtr, Jsi_Value **ret)
{
    char *cp = NULL;
    int len = Jsi_DSLength(dsPtr);
    if (len && !(cp=(char*)dsPtr->strA)) 
        cp = Jsi_StrdupLen(dsPtr->Str, len);
    dsPtr->strA = NULL;
    dsPtr->Str[0] = 0;
    dsPtr->len = 0;
    dsPtr->spaceAvl = dsPtr->staticSize;
    if (!cp)
        Jsi_ValueMakeStringDup(interp, ret, "");
    else
        Jsi_ValueMakeBlob(interp, ret, (uchar*)cp, len);
}