#pragma once

#include "stdafx.h"
#include "ResourceManager.h"

class EntityManager;

class Entity : public sf::NonCopyable{
public:
	enum ENTITY_DYNAMIC{
		DYNAMIC_MOVING,
		DYNAMIC_STATIC
	};
	enum ENTITY_SHAPE{
		SHAPE_CIRCLE,
		SHAPE_SQUARE
	};
	enum ENTITY_TYPE{
		TYPE_WALL,
		TYPE_PLAYER,
		TYPE_DOOR
	};
	Entity();
	~Entity();
	virtual void constructEntity(ResourceManager<sf::Texture, std::string>* resourceManager, std::string spriteName, EntityManager* entityManager, sfld::Vector2f position, 
		bool walkthrough, ENTITY_SHAPE shape, ENTITY_DYNAMIC dynamic,ENTITY_TYPE type);
	virtual void update(int frameTime);
	virtual void render(sf::RenderTarget* target);

	ENTITY_SHAPE getShape() const;
	ENTITY_DYNAMIC getDynamic() const;
	ENTITY_TYPE getType() const;
	bool isWalkthrough() const;

	sfld::Vector2f getPosition() const;
	sf::Sprite getSprite() const;

	bool contains(sfld::Vector2f point) const;

	virtual void collided(Entity* other);
	float getRadius() const;

	virtual void damaged(int amount);

	bool isDestroyed() const;
	void setPosition(sfld::Vector2f position);

	bool isActive() const;
	void setActive(bool active);
protected:
	bool rotating_;
	void destroy();

	void centreOrigin();
	void move(sfld::Vector2f direction,int frameTime,float magnitude); //moves entity, including collisions with walls

	void doOffset(sfld::Vector2f offset);
	

	ResourceManager<sf::Texture, std::string>* resourceManager_;
	EntityManager* entityManager_;
	sf::Sprite sprite_;

	void setWalkthrough(bool walkthrough);
private:
	bool active_; //Active if visible and moving. Inactive means not visible and not moving (e.g. in unlit room).

	bool destroyed_;

	ENTITY_SHAPE shape_;
	ENTITY_DYNAMIC dynamic_;
	ENTITY_TYPE type_;

	bool walkthrough_;

	sfld::Vector2f position_; //centre position
	
};