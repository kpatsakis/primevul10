void Commissioner::SignalJoinerEvent(otCommissionerJoinerEvent aEvent, const Mac::ExtAddress &aJoinerId)
{
    if (mJoinerCallback)
    {
        mJoinerCallback(aEvent, &aJoinerId, mCallbackContext);
    }
}