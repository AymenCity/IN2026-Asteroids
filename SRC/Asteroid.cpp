#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"

Asteroid::Asteroid(void) : GameObject("Asteroid")
{
	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD*mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD*mAngle);
	mVelocity.z = 0.0;
}

Asteroid::~Asteroid(void)
{
}

bool Asteroid::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Asteroid::OnCollision(const GameObjectList& objects)
{
	//mWorld->FlagForRemoval(GetThisPtr());

	// Split the asteroid into smaller asteroids
	const int numSmallerAsteroids = 3; // Number of smaller asteroids to create
	for (int i = 0; i < numSmallerAsteroids; ++i) {
		// Create a new smaller asteroid
		shared_ptr<Asteroid> smallerAsteroid = make_shared<Asteroid>();



		// Set the position of the smaller asteroid to be close to the original asteroid
		smallerAsteroid->SetPosition(mPosition.x + (rand() % 10 - 5));


		// Set the velocity of the smaller asteroid to be similar to the original asteroid
		smallerAsteroid->SetVelocity(mVelocity.x + (rand() % 6 - 3));

		// Add the smaller asteroid to the game world
		mWorld->AddObject(smallerAsteroid);
	}

}
