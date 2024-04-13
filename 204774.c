connect_sync_or_async(MYSQL *mysql, NET *net, my_socket fd,
                      const struct sockaddr *name, uint namelen)
{
  if (mysql->options.extension && mysql->options.extension->async_context &&
      mysql->options.extension->async_context->active)
  {
    my_bool old_mode;
    vio_blocking(net->vio, FALSE, &old_mode);
    return my_connect_async(mysql->options.extension->async_context, fd,
                            name, namelen, mysql->options.connect_timeout);
  }

  return my_connect(fd, name, namelen, mysql->options.connect_timeout);
}