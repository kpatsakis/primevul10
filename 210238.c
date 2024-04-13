jsi_SetOption_(Jsi_Interp *interp, Jsi_OptionSpec *specPtr, const char *string /*UNUSED*/, void* rec, Jsi_Value *argValue, Jsi_Wide flags, bool isSafe)
{
    Jsi_Wide wcount = 0;
    bool bn;
    Jsi_Number nv;
    bool isIncr = (flags & JSI_OPTS_INCR);
    const char *expType = NULL;
    char *record = (char*)rec, *ptr = record + specPtr->offset;
    Jsi_OptionCustom* cust = NULL;
    const char *emsg = NULL, *epre = "";

    if (specPtr->id<JSI_OPTION_BOOL || specPtr->id>=JSI_OPTION_END) 
        return Jsi_LogBug("unknown option id \"%d\" for \"%s\"", specPtr->id, specPtr->name);
    if (specPtr->custom  && specPtr->id == JSI_OPTION_CUSTOM) {
        cust = Jsi_OptionCustomBuiltin(specPtr->custom);
        if (cust && cust->parseProc) {
            int lastErrCnt = interp->logErrorCnt;
            Jsi_OptionSpec *oep = interp->parseMsgSpec;
            interp->parseMsgSpec = specPtr;
            Jsi_RC rc = (*cust->parseProc)(interp, specPtr, argValue, NULL, record, flags);
            if (rc != JSI_OK) {
                if (!interp->csc || lastErrCnt == interp->logErrorCnt)
                    Jsi_LogError("invalid value");
                interp->parseMsgSpec = oep;
                return JSI_ERROR;
            }
            interp->parseMsgSpec = oep;
        } else 
            return Jsi_LogBug("missing or bad custom for \"%s\"", specPtr->name);
        goto done;
    }

    switch (specPtr->id) {
    case JSI_OPTION_CUSTOM:
        if (!specPtr->custom) 
            return Jsi_LogBug("missing or custom for \"%s\"", specPtr->name);

    case JSI_OPTION_BOOL: {
        if (!argValue)
            *(char*)ptr = 0;
        else if (!Jsi_ValueIsBoolean(interp, argValue))
            goto bail;
        Jsi_GetBoolFromValue(interp, argValue, &bn);
        *(char*)ptr = bn;
        break;
    }

    case JSI_OPTION_INT:
    case JSI_OPTION_UINT:
    case JSI_OPTION_LONG:
    case JSI_OPTION_INTPTR_T:
    case JSI_OPTION_UINTPTR_T:
    case JSI_OPTION_SIZE_T:
    case JSI_OPTION_SSIZE_T:
    case JSI_OPTION_ULONG:
    case JSI_OPTION_SHORT:
    case JSI_OPTION_USHORT:
    case JSI_OPTION_UINT64:
    case JSI_OPTION_INT64:
    case JSI_OPTION_INT8:
    case JSI_OPTION_UINT8:
    case JSI_OPTION_INT16:
    case JSI_OPTION_UINT16:
    case JSI_OPTION_INT32:
    case JSI_OPTION_UINT32:
        wcount = 0;
        if (argValue) {
            if (!Jsi_ValueIsNumber(interp, argValue))
                goto bail;

            if (Jsi_GetWideFromValue(interp, argValue, &wcount) != JSI_OK) {
                return JSI_ERROR;
            }
            
        }
        switch (specPtr->id) {
#define _JSI_OPTSETTYP(typ, n, ptr) if (isIncr) n += *((typ *)ptr); \
    if (isIncr || ((Jsi_Wide)(typ)(n)) == (n)) interp->cdataIncrVal = *((typ *)ptr) = (typ)(n); else expType = #typ;
            case JSI_OPTION_INT:    _JSI_OPTSETTYP(int, wcount, ptr); break;
            case JSI_OPTION_UINT:   _JSI_OPTSETTYP(uint, wcount, ptr); break;
            case JSI_OPTION_INTPTR_T: _JSI_OPTSETTYP(intptr_t, wcount, ptr); break;
            case JSI_OPTION_UINTPTR_T:_JSI_OPTSETTYP(uintptr_t, wcount, ptr); break;
            case JSI_OPTION_SIZE_T:   _JSI_OPTSETTYP(size_t, wcount, ptr); break;
            case JSI_OPTION_SSIZE_T:  _JSI_OPTSETTYP(ssize_t, wcount, ptr); break;
            case JSI_OPTION_LONG:   _JSI_OPTSETTYP(long, wcount, ptr); break;
            case JSI_OPTION_ULONG:  _JSI_OPTSETTYP(ulong, wcount, ptr); break;
            case JSI_OPTION_SHORT:  _JSI_OPTSETTYP(short, wcount, ptr); break;
            case JSI_OPTION_USHORT: _JSI_OPTSETTYP(ushort, wcount, ptr); break;
            case JSI_OPTION_INT8:   _JSI_OPTSETTYP(int8_t, wcount, ptr); break;
            case JSI_OPTION_UINT8:  _JSI_OPTSETTYP(uint8_t, wcount, ptr) break;
            case JSI_OPTION_INT16:  _JSI_OPTSETTYP(int16_t, wcount, ptr); break;
            case JSI_OPTION_UINT16: _JSI_OPTSETTYP(uint16_t, wcount, ptr); break;
            case JSI_OPTION_INT32:  _JSI_OPTSETTYP(int32_t, wcount, ptr); break;
            case JSI_OPTION_UINT32: _JSI_OPTSETTYP(uint32_t, wcount, ptr); break;
            case JSI_OPTION_INT64:  _JSI_OPTSETTYP(int64_t, wcount, ptr); break;
            case JSI_OPTION_UINT64: _JSI_OPTSETTYP(uint64_t, wcount, ptr); break; // TODO: might loose top sign bit...
            default: break;
        }
        if (expType)
            return Jsi_LogType("not a %s", expType);
        isIncr = 0;
        break;

    case JSI_OPTION_NUMBER:
    case JSI_OPTION_DOUBLE:
    case JSI_OPTION_LDOUBLE:
    case JSI_OPTION_FLOAT:
        nv = 0;
        if (argValue) {
            if (!Jsi_ValueIsNumber(interp, argValue))
                goto bail;
            if (Jsi_GetNumberFromValue(interp, argValue, &nv) != JSI_OK) {
                return JSI_ERROR;
            }
        }
            
        switch (specPtr->id) {
#define _JSI_OPTSETNTYP(typ, n, ptr) if (!argValue) *(typ*)ptr = 0; else { if (isIncr) n += *((typ *)ptr); \
            interp->cdataIncrVal = *((typ *)ptr) = (typ)(n); \
            if (interp->strict && Jsi_NumberIsNaN((Jsi_Number)(*((typ *)ptr)))) return Jsi_LogError("not a number"); }

            case JSI_OPTION_NUMBER: _JSI_OPTSETNTYP(Jsi_Number, nv, ptr); break;
            case JSI_OPTION_LDOUBLE: _JSI_OPTSETNTYP(ldouble, nv, ptr); break;
            case JSI_OPTION_FLOAT: _JSI_OPTSETNTYP(float, nv, ptr); break;
            case JSI_OPTION_DOUBLE: _JSI_OPTSETNTYP(double, nv, ptr); break;
            default: break;
        }
        isIncr = 0;
        break;
    case JSI_OPTION_STRKEY:
    {
        if (argValue == NULL || Jsi_ValueIsNull(interp, argValue))
            *(const char**)ptr = NULL;
        else {
            const char *scp;
            if (Jsi_GetStringFromValue(interp, argValue, &scp) != JSI_OK) {
                return JSI_ERROR;
            }
            *(const char**)ptr = Jsi_KeyAdd(interp,scp);
        }
    }
    break;
    case JSI_OPTION_STRBUF:
    {
        if (argValue == NULL || Jsi_ValueIsNull(interp, argValue))
            *(char*)ptr = 0;
        else {
            int sLen;
            const char *scp = Jsi_ValueString(interp, argValue, &sLen);
            if (!scp)
                goto bail;
            if (sLen>(int)(specPtr->size-1)) {
                return Jsi_LogError("String too long");
                //sLen = specPtr->size-1;
            }
            memcpy((char*)ptr, scp, sLen);
            ((char*)ptr)[sLen] = 0;
        }
    }
    break;
    
#define _JSI_OPT_CHECKNULL(argValue) if (!argValue || Jsi_ValueIsNull(interp, argValue)) { \
         if (*((Jsi_Value **)ptr)) \
            Jsi_DecrRefCount(interp, *((Jsi_Value **)ptr)); \
        *((Jsi_Value **)ptr) = NULL; \
        break; \
    }

