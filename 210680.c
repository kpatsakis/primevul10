void MediaElementAudioSourceNode::setFormat(size_t numberOfChannels, float sourceSampleRate)
{
    if (numberOfChannels != m_sourceNumberOfChannels || sourceSampleRate != m_sourceSampleRate) {
        if (!numberOfChannels || numberOfChannels > AudioContext::maxNumberOfChannels() || !AudioUtilities::isValidAudioBufferSampleRate(sourceSampleRate)) {
            WTF_LOG(Media, "MediaElementAudioSourceNode::setFormat(%u, %f) - unhandled format change", static_cast<unsigned>(numberOfChannels), sourceSampleRate);
            m_sourceNumberOfChannels = 0;
            m_sourceSampleRate = 0;
            return;
        }

        m_sourceNumberOfChannels = numberOfChannels;
        m_sourceSampleRate = sourceSampleRate;

        Locker<MediaElementAudioSourceNode> locker(*this);

        if (sourceSampleRate != sampleRate()) {
            double scaleFactor = sourceSampleRate / sampleRate();
            m_multiChannelResampler = adoptPtr(new MultiChannelResampler(scaleFactor, numberOfChannels));
        } else {
            m_multiChannelResampler.clear();
        }

        {
            AudioContext::AutoLocker contextLocker(context());

            output(0)->setNumberOfChannels(numberOfChannels);
        }
    }
}
