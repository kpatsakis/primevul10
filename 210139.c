static Jsi_RC jsi_wswebsocketObjFree(Jsi_Interp *interp, void *data)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)data;
    WSSIGASSERT(cmdPtr,OBJ);
    cmdPtr->deleted = 1;
    struct lws_context *ctx = cmdPtr->context;
    if (ctx)
        lws_context_destroy(ctx);
    cmdPtr->context = NULL;
    cmdPtr->_->activeCnt--;
    jsi_wswebsocketObjErase(cmdPtr);
    _JSI_MEMCLEAR(cmdPtr);
    Jsi_Free(cmdPtr);
    return JSI_OK;
}