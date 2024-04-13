void jsiFlagDebugValues(Jsi_Interp *interp, Jsi_Obj *obj)
{
    Jsi_Value *v;
    int oflags;
    if (obj->ot != JSI_OT_OBJECT && obj->ot != JSI_OT_ARRAY)
        return;
    if (obj->tree) {
        Jsi_TreeEntry *hPtr;
        Jsi_TreeSearch srch;
        for (hPtr=Jsi_TreeSearchFirst(obj->tree, &srch,  JSI_TREE_ORDER_IN, NULL); hPtr;
            hPtr=Jsi_TreeSearchNext(&srch)) {
            v = (Jsi_Value*)Jsi_TreeValueGet(hPtr);
            if (v == NULL || v->sig != JSI_SIG_VALUE) continue;
            oflags = v->VD.flags;
            v->VD.flags |= (MDB_VISITED|MDB_INOBJ);
            if (oflags&MDB_VISITED || v->vt != JSI_VT_OBJECT)
                continue;
            jsiFlagDebugValues(interp, v->d.obj);
        }
    }
    if (obj->arr) {
        uint i;
        for (i=0; i<obj->arrCnt; i++) {
            v = obj->arr[i];
            if (v == NULL || v->sig != JSI_SIG_VALUE) continue;
            oflags = v->VD.flags;
            v->VD.flags |= (MDB_VISITED|MDB_INOBJ);
            if (oflags&MDB_VISITED || v->vt != JSI_VT_OBJECT)
                continue;
            jsiFlagDebugValues(interp, v->d.obj);
        }
    }
}