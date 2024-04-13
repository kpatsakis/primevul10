void update_register_client_callbacks(rdpUpdate* update)
{
	update->RefreshRect = update_send_refresh_rect;
	update->SuppressOutput = update_send_suppress_output;
	update->SurfaceFrameAcknowledge = update_send_frame_acknowledge;
}