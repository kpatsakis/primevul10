static jsi_CaseList *caselist_new(jsi_Pstate *pstate, jsi_CaseExprStat *es)
{
    jsi_CaseList *a = (jsi_CaseList*)Jsi_Calloc(1,sizeof(*a));
    a->sig = JSI_SIG_CASELIST;
    a->es = es;
    a->tail = a;
    return a;
}