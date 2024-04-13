  void ClearNetworks() {
    if (ethernet_)
      delete ethernet_;
    ethernet_ = NULL;
    wifi_ = NULL;
    cellular_ = NULL;
    STLDeleteElements(&wifi_networks_);
    wifi_networks_.clear();
    STLDeleteElements(&cellular_networks_);
    cellular_networks_.clear();
    STLDeleteElements(&remembered_wifi_networks_);
    remembered_wifi_networks_.clear();
  }
