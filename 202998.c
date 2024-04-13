static void virtio_blk_handle_flush(VirtIOBlockReq *req, MultiReqBuffer *mrb)
{
    BlockDriverAIOCB *acb;

    /*
     * Make sure all outstanding writes are posted to the backing device.
     */
    virtio_submit_multiwrite(req->dev->bs, mrb);

    acb = bdrv_aio_flush(req->dev->bs, virtio_blk_flush_complete, req);
    if (!acb) {
        virtio_blk_flush_complete(req, -EIO);
    }
}