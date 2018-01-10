

int MAX_GO_IN_SCENE = 10; // hilariously small atm

struct Scene
{
	game_object *gameObjects;
	int gameObjectCount;
	int maxCount;

	void init(memory_block *memory)
	{
		int space_needed = MAX_GO_IN_SCENE * sizeof(game_object);
		gameObjects = (game_object*)memory->allocate(space_needed);
		gameObjectCount = 0;
		maxCount = MAX_GO_IN_SCENE;
	}

	void init_with_malloc()
	{
		int space_needed = MAX_GO_IN_SCENE * sizeof(game_object);
		gameObjects = (game_object*)malloc(space_needed);
		gameObjectCount = 0;
		maxCount = MAX_GO_IN_SCENE;
	}

	void add(game_object gameObject)
	{
		gameObjects[gameObjectCount++] = gameObject;
	}

};