static void virtio_blk_change_cb(void *opaque, int reason)
{
    VirtIOBlock *s = opaque;

    if (reason & CHANGE_SIZE) {
        virtio_notify_config(&s->vdev);
    }
}