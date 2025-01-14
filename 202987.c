static void virtio_blk_handle_scsi(VirtIOBlockReq *req)
{
    struct sg_io_hdr hdr;
    int ret;
    int status;
    int i;

    /*
     * We require at least one output segment each for the virtio_blk_outhdr
     * and the SCSI command block.
     *
     * We also at least require the virtio_blk_inhdr, the virtio_scsi_inhdr
     * and the sense buffer pointer in the input segments.
     */
    if (req->elem.out_num < 2 || req->elem.in_num < 3) {
        virtio_blk_req_complete(req, VIRTIO_BLK_S_IOERR);
        return;
    }

    /*
     * No support for bidirection commands yet.
     */
    if (req->elem.out_num > 2 && req->elem.in_num > 3) {
        virtio_blk_req_complete(req, VIRTIO_BLK_S_UNSUPP);
        return;
    }

    /*
     * The scsi inhdr is placed in the second-to-last input segment, just
     * before the regular inhdr.
     */
    req->scsi = (void *)req->elem.in_sg[req->elem.in_num - 2].iov_base;

    memset(&hdr, 0, sizeof(struct sg_io_hdr));
    hdr.interface_id = 'S';
    hdr.cmd_len = req->elem.out_sg[1].iov_len;
    hdr.cmdp = req->elem.out_sg[1].iov_base;
    hdr.dxfer_len = 0;

    if (req->elem.out_num > 2) {
        /*
         * If there are more than the minimally required 2 output segments
         * there is write payload starting from the third iovec.
         */
        hdr.dxfer_direction = SG_DXFER_TO_DEV;
        hdr.iovec_count = req->elem.out_num - 2;

        for (i = 0; i < hdr.iovec_count; i++)
            hdr.dxfer_len += req->elem.out_sg[i + 2].iov_len;

        hdr.dxferp = req->elem.out_sg + 2;

    } else if (req->elem.in_num > 3) {
        /*
         * If we have more than 3 input segments the guest wants to actually
         * read data.
         */
        hdr.dxfer_direction = SG_DXFER_FROM_DEV;
        hdr.iovec_count = req->elem.in_num - 3;
        for (i = 0; i < hdr.iovec_count; i++)
            hdr.dxfer_len += req->elem.in_sg[i].iov_len;

        hdr.dxferp = req->elem.in_sg;
    } else {
        /*
         * Some SCSI commands don't actually transfer any data.
         */
        hdr.dxfer_direction = SG_DXFER_NONE;
    }

    hdr.sbp = req->elem.in_sg[req->elem.in_num - 3].iov_base;
    hdr.mx_sb_len = req->elem.in_sg[req->elem.in_num - 3].iov_len;

    ret = bdrv_ioctl(req->dev->bs, SG_IO, &hdr);
    if (ret) {
        status = VIRTIO_BLK_S_UNSUPP;
        hdr.status = ret;
        hdr.resid = hdr.dxfer_len;
    } else if (hdr.status) {
        status = VIRTIO_BLK_S_IOERR;
    } else {
        status = VIRTIO_BLK_S_OK;
    }

    stl_p(&req->scsi->errors, hdr.status);
    stl_p(&req->scsi->residual, hdr.resid);
    stl_p(&req->scsi->sense_len, hdr.sb_len_wr);
    stl_p(&req->scsi->data_len, hdr.dxfer_len);

    virtio_blk_req_complete(req, status);
}