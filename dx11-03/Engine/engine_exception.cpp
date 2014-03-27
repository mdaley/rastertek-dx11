#include "engine_exception.h"

engine_exception::engine_exception(const engine_exception& from) : runtime_error("")
{
    this->m_Message += from.m_Message;
}

engine_exception::engine_exception(const string& message) : runtime_error("")
{
    this->m_Message += message;
}

engine_exception::engine_exception(const char* message) : runtime_error("")
{
    this->m_Message += string(message);
}

const char* engine_exception::what() const
{
    return m_Message.c_str();
}