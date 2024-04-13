static Jsi_RC dbEvalCallCmd( DbEvalContext *p, Jsi_Interp *interp, Jsi_RC result)
{
    int cnt = 0;
    Jsi_RC rc = result;
    Jsi_Value *varg1;
    Jsi_Obj *argso;
    char **apColName = NULL;
    int *apColType = NULL;
    if (p->jdb->debug & TMODE_EVAL)
        JSI_DBQUERY_PRINTF( "DEBUG: eval\n");

    while( (rc==JSI_OK) && JSI_OK==(rc = dbEvalStep(p)) ) {
        int i;
        int nCol;

        cnt++;
        dbEvalRowInfo(p, &nCol, &apColName, &apColType);
        if (nCol<=0)
            continue;
        if (Jsi_ValueIsNull(interp,p->tocall))
            continue;
        /* Single object containing sql result members. */
        varg1 = Jsi_ValueMakeObject(interp, NULL, argso = Jsi_ObjNew(interp));
        for(i=0; i<nCol; i++) {
            Jsi_Value *nnv = dbEvalSetColumnValue(p, i, NULL);
            Jsi_ObjInsert(interp, argso, apColName[i], nnv, 0);
        }
        Jsi_IncrRefCount(interp, varg1);
        bool rb = Jsi_FunctionInvokeBool(interp, p->tocall, varg1);
        Jsi_DecrRefCount(interp, varg1);
        if (Jsi_InterpGone(interp))
            return JSI_ERROR;
        if (rb)
            break;
    }
    //dbEvalFinalize(p);

    if( rc==JSI_OK || rc==JSI_BREAK ) {
        //Jsi_ResetResult(interp);
        rc = JSI_OK;
    }
    return rc;
}