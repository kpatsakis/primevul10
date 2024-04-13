tunnel_server(struct context *top)
{
    ASSERT(top->options.mode == MODE_SERVER);

    if (proto_is_dgram(top->options.ce.proto))
    {
        tunnel_server_udp(top);
    }
    else
    {
        tunnel_server_tcp(top);
    }
}