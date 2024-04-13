void Commissioner::HandleCoapsConnected(bool aConnected)
{
    otCommissionerJoinerEvent event;
    Mac::ExtAddress           joinerId;

    event = aConnected ? OT_COMMISSIONER_JOINER_CONNECTED : OT_COMMISSIONER_JOINER_END;

    joinerId.Set(mJoinerIid);
    joinerId.ToggleLocal();

    SignalJoinerEvent(event, joinerId);
}