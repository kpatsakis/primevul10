const h2_priority *h2_stream_get_priority(h2_stream *stream, 
                                          h2_headers *response)
{
    if (response && stream->initiated_on) {
        const char *ctype = apr_table_get(response->headers, "content-type");
        if (ctype) {
            /* FIXME: Not good enough, config needs to come from request->server */
            return h2_cconfig_get_priority(stream->session->c, ctype);
        }
    }
    return NULL;
}