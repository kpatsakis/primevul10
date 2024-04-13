void WebGraphicsContext3DDefaultImpl::deleteProgram(unsigned program)
{
    makeContextCurrent();
    glDeleteProgram(program);
}
