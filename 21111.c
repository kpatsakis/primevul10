atusb_set_frame_retries(struct ieee802154_hw *hw, s8 retries)
{
	struct atusb *atusb = hw->priv;

	return atusb_write_subreg(atusb, SR_MAX_FRAME_RETRIES, retries);
}