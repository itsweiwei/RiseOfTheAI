#include "Entity.h"

Entity::Entity()
{
    entityType = PLATFORM;
    isStatic = true;
	isActive = true;
    position = glm::vec3(0);
    speed = 0;
    width = 1;
    height = 1;
}

bool Entity::CheckCollision(Entity other)
{
    if (isStatic == true) return false;
    if (isActive == false || other.isActive == false) return false;
    
    float xdist = fabs(position.x - other.position.x) - ((width + other.width) / 2.0f);
    float ydist = fabs(position.y - other.position.y) - ((height + other.height) / 2.0f);

    if (xdist < 0 && ydist < 0)
    {
        if (entityType == PLAYER && other.entityType == ENEMY)
        {
			other.aiState = DEAD;
            other.isActive = false;
        }
        
        return true;
    }
    
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity object = objects[i];
        
        if (CheckCollision(object)){
            float ydist = fabs(position.y - object.position.y);
            float penetrationY = fabs(ydist - (height / 2) - (object.height / 2));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
			if (entityType == PLAYER && object.entityType == ENEMY && collidedBottom) {
				object.isActive = false;
			}
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity object = objects[i];
        
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object.position.x);
            float penetrationX = fabs(xdist - (width / 2) - (object.width / 2));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
			if (entityType == PLAYER && object.entityType == ENEMY) {
				isActive = false;
			}
			if (entityType == ENEMY && aiType == PATROL && object.entityType == PLATFORM) {
				velocity.x *= -1.0f;
			}
        }
    }
}

void Entity::AI_Normal(Entity player) {
	switch (aiState) { 
	case IDLE:
		if (glm::distance(position, player.position) < 3.0f) {
			aiState = WALKING;
		}
		break;

	case WALKING:
		if (player.position.x > position.x) {
			velocity.x = 1.0f;
		}
		else {
			velocity.x = -1.0f;
		}
		if (glm::distance(position, player.position) > 3.0f) {
			aiState = IDLE;
			velocity.x = 0;
		}
		break;
	}
}

void Entity::AI_Patrol(Entity player) {
	velocity.x = 1.0f;
	if (collidedLeft) {
		velocity.x = 1.0f;
	}
	if (collidedRight) {
		velocity.x = -1.0f;
	}
}

void Entity::AI_Flying(Entity player) {
	if (player.position.x > position.x) {
		acceleration.x = 0.05f;
	}
	else {
		acceleration.x = -0.05f;
	}
	if (player.position.y > position.y) {
		acceleration.y = 0.05f;
	}
	else {
		acceleration.y = -0.05f;
	}
}


void Entity::Jump()
{
    if (collidedBottom)
    {
        velocity.y = 5.0f;
    }
}


void Entity::Update(float deltaTime, Entity player, Entity *objects, int objectCount, Entity* enemies, int enemyCount)
{
	collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

	if (entityType == ENEMY) {
		if (aiType == WALKER) {
			AI_Normal(player);
		}
		if (aiType == PATROL) {
			AI_Patrol(player);
		}
		if (aiType == FLYER) {
			AI_Flying(player);
		}
	}
    
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;        // Move on Y
    CheckCollisionsY(objects, objectCount);    // Fix if needed

	if (entityType == PLAYER) {
		CheckCollisionsY(enemies, enemyCount);
	}
    
    position.x += velocity.x * deltaTime;        // Move on X
    CheckCollisionsX(objects, objectCount);    // Fix if needed

	if (entityType == PLAYER) {
		CheckCollisionsX(enemies, enemyCount);
	}
}



void Entity::Render(ShaderProgram *program) {
	if (isActive) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
		program->SetModelMatrix(modelMatrix);

		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glBindTexture(GL_TEXTURE_2D, textureID);

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
}

