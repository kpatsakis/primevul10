connection_state_machine (connection * const con)
{
    request_st * const r = &con->request;
    if (r->http_version == HTTP_VERSION_2)
        connection_state_machine_h2(r, con);
    else /* if (r->http_version <= HTTP_VERSION_1_1) */
        connection_state_machine_h1(r, con);
}