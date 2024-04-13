void printServiceList(
	service_info *service, Upnp_LogLevel level, Dbg_Module module)
{
	while (service) {
		if (service->serviceType) {
			UpnpPrintf(level,
				module,
				__FILE__,
				__LINE__,
				"serviceType: %s\n",
				service->serviceType);
		}
		if (service->serviceId) {
			UpnpPrintf(level,
				module,
				__FILE__,
				__LINE__,
				"serviceId: %s\n",
				service->serviceId);
		}
		if (service->SCPDURL) {
			UpnpPrintf(level,
				module,
				__FILE__,
				__LINE__,
				"SCPDURL: %s\n",
				service->SCPDURL);
		}
		if (service->controlURL) {
			UpnpPrintf(level,
				module,
				__FILE__,
				__LINE__,
				"controlURL: %s\n",
				service->controlURL);
		}
		if (service->eventURL) {
			UpnpPrintf(level,
				module,
				__FILE__,
				__LINE__,
				"eventURL: %s\n",
				service->eventURL);
		}
		if (service->UDN) {
			UpnpPrintf(level,
				module,
				__FILE__,
				__LINE__,
				"UDN: %s\n\n",
				service->UDN);
		}
		if (service->active) {
			UpnpPrintf(level,
				module,
				__FILE__,
				__LINE__,
				"Service is active\n");
		} else {
			UpnpPrintf(level,
				module,
				__FILE__,
				__LINE__,
				"Service is inactive\n");
		}
		service = service->next;
	}
}