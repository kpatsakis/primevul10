int removeServiceTable(IXML_Node *node, service_table *in)
{
	IXML_Node *root = NULL;
	IXML_Node *currentUDN = NULL;
	DOMString UDN = NULL;
	IXML_NodeList *deviceList = NULL;
	service_info *current_service = NULL;
	service_info *start_search = NULL;
	service_info *prev_service = NULL;
	long unsigned int NumOfDevices = 0lu;
	long unsigned int i = 0lu;

	if (getSubElement("root", node, &root)) {
		start_search = in->serviceList;
		deviceList = ixmlElement_getElementsByTagName(
			(IXML_Element *)root, "device");
		if (deviceList) {
			NumOfDevices = ixmlNodeList_length(deviceList);
			for (i = 0lu; i < NumOfDevices; i++) {
				if ((start_search) &&
					((getSubElement(
						 "UDN", node, &currentUDN)) &&
						(UDN = getElementValue(
							 currentUDN)))) {
					current_service = start_search;
					/* Services are put in the service table
					 * in the order in which they appear in
					 * the description document, therefore
					 * we go through the list only once to
					 * remove a particular root device */
					while ((current_service) &&
						(strcmp(current_service->UDN,
							UDN))) {
						current_service =
							current_service->next;
						if (current_service != NULL)
							prev_service =
								current_service
									->next;
					}
					while ((current_service) &&
						(!strcmp(current_service->UDN,
							UDN))) {
						if (prev_service) {
							prev_service->next =
								current_service
									->next;
						} else {
							in->serviceList =
								current_service
									->next;
						}
						if (current_service ==
							in->endServiceList)
							in->endServiceList =
								prev_service;
						start_search =
							current_service->next;
						freeService(current_service);
						current_service = start_search;
					}
					ixmlFreeDOMString(UDN);
					UDN = NULL;
				}
			}

			ixmlNodeList_free(deviceList);
		}
	}
	return 1;
}