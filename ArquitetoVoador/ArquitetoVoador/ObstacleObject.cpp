/******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "ObstacleObject.h"

bool up = true;

ObstacleObject::ObstacleObject()
	: GameObject(), Stuck(true) { }

ObstacleObject::ObstacleObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite)
	: GameObject(pos,                 //posicao
		size,                         //size
		sprite,                       //sprite
		glm::vec3(1.0f),              //color
		glm::vec2(4.0f, 0.0f)),       //velocity
	Stuck(true), firsPos(pos) { }

void ObstacleObject::Move(ObstacleObject *obstacle, bool isBird)
{
	if (!obstacle->Stuck) {
		obstacle->Position.x -= obstacle->Velocity.x;

		if (obstacle->Position.x <= -1280.0f) {
			obstacle->Position.x = 1280.0f;
			obstacle->Destroyed = false;
		}
		if (isBird) {
			int yUp = obstacle->firsPos.y - 100.0f;
			int yDown = obstacle->firsPos.y + 100.0f;
			if (up) {
				if (obstacle->Position.y > yUp) {
					obstacle->Position.y -= 2.0f;
				}
				else
				{
					up = false;
				}
			}
			else
			{
				
				if (obstacle->Position.y <= yDown) {
					obstacle->Position.y += 2.0f;
				}
				else
				{
					up = true;
				}
			}
		}
	}
}

// Resets the ball to initial Stuck Position (if ball is outside window bounds)
void ObstacleObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->Stuck = true;
}