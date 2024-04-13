void Commissioner::HandleLeaderKeepAliveResponse(Coap::Message *         aMessage,
                                                 const Ip6::MessageInfo *aMessageInfo,
                                                 otError                 aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    StateTlv state;

    VerifyOrExit(mState == OT_COMMISSIONER_STATE_ACTIVE, SetState(OT_COMMISSIONER_STATE_DISABLED));
    VerifyOrExit(aResult == OT_ERROR_NONE && aMessage->GetCode() == OT_COAP_CODE_CHANGED,
                 SetState(OT_COMMISSIONER_STATE_DISABLED));

    otLogInfoMeshCoP("received Leader keep-alive response");

    SuccessOrExit(Tlv::GetTlv(*aMessage, Tlv::kState, sizeof(state), state));
    VerifyOrExit(state.IsValid());

    VerifyOrExit(state.GetState() == StateTlv::kAccept, SetState(OT_COMMISSIONER_STATE_DISABLED));

    mTimer.Start(Time::SecToMsec(kKeepAliveTimeout) / 2);

exit:

    if (mState != OT_COMMISSIONER_STATE_ACTIVE)
    {
        Get<ThreadNetif>().RemoveUnicastAddress(mCommissionerAloc);
        RemoveCoapResources();
    }
}