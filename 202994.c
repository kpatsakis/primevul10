static void virtio_blk_handle_request(VirtIOBlockReq *req,
    MultiReqBuffer *mrb)
{
    uint32_t type;

    if (req->elem.out_num < 1 || req->elem.in_num < 1) {
        error_report("virtio-blk missing headers");
        exit(1);
    }

    if (req->elem.out_sg[0].iov_len < sizeof(*req->out) ||
        req->elem.in_sg[req->elem.in_num - 1].iov_len < sizeof(*req->in)) {
        error_report("virtio-blk header not in correct element");
        exit(1);
    }

    req->out = (void *)req->elem.out_sg[0].iov_base;
    req->in = (void *)req->elem.in_sg[req->elem.in_num - 1].iov_base;

    type = ldl_p(&req->out->type);

    if (type & VIRTIO_BLK_T_FLUSH) {
        virtio_blk_handle_flush(req, mrb);
    } else if (type & VIRTIO_BLK_T_SCSI_CMD) {
        virtio_blk_handle_scsi(req);
    } else if (type & VIRTIO_BLK_T_GET_ID) {
        VirtIOBlock *s = req->dev;

        memcpy(req->elem.in_sg[0].iov_base, s->sn,
               MIN(req->elem.in_sg[0].iov_len, sizeof(s->sn)));
        virtio_blk_req_complete(req, VIRTIO_BLK_S_OK);
    } else if (type & VIRTIO_BLK_T_OUT) {
        qemu_iovec_init_external(&req->qiov, &req->elem.out_sg[1],
                                 req->elem.out_num - 1);
        virtio_blk_handle_write(req, mrb);
    } else {
        qemu_iovec_init_external(&req->qiov, &req->elem.in_sg[0],
                                 req->elem.in_num - 1);
        virtio_blk_handle_read(req);
    }
}