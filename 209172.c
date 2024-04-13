WriterGetMemunitFromDict(Tcl_Interp *interp, Tcl_Obj *dictObj, Tcl_Obj *keyObj, Ns_ObjvValueRange *rangePtr,
                         Tcl_WideInt *valuePtr)
{
    Tcl_Obj *intObj = NULL;
    int      result;

    NS_NONNULL_ASSERT(interp != NULL);
    NS_NONNULL_ASSERT(dictObj != NULL);
    NS_NONNULL_ASSERT(keyObj != NULL);
    NS_NONNULL_ASSERT(valuePtr != NULL);

    result = Tcl_DictObjGet(interp, dictObj, keyObj, &intObj);
    if (result == TCL_OK && intObj != NULL) {
        result = Ns_TclGetMemUnitFromObj(interp, intObj, valuePtr);
        if (result == TCL_OK && rangePtr != NULL) {
            result = Ns_CheckWideRange(interp, Tcl_GetString(keyObj), rangePtr, *valuePtr);
        }
    }

    return result;
}