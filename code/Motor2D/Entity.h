﻿#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "SDL/include/SDL_rect.h"
#include "Point.h"						//Required. For some reason the iPoint namespace does not work without it, even with a forward declaration like class iPoint.

class Animation;
class UI_Image;
class UI_Healthbar;

struct Collider;
struct SDL_Texture;	
struct SDL_Rect;


enum class ENTITY_TYPE
{
	UNKNOWN_TYPE,
	GATHERER,
	INFANTRY,
	ENEMY,
	TOWNHALL,
	ENEMY_TOWNHALL,
	BARRACKS,
	ENEMY_BARRACKS,
	ROCK,
	TREE
};

struct EntityData
{
	ENTITY_TYPE		type;
	iPoint			position;
};

class Entity
{
public:
	Entity();
	Entity(int x, int y, ENTITY_TYPE type, int level);	// here we will set initial position

	//Must be virtual so when an entity is executed the compiler goes to the entity's methods instead of these
	virtual bool Awake(pugi::xml_node&);

	virtual bool Start();

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

public:
	//Entity Methods
	/*virtual bool Save(pugi::xml_node&) const;
	virtual bool Load(pugi::xml_node&);
	virtual void Restart();*/							//Maybe not needed THIS HERE REVISE, CHANGE
	
	virtual void InitEntity();

	virtual void AttachHealthbarToEntity();
	
	virtual void Draw();

	virtual void OnCollision(Collider* c1, Collider* c2);						//If {} are used then the OnCollision on the entity.cpp needs to be erased.

	void ApplyDamage(Entity* target);											//Applies the current unit damage to the referenced target, also handles the target health bar.

public:
	//Entity Variables
	ENTITY_TYPE		type;

	bool			is_selected;

	int				max_health;
	int				current_health;
	int				attack_damage;
	int				level;

	std::string		name_tag;													//Name of the entity in the xml file.

	fPoint			pixel_position;			
	iPoint			tile_position;
	//SPRITE ORDERING
	fPoint			center_point;												//Used for sprite ordering

	SDL_Rect		healthbar_background_rect;
	SDL_Rect		healthbar_rect;
	iPoint			healthbar_position_offset;

	UI_Healthbar*	healthbar;

	SDL_Texture*	entity_sprite;

	Collider*		collider;

	Animation*		animation;

	SDL_Rect		selection_collider;
};

#endif // __ENTITY_H__