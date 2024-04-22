#ifndef __ITEMLIFE_H__
#define __ITEMLIFE_H__

#include "GameObject.h"

class ItemLife : public GameObject
{
public:
	ItemLife(void);
	~ItemLife(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif

