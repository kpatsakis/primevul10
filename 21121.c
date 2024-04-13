atusb_txpower(struct ieee802154_hw *hw, s32 mbm)
{
	struct atusb *atusb = hw->priv;

	if (atusb->data)
		return atusb->data->set_txpower(hw, mbm);
	else
		return -ENOTSUPP;
}