/******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "PlayerObject.h"

PlayerObject::PlayerObject()
	: GameObject(), Stuck(true) { }

PlayerObject::PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity)
	: GameObject(pos,                   //posicao
		size,							//size 58 100 plane: 140 45
		sprite,                         //sprite
		glm::vec3(1.0f),                //color
		velocity),						//velocity
	Stuck(true) { }

void PlayerObject::Move(PlayerObject* bullet) {
	if (!bullet->Stuck) {
		bullet->Position.x += bullet->Velocity.x;
	}
}

// Resets the ball to initial Stuck Position (if ball is outside window bounds)
void PlayerObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->Stuck = true;
}