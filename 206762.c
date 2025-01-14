static void fdctrl_start_transfer(FDCtrl *fdctrl, int direction)
{
    FDrive *cur_drv;
    uint8_t kh, kt, ks;

    SET_CUR_DRV(fdctrl, fdctrl->fifo[1] & FD_DOR_SELMASK);
    cur_drv = get_cur_drv(fdctrl);
    kt = fdctrl->fifo[2];
    kh = fdctrl->fifo[3];
    ks = fdctrl->fifo[4];
    FLOPPY_DPRINTF("Start transfer at %d %d %02x %02x (%d)\n",
                   GET_CUR_DRV(fdctrl), kh, kt, ks,
                   fd_sector_calc(kh, kt, ks, cur_drv->last_sect,
                                  NUM_SIDES(cur_drv)));
    switch (fd_seek(cur_drv, kh, kt, ks, fdctrl->config & FD_CONFIG_EIS)) {
    case 2:
        /* sect too big */
        fdctrl_stop_transfer(fdctrl, FD_SR0_ABNTERM, 0x00, 0x00);
        fdctrl->fifo[3] = kt;
        fdctrl->fifo[4] = kh;
        fdctrl->fifo[5] = ks;
        return;
    case 3:
        /* track too big */
        fdctrl_stop_transfer(fdctrl, FD_SR0_ABNTERM, FD_SR1_EC, 0x00);
        fdctrl->fifo[3] = kt;
        fdctrl->fifo[4] = kh;
        fdctrl->fifo[5] = ks;
        return;
    case 4:
        /* No seek enabled */
        fdctrl_stop_transfer(fdctrl, FD_SR0_ABNTERM, 0x00, 0x00);
        fdctrl->fifo[3] = kt;
        fdctrl->fifo[4] = kh;
        fdctrl->fifo[5] = ks;
        return;
    case 1:
        fdctrl->status0 |= FD_SR0_SEEK;
        break;
    default:
        break;
    }

    /* Check the data rate. If the programmed data rate does not match
     * the currently inserted medium, the operation has to fail. */
    if ((fdctrl->dsr & FD_DSR_DRATEMASK) != cur_drv->media_rate) {
        FLOPPY_DPRINTF("data rate mismatch (fdc=%d, media=%d)\n",
                       fdctrl->dsr & FD_DSR_DRATEMASK, cur_drv->media_rate);
        fdctrl_stop_transfer(fdctrl, FD_SR0_ABNTERM, FD_SR1_MA, 0x00);
        fdctrl->fifo[3] = kt;
        fdctrl->fifo[4] = kh;
        fdctrl->fifo[5] = ks;
        return;
    }

    /* Set the FIFO state */
    fdctrl->data_dir = direction;
    fdctrl->data_pos = 0;
    assert(fdctrl->msr & FD_MSR_CMDBUSY);
    if (fdctrl->fifo[0] & 0x80)
        fdctrl->data_state |= FD_STATE_MULTI;
    else
        fdctrl->data_state &= ~FD_STATE_MULTI;
    if (fdctrl->fifo[5] == 0) {
        fdctrl->data_len = fdctrl->fifo[8];
    } else {
        int tmp;
        fdctrl->data_len = 128 << (fdctrl->fifo[5] > 7 ? 7 : fdctrl->fifo[5]);
        tmp = (fdctrl->fifo[6] - ks + 1);
        if (tmp < 0) {
            FLOPPY_DPRINTF("invalid EOT: %d\n", tmp);
            fdctrl_stop_transfer(fdctrl, FD_SR0_ABNTERM, FD_SR1_MA, 0x00);
            fdctrl->fifo[3] = kt;
            fdctrl->fifo[4] = kh;
            fdctrl->fifo[5] = ks;
            return;
        }
        if (fdctrl->fifo[0] & 0x80)
            tmp += fdctrl->fifo[6];
        fdctrl->data_len *= tmp;
    }
    fdctrl->eot = fdctrl->fifo[6];
    if (fdctrl->dor & FD_DOR_DMAEN) {
        /* DMA transfer is enabled. */
        IsaDmaClass *k = ISADMA_GET_CLASS(fdctrl->dma);

        FLOPPY_DPRINTF("direction=%d (%d - %d)\n",
                       direction, (128 << fdctrl->fifo[5]) *
                       (cur_drv->last_sect - ks + 1), fdctrl->data_len);

        /* No access is allowed until DMA transfer has completed */
        fdctrl->msr &= ~FD_MSR_RQM;
        if (direction != FD_DIR_VERIFY) {
            /*
             * Now, we just have to wait for the DMA controller to
             * recall us...
             */
            k->hold_DREQ(fdctrl->dma, fdctrl->dma_chann);
            k->schedule(fdctrl->dma);
        } else {
            /* Start transfer */
            fdctrl_transfer_handler(fdctrl, fdctrl->dma_chann, 0,
                    fdctrl->data_len);
        }
        return;
    }
    FLOPPY_DPRINTF("start non-DMA transfer\n");
    fdctrl->msr |= FD_MSR_NONDMA | FD_MSR_RQM;
    if (direction != FD_DIR_WRITE)
        fdctrl->msr |= FD_MSR_DIO;
    /* IO based transfer: calculate len */
    fdctrl_raise_irq(fdctrl);
}