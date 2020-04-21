#include "Application.h"
#include "Map.h"
#include "Pathfinding.h"
#include "EntityManager.h"

#include "Static_Object.h"


Static_Object::Static_Object(int x, int y, ENTITY_TYPE type, int level) : Entity(x, y, type, level)
{
	
}

bool Static_Object::Awake(pugi::xml_node&)
{
	return true;
}

bool Static_Object::Start()
{
	App->pathfinding->ChangeWalkability(tile_position, this, NON_WALKABLE);
	
	//for (int x_tile = tile_position.x; x_tile < tile_position.x + tiles_occupied_x; ++x_tile)
	//{
	//	for (int y_tile = tile_position.y; y_tile < tile_position.y + tiles_occupied_y; ++y_tile)
	//	{
	//		
	//		//App->entity_manager->ChangeEntityMap(iPoint(x_tile, y_tile), this);						//Adds the generated entity to entity_map.
	//	}
	//}

	return true;
}

bool Static_Object::PreUpdate()
{
	

	return true;
}

bool Static_Object::Update(float dt, bool doLogic)
{
	return true;
}

bool Static_Object::PostUpdate()
{
	return true;
}

bool Static_Object::CleanUp()
{
	return true;
}

void Static_Object::Draw()
{
	return;
}