static void atusb_tx_done(struct atusb *atusb, u8 seq)
{
	struct usb_device *usb_dev = atusb->usb_dev;
	u8 expect = atusb->tx_ack_seq;

	dev_dbg(&usb_dev->dev, "%s (0x%02x/0x%02x)\n", __func__, seq, expect);
	if (seq == expect) {
		/* TODO check for ifs handling in firmware */
		ieee802154_xmit_complete(atusb->hw, atusb->tx_skb, false);
	} else {
		/* TODO I experience this case when atusb has a tx complete
		 * irq before probing, we should fix the firmware it's an
		 * unlikely case now that seq == expect is then true, but can
		 * happen and fail with a tx_skb = NULL;
		 */
		ieee802154_wake_queue(atusb->hw);
		if (atusb->tx_skb)
			dev_kfree_skb_irq(atusb->tx_skb);
	}
}