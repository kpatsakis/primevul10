static void powercap_release(struct device *dev)
{
	bool allocated;

	if (dev->parent) {
		struct powercap_zone *power_zone = to_powercap_zone(dev);

		/* Store flag as the release() may free memory */
		allocated = power_zone->allocated;
		/* Remove id from parent idr struct */
		idr_remove(power_zone->parent_idr, power_zone->id);
		/* Destroy idrs allocated for this zone */
		idr_destroy(&power_zone->idr);
		kfree(power_zone->name);
		kfree(power_zone->zone_dev_attrs);
		kfree(power_zone->constraints);
		if (power_zone->ops->release)
			power_zone->ops->release(power_zone);
		if (allocated)
			kfree(power_zone);
	} else {
		struct powercap_control_type *control_type =
						to_powercap_control_type(dev);

		/* Store flag as the release() may free memory */
		allocated = control_type->allocated;
		idr_destroy(&control_type->idr);
		mutex_destroy(&control_type->lock);
		if (control_type->ops && control_type->ops->release)
			control_type->ops->release(control_type);
		if (allocated)
			kfree(control_type);
	}
}