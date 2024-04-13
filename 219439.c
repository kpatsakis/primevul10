sync_handle_cnum_entry(Slapi_Entry *e, void *cb_data)
{
    int rc = 0;
    Sync_CallBackData *cb = (Sync_CallBackData *)cb_data;
    Slapi_Value *sval = NULL;
    const struct berval *value;

    cb->changenr = 0;

    if (NULL != e) {
        Slapi_Attr *chattr = NULL;
        sval = NULL;
        value = NULL;
        if (slapi_entry_attr_find(e, CL_ATTR_CHANGENUMBER, &chattr) == 0) {
            slapi_attr_first_value(chattr, &sval);
            if (NULL != sval) {
                value = slapi_value_get_berval(sval);
                if (value && value->bv_val && ('\0' != value->bv_val[0])) {
                    cb->changenr = sync_number2ulong(value->bv_val);
                    if (SYNC_INVALID_CHANGENUM != cb->changenr) {
                        cb->cb_err = 0; /* changenr successfully set */
                    }
                }
            }
        }
    }
    return (rc);
}