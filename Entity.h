#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum EntityType { PLAYER, PLATFORM, COIN, ENEMY };

enum AIState { IDLE, WALKING, DEAD };

enum AIType{ WALKER, JUMPER, FLYER, PATROL };

class Entity {
public:
    
    EntityType entityType;
	AIState aiState;
	AIType aiType;

    bool isStatic;
    bool isActive;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float width;
    float height;
    
    float speed;
    
    GLuint textureID;
    
    Entity();
    
    bool CheckCollision(Entity other);
    
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);
    
    void Update(float deltaTime, Entity player, Entity *objects, int objectCount, Entity* enemies, int enemyCount);
    void Render(ShaderProgram *program);
    
    void Jump();

	void AI_Normal(Entity player);
	void AI_Patrol(Entity player);
	void AI_Flying(Entity player);
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    
};



