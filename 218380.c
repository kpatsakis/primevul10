static apr_status_t setup_input(h2_stream *stream) {
    if (stream->input == NULL) {
        int empty = (stream->input_eof 
                     && (!stream->in_buffer 
                         || APR_BRIGADE_EMPTY(stream->in_buffer)));
        if (!empty) {
            h2_beam_create(&stream->input, stream->pool, stream->id, 
                           "input", H2_BEAM_OWNER_SEND, 0, 
                           stream->session->s->timeout);
            h2_beam_send_from(stream->input, stream->pool);
        }
    }
    return APR_SUCCESS;
}