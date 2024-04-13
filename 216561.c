void Commissioner::SetState(otCommissionerState aState)
{
    VerifyOrExit(mState != aState);

    otLogInfoMeshCoP("Commissioner State: %s -> %s", StateToString(mState), StateToString(aState));

    mState = aState;

    if (mStateCallback)
    {
        mStateCallback(mState, mCallbackContext);
    }

exit:
    return;
}