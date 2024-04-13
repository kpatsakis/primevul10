hulusb_set_lbt(struct ieee802154_hw *hw, bool on)
{
	struct atusb *atusb = hw->priv;

	return atusb_write_subreg(atusb, SR_CSMA_LBT_MODE, on);
}