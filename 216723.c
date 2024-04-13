setenv_stats(struct context *c)
{
    setenv_counter(c->c2.es, "bytes_received", c->c2.link_read_bytes);
    setenv_counter(c->c2.es, "bytes_sent", c->c2.link_write_bytes);
}