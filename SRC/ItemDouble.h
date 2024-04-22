#ifndef __ITEMDOUBLE_H__
#define __ITEMDOUBLE_H__

#include "GameObject.h"

class ItemDouble : public GameObject
{
public:
	ItemDouble(void);
	~ItemDouble(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif



