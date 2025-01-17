ssize_t nbd_receive_reply(QIOChannel *ioc, NBDReply *reply)
{
    uint8_t buf[NBD_REPLY_SIZE];
    uint32_t magic;
    ssize_t ret;

    ret = read_sync(ioc, buf, sizeof(buf));
    if (ret <= 0) {
        return ret;
    }

    if (ret != sizeof(buf)) {
        LOG("read failed");
        return -EINVAL;
    }

    /* Reply
       [ 0 ..  3]    magic   (NBD_REPLY_MAGIC)
       [ 4 ..  7]    error   (0 == no error)
       [ 7 .. 15]    handle
     */

    magic = ldl_be_p(buf);
    reply->error  = ldl_be_p(buf + 4);
    reply->handle = ldq_be_p(buf + 8);

    reply->error = nbd_errno_to_system_errno(reply->error);

    if (reply->error == ESHUTDOWN) {
        /* This works even on mingw which lacks a native ESHUTDOWN */
        LOG("server shutting down");
        return -EINVAL;
    }
    TRACE("Got reply: { magic = 0x%" PRIx32 ", .error = % " PRId32
          ", handle = %" PRIu64" }",
          magic, reply->error, reply->handle);

    if (magic != NBD_REPLY_MAGIC) {
        LOG("invalid magic (got 0x%" PRIx32 ")", magic);
        return -EINVAL;
    }
    return 0;
}