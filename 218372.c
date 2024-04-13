static void set_policy_for(h2_stream *stream, h2_request *r) 
{
    int enabled = h2_session_push_enabled(stream->session);
    stream->push_policy = h2_push_policy_determine(r->headers, stream->pool, enabled);
    r->serialize = h2_config_sgeti(stream->session->s, H2_CONF_SER_HEADERS);
}