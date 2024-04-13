WriterStreamingObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp,
                      int objc, Tcl_Obj *const* objv)
{
    int          boolValue = -1, result = TCL_OK;
    Tcl_Obj     *driverObj = NULL;
    Ns_Conn     *conn = NULL;
    const char  *firstArgString;
    Ns_ObjvSpec *opts, optsNew[] = {
        {"-driver", Ns_ObjvObj, &driverObj, NULL},
        {NULL, NULL, NULL, NULL}
    };
    Ns_ObjvSpec *args, argsNew[] = {
        {"?value", Ns_ObjvBool, &boolValue, NULL},
        {NULL, NULL, NULL, NULL}
    };
    Ns_ObjvSpec  argsLegacy[] = {
        {"driver", Ns_ObjvObj,  &driverObj, NULL},
        {"?value", Ns_ObjvBool, &boolValue, NULL},
        {NULL, NULL, NULL, NULL}
    };

    firstArgString = objc > 2 ? Tcl_GetString(objv[2]) : NULL;
    if (firstArgString != NULL) {
        int argValue;
        if (*firstArgString != '-'
            && ((objc == 3 &&  Tcl_ExprBoolean(interp, firstArgString, &argValue) == TCL_OK) ||
                objc == 4)) {
            args = argsLegacy;
            opts = NULL;
            Ns_LogDeprecated(objv, objc, "ns_writer streaming ?-driver drv? ?value?", NULL);
        } else {
            args = argsNew;
            opts = optsNew;
        }
    } else {
        args = argsNew;
        opts = optsNew;
    }

    if (Ns_ParseObjv(opts, args, interp, 2, objc, objv) != NS_OK) {
        result = TCL_ERROR;

    } else if ((driverObj == NULL)
               && NsConnRequire(interp, NS_CONN_REQUIRE_ALL, &conn) != NS_OK) {
        result = TCL_ERROR;

    } else {
        DrvWriter *wrPtr;

        if (DriverWriterFromObj(interp, driverObj, conn, &wrPtr) != NS_OK) {
            result = TCL_ERROR;

        } else if (boolValue != -1) {
            /*
             * The optional argument was provided.
             */
            wrPtr->doStream = (boolValue == 1 ? NS_WRITER_STREAM_ACTIVE : NS_WRITER_STREAM_NONE);
        }

        if (result == TCL_OK) {
            Tcl_SetObjResult(interp, Tcl_NewIntObj(wrPtr->doStream == NS_WRITER_STREAM_ACTIVE ? 1 : 0));
        }
    }

    return result;
}