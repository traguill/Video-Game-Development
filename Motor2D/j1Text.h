#ifndef __j1TEXT_H__
#define __j1TEXT_H__

#include "j1Module.h"

struct SDL_Surface;
struct SDL_Texture;
typedef struct _TTF_Font TTF_Font;

class j1Text : public j1Module
{
public:

	j1Text();

	// Destructor
	virtual ~j1Text();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();

	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Display Text
	void Write(p2SString content);
	

private:
	TTF_Font* font;
	SDL_Surface* text;
	SDL_Texture* text_tx;

	p2SString console;
};

#endif // __j1TEXT_H__