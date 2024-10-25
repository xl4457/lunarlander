/**
* Author: [Amy Li]
* Assignment: Lunar Lander
* Date due: 2024-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

Entity::Entity()
{
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
}

Entity::~Entity(){};

CollisionType const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    CollisionType result = NOCOLLISION;

    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));

            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }

            if (collidable_entity->get_platform_type() == NORMAL) {
                result = GROUND;
            }
            else if (collidable_entity->get_platform_type() == TRAP) {
                result = HITTARGET;
            }
        }
    }
    return result;
};

CollisionType const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{

    CollisionType result = NOCOLLISION;

    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }

            if (collidable_entity->get_platform_type() == NORMAL) {
                result = GROUND;
            }
            else if (collidable_entity->get_platform_type() == TRAP) {
                result = HITTARGET;
            }

        }
    }
    return result;
};

CollisionType Entity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count)
{
    if (!m_is_active) { return NOCOLLISION; };

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    m_velocity += m_acceleration * delta_time;

    const float MAX_VELOCITY = 5.0f;
    if (glm::length(m_velocity) > MAX_VELOCITY)
    {
        m_velocity = glm::normalize(m_velocity) * MAX_VELOCITY;
    }

    m_position += m_velocity * delta_time;

    CollisionType y_direction = check_collision_y(collidable_entities, collidable_entity_count);
    CollisionType x_direction = check_collision_x(collidable_entities, collidable_entity_count);

    if (y_direction == GROUND || x_direction == GROUND) {
        return GROUND;
    }
    else if (y_direction == HITTARGET || x_direction == HITTARGET) {
        return HITTARGET;
    }
        
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);

    return NOCOLLISION;
};

void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
};

bool const Entity::check_collision(Entity* other) const
{
    if (!m_is_active || !other->m_is_active) { return false; };

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
};
