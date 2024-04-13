void virtio_blk_exit(VirtIODevice *vdev)
{
    VirtIOBlock *s = to_virtio_blk(vdev);
    unregister_savevm(s->qdev, "virtio-blk", s);
}