#define _JSI_OPT_ARGSET(argValue, ptr) \
    if (!(specPtr->flags&JSI_OPT_NO_DUPVALUE)) {\
        Jsi_IncrRefCount(interp, argValue); \
        if (*((Jsi_Value **)ptr)) Jsi_DecrRefCount(interp, *((Jsi_Value **)ptr)); \
    }\
    *((Jsi_Value **)ptr) = argValue;
    
    case JSI_OPTION_STRING:
        if (argValue == *((Jsi_Value **)ptr))
            break;
        _JSI_OPT_CHECKNULL(argValue);
        if (!Jsi_ValueIsString(interp, argValue))
            goto bail;
        _JSI_OPT_ARGSET(argValue, ptr);
        break;
        
    case JSI_OPTION_DSTRING:
        Jsi_DSInit((Jsi_DString *)ptr);
        if (argValue && !Jsi_ValueIsNull(interp, argValue))
        {
            int sLen;
            const char *scp = Jsi_ValueString(interp, argValue, &sLen);
            if (!scp)
                goto bail;
            Jsi_DSAppendLen((Jsi_DString *)ptr, scp, sLen);
        }
        break;
    
    case JSI_OPTION_TIME_D: {
       if (argValue)
        {
            Jsi_Number nv = 0;
            if (Jsi_ValueIsNumber(interp, argValue)) {
                Jsi_GetNumberFromValue(interp, argValue, &nv);
                *(double*)ptr = nv;
            } else {
                const char *scp;
                if (Jsi_GetStringFromValue(interp, argValue, &scp) != JSI_OK) {
                    return JSI_ERROR;
                }
                if (JSI_OK != Jsi_DatetimeParse(interp, scp, "", 0, &nv, false))
                    return JSI_ERROR;
                *(double*)ptr = nv;
            }
        } else {
            *(double*)ptr = 0;
        }
        break;
    }
    case JSI_OPTION_TIME_W: {
       if (argValue)
        {
            if (Jsi_ValueIsNumber(interp, argValue)) {
                Jsi_GetNumberFromValue(interp, argValue, (Jsi_Number*)ptr);
            } else {
                const char *scp;
                Jsi_Number num;
                if (Jsi_GetStringFromValue(interp, argValue, &scp) != JSI_OK) {
                    return JSI_ERROR;
                }
                if (JSI_OK != Jsi_DatetimeParse(interp, scp, "", 0, &num, false))
                    return JSI_ERROR;
                *(Jsi_Wide*)ptr = (Jsi_Wide)num;
            }
        } else {
            *(Jsi_Wide*)ptr = 0;
        }
        break;
    }
    case JSI_OPTION_TIME_T: {
       if (argValue)
        {
            if (Jsi_ValueIsNumber(interp, argValue)) {
                Jsi_Number num;
                Jsi_GetNumberFromValue(interp, argValue, &num);
                *(time_t*)ptr = (time_t)num;
            } else {
                const char *scp;
                if (Jsi_GetStringFromValue(interp, argValue, &scp) != JSI_OK) {
                    return JSI_ERROR;
                }
                Jsi_Number nval;
                if (JSI_OK != Jsi_DatetimeParse(interp, scp, "", 0, &nval, false))
                    return JSI_ERROR;
                *(time_t*)ptr = nval/1000LL;
            }
        } else {
            *(time_t*)ptr = 0;
        }
        break;
    }
    case JSI_OPTION_VAR:
        _JSI_OPT_CHECKNULL(argValue);
        if (argValue->vt != JSI_VT_NULL && argValue->vt != JSI_VT_VARIABLE) 
            goto bail;
        _JSI_OPT_ARGSET(argValue, ptr);
        break;

    case JSI_OPTION_FUNC:
        _JSI_OPT_CHECKNULL(argValue);
        if (argValue->vt != JSI_VT_OBJECT || argValue->d.obj->ot != JSI_OT_FUNCTION) 
            goto bail;
        if (specPtr->data && (interp->typeCheck.run|interp->typeCheck.all))
            if (!jsi_FuncArgCheck(interp, argValue->d.obj->d.fobj->func, (char*)specPtr->data)) 
                return Jsi_LogError("failed setting func pointer for %s", specPtr->name);

        _JSI_OPT_ARGSET(argValue, ptr);
        break;
        
    case JSI_OPTION_USEROBJ:
        _JSI_OPT_CHECKNULL(argValue);
        if (argValue->vt != JSI_VT_OBJECT || argValue->d.obj->ot != JSI_OT_USEROBJ) 
            goto bail;
        if (specPtr->data && Jsi_Strcmp((char*)specPtr->data, argValue->d.obj->d.uobj->reg->name)) 
            return Jsi_LogError("expected id %s for %s",(char*)specPtr->data,  specPtr->name);
        _JSI_OPT_ARGSET(argValue, ptr);
        break;

    case JSI_OPTION_REGEXP:
        _JSI_OPT_CHECKNULL(argValue);
        if (argValue->vt != JSI_VT_OBJECT || argValue->d.obj->ot != JSI_OT_REGEXP)
            goto bail;
        _JSI_OPT_ARGSET(argValue, ptr);
        break;

    case JSI_OPTION_OBJ:
        _JSI_OPT_CHECKNULL(argValue);
        if (argValue->vt != JSI_VT_OBJECT)
            goto bail;

    case JSI_OPTION_VALUE:
         _JSI_OPT_CHECKNULL(argValue);
         _JSI_OPT_ARGSET(argValue, ptr);
        break;
    case JSI_OPTION_ARRAY:
        _JSI_OPT_CHECKNULL(argValue);
        if (argValue->vt != JSI_VT_OBJECT || !argValue->d.obj->isarrlist)
            goto bail;
        _JSI_OPT_ARGSET(argValue, ptr);
        break;
#ifdef __cplusplus
    case JSI_OPTION_END:
#else
    default:
#endif
        Jsi_LogBug("invalid option id: %d", specPtr->id);
    }
done:
    specPtr->flags |= JSI_OPT_IS_SPECIFIED;
    if (isIncr)
        return Jsi_LogError("incr invalid for %s", specPtr->name);
    return JSI_OK;

bail:
    if (!emsg) {
        emsg = jsi_OptTypeInfo[specPtr->id].cName;
        epre = "expected ";
    }
    return Jsi_LogError("%s%s: for %s option \"%.40s\"", epre, emsg, (cust?cust->name:""), specPtr->name);
}