static jsi_CaseExprStat *exprstat_new(jsi_Pstate *pstate, Jsi_OpCodes *expr, Jsi_OpCodes *stat, int isdef)
{
    jsi_CaseExprStat *r = (jsi_CaseExprStat*)Jsi_Calloc(1,sizeof(*r));
    r->sig = JSI_SIG_CASESTAT;
    r->expr = expr;
    r->stat = stat;
    r->isdefault = isdef;
    return r;
}