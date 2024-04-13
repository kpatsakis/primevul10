static apr_status_t add_buffered_data(h2_stream *stream, apr_off_t requested,
                                      apr_off_t *plen, int *peos, int *is_all, 
                                      h2_headers **pheaders)
{
    apr_bucket *b, *e;
    
    *peos = 0;
    *plen = 0;
    *is_all = 0;
    if (pheaders) {
        *pheaders = NULL;
    }

    H2_STREAM_OUT_LOG(APLOG_TRACE2, stream, "add_buffered_data");
    b = APR_BRIGADE_FIRST(stream->out_buffer);
    while (b != APR_BRIGADE_SENTINEL(stream->out_buffer)) {
        e = APR_BUCKET_NEXT(b);
        if (APR_BUCKET_IS_METADATA(b)) {
            if (APR_BUCKET_IS_FLUSH(b)) {
                APR_BUCKET_REMOVE(b);
                apr_bucket_destroy(b);
            }
            else if (APR_BUCKET_IS_EOS(b)) {
                *peos = 1;
                return APR_SUCCESS;
            }
            else if (H2_BUCKET_IS_HEADERS(b)) {
                if (*plen > 0) {
                    /* data before the response, can only return up to here */
                    return APR_SUCCESS;
                }
                else if (pheaders) {
                    *pheaders = h2_bucket_headers_get(b);
                    APR_BUCKET_REMOVE(b);
                    apr_bucket_destroy(b);
                    ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c,
                                  H2_STRM_MSG(stream, "prep, -> response %d"), 
                                  (*pheaders)->status);
                    return APR_SUCCESS;
                }
                else {
                    return APR_EAGAIN;
                }
            }
        }
        else if (b->length == 0) {
            APR_BUCKET_REMOVE(b);
            apr_bucket_destroy(b);
        }
        else {
            ap_assert(b->length != (apr_size_t)-1);
            *plen += b->length;
            if (*plen >= requested) {
                *plen = requested;
                return APR_SUCCESS;
            }
        }
        b = e;
    }
    *is_all = 1;
    return APR_SUCCESS;
}