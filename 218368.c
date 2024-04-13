apr_status_t h2_stream_in_consumed(h2_stream *stream, apr_off_t amount)
{
    h2_session *session = stream->session;
    
    if (amount > 0) {
        apr_off_t consumed = amount;
        
        while (consumed > 0) {
            int len = (consumed > INT_MAX)? INT_MAX : (int)consumed;
            nghttp2_session_consume(session->ngh2, stream->id, len);
            consumed -= len;
        }

#ifdef H2_NG2_LOCAL_WIN_SIZE
        if (1) {
            int cur_size = nghttp2_session_get_stream_local_window_size(
                session->ngh2, stream->id);
            int win = stream->in_window_size;
            int thigh = win * 8/10;
            int tlow = win * 2/10;
            const int win_max = 2*1024*1024;
            const int win_min = 32*1024;
            
            /* Work in progress, probably should add directives for these
             * values once this stabilizes somewhat. The general idea is
             * to adapt stream window sizes if the input window changes
             * a) very quickly (< good RTT) from full to empty
             * b) only a little bit (> bad RTT)
             * where in a) it grows and in b) it shrinks again.
             */
            if (cur_size > thigh && amount > thigh && win < win_max) {
                /* almost empty again with one reported consumption, how
                 * long did this take? */
                long ms = apr_time_msec(apr_time_now() - stream->in_last_write);
                if (ms < 40) {
                    win = H2MIN(win_max, win + (64*1024));
                }
            }
            else if (cur_size < tlow && amount < tlow && win > win_min) {
                /* staying full, for how long already? */
                long ms = apr_time_msec(apr_time_now() - stream->in_last_write);
                if (ms > 700) {
                    win = H2MAX(win_min, win - (32*1024));
                }
            }
            
            if (win != stream->in_window_size) {
                stream->in_window_size = win;
                nghttp2_session_set_local_window_size(session->ngh2, 
                        NGHTTP2_FLAG_NONE, stream->id, win);
            } 
            ap_log_cerror(APLOG_MARK, APLOG_TRACE2, 0, session->c,
                          "h2_stream(%ld-%d): consumed %ld bytes, window now %d/%d",
                          session->id, stream->id, (long)amount, 
                          cur_size, stream->in_window_size);
        }
#endif
    }
    return APR_SUCCESS;   
}