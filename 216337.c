service_info *getAllServiceList(
	IXML_Node *node, char *URLBase, service_info **out_end)
{
	service_info *head = NULL;
	service_info *end = NULL;
	service_info *next_end = NULL;
	IXML_NodeList *deviceList = NULL;
	IXML_Node *currentDevice = NULL;

	long unsigned int NumOfDevices = 0lu;
	long unsigned int i = 0lu;

	(*out_end) = NULL;
	deviceList = ixmlElement_getElementsByTagName(
		(IXML_Element *)node, "device");
	if (deviceList) {
		NumOfDevices = ixmlNodeList_length(deviceList);
		for (i = 0lu; i < NumOfDevices; i++) {
			currentDevice = ixmlNodeList_item(deviceList, i);
			if (head) {
				end->next = getServiceList(
					currentDevice, &next_end, URLBase);
				if (next_end)
					end = next_end;
			} else {
				head = getServiceList(
					currentDevice, &end, URLBase);
			}
		}
		ixmlNodeList_free(deviceList);
	}

	(*out_end) = end;
	return head;
}