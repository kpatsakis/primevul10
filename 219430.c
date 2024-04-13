sync_cookie_update(Sync_Cookie *sc, Slapi_Entry *ec)
{
    const char *uniqueid = NULL;
    Slapi_Attr *attr;
    Slapi_Value *val;

    slapi_entry_attr_find(ec, SLAPI_ATTR_UNIQUEID, &attr);
    slapi_attr_first_value(attr, &val);
    uniqueid = slapi_value_get_string(val);

    sc->cookie_change_info = sync_cookie_get_change_number(sc->cookie_change_info, uniqueid);
}