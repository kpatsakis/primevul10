WriterSizeObjCmd(ClientData UNUSED(clientData), Tcl_Interp *interp, int objc, Tcl_Obj *const* objv)
{
    int               result = TCL_OK;
    Tcl_Obj          *driverObj = NULL;
    Ns_Conn          *conn = NULL;
    Tcl_WideInt       intValue = -1;
    const char       *firstArgString;
    Ns_ObjvValueRange range = {1024, INT_MAX};
    Ns_ObjvSpec   *opts, optsNew[] = {
        {"-driver", Ns_ObjvObj, &driverObj, NULL},
        {NULL, NULL, NULL, NULL}
    };
    Ns_ObjvSpec   *args, argsNew[] = {
        {"?value", Ns_ObjvMemUnit, &intValue, &range},
        {NULL, NULL, NULL, NULL}
    };
    Ns_ObjvSpec   argsLegacy[] = {
        {"driver", Ns_ObjvObj,     &driverObj, NULL},
        {"?value", Ns_ObjvMemUnit, &intValue, &range},
        {NULL, NULL, NULL, NULL}
    };

    firstArgString = objc > 2 ? Tcl_GetString(objv[2]) : NULL;
    if (firstArgString != NULL) {
        if (*firstArgString != '-'
            && ((objc == 3 && CHARTYPE(digit, *firstArgString) == 0) ||
                objc == 4)) {
            args = argsLegacy;
            opts = NULL;
            Ns_LogDeprecated(objv, objc, "ns_writer size ?-driver drv? ?size?", NULL);
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

        } else if (intValue != -1) {
            /*
             * The optional argument was provided.
             */
            wrPtr->writersize = (size_t)intValue;
        }

        if (result == TCL_OK) {
            Tcl_SetObjResult(interp, Tcl_NewIntObj((int)wrPtr->writersize));
        }
    }

    return result;
}