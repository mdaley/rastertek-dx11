#include "systemclass.h"
#include <memory>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    std::unique_ptr<SystemClass> System(new SystemClass());

    try
    {

        System->Initialize();
        
        System->Run();
    }
    catch (engine_exception e)
    {
        stringstream oss;
        oss << "Caught engine_exception: " << e.what();
        OutputDebugStringA(oss.str().c_str());
    }

    return 0;
}