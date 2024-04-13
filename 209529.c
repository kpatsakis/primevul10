Jsi_OptionsFree(Jsi_Interp *interp, Jsi_OptionSpec *specs, void *rec, Jsi_Wide flags /*unused*/)
{
    Jsi_OptionSpec *specPtr;
    char *record = (char*)rec;
    for (specPtr = specs; specPtr->id>=JSI_OPTION_BOOL && specPtr->id < JSI_OPTION_END && specPtr->name; specPtr++) {
        char *ptr = record + specPtr->offset;
        if (specPtr->flags&JSI_OPT_NO_CLEAR)
            continue;
        if (specPtr->custom) {
            Jsi_OptionCustom* cust = Jsi_OptionCustomBuiltin(specPtr->custom);
            if (cust->freeProc != NULL) {
                (*cust->freeProc)(interp, specPtr, (char **)ptr);
                continue;
            }
        }

        switch (specPtr->id) {
#ifndef JSI_LITE_ONLY
        case JSI_OPTION_VALUE:
        case JSI_OPTION_STRING:
        case JSI_OPTION_OBJ:
        case JSI_OPTION_REGEXP:
        case JSI_OPTION_ARRAY:
        case JSI_OPTION_FUNC:
        case JSI_OPTION_USEROBJ:
        case JSI_OPTION_VAR:
        {
            Jsi_Value **vpp = (Jsi_Value**)ptr, *vPtr = *vpp;
            if (vPtr && (!(specPtr->flags&JSI_OPT_NO_DUPVALUE)))
                Jsi_DecrRefCount(interp, vPtr);
            *vpp = 0;
            break;
        }
#endif
        case JSI_OPTION_STRKEY:
            *(char**)ptr = 0;
            break;
        case JSI_OPTION_STRBUF:
            *(char*)ptr = 0;
            break;
        case JSI_OPTION_DSTRING:
            Jsi_DSFree((Jsi_DString *)ptr);
            break;
        case JSI_OPTION_TIME_T:
            *(time_t*)ptr = 0;
            break;
        case JSI_OPTION_NUMBER:
            *(Jsi_Number*)ptr = 0;
            break;
        case JSI_OPTION_FLOAT:
            *(float*)ptr = 0;
            break;
        case JSI_OPTION_LDOUBLE:
            *(ldouble*)ptr = 0;
            break;
        case JSI_OPTION_DOUBLE:
        case JSI_OPTION_TIME_D:
            *(double*)ptr = 0;
            break;
        case JSI_OPTION_TIME_W:
            *(Jsi_Wide*)ptr = 0;
            break;
        case JSI_OPTION_INT8:  *(int8_t*)ptr = 0; break;
        case JSI_OPTION_INT16: *(int16_t*)ptr = 0; break;
        case JSI_OPTION_INT32: *(int32_t*)ptr = 0; break;
        case JSI_OPTION_INT64: *(int64_t*)ptr = 0; break;
        case JSI_OPTION_UINT8: *(uint8_t*)ptr = 0; break;
        case JSI_OPTION_UINT16:*(uint16_t*)ptr = 0; break;
        case JSI_OPTION_UINT32:*(uint32_t*)ptr = 0; break;
        case JSI_OPTION_UINT64:*(uint64_t*)ptr = 0; break;
        case JSI_OPTION_INT: *(int*)ptr = 0; break;
        case JSI_OPTION_UINT: *(uint*)ptr = 0; break;
        case JSI_OPTION_BOOL: *(bool*)ptr = 0; break;
        case JSI_OPTION_CUSTOM:
            break;
        default:
            if (specPtr->size>0)
                memset(ptr, 0, specPtr->size);
            break;
        }
    }
}