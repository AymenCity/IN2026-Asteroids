#include "Asteroid.h"
#include "ItemLife.h"
#include "ItemDouble.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "CPU.h"


// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
	mItemsCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");
	Animation *item1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("item1", 128, 8192, 128, 128, "itemLife_fs.png"); //object - item
	Animation *item1explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("power1", 64, 1024, 64, 64, "itemLifeExplosion_fs.png"); //explosion - item
	Animation *item2_anim = AnimationManager::GetInstance().CreateAnimationFromFile("item2", 128, 8192, 128, 128, "itemDouble_fs.png"); //object - item
	Animation *item2explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("power2", 64, 1024, 64, 64, "itemDoubleExplosion_fs.png"); //explosion - item

	// Create a spaceship and add it to the world
	// mGameWorld->AddObject(CreateSpaceship());
	// Create some asteroids and add them to the world

	//CreateAsteroids(1);

	//Create the GUI
	CreateGUI();

	mStartLabel->SetVisible(true);
	mStartLabel2->SetVisible(true);
	mStartLabel3->SetVisible(true);
	mStartLabel4->SetVisible(true);
	mScoreLabel->SetVisible(false);
	mLivesLabel->SetVisible(false);

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	//StartDemoMode();

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	key = toupper(key); //https://www.programiz.com/cpp-programming/library-function/cctype/toupper

	if (key == ' ' && !mSpaceship)
	{
		mStartLabel->SetVisible(false);	// hides start label
		mStartLabel2->SetVisible(false);
		mStartLabel3->SetVisible(false);
		mStartLabel4->SetVisible(false);
		mScoreLabel->SetVisible(true);	// displays score label
		mLivesLabel->SetVisible(true);	// displays lives label
		mGameWorld->AddObject(CreateSpaceship());	// spawns player/spaceship
		CreateAsteroids(3);							// spawns more asteroids
		//CreateItem1(3);
		//CreateItem2(2);
	}
	if (key == 'D'  && !mSpaceship)
	{
		StartDemoMode();
	}
	else {
		switch (key)
		{
		case ' ':
			mSpaceship->Shoot();
			break;
		default:
			break;
		}
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	if (mSpaceship) {	// condition prevents game freezing during start screen
		switch (key)
		{
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
			// If left arrow key is pressed start rotating anti-clockwise
		case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
			// If right arrow key is pressed start rotating clockwise
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
			// Default case - do nothing
		default: break;
		}
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	if (mSpaceship) {	// condition prevents game freezing during start screen
		switch (key)
		{
			// If up arrow key is released stop applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
			// If left arrow key is released stop rotating
		case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
			// If right arrow key is released stop rotating
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
			// Default case - do nothing
		default: break;
		}
	}
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	//bool isPower = false;
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mAsteroidCount <= 0) 
		{ 
			SetTimer(500, START_NEXT_LEVEL); 
		}
		isPower = false;
	}
	if (object->GetType() == GameObjectType("ItemLife"))
	{
		shared_ptr<GameObject> power1 = CreatePowerExplosion();
		power1->SetPosition(object->GetPosition());
		power1->SetRotation(object->GetRotation());
		mGameWorld->AddObject(power1);
	
		//if (mAsteroidCount <= 0)
		//{
		//	SetTimer(500, START_NEXT_LEVEL);
		//}
		isPower = true;
	}
	if (object->GetType() == GameObjectType("ItemDouble"))
	{
		shared_ptr<GameObject> power2 = CreatePowerExplosion2();
		power2->SetPosition(object->GetPosition());
		power2->SetRotation(object->GetRotation());
		mGameWorld->AddObject(power2);
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 5 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
	}

	if (value == ITEM_PICKED)
	{
		mGameWorld->AddObject(mSpaceship);
	}
	if (value == DEMO_MODE_ACTIONS) 
	{
		int action = rand() % 3; // generate a random number to determine the action

		// thrust
		int thrustPower = 20;
		if (action == 0 && mSpaceship) {
			int thrustDirection = rand() % 360; // Randomly choose thrust direction
			mSpaceship->Thrust(thrustPower);
			mSpaceship->SetRotation(thrustDirection);
		}
		else // prevents spaceship from moving forever in the same direction
		{
			mSpaceship->Thrust(0); // stops the thrust if action isnt chosen
		}

		// rotates left
		if (action == 1 && mSpaceship) {
			mSpaceship->Rotate(90);
		}

		// rotates right
		if (action == 2 && mSpaceship) {
			mSpaceship->Rotate(-90);
		}

		SetTimer(500, DEMO_MODE_ACTIONS);	// new demo action every 500 timer
		SetTimer(100, DEMO_MODE_SHOOT);		// new demo shooting every 100 timer
	}
	if (value == DEMO_MODE_SHOOT)
	{
		mSpaceship->Shoot();
	}
}

	


// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateItem1(const uint num_items)
{
	mItemsCount = num_items;
	for (uint i = 0; i < num_items; i++)
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("item1");
		shared_ptr<Sprite> item1_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		item1_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> item = make_shared<ItemLife>();
		item->SetBoundingShape(make_shared<BoundingSphere>(item->GetThisPtr(), 10.0f));
		item->SetSprite(item1_sprite);
		item->SetScale(0.2f);
		mGameWorld->AddObject(item);
	}
}

void Asteroids::CreateItem2(const uint num_items)
{
	mItemsCount = num_items;
	for (uint i = 0; i < num_items; i++)
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("item2");
		shared_ptr<Sprite> item2_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		item2_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> item = make_shared<ItemDouble>();
		item->SetBoundingShape(make_shared<BoundingSphere>(item->GetThisPtr(), 10.0f));
		item->SetSprite(item2_sprite);
		item->SetScale(0.2f);
		mGameWorld->AddObject(item);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

	// create start_screen label
	mStartLabel = shared_ptr<GUILabel>(new GUILabel("--- ASTEROID GAME ---"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mStartLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mStartLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mStartLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> start_component
		= static_pointer_cast<GUIComponent>(mStartLabel);
	mGameDisplay->GetContainer()->AddComponent(start_component, GLVector2f(0.5f, 0.75f));

	// create start_screen label
	mStartLabel2 = shared_ptr<GUILabel>(new GUILabel("BY AYMEN SAID"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mStartLabel2->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mStartLabel2->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mStartLabel2->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> start_component2
		= static_pointer_cast<GUIComponent>(mStartLabel2);
	mGameDisplay->GetContainer()->AddComponent(start_component2, GLVector2f(0.5f, 0.69f));

	// create start_screen label
	mStartLabel3 = shared_ptr<GUILabel>(new GUILabel("PRESS SPACEBAR TO PLAY"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mStartLabel3->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mStartLabel3->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mStartLabel3->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> start_component3
		= static_pointer_cast<GUIComponent>(mStartLabel3);
	mGameDisplay->GetContainer()->AddComponent(start_component3, GLVector2f(0.5f, 0.31f));

	// create start_screen label
	mStartLabel4 = shared_ptr<GUILabel>(new GUILabel("PRESS D TO GO DEMO MODE"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mStartLabel4->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mStartLabel4->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mStartLabel4->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> start_component4
		= static_pointer_cast<GUIComponent>(mStartLabel4);
	mGameDisplay->GetContainer()->AddComponent(start_component4, GLVector2f(0.5f, 0.25f));

}



void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);

	if (score % 100 == 0) {		// condition where every 100 points, an item spawns
		// variable generates random number
		// https://www.geeksforgeeks.org/rand-and-srand-in-ccpp/
		int randomNum = rand() % 2;

		// chooses a random item based on variable
		if (randomNum == 0) {
			CreateItem1(1);
		}
		else {
			CreateItem2(1);
		}
	}
}

void Asteroids::OnPlayerKilled(int lives_left)
{

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0 && isPower == false)
	{
		SetTimer(1000, CREATE_NEW_PLAYER);
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(mSpaceship->GetPosition());
		explosion->SetRotation(mSpaceship->GetRotation());
		mGameWorld->AddObject(explosion);
	}
	if (lives_left > 0 && isPower == true)
	{
		SetTimer(0, ITEM_PICKED);
	}
	if (lives_left <= 0)
	{
		SetTimer(500, SHOW_GAME_OVER);
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(mSpaceship->GetPosition());
		explosion->SetRotation(mSpaceship->GetRotation());
		mGameWorld->AddObject(explosion);
	}
}


shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}

shared_ptr<GameObject> Asteroids::CreatePowerExplosion()
{
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("power1");
	shared_ptr<Sprite> item1explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	item1explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> power1 = make_shared<Explosion>();
	power1->SetSprite(item1explosion_sprite);
	power1->Reset();
	return power1;
}

shared_ptr<GameObject> Asteroids::CreatePowerExplosion2()
{
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("power2");
	shared_ptr<Sprite> item2explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	item2explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> power2 = make_shared<Explosion>();
	power2->SetSprite(item2explosion_sprite);
	power2->Reset();
	return power2;
}

void Asteroids::StartDemoMode() {
	mStartLabel->SetVisible(false);
	mStartLabel2->SetVisible(false);
	mStartLabel3->SetVisible(false);
	mStartLabel4->SetVisible(false);
	mScoreLabel->SetVisible(true);
	mLivesLabel->SetVisible(true);

	CreateAsteroids(3);

	// pointer
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add computer player as a listener to game world
	mGameWorld->AddListener(&mComputerPlayer);

	// Add this class as a listener of the score keeper
	mComputerPlayer.AddListener(thisPtr);

	if (!mSpaceship) {
		mGameWorld->AddObject(CreateSpaceship());
	}

	SetTimer(500, DEMO_MODE_ACTIONS);
}







