#include "stdafx.h"
#include "StaticObj.h"

StaticObj::StaticObj(ResourceManager<sf::Texture, std::string>* resourceManager, std::string sprite_name, EntityManager* entity_manager,
	sfld::Vector2f position, ENTITY_SHAPE shape, ENTITY_TYPE type) {
	constructEntity(resourceManager, sprite_name, entity_manager, position, false, shape, Entity::DYNAMIC_STATIC, type);
}