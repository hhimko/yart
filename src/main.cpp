#include "application.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//  YART Entry Point
////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    auto app = yart::Application(1280, 720);
    return app.Run();
}
