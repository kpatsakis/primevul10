static Jsi_RC mdbEvalStep(MyDbEvalContext *p) {
    Jsi_RC rc = JSI_OK;
    if( p->prep==0)
        rc = mdbEvalPrep(p);
    if (rc == JSI_BREAK)
        return JSI_BREAK;
    if (rc == JSI_OK)
        rc = mdbEvalStepSub(p, 1, NULL);
    return rc;
}