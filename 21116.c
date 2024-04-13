static void atusb_work_urbs(struct work_struct *work)
{
	struct atusb *atusb =
	    container_of(to_delayed_work(work), struct atusb, work);
	struct usb_device *usb_dev = atusb->usb_dev;
	struct urb *urb;
	int ret;

	if (atusb->shutdown)
		return;

	do {
		urb = usb_get_from_anchor(&atusb->idle_urbs);
		if (!urb)
			return;
		ret = atusb_submit_rx_urb(atusb, urb);
	} while (!ret);

	usb_anchor_urb(urb, &atusb->idle_urbs);
	dev_warn_ratelimited(&usb_dev->dev,
			     "atusb_in: can't allocate/submit URB (%d)\n", ret);
	schedule_delayed_work(&atusb->work,
			      msecs_to_jiffies(ATUSB_ALLOC_DELAY_MS) + 1);
}