#ifndef __POWER_H__
#define __POWER_H__

#include "GameObject.h"

class Power : public GameObject
{
public:
	Power(void);
	~Power(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif

