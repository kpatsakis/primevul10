hulusb_set_txpower(struct ieee802154_hw *hw, s32 mbm)
{
	u32 i;

	for (i = 0; i < hw->phy->supported.tx_powers_size; i++) {
		if (hw->phy->supported.tx_powers[i] == mbm)
			return atusb_write_subreg(hw->priv, SR_TX_PWR_212, i);
	}

	return -EINVAL;
}