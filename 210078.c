static void jsiObjGetNames(Jsi_Interp *interp, Jsi_Obj *obj, Jsi_DString* dStr, int flags) {
    Jsi_TreeEntry *hPtr;
    Jsi_TreeSearch srch;
    Jsi_Value *v;
    int m = 0;
    Jsi_DSInit(dStr);
    if (obj->isarrlist)
        obj = interp->Array_prototype->d.obj;
    else if (!obj->tree->numEntries && obj->__proto__)
        obj = obj->__proto__->d.obj;
    for (hPtr=Jsi_TreeSearchFirst(obj->tree, &srch,  JSI_TREE_ORDER_IN, NULL); hPtr; hPtr=Jsi_TreeSearchNext(&srch)) {
        v = (Jsi_Value*)Jsi_TreeValueGet(hPtr);
        if (!v) continue;
        if ((flags&JSI_NAME_FUNCTIONS) && !Jsi_ValueIsFunction(interp,v)) {
            continue;
        }
        if ((flags&JSI_NAME_DATA) && Jsi_ValueIsFunction(interp,v)) {
            continue;
        }

        Jsi_DSAppend(dStr, (m++?" ":""), Jsi_TreeKeyGet(hPtr), NULL);
    }
    Jsi_TreeSearchDone(&srch);
}