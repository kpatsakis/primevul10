void Commissioner::SendJoinFinalizeResponse(const Coap::Message &aRequest, StateTlv::State aState)
{
    otError           error = OT_ERROR_NONE;
    Ip6::MessageInfo  joinerMessageInfo;
    MeshCoP::StateTlv stateTlv;
    Coap::Message *   message;
    Mac::ExtAddress   joinerId;

    VerifyOrExit((message = NewMeshCoPMessage(Get<Coap::CoapSecure>())) != NULL, error = OT_ERROR_NO_BUFS);

    SuccessOrExit(error = message->SetDefaultResponseHeader(aRequest));
    SuccessOrExit(error = message->SetPayloadMarker());
    message->SetOffset(message->GetLength());
    message->SetSubType(Message::kSubTypeJoinerFinalizeResponse);

    stateTlv.Init();
    stateTlv.SetState(aState);
    SuccessOrExit(error = message->AppendTlv(stateTlv));

    joinerMessageInfo.SetPeerAddr(Get<Mle::MleRouter>().GetMeshLocal64());
    joinerMessageInfo.GetPeerAddr().SetIid(mJoinerIid);
    joinerMessageInfo.SetPeerPort(mJoinerPort);

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    uint8_t buf[OPENTHREAD_CONFIG_MESSAGE_BUFFER_SIZE];

    VerifyOrExit(message->GetLength() <= sizeof(buf));
    message->Read(message->GetOffset(), message->GetLength() - message->GetOffset(), buf);
    otDumpCertMeshCoP("[THCI] direction=send | type=JOIN_FIN.rsp |", buf, message->GetLength() - message->GetOffset());
#endif

    SuccessOrExit(error = Get<Coap::CoapSecure>().SendMessage(*message, joinerMessageInfo));

    joinerId.Set(mJoinerIid);
    joinerId.ToggleLocal();
    SignalJoinerEvent(OT_COMMISSIONER_JOINER_FINALIZE, joinerId);

    if (!mJoiners[mJoinerIndex].mAny)
    {
        // remove after kRemoveJoinerDelay (seconds)
        RemoveJoiner(&mJoiners[mJoinerIndex].mEui64, kRemoveJoinerDelay);
    }

    otLogInfoMeshCoP("sent joiner finalize response");

exit:

    if (error != OT_ERROR_NONE && message != NULL)
    {
        message->Free();
    }
}