connection_transition_h2 (request_st * const h2r, connection * const con)
{
    buffer_copy_string_len(&h2r->target,      CONST_STR_LEN("*"));
    buffer_copy_string_len(&h2r->target_orig, CONST_STR_LEN("*"));
    buffer_copy_string_len(&h2r->uri.path,    CONST_STR_LEN("*"));
    h2r->http_method = HTTP_METHOD_PRI;
    h2r->reqbody_length = -1; /*(unnecessary for h2r?)*/
    h2r->conf.stream_request_body |= FDEVENT_STREAM_REQUEST_POLLIN;

    /* (h2r->state == CON_STATE_READ) for transition by ALPN
     *   or starting cleartext HTTP/2 with Prior Knowledge
     *   (e.g. via HTTP Alternative Services)
     * (h2r->state == CON_STATE_RESPONSE_END) for Upgrade: h2c */

    if (h2r->state != CON_STATE_ERROR)
        connection_set_state(h2r, CON_STATE_WRITE);

  #if 0 /* ... if it turns out we need a separate fdevent handler for HTTP/2 */
    con->fdn->handler = connection_handle_fdevent_h2;
  #endif

    if (NULL == con->h2) /*(not yet transitioned to HTTP/2; not Upgrade: h2c)*/
        h2_init_con(h2r, con, NULL);
}