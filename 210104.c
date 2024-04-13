static void jsi_wsDumpQuery(jsi_wsCmdObj *cmdPtr, jsi_wsPss *pss, const char *name, Jsi_Value **ret)
{
    int n = 0;
    Jsi_Interp *interp = cmdPtr->interp;
    Jsi_Obj *nobj;
    Jsi_Value *nv;
    if (!name) {
        nobj = Jsi_ObjNewType(interp, JSI_OT_OBJECT);
        Jsi_ValueMakeObject(interp, ret, nobj);
    }
    for (n = 0; n<pss->paramc; n++) {
        const char *cp = lws_spa_get_string(pss->spa, n);
        if (!cp) cp = "";
        if (name) {
            if (Jsi_Strcmp(name, pss->paramv[n])) { n++; continue; }
            Jsi_ValueMakeStringDup(interp, ret, cp);
            return;
        }
        nv = Jsi_ValueNewStringDup(interp, cp);
        Jsi_ObjInsert(interp, nobj, pss->paramv[n], nv, 0);
    }
}