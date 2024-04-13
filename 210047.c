static Jsi_RC jsiEvalOp(Jsi_Interp* interp, jsi_Pstate *ps, char *program,
                       jsi_ScopeChain *scope, Jsi_Value *currentScope, Jsi_Value *_this, Jsi_Value **ret)
{
    Jsi_RC r = JSI_OK;
    jsi_Pstate *newps = jsiNewParser(interp, program, NULL, 1);
    if (newps) {
        int oef = newps->eval_flag;
        newps->eval_flag = 1;
        interp->ps = newps;
        r = jsi_evalcode(newps, NULL, newps->opcodes, scope, currentScope, _this, ret);
        if (r) {
            Jsi_ValueDup2(interp, &ps->last_exception, newps->last_exception);
        }
        newps->eval_flag = oef;
        interp->ps = ps;
    } else  {
        //Jsi_ValueMakeStringKey(interp, &ps->last_exception, "Syntax Error");
        r = JSI_ERROR;
    }
    return r;
}