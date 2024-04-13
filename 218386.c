apr_status_t h2_stream_flush_input(h2_stream *stream)
{
    apr_status_t status = APR_SUCCESS;
    
    if (stream->in_buffer && !APR_BRIGADE_EMPTY(stream->in_buffer)) {
        setup_input(stream);
        status = h2_beam_send(stream->input, stream->in_buffer, APR_BLOCK_READ);
        stream->in_last_write = apr_time_now();
    }
    if (stream->input_eof 
        && stream->input && !h2_beam_is_closed(stream->input)) {
        status = h2_beam_close(stream->input);
    }
    return status;
}