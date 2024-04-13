multi_client_disconnect_setenv(struct multi_instance *mi)
{
    /* setenv client real IP address */
    setenv_trusted(mi->context.c2.es, get_link_socket_info(&mi->context));

    /* setenv stats */
    setenv_stats(&mi->context);

    /* setenv connection duration */
    setenv_long_long(mi->context.c2.es, "time_duration", now - mi->created);
}