static int vidioc_enum_input(struct file *file, void *priv,
				struct v4l2_input *vi)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int chan;

	if (vi->index >= usbvision->video_inputs)
		return -EINVAL;
	if (usbvision->have_tuner)
		chan = vi->index;
	else
		chan = vi->index + 1; /* skip Television string*/

	/* Determine the requested input characteristics
	   specific for each usbvision card model */
	switch (chan) {
	case 0:
		if (usbvision_device_data[usbvision->dev_model].video_channels == 4) {
			strcpy(vi->name, "White Video Input");
		} else {
			strcpy(vi->name, "Television");
			vi->type = V4L2_INPUT_TYPE_TUNER;
			vi->tuner = chan;
			vi->std = USBVISION_NORMS;
		}
		break;
	case 1:
		vi->type = V4L2_INPUT_TYPE_CAMERA;
		if (usbvision_device_data[usbvision->dev_model].video_channels == 4)
			strcpy(vi->name, "Green Video Input");
		else
			strcpy(vi->name, "Composite Video Input");
		vi->std = USBVISION_NORMS;
		break;
	case 2:
		vi->type = V4L2_INPUT_TYPE_CAMERA;
		if (usbvision_device_data[usbvision->dev_model].video_channels == 4)
			strcpy(vi->name, "Yellow Video Input");
		else
			strcpy(vi->name, "S-Video Input");
		vi->std = USBVISION_NORMS;
		break;
	case 3:
		vi->type = V4L2_INPUT_TYPE_CAMERA;
		strcpy(vi->name, "Red Video Input");
		vi->std = USBVISION_NORMS;
		break;
	}
	return 0;
}