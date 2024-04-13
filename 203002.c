static VirtIOBlockReq *virtio_blk_alloc_request(VirtIOBlock *s)
{
    VirtIOBlockReq *req = qemu_malloc(sizeof(*req));
    req->dev = s;
    req->qiov.size = 0;
    req->next = NULL;
    return req;
}