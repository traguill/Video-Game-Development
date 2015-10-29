#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Text.h"

j1PathFinding::j1PathFinding() : j1Module()
{
	name.create("path");
	begin_tile = path_tile = NULL;
	player_x = 3;
	player_y = 16;
	result_node = NULL;
}

// Destructor
j1PathFinding::~j1PathFinding()
{}

// Called before render is available
bool j1PathFinding::Awake(pugi::xml_node& config)
{
	LOG("Loading PathFinding");
	bool ret = true;

	

	return ret;
}

// Called before the first frame
bool j1PathFinding::Start()
{

	begin_tile = App->tex->Load("textures/begin.png");
	path_tile = App->tex->Load("textures/path.png");

	FillColliders(&colliders);

	App->text->Write("Instructions:\n-Press F1 to hide DebugMode\n-Move player (WASD)\n-Click to calculate path\n \n");

	return true;
}

// Called each loop iteration
bool j1PathFinding::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		if (Walkable(player_x, player_y -1))
		player_y--;
	}


	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		if (Walkable(player_x, player_y + 1))
		player_y++;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
	{
		if (Walkable(player_x - 1 , player_y))
		player_x--;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		if (Walkable(player_x + 1, player_y))
		player_x++;
	}

	if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
	{
		LOG("MouseClick");
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
		map_coordinates.x -= 1;
		result_node = FindPath(map_coordinates);
	}
	
	if (result_node != NULL)
	{
		DrawPath(result_node);
	}

	iPoint p = App->map->MapToWorld(player_x, player_y);
	App->render->Blit(begin_tile, p.x, p.y);


	return true;
}



// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing PathFinding");



	return true;
}


//Utilities---------------------------------------------------------------
void j1PathFinding::DrawPath(node_p* path)
{
	node_p* item;
	item = path;

	while (item != NULL)
	{
		iPoint p = App->map->MapToWorld(item->x, item->y);
		App->render->Blit(path_tile, p.x, p.y);
		item = item->parent;
	}
}

bool j1PathFinding::Walkable(int x, int y)
{
	p2List_item<iPoint*>* item;
	item = colliders.start;

	if (x < 0 || y < 0 || x >= App->map->data.width || y >= App->map->data.height)
		return false;

	while (item != NULL)
	{
		if (item->data->x == x && item->data->y == y)
			return false;
		item = item->next;
	}
	return true;
}

void j1PathFinding::FillColliders(p2List<iPoint*>* list)
{
	p2List_item<MapLayer*>* item;
	item = App->map->data.layers.start;

	while (item != NULL)
	{
		if (item->data->properties.GetProperty("Navigation") == 1)
		{
			for (int y = 0; y < App->map->data.height; ++y)
			{
				for (int x = 0; x < App->map->data.width; ++x)
				{
					int tile_id = item->data->Get(x, y);
					if (tile_id > 0)
					{
						TileSet* tileset = App->map->GetTilesetFromTileId(tile_id);

						if (tileset != NULL)
						{
							iPoint* pos = new iPoint(x, y);
							list->add(pos);
						}
					}
				}
			}
		}

		item = item->next;
	}
}

node_p* j1PathFinding::FindPath(iPoint end)
{
	if (Walkable(end.x, end.y) == false)
	{
		App->text->Write("Invalid cell!");
		return NULL;
	}
		

	p2List<node_p*> total;
	p2DynArray<node_p*> best;

	node_p* player = new node_p();
	player->parent = NULL;
	player->x = player_x;
	player->y = player_y;
	player->h = 0;
	player->i = abs(end.x - player->x) + abs(end.y - player->y);
	player->g = player->h + player->i;

	total.add(player);
	best.PushBack(player);

	return FindRecursive(&total, &best, end);
}

node_p* j1PathFinding::FindRecursive(p2List<node_p*>* total, p2DynArray<node_p*>* best, iPoint end)
{
	node_p*	item;
	while (best->Pop(item) != NULL)
	{
		node_p* result = InsertSides(item, total, end);
		if (result != NULL)
			return result;
	}

	PickBestScore(total, best);

	return FindRecursive(total, best, end);
}

int j1PathFinding::PickBestScore(p2List<node_p*>* total, p2DynArray<node_p*>* best)
{
	int ret = -1; //Returns -1 if we can't find the best score

	//Debug
	assert(best->Count() == 0); //best list must be empty

	p2List_item<node_p*>* item;
	item = total->start;
	
	int low_score = 4000; //Pick the first score on the list as a reference
	item = item->next; //We dont check the first node because its the root
	while (item != NULL)
	{
		if (item->data->visited == false)
		{
			if (item->data->g == low_score)
			{
				best->PushBack(item->data);
			}

			if (item->data->g < low_score)
			{
				best->Clear();
				best->PushBack(item->data);
				low_score = item->data->g;
			}
		}
		
		item = item->next;
	}
	
	return ret;
}

node_p* j1PathFinding::InsertSides(node_p* parent, p2List<node_p*>* list, iPoint end)
{
	//Right
	if (Walkable(parent->x + 1, parent->y))
	{
		if (SearchInList(list, parent) == true)
		{
			node_p* node_new = InsertNode(parent, 1, 0, end);

			if (node_new->i != 0)
			{
				list->add(node_new);
			}
			else
				return node_new;
		}
	}

	//Down
	if (Walkable(parent->x , parent->y+1))
	{
		if (SearchInList(list, parent) == true)
		{
			node_p* node_new = InsertNode(parent, 0, 1, end);

			if (node_new->i != 0)
			{
				list->add(node_new);
			}
			else
				return node_new;
		}
	}

	//Left
	if (Walkable(parent->x - 1, parent->y))
	{
		if (SearchInList(list, parent) == true)
		{
			node_p* node_new = InsertNode(parent, -1, 0, end);

			if (node_new->i != 0)
			{
				list->add(node_new);
			}
			else
				return node_new;
		}
	}

	//Up
	if (Walkable(parent->x, parent->y-1))
	{
		if (SearchInList(list, parent) == true)
		{
			node_p* node_new = InsertNode(parent, 0, -1, end);

			if (node_new->i != 0)
			{
				list->add(node_new);
			}
			else
				return node_new;
		}
	}

	parent->visited = true;

	return NULL;
}

node_p* j1PathFinding::InsertNode(node_p* parent, int offSetX, int offSetY, iPoint end)
{
	node_p* n = new node_p();
	n->parent = parent;
	n->x = parent->x + offSetX;
	n->y = parent->y + offSetY;
	n->h = parent->h + 1;
	n->i = abs(end.x - n->x) + abs(end.y - n->y);
	n->g = n->h + n->i;

	return n;
}

bool j1PathFinding::SearchInList(p2List<node_p*>* list, node_p* node)const
{
	if (list->find(node) == -1)
		return false;
	else
		return true;
}