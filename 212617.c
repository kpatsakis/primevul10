void LanLinkProvider::incomingPairPacket(DeviceLink* deviceLink, const NetworkPacket& np)
{
    LanPairingHandler* ph = createPairingHandler(deviceLink);
    ph->packetReceived(np);
}