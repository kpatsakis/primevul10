static void atusb_free_urbs(struct atusb *atusb)
{
	struct urb *urb;

	while (1) {
		urb = usb_get_from_anchor(&atusb->idle_urbs);
		if (!urb)
			break;
		kfree_skb(urb->context);
		usb_free_urb(urb);
	}
}