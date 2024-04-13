static void caselist_free(jsi_CaseList *c)
{
    jsi_CaseList *a = c;
    while (a) {
        a = c->next;
        if (c->es) Jsi_Free(c->es);
        Jsi_Free(c);
        c = a;
    }
}