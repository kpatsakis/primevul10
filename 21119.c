static int atusb_get_and_clear_error(struct atusb *atusb)
{
	int err = atusb->err;

	atusb->err = 0;
	return err;
}