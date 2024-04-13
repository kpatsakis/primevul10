AP_DECLARE_NONSTD(int) ap_core_translate(request_rec *r)
{
    apr_status_t rv;
    char *path;

    /* XXX this seems too specific, this should probably become
     * some general-case test
     */
    if (r->proxyreq) {
        return HTTP_FORBIDDEN;
    }
    if (!r->uri || ((r->uri[0] != '/') && strcmp(r->uri, "*"))) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00126)
                     "Invalid URI in request '%s' '%s'", r->uri, r->the_request);
        return HTTP_BAD_REQUEST;
    }

    if (r->server->path
        && !strncmp(r->uri, r->server->path, r->server->pathlen)
        && (r->server->path[r->server->pathlen - 1] == '/'
            || r->uri[r->server->pathlen] == '/'
            || r->uri[r->server->pathlen] == '\0'))
    {
        path = r->uri + r->server->pathlen;
    }
    else {
        path = r->uri;
    }
    /*
     * Make sure that we do not mess up the translation by adding two
     * /'s in a row.  This happens under windows when the document
     * root ends with a /
     */
    /* skip all leading /'s (e.g. http://localhost///foo)
     * so we are looking at only the relative path.
     */
    while (*path == '/') {
        ++path;
    }
    if ((rv = apr_filepath_merge(&r->filename, ap_document_root(r), path,
                                 APR_FILEPATH_TRUENAME
                               | APR_FILEPATH_SECUREROOT, r->pool))
                != APR_SUCCESS) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(00127)
                     "Cannot map %s to file", r->the_request);
        return HTTP_FORBIDDEN;
    }
    r->canonical_filename = r->filename;

    return OK;
}