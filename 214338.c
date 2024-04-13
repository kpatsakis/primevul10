static apr_status_t core_dirwalk_stat(apr_finfo_t *finfo, request_rec *r,
                                      apr_int32_t wanted) 
{
    return apr_stat(finfo, r->filename, wanted, r->pool);
}