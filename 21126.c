atusb_set_csma_params(struct ieee802154_hw *hw, u8 min_be, u8 max_be, u8 retries)
{
	struct atusb *atusb = hw->priv;
	int ret;

	ret = atusb_write_subreg(atusb, SR_MIN_BE, min_be);
	if (ret)
		return ret;

	ret = atusb_write_subreg(atusb, SR_MAX_BE, max_be);
	if (ret)
		return ret;

	return atusb_write_subreg(atusb, SR_MAX_CSMA_RETRIES, retries);
}