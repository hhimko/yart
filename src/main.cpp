#include "application.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//  YART Entry Point
////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    auto& app = yart::Application::Get();
    return app.Run();
}