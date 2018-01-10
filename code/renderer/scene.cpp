

int MAX_GO_IN_SCENE = 10; // hilariously small atm

struct Scene
{

	int gameObjectCount;
	int maxCount;
	game_object *gameObjects;

	void Init(memory_block *memory)
	{
		gameObjects = (game_object*)memory->allocate(MAX_GO_IN_SCENE * sizeof(game_object));
		gameObjectCount = 0;
		maxCount = MAX_GO_IN_SCENE;
	}

	void add(game_object gameObject)
	{
		gameObjects[gameObjectCount++] = gameObject;
	}

};