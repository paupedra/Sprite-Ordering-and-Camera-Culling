#ifndef __ROCK_H__
#define __ROCK_H__

#include "Static_Object.h"

class Rock : public Static_Object
{
public:

	Rock(int x, int y, ENTITY_TYPE type, int level);

	virtual bool Awake(pugi::xml_node&);

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

	virtual void Draw();

public:
	void InitEntity();

public:
	int ore;

	int gather_time;

private:
	SDL_Rect* blit_section;

};

#endif // !__ROCK_H__