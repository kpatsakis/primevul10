sync_cookie_get_client_info(Slapi_PBlock *pb)
{
    char *targetdn;
    char *strfilter;
    char *clientdn;
    char *clientinfo;

    slapi_pblock_get(pb, SLAPI_TARGET_DN, &targetdn);
    slapi_pblock_get(pb, SLAPI_SEARCH_STRFILTER, &strfilter);
    slapi_pblock_get(pb, SLAPI_REQUESTOR_DN, &clientdn);
    clientinfo = slapi_ch_smprintf("%s:%s:%s", clientdn, targetdn, strfilter);
    return (clientinfo);
}