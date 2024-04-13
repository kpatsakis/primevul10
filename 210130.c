Jsi_RC jsi_FuncArgsToString(Jsi_Interp *interp, Jsi_Func *f, Jsi_DString *dStr, int withTypes)
{
    if (f->type == FC_NORMAL) {
        int i;
        for (i = 0; i < f->argnames->count; ++i) {
            jsi_ArgValue *av = f->argnames->args+i;
            if (i) Jsi_DSAppend(dStr, ", ", NULL);
            Jsi_DSAppend(dStr,  jsi_ScopeStrsGet(f->argnames, i), NULL);
            if (withTypes && av) {
                Jsi_DString tStr = {};
                int atyp = av->type;
                if (av->defValue)
                    atyp &= ~(av->defValue->vt==JSI_VT_NULL?JSI_TT_NULL:(1<<av->defValue->vt));
                if (atyp) {
                    Jsi_DSAppend(dStr, ":", jsi_typeName(interp, atyp, &tStr), NULL);
                }
                Jsi_DSSetLength(&tStr, 0);
                if (av->defValue)
                    Jsi_DSAppend(dStr, "=", Jsi_ValueGetDString(interp, av->defValue, &tStr, 1), NULL);
                Jsi_DSFree(&tStr);
            }
        }
    } else if (f->cmdSpec && f->cmdSpec->argStr)
        Jsi_DSAppend(dStr, f->cmdSpec->argStr, NULL);
    return JSI_OK;
}