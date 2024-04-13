PeriodicWave* AudioContext::createPeriodicWave(DOMFloat32Array* real, DOMFloat32Array* imag, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (!real) {
        exceptionState.throwDOMException(
            SyntaxError,
            "invalid real array");
        return 0;
    }

    if (!imag) {
        exceptionState.throwDOMException(
            SyntaxError,
            "invalid imaginary array");
        return 0;
    }

    if (real->length() != imag->length()) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "length of real array (" + String::number(real->length())
            + ") and length of imaginary array (" +  String::number(imag->length())
            + ") must match.");
        return 0;
    }

    if (real->length() > 4096) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "length of real array (" + String::number(real->length())
            + ") exceeds allowed maximum of 4096");
        return 0;
    }

    if (imag->length() > 4096) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "length of imaginary array (" + String::number(imag->length())
            + ") exceeds allowed maximum of 4096");
        return 0;
    }

    return PeriodicWave::create(sampleRate(), real, imag);
}
