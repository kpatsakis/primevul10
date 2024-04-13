service_info *getServiceList(IXML_Node *node, service_info **end, char *URLBase)
{
	IXML_Node *serviceList = NULL;
	IXML_Node *current_service = NULL;
	IXML_Node *UDN = NULL;

	IXML_Node *serviceType = NULL;
	IXML_Node *serviceId = NULL;
	IXML_Node *SCPDURL = NULL;
	IXML_Node *controlURL = NULL;
	IXML_Node *eventURL = NULL;
	DOMString tempDOMString = NULL;
	service_info *head = NULL;
	service_info *current = NULL;
	service_info *previous = NULL;
	IXML_NodeList *serviceNodeList = NULL;
	long unsigned int NumOfServices = 0lu;
	long unsigned int i = 0lu;
	int fail = 0;

	if (getSubElement("UDN", node, &UDN) &&
		getSubElement("serviceList", node, &serviceList)) {
		serviceNodeList = ixmlElement_getElementsByTagName(
			(IXML_Element *)serviceList, "service");
		if (serviceNodeList) {
			NumOfServices = ixmlNodeList_length(serviceNodeList);
			for (i = 0lu; i < NumOfServices; i++) {
				current_service =
					ixmlNodeList_item(serviceNodeList, i);
				fail = 0;
				if (current) {
					current->next =
						malloc(sizeof(service_info));
					previous = current;
					current = current->next;
				} else {
					head = malloc(sizeof(service_info));
					current = head;
				}
				if (!current) {
					freeServiceList(head);
					ixmlNodeList_free(serviceNodeList);
					return NULL;
				}
				current->next = NULL;
				current->controlURL = NULL;
				current->eventURL = NULL;
				current->serviceType = NULL;
				current->serviceId = NULL;
				current->SCPDURL = NULL;
				current->active = 1;
				current->subscriptionList = NULL;
				current->TotalSubscriptions = 0;
				if (!(current->UDN = getElementValue(UDN)))
					fail = 1;
				if (!getSubElement("serviceType",
					    current_service,
					    &serviceType) ||
					!(current->serviceType =
							getElementValue(
								serviceType)))
					fail = 1;
				if (!getSubElement("serviceId",
					    current_service,
					    &serviceId) ||
					!(current->serviceId = getElementValue(
						  serviceId)))
					fail = 1;
				if (!getSubElement("SCPDURL",
					    current_service,
					    &SCPDURL) ||
					!(tempDOMString = getElementValue(
						  SCPDURL)) ||
					!(current->SCPDURL = resolve_rel_url(
						  URLBase, tempDOMString)))
					fail = 1;
				ixmlFreeDOMString(tempDOMString);
				tempDOMString = NULL;
				if (!(getSubElement("controlURL",
					    current_service,
					    &controlURL)) ||
					!(tempDOMString = getElementValue(
						  controlURL)) ||
					!(current->controlURL = resolve_rel_url(
						  URLBase, tempDOMString))) {
					UpnpPrintf(UPNP_INFO,
						GENA,
						__FILE__,
						__LINE__,
						"BAD OR MISSING CONTROL URL");
					UpnpPrintf(UPNP_INFO,
						GENA,
						__FILE__,
						__LINE__,
						"CONTROL URL SET TO NULL IN "
						"SERVICE INFO");
					current->controlURL = NULL;
					fail = 0;
				}
				ixmlFreeDOMString(tempDOMString);
				tempDOMString = NULL;
				if (!getSubElement("eventSubURL",
					    current_service,
					    &eventURL) ||
					!(tempDOMString = getElementValue(
						  eventURL)) ||
					!(current->eventURL = resolve_rel_url(
						  URLBase, tempDOMString))) {
					UpnpPrintf(UPNP_INFO,
						GENA,
						__FILE__,
						__LINE__,
						"BAD OR MISSING EVENT URL");
					UpnpPrintf(UPNP_INFO,
						GENA,
						__FILE__,
						__LINE__,
						"EVENT URL SET TO NULL IN "
						"SERVICE INFO");
					current->eventURL = NULL;
					fail = 0;
				}
				ixmlFreeDOMString(tempDOMString);
				tempDOMString = NULL;
				if (fail) {
					freeServiceList(current);
					if (previous)
						previous->next = NULL;
					else
						head = NULL;
					current = previous;
				}
			}
			ixmlNodeList_free(serviceNodeList);
		}
		(*end) = current;
		return head;
	} else {
		(*end) = NULL;
		return NULL;
	}
}