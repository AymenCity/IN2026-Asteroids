#ifndef __CPU_H__
#define __CPU_H__

#include "GameUtil.h"

#include "GameObject.h"
#include "GameObjectType.h"
#include "IGameWorldListener.h"

#include "IPlayerListener.h"

class CPU : public IGameWorldListener {
public:
	CPU() { mLives = 3; }
	virtual ~CPU() {}

	void OnWorldUpdated(GameWorld* world) {}

	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}

	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
	{
		if (object->GetType() == GameObjectType("Spaceship")) {
			mLives -= 1;
			FirePlayerKilled();
		}
		if (object->GetType() == GameObjectType("ItemLife")) {
			mLives++;
			FirePlayerKilled();
		}
	}

	void AddListener(shared_ptr<IPlayerListener> listener)
	{
		mListeners.push_back(listener);
	}

	void FirePlayerKilled()
	{
		// Send message to all listeners
		for (CPUListenerList::iterator lit = mListeners.begin();
			lit != mListeners.end(); ++lit) {
			(*lit)->OnPlayerKilled(mLives);
		}
	}


private:
	int mLives;

	typedef std::list< shared_ptr<IPlayerListener> > CPUListenerList;

	CPUListenerList mListeners;

};

#endif
