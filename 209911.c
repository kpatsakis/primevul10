static Jsi_RC inline jsi_ValueAssign(Jsi_Interp *interp, Jsi_Value *dst, Jsi_Value* src, int lop)
{
    Jsi_Value *v;
    if (dst->vt != JSI_VT_VARIABLE) {
        if (interp->strict) 
            return Jsi_LogError("operand not a left value");
    } else {
        v = dst->d.lval;
        SIGASSERT(v, VALUE);
        int strict = interp->strict;
        if (strict && lop == OP_PUSHFUN && interp->curIp[-1].local)
            dst->f.bits.local = 1;
        if (strict && dst->f.bits.local==0) {
            const char *varname = "";
            if (v->f.bits.lookupfailed)
                varname = v->d.lookupFail;
            Jsi_RC rc = Jsi_LogType("function created global: \"%s\"", varname);
            dst->f.bits.local=1;
            if (rc != JSI_OK)
                return rc;
        }
        if (v == src)
            return JSI_OK;
        if (v->f.bits.readonly) {
            if (interp->strict) 
                return Jsi_LogError("assign to readonly variable");
            return JSI_OK;
        }
        if (Jsi_ValueIsFunction(interp, src))
            Jsi_ValueMove(interp,v, src);
        else
            Jsi_ValueCopy(interp,v, src);
        SIGASSERT(v, VALUE);
        v->f.bits.readonly = 0;
#ifdef JSI_MEM_DEBUG
    if (!v->VD.label2)
        v->VD.label2 = "ValueAssign";
#endif
    }
    return JSI_OK;
}