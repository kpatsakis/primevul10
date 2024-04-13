Jsi_RC jsiEvalFunction(jsi_Pstate *ps, jsi_OpCode *ip, int discard) {
    Jsi_Interp *interp = ps->interp;
    int stackargc = (int)(uintptr_t)ip->data;
    jsiVarDeref(interp, stackargc + 1);
    int tocall_index = interp->framePtr->Sp - stackargc - 1;
    Jsi_Value *_this = _jsi_THISIDX(tocall_index),
        *tocall = _jsi_STACKIDX(tocall_index), **spargs = _jsi_STACK+(interp->framePtr->Sp - stackargc),   
        *spretPtr = Jsi_ValueNew1(interp), *spretPtrOld = spretPtr,
        *args = Jsi_ValueNewArrayObj(interp, spargs, stackargc, 1);
        
    Jsi_IncrRefCount(interp, args);
    if (_this->vt != JSI_VT_OBJECT)
        _this = Jsi_ValueDup(interp, interp->Top_object);
    else {
        _this = Jsi_ValueDup(interp, _this);
        jsiClearThis(interp, tocall_index);
    }   
    Jsi_RC rc = jsi_FunctionSubCall(interp, args, _this, &spretPtr, tocall, discard);
    
    jsiPop(interp, stackargc);
    jsiClearStack(interp,1);
    if (rc == JSI_CONTINUE) {
        Jsi_ValueMakeUndef(interp, &_jsi_TOP);
        rc = JSI_OK;
    }
    if (spretPtr == spretPtrOld) {
        Jsi_ValueMove(interp, _jsi_TOP, spretPtr);
        Jsi_DecrRefCount(interp, spretPtr);
    } else {
        /*  returning a (non-copied) value reference */
        Jsi_DecrRefCount(interp, _jsi_TOP);
        _jsi_TOP = spretPtr;
    }
    Jsi_DecrRefCount(interp, args);
    return rc;
}