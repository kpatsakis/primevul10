static Jsi_RC _object_get_callback(Jsi_Tree *tree, Jsi_TreeEntry *hPtr, void *data)
{
    Jsi_Value *v;
    objwalker *ow = (objwalker *)data;
    Jsi_DString *dStr = ow->dStr;
    int len;
    char *str;
    if ((hPtr->f.bits.dontenum))
        return JSI_OK;
    v =(Jsi_Value*) Jsi_TreeValueGet(hPtr);
    if ((ow->quote&JSI_OUTPUT_JSON) && v && v->vt == JSI_VT_UNDEF)
        return JSI_OK;
    str = (char*)Jsi_TreeKeyGet(hPtr);
    char *cp = Jsi_DSValue(dStr);
    len = Jsi_DSLength(dStr);
    if (len>=2 && (cp[len-2] != '{' || cp[len-1] == '}'))
        Jsi_DSAppend(dStr, ", ", NULL);
    if (((ow->quote&JSI_OUTPUT_JSON) == 0 || (ow->quote&JSI_JSON_STRICT) == 0) && Jsi_StrIsAlnum(str)
        && !Jsi_HashEntryFind(tree->opts.interp->lexkeyTbl, str))
        Jsi_DSAppend(dStr, str, NULL);
    else
        /* JSON/spaces, etc requires quoting the name. */
        Jsi_DSAppend(dStr, "\"", str, "\"", NULL);
    Jsi_DSAppend(dStr, ":", NULL);
    ow->depth++;
    Jsi_RC rc = jsiValueGetString(tree->opts.interp, v, dStr, ow);
    ow->depth--;
    return rc;
}