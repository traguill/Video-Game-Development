#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1FileSystem.h"
#include "j1Text.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"

#include "SDL2_ttf\include\SDL_ttf.h"
#pragma comment( lib, "SDL2_ttf/lib/x86/SDL2_ttf.lib" )

j1Text::j1Text() : j1Module()
{
	name.create("text");
}

// Destructor
j1Text::~j1Text()
{}

// Called before render is available
bool j1Text::Awake(pugi::xml_node& config)
{
	LOG("Loading Text Display");
	bool ret = true;

	if (TTF_Init() != 0)
	{
		LOG("SDL_TTF could not initialize! SDL_Error: %s\n", TTF_GetError());
		active = false;
		ret = false;
	}

	//Load a font
	font = TTF_OpenFontRW(App->fs->Load(config.child("font").attribute("value").as_string()),0, 12);
	if (font == NULL)
	{
		LOG("Error while opening font %s - %s",config.child("font").attribute("value").as_string(), TTF_GetError());
		active = false;
		ret = false;
	}

	Write("Console:");

	return ret;
}

bool j1Text::Start()
{
	

	return true;
}

void j1Text::Write(p2SString content)
{
	console += " \n ";
	console += content;
	SDL_Color color = { 255, 255, 255 };
	text = TTF_RenderText_Blended_Wrapped(font, console.GetString(), color, 400);
	
	if (text == NULL)
	{
		LOG("Could not display text &s, because: %s", content.GetString(), TTF_GetError());
	}

	text_tx = SDL_CreateTextureFromSurface(App->render->renderer, text);

	if (text_tx == NULL)
	{
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	SDL_FreeSurface(text);
}

bool j1Text::Update(float dt)
{

	if (App->input->debug)
	{
		App->render->Blit(text_tx, -App->render->camera.x, -App->render->camera.y);
	}
		

	return true;
}

// Load Game State
bool j1Text::Load(pugi::xml_node& data)
{

	return true;
}

// Save Game State
bool j1Text::Save(pugi::xml_node& data) const
{

	return true;
}

// Called before quitting
bool j1Text::CleanUp()
{
	if (!active)
		return true;

	LOG("Closing Text subsystem");

	//TODO: I should do something with the pointers that I have... ?¿

	TTF_Quit();

	return true;
}

