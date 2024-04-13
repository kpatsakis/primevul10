void jsi_InitLocalVar(Jsi_Interp *interp, Jsi_Value *arguments, Jsi_Func *who)
{
    SIGASSERTV(who, FUNC);
    if (who->localnames) {
        int i;
        for (i = 0; i < who->localnames->count; ++i) {
            const char *argkey = jsi_ScopeStrsGet(who->localnames, i);
            if (argkey) {
                DECL_VALINIT(key);// = VALINIT;
                Jsi_Value *v __attribute__((unused));
                Jsi_Value *kPtr = &key; // Note: a string key so no reset needed.
                Jsi_ValueMakeStringKey(interp, &kPtr, argkey);
                v = jsi_ValueObjKeyAssign(interp, arguments, kPtr, NULL, JSI_OM_DONTENUM);
                jsi_ValueDebugLabel(v, "locals", who->name);
            }
        }
    }
}