#pragma once
#include <stdbool.h>
#include "player/player.h"
#include "object.h"

void playerphysicsUpdate(Player* player, const Object* objects, const double deltaTime);
