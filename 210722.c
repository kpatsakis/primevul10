SoftwareFrame::SoftwareFrame(
    base::WeakPtr<SoftwareFrameManagerClient> frame_manager_client,
    uint32 output_surface_id,
    unsigned frame_id,
    float frame_device_scale_factor,
    gfx::Size frame_size_pixels,
    scoped_ptr<base::SharedMemory> shared_memory)
    : frame_manager_client_(frame_manager_client),
      output_surface_id_(output_surface_id),
      frame_id_(frame_id),
      frame_device_scale_factor_(frame_device_scale_factor),
      frame_size_pixels_(frame_size_pixels),
      shared_memory_(shared_memory.Pass()) {}
