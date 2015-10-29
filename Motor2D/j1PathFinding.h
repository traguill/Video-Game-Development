#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"

struct SDL_Texture;

struct node_p
{
	int x;
	int y;
	node_p* parent;

	int g; //Result
	int h; //Steps
	int i; //Distance
	bool visited = false;
};

class j1PathFinding : public j1Module
{
public:

	j1PathFinding();

	// Destructor
	virtual ~j1PathFinding();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

private:

	void DrawPath(node_p* path);
	bool Walkable(int x, int y);

	void FillColliders(p2List<iPoint*>* list);

	float player_x;
	float player_y;

	SDL_Texture* begin_tile;
	SDL_Texture* path_tile;

	p2List<iPoint*> colliders;
	node_p* result_node;

	node_p* FindPath(iPoint end);

	node_p* FindRecursive(p2List<node_p*>* total, p2DynArray<node_p*>* best, iPoint end);

	bool SearchInList(p2List<node_p*>* list, node_p* node) const;
	node_p* InsertNode(node_p* parent, int offsetX, int offsetY, iPoint end);
	node_p* InsertSides(node_p* parent, p2List<node_p*>* list, iPoint end);
	int PickBestScore(p2List<node_p*>* total, p2DynArray<node_p*>* best);

};

#endif // __j1PathFinding_H__