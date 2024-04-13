atusb_set_cca_ed_level(struct ieee802154_hw *hw, s32 mbm)
{
	struct atusb *atusb = hw->priv;
	u32 i;

	for (i = 0; i < hw->phy->supported.cca_ed_levels_size; i++) {
		if (hw->phy->supported.cca_ed_levels[i] == mbm)
			return atusb_write_subreg(atusb, SR_CCA_ED_THRES, i);
	}

	return -EINVAL;
}