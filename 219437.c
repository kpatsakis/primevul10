sync_pblock_copy(Slapi_PBlock *src)
{
    Slapi_Operation *operation;
    Slapi_Operation *operation_new;
    Slapi_Connection *connection;
    int *scope;
    int *deref;
    int *filter_normalized;
    char *fstr;
    char **attrs, **attrs_dup;
    char **reqattrs, **reqattrs_dup;
    int *attrsonly;
    int *isroot;
    int *sizelimit;
    int *timelimit;
    struct slapdplugin *pi;
    ber_int_t msgid;
    ber_tag_t tag;

    slapi_pblock_get(src, SLAPI_OPERATION, &operation);
    slapi_pblock_get(src, SLAPI_CONNECTION, &connection);
    slapi_pblock_get(src, SLAPI_SEARCH_SCOPE, &scope);
    slapi_pblock_get(src, SLAPI_SEARCH_DEREF, &deref);
    slapi_pblock_get(src, SLAPI_PLUGIN_SYNTAX_FILTER_NORMALIZED, &filter_normalized);
    slapi_pblock_get(src, SLAPI_SEARCH_STRFILTER, &fstr);
    slapi_pblock_get(src, SLAPI_SEARCH_ATTRS, &attrs);
    slapi_pblock_get(src, SLAPI_SEARCH_REQATTRS, &reqattrs);
    slapi_pblock_get(src, SLAPI_SEARCH_ATTRSONLY, &attrsonly);
    slapi_pblock_get(src, SLAPI_REQUESTOR_ISROOT, &isroot);
    slapi_pblock_get(src, SLAPI_SEARCH_SIZELIMIT, &sizelimit);
    slapi_pblock_get(src, SLAPI_SEARCH_TIMELIMIT, &timelimit);
    slapi_pblock_get(src, SLAPI_PLUGIN, &pi);

    Slapi_PBlock *dest = slapi_pblock_new();
    operation_new = slapi_operation_new(0);
    msgid = slapi_operation_get_msgid(operation);
    slapi_operation_set_msgid(operation_new, msgid);
    tag = slapi_operation_get_tag(operation);
    slapi_operation_set_tag(operation_new, tag);
    slapi_pblock_set(dest, SLAPI_OPERATION, operation_new);
    slapi_pblock_set(dest, SLAPI_CONNECTION, connection);
    slapi_pblock_set(dest, SLAPI_SEARCH_SCOPE, &scope);
    slapi_pblock_set(dest, SLAPI_SEARCH_DEREF, &deref);
    slapi_pblock_set(dest, SLAPI_PLUGIN_SYNTAX_FILTER_NORMALIZED, &filter_normalized);
    slapi_pblock_set(dest, SLAPI_SEARCH_STRFILTER, slapi_ch_strdup(fstr));
    attrs_dup = slapi_ch_array_dup(attrs);
    reqattrs_dup = slapi_ch_array_dup(reqattrs);
    slapi_pblock_set(dest, SLAPI_SEARCH_ATTRS, attrs_dup);
    slapi_pblock_set(dest, SLAPI_SEARCH_REQATTRS, reqattrs_dup);
    slapi_pblock_set(dest, SLAPI_SEARCH_ATTRSONLY, &attrsonly);
    slapi_pblock_set(dest, SLAPI_REQUESTOR_ISROOT, &isroot);
    slapi_pblock_set(dest, SLAPI_SEARCH_SIZELIMIT, &sizelimit);
    slapi_pblock_set(dest, SLAPI_SEARCH_TIMELIMIT, &timelimit);
    slapi_pblock_set(dest, SLAPI_PLUGIN, pi);

    return dest;
}