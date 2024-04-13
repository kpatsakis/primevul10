jsi_GetOption(Jsi_Interp *interp, Jsi_OptionSpec *specPtr, void* record, const char *option, Jsi_Value **valuePtr, Jsi_Wide flags)
{
    char *ptr;
    
    if (specPtr == NULL) 
        return Jsi_LogError("no such option: %s", option);
    //isNull = ((*string == '\0') && (specPtr->flags & JSI_OPTION_NULL_OK));
    
    ptr = (char *)record + specPtr->offset;
    if (_JSICASTINT(specPtr->id)<0 || specPtr->id>=JSI_OPTION_END) 
        return Jsi_LogError("no such option: %s", option);
    if (specPtr->custom) {
        Jsi_OptionCustom* cust = Jsi_OptionCustomBuiltin(specPtr->custom);
        if (cust->formatProc)
            return (*cust->formatProc) (interp, specPtr, valuePtr, NULL, record, flags);
    }

    switch (specPtr->id) {
    case JSI_OPTION_BOOL:
        Jsi_ValueMakeBool(interp, valuePtr,*(bool*)ptr );
        break;
    case JSI_OPTION_INT:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(int *)ptr));
        break;
    case JSI_OPTION_UINT:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(uint *)ptr));
        break;
    case JSI_OPTION_INT8:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(int8_t *)ptr));
        break;
    case JSI_OPTION_INT16:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(int16_t *)ptr));
        break;
    case JSI_OPTION_INT32:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(int32_t *)ptr));
        break;
    case JSI_OPTION_INT64:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(Jsi_Wide *)ptr));
        break;
    case JSI_OPTION_UINT8:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(uint8_t *)ptr));
        break;
    case JSI_OPTION_UINT16:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(uint16_t *)ptr));
        break;
    case JSI_OPTION_UINT32:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(uint32_t *)ptr));
        break;
    case JSI_OPTION_UINT64:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)(*(uint64_t *)ptr));
        break;
    case JSI_OPTION_FLOAT:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)*(float *)ptr);
        break;
    case JSI_OPTION_DOUBLE:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)*(double *)ptr);
        break;
    case JSI_OPTION_LDOUBLE:
        Jsi_ValueMakeNumber(interp, valuePtr, (Jsi_Number)*(ldouble *)ptr);
        break;
    case JSI_OPTION_NUMBER:
        Jsi_ValueMakeNumber(interp, valuePtr, *(Jsi_Number *)ptr);
        break;
    case JSI_OPTION_SIZE_T:
        Jsi_ValueMakeNumber(interp, valuePtr, *(size_t *)ptr);
        break;
    case JSI_OPTION_SSIZE_T:
        Jsi_ValueMakeNumber(interp, valuePtr, *(ssize_t *)ptr);
        break;
    case JSI_OPTION_INTPTR_T:
        Jsi_ValueMakeNumber(interp, valuePtr, *(intptr_t *)ptr);
        break;
    case JSI_OPTION_UINTPTR_T:
        Jsi_ValueMakeNumber(interp, valuePtr, *(uintptr_t *)ptr);
        break;
    case JSI_OPTION_LONG:
        Jsi_ValueMakeNumber(interp, valuePtr, *(long *)ptr);
        break;
    case JSI_OPTION_ULONG:
        Jsi_ValueMakeNumber(interp, valuePtr, *(ulong *)ptr);
        break;
    case JSI_OPTION_SHORT:
        Jsi_ValueMakeNumber(interp, valuePtr, *(short *)ptr);
        break;
    case JSI_OPTION_USHORT:
        Jsi_ValueMakeNumber(interp, valuePtr, *(ushort *)ptr);
        break;
        
    case JSI_OPTION_DSTRING:
        Jsi_ValueFromDS(interp, (Jsi_DString*)ptr, valuePtr);
        break;
    
    case JSI_OPTION_TIME_W: {
        Jsi_DString dStr = {};
        Jsi_DatetimeFormat(interp, (Jsi_Number)(*(Jsi_Wide*)ptr), "", 0, &dStr);
        Jsi_ValueFromDS(interp, &dStr, valuePtr);
        break;
    }
    case JSI_OPTION_TIME_D: {
        Jsi_DString dStr = {};
        Jsi_DatetimeFormat(interp, (Jsi_Number)(*(double*)ptr), "", 0, &dStr);
        Jsi_ValueFromDS(interp, &dStr, valuePtr);
        break;
    }
    case JSI_OPTION_TIME_T: {
        Jsi_DString dStr = {};
        Jsi_DatetimeFormat(interp, 1000LL* (Jsi_Number)*(time_t*)ptr, "%Y-%m-%d %H:%M:%S", 0, &dStr);
        Jsi_ValueFromDS(interp, &dStr, valuePtr);
        break;
    }
    case JSI_OPTION_STRBUF:
        if (ptr)
            Jsi_ValueMakeStringDup(interp, valuePtr, ptr);
        else
            Jsi_ValueMakeNull(interp, valuePtr);
        break;

    case JSI_OPTION_STRKEY:
        ptr = *(char **)ptr;
        if (ptr)
            Jsi_ValueMakeStringDup(interp, valuePtr, ptr);
        else
            Jsi_ValueMakeNull(interp, valuePtr);
        break;

    case JSI_OPTION_STRING:
    case JSI_OPTION_VAR:
    case JSI_OPTION_FUNC:
    case JSI_OPTION_USEROBJ:
    case JSI_OPTION_OBJ:
    case JSI_OPTION_VALUE:
    case JSI_OPTION_REGEXP:
    case JSI_OPTION_ARRAY:
        if (*(Jsi_Value **)ptr)
            Jsi_ValueCopy(interp, *valuePtr, *(Jsi_Value **)ptr);
        else
            Jsi_ValueMakeNull(interp, valuePtr);
        break;

    case JSI_OPTION_CUSTOM:
        break;

#ifdef __cplusplus
    case JSI_OPTION_END:
#else
    default:
#endif
        Jsi_LogBug("invalid option id %d", specPtr->id);
        return JSI_ERROR;
    }

    return JSI_OK;
}