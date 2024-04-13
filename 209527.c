void Jsi_TreeValueSet(Jsi_TreeEntry *hPtr, void *value)
{
    Jsi_Value *v = (Jsi_Value*)value;
#if JSI__MEMDEBUG
    SIGASSERTV(hPtr, TREEENTRY);
    if (hPtr->treePtr->flags.valuesonly)
        SIGASSERTV(v,VALUE);
#endif
    hPtr->value = v;
}