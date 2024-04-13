static void jsi_wswebsocketObjErase(jsi_wsCmdObj *cmdPtr)
{
    if (cmdPtr->interp) {
        if (cmdPtr->event)
            Jsi_EventFree(cmdPtr->interp, cmdPtr->event);
        cmdPtr->event = NULL;
        if (cmdPtr->hasOpts)
            Jsi_OptionsFree(cmdPtr->interp, WSOptions, cmdPtr, 0);
        cmdPtr->hasOpts = 0;
        if (cmdPtr->handlers)
            Jsi_HashDelete(cmdPtr->handlers);
        cmdPtr->handlers = NULL;
        if (cmdPtr->pssTable)
            Jsi_HashDelete(cmdPtr->pssTable);
        cmdPtr->pssTable = NULL;
        if (cmdPtr->fileHash)
            Jsi_HashDelete(cmdPtr->fileHash);
        cmdPtr->fileHash = NULL;
    }
    cmdPtr->interp = NULL;
}