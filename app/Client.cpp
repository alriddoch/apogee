#include "Client.h"

#include <visual/Renderer.h>

#include <iostream>

int Client::update()
{
    renderer->clear();
    //renderer->drawCharacter();
    renderer->draw2Dtest();
    renderer->draw3Dtest();
    renderer->flip();
    return 0;
}

bool Client::event(SDL_Event & event)
{
    return false;
}
