VideoRendererBase::VideoRendererBase(const base::Closure& paint_cb,
                                     const SetOpaqueCB& set_opaque_cb,
                                     bool drop_frames)
    : frame_available_(&lock_),
      state_(kUninitialized),
      thread_(base::kNullThreadHandle),
      pending_read_(false),
      pending_paint_(false),
      pending_paint_with_last_available_(false),
      drop_frames_(drop_frames),
      playback_rate_(0),
      paint_cb_(paint_cb),
      set_opaque_cb_(set_opaque_cb) {
  DCHECK(!paint_cb_.is_null());
}
