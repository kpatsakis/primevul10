otError Commissioner::Stop(void)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mState != OT_COMMISSIONER_STATE_DISABLED, error = OT_ERROR_INVALID_STATE);

    Get<Coap::CoapSecure>().Stop();

    Get<ThreadNetif>().RemoveUnicastAddress(mCommissionerAloc);
    RemoveCoapResources();
    ClearJoiners();
    mTransmitAttempts = 0;

    mTimer.Stop();
    Get<Coap::CoapSecure>().Stop();

    SetState(OT_COMMISSIONER_STATE_DISABLED);

    SendKeepAlive();

exit:
    return error;
}