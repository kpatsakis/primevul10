void jsi_AllValueOp(Jsi_Interp *interp, Jsi_Value* val, int op) {
    if (op==1) {
        //printf("ADD: %p : %p : %d\n", interp, val, val->VD.Idx);
        assert(interp->allValues!=val);
        val->next = interp->allValues;
        if (interp->allValues)
            interp->allValues->prev = val;
        interp->allValues = val;
        return;
    }
    if (op==0) {
        //printf("DEL: %p : %p\n", interp, val);
        if (!val || !interp->allValues) return;
        if (val == interp->allValues)
            interp->allValues = val->next;
        if (val->next)
            val->next->prev = val->prev;
        if (val->prev)  
            val->prev->next = val->next; 
        return;
    }
    if (op == -1) {
        while (interp->allValues) {
            printf("NEED CLEANUP: %p\n", interp->allValues);
            Jsi_ValueFree(interp, interp->allValues);
        }
        return;
    }
#if JSI__MEMDEBUG
    assert(0);
    abort();
#endif
}