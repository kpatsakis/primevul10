bool WebGraphicsContext3DDefaultImpl::angleValidateShaderSource(ShaderSourceEntry& entry)
{
    entry.isValid = false;
    if (entry.translatedSource) {
        fastFree(entry.translatedSource);
        entry.translatedSource = 0;
    }
    if (entry.log) {
        fastFree(entry.log);
        entry.log = 0;
    }

    ShHandle compiler = 0;
    switch (entry.type) {
    case GL_FRAGMENT_SHADER:
        compiler = m_fragmentCompiler;
        break;
    case GL_VERTEX_SHADER:
        compiler = m_vertexCompiler;
        break;
    }
    if (!compiler)
        return false;

    if (!ShCompile(compiler, &entry.source, 1, SH_OBJECT_CODE)) {
        int logSize = 0;
        ShGetInfo(compiler, SH_INFO_LOG_LENGTH, &logSize);
        if (logSize > 1 && tryFastMalloc(logSize * sizeof(char)).getValue(entry.log))
            ShGetInfoLog(compiler, entry.log);
        return false;
    }

    int length = 0;
    ShGetInfo(compiler, SH_OBJECT_CODE_LENGTH, &length);
    if (length > 1) {
        if (!tryFastMalloc(length * sizeof(char)).getValue(entry.translatedSource))
            return false;
        ShGetObjectCode(compiler, entry.translatedSource);
    }
    entry.isValid = true;
    return true;
}
