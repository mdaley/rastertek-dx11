#pragma once

#include <stdexcept>
#include <sstream>

using namespace std;

class engine_exception : public runtime_error
{
public:
    engine_exception(const engine_exception& from);
    explicit engine_exception(const std::string& message);
    explicit engine_exception(const char* message);
    const char* what() const override;

    template<typename T>
    engine_exception& operator<< (const T& t)
    {
        stringstream ss;
        ss << t;
        m_Message += ss.str();
        return *this;
    }

private:
    string m_Message;
};
