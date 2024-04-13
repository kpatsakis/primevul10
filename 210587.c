SoftwareFrame::~SoftwareFrame() {
  if (frame_manager_client_) {
    frame_manager_client_->SoftwareFrameWasFreed(
        output_surface_id_, frame_id_);
  }
}
