static void atusb_xmit_complete(struct urb *urb)
{
	dev_dbg(&urb->dev->dev, "atusb_xmit urb completed");
}