//
// Created by stachelbeere1248 on 1/28/24.
//

#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "game.h"
#include "arena.h"
#include "node.h"

void map_init(Level *level, u32 stage, Arena *arena, Node *scene_root);

Sprite GetSpriteFromGridPos(Level *level, u32 y, u32 x, u8 type);

#endif //GAME_MAP_H
