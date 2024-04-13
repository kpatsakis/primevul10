void Commissioner::HandleRelayReceive(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    otError                error;
    JoinerUdpPortTlv       joinerPort;
    JoinerIidTlv           joinerIid;
    JoinerRouterLocatorTlv joinerRloc;
    Ip6::MessageInfo       joinerMessageInfo;
    uint16_t               offset;
    uint16_t               length;
    bool                   enableJoiner = false;
    Mac::ExtAddress        receivedId;
    Mac::ExtAddress        joinerId;

    VerifyOrExit(mState == OT_COMMISSIONER_STATE_ACTIVE, error = OT_ERROR_INVALID_STATE);

    VerifyOrExit(aMessage.GetType() == OT_COAP_TYPE_NON_CONFIRMABLE && aMessage.GetCode() == OT_COAP_CODE_POST);

    SuccessOrExit(error = Tlv::GetTlv(aMessage, Tlv::kJoinerUdpPort, sizeof(joinerPort), joinerPort));
    VerifyOrExit(joinerPort.IsValid(), error = OT_ERROR_PARSE);

    SuccessOrExit(error = Tlv::GetTlv(aMessage, Tlv::kJoinerIid, sizeof(joinerIid), joinerIid));
    VerifyOrExit(joinerIid.IsValid(), error = OT_ERROR_PARSE);

    SuccessOrExit(error = Tlv::GetTlv(aMessage, Tlv::kJoinerRouterLocator, sizeof(joinerRloc), joinerRloc));
    VerifyOrExit(joinerRloc.IsValid(), error = OT_ERROR_PARSE);

    SuccessOrExit(error = Tlv::GetValueOffset(aMessage, Tlv::kJoinerDtlsEncapsulation, offset, length));
    VerifyOrExit(length <= aMessage.GetLength() - offset, error = OT_ERROR_PARSE);

    if (!Get<Coap::CoapSecure>().IsConnectionActive())
    {
        memcpy(mJoinerIid, joinerIid.GetIid(), sizeof(mJoinerIid));

        receivedId.Set(mJoinerIid);
        receivedId.ToggleLocal();

        for (Joiner *joiner = &mJoiners[0]; joiner < OT_ARRAY_END(mJoiners); joiner++)
        {
            if (!joiner->mValid)
            {
                continue;
            }

            ComputeJoinerId(joiner->mEui64, joinerId);

            if (joiner->mAny || (joinerId == receivedId))
            {
                error = Get<Coap::CoapSecure>().SetPsk(reinterpret_cast<const uint8_t *>(joiner->mPsk),
                                                       static_cast<uint8_t>(strlen(joiner->mPsk)));
                SuccessOrExit(error);
                mJoinerIndex = static_cast<uint8_t>(joiner - mJoiners);
                enableJoiner = true;

                otLogInfoMeshCoP("found joiner, starting new session");
                SignalJoinerEvent(OT_COMMISSIONER_JOINER_START, joinerId);

                break;
            }
        }
    }
    else
    {
        enableJoiner = (memcmp(mJoinerIid, joinerIid.GetIid(), sizeof(mJoinerIid)) == 0);
    }

    VerifyOrExit(enableJoiner);

    mJoinerPort = joinerPort.GetUdpPort();
    mJoinerRloc = joinerRloc.GetJoinerRouterLocator();

    otLogInfoMeshCoP("Remove Relay Receive (%02x%02x%02x%02x%02x%02x%02x%02x, 0x%04x)", mJoinerIid[0], mJoinerIid[1],
                     mJoinerIid[2], mJoinerIid[3], mJoinerIid[4], mJoinerIid[5], mJoinerIid[6], mJoinerIid[7],
                     mJoinerRloc);

    aMessage.SetOffset(offset);
    SuccessOrExit(error = aMessage.SetLength(offset + length));

    joinerMessageInfo.SetPeerAddr(Get<Mle::MleRouter>().GetMeshLocal64());
    joinerMessageInfo.GetPeerAddr().SetIid(mJoinerIid);
    joinerMessageInfo.SetPeerPort(mJoinerPort);

    Get<Coap::CoapSecure>().HandleUdpReceive(aMessage, joinerMessageInfo);

exit:
    return;
}