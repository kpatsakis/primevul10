sync_cookie_get_change_number(int lastnr, const char *uniqueid)
{
    Slapi_PBlock *srch_pb;
    Slapi_Entry **entries;
    Slapi_Entry *cl_entry;
    int rv;
    unsigned long newnr = SYNC_INVALID_CHANGENUM;
    LDAPControl **ctrls = NULL;
    
    ctrls = (LDAPControl **)slapi_ch_calloc(2, sizeof(LDAPControl *));
    char *filter = slapi_ch_smprintf("(&(changenumber>=%d)(targetuniqueid=%s))", lastnr + 1, uniqueid);
    ctrls[0] = sync_build_sort_control("changenumber");

    srch_pb = slapi_pblock_new();
    slapi_search_internal_set_pb(srch_pb, CL_SRCH_BASE, LDAP_SCOPE_SUBTREE, filter,
                                 NULL, 0, ctrls, NULL, plugin_get_default_component_id(), 0);
    slapi_search_internal_pb(srch_pb);
    slapi_pblock_get(srch_pb, SLAPI_PLUGIN_INTOP_RESULT, &rv);
    if (rv == LDAP_SUCCESS) {
        slapi_pblock_get(srch_pb, SLAPI_PLUGIN_INTOP_SEARCH_ENTRIES, &entries);
        if (entries && *entries) {
            Slapi_Attr *attr;
            Slapi_Value *val;
            cl_entry = *entries; /* only use teh first one */
            slapi_entry_attr_find(cl_entry, CL_ATTR_CHANGENUMBER, &attr);
            slapi_attr_first_value(attr, &val);
            newnr = sync_number2ulong((char *)slapi_value_get_string(val));
        }
#if DEBUG
        slapi_log_err(SLAPI_LOG_PLUGIN, SYNC_PLUGIN_SUBSYSTEM, "sync_cookie_get_change_number looks for \"%s\"\n",
                    filter);
        for (size_t i = 0; entries[i]; i++) {
            Slapi_Attr *attr;
            Slapi_Value *val;
            char *entrydn;
            unsigned long nr;
            slapi_entry_attr_find(entries[i], CL_ATTR_ENTRYDN, &attr);
            slapi_attr_first_value(attr, &val);
            entrydn = (char *)slapi_value_get_string(val);
            slapi_entry_attr_find(entries[i], CL_ATTR_CHANGENUMBER, &attr);
            slapi_attr_first_value(attr, &val);
            nr = sync_number2ulong((char *)slapi_value_get_string(val));
            slapi_log_err(SLAPI_LOG_PLUGIN, SYNC_PLUGIN_SUBSYSTEM, "sync_cookie_get_change_number after %d: %d %s\n",
                    lastnr, (int) nr, entrydn);
        }
#endif
    }

    slapi_free_search_results_internal(srch_pb);
    slapi_pblock_destroy(srch_pb);
    slapi_ch_free((void **)&filter);
    return (newnr);
}