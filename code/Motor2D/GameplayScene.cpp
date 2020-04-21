#include "Brofiler\Brofiler.h"
//#include "mmgr/mmgr.h"

#include "p2Log.h"
#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Audio.h"
#include "Collisions.h"
#include "Map.h"
#include "Minimap.h"
#include "Pathfinding.h"
#include "Console.h"
#include "Player.h"
#include "Scene.h"

#include "EntityManager.h"
#include "Entity.h"
#include "Static_Object.h"
#include "TownHall.h"
#include "Barracks.h"
#include "Barracks.h"
#include "Gatherer.h"
#include "Infantry.h"
#include "Rock.h"
#include "Tree.h"

#include "Gui.h"
#include "UI.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_Scrollbar.h"
#include "UI_Image.h"

#include "SceneManager.h"
#include "TransitionManager.h"

#include "GameplayScene.h"


GameplayScene::GameplayScene() : Scene(SCENES::GAMEPLAY_SCENE)
{
	name_tag = ("scene");
}

// Destructor
GameplayScene::~GameplayScene()
{}

// Called before render is available
bool GameplayScene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");

	bool ret = true;

	/*for (pugi::xml_node map = config.child("map_name"); map; map = map.next_sibling("map_name"))
	{
		std::string data;

		data = (map.attribute("name").as_string());
		map_names.push_back(data);
	}*/

	music_path = (config.child("audio").attribute("path").as_string());
	music_path2 = (config.child("audio2").attribute("path").as_string());
	music_path3 = (config.child("audio3").attribute("path").as_string());

	return ret;
}

// Called before the first frame
bool GameplayScene::Start()
{
	bool ret = true;

	InitScene();

	return ret;
}

// Called each loop iteration
bool GameplayScene::PreUpdate()
{
	// debug pathfing ------------------
	if (App->map->pathfinding_meta_debug)
	{
		PathfindingDebug();
	}

	return true;
}

// Called each loop iteration
bool GameplayScene::Update(float dt)														//Receives dt as an argument.
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);

	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);

	App->map->Draw();																		//Map Draw

	App->minimap->BlitMinimap();

	if (App->map->walkability_debug || App->map->entity_map_debug)
	{
		App->map->DataMapDebug();																// Will print on screen the debug tiles of the walkability map and the entity map.
	}

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		App->win->ToggleFullscreen();
	}


	if (App->map->pathfinding_meta_debug)
	{
		DrawPathfindingDebug();																//Pathfinding Debug. Shows a debug texture on the path's tiles.
	}

	AdjustVolumeWithScrollbar();

	DebugHUDSpawn();

	// Refresh resources on screen
	std::string HUD_data_resource_string = std::to_string(App->entity_manager->resource_data);
	HUD_data_resource_text->RefreshTextInput(HUD_data_resource_string.c_str());

	std::string HUD_electricity_resource_string = std::to_string(App->entity_manager->resource_electricity);
	HUD_electricity_resource_text->RefreshTextInput(HUD_electricity_resource_string.c_str());



	return true;
}


// Called each loop iteration
bool GameplayScene::PostUpdate()
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);
	bool ret = true;

	DebugKeys();																	//Debug Keys

	if (App->player->god_mode)
	{
		CameraDebugMovement(App->GetDt());
		UnitDebugKeys();
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		App->render->camera.x = 1550;
		App->render->camera.y = -600;
	}

	CheckForWinLose();

	//Transition To Any Scene. Load Scene / Unload GameplayScene
	ExecuteTransition();

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		App->pause = !App->pause;

		App->gui->SetElementsVisibility(in_game_background, !in_game_background->isVisible);
		App->gui->SetElementsVisibility(in_game_options_parent, !in_game_options_parent);
		App->audio->PlayFx(App->gui->appear_menu_fx, 0);
	}

	if (App->player->god_mode)
	{
		App->gui->SetElementsVisibility(God_Mode_Activated, true);
	}
	if (App->player->god_mode == false)
	{
		App->gui->SetElementsVisibility(God_Mode_Activated, false);
	}

	App->minimap->BlitMinimap();

	return ret;
}

void GameplayScene::CheckForWinLose() {

	if (!App->player->god_mode)
	{
		//Check for an enemy townhall alive. If none is found the player has won, thus we call the transition to win scene
		bool exists_enemytownhall = false;
		for (int i = 0; i < App->entity_manager->entities.size(); ++i)
		{
			if (App->entity_manager->entities[i]->type == ENTITY_TYPE::ENEMY_TOWNHALL)
			{
				exists_enemytownhall = true;
				break;
			}
		}
		if (exists_enemytownhall == false)
		{
			App->transition_manager->CreateSlide(SCENES::WIN_SCENE, 0.5f, true, true, false, false, Black);
		}

		//Same but for allied town halls. We put it second so in case they break at the same frame (not gonna happen) the player wins.
		bool exists_townhall = false;
		for (int i = 0; i < App->entity_manager->entities.size(); ++i)
		{
			if (App->entity_manager->entities[i]->type == ENTITY_TYPE::TOWNHALL)
			{
				exists_townhall = true;
				break;
			}
		}
		if (exists_townhall == false)
		{
			//App->transition_manager->CreateAlternatingBars(SCENES::LOSE_SCENE, 0.5f, true, 5, true, true);
			App->transition_manager->CreateSlide(SCENES::LOSE_SCENE, 0.5f, true, true, false, false, Black);
		}

		bool exists_allyunits = false;
		for (int i = 0; i < App->entity_manager->entities.size(); ++i)
		{
			if (App->entity_manager->entities[i]->type == ENTITY_TYPE::BARRACKS || App->entity_manager->entities[i]->type == ENTITY_TYPE::INFANTRY || App->entity_manager->entities[i]->type == ENTITY_TYPE::GATHERER)
			{
				exists_allyunits = true;
				break;
			}
		}
		if (exists_allyunits == false)
		{
			//App->transition_manager->CreateAlternatingBars(SCENES::LOSE_SCENE, 0.5f, true, 5, true, true);

			App->transition_manager->CreateSlide(SCENES::LOSE_SCENE, 0.5f, true, true, false, false, Black);
		}


	}
}

// Called before quitting
bool GameplayScene::CleanUp()
{
	App->tex->UnLoad(background_texture);
	App->tex->UnLoad(occupied_debug);
	App->tex->UnLoad(occupied_by_entity_debug);
	App->tex->UnLoad(path_debug_tex);

	App->collisions->CleanUp();								//Deletes all colliders that were loaded for this scene / map.
	App->player->ClearEntityBuffers();						//Clears the entity list
	App->entity_manager->DestroyEntities();					//Destroys all non-player entities.
	App->map->CleanUp();									//Deletes everything related with the map from memory. (Tilesets, Layers and ObjectGroups)
	App->gui->CleanUp();

	return true;
}

void GameplayScene::InitScene()
{
	bool ret = true;

	//cam_debug_speed = App->render->cam.camera_debug_speed;				//Sets the camera speed in debug mode.


	App->entity_manager->resource_data = 0;
	App->entity_manager->resource_electricity = 0;

	App->render->camera.x = 1550;
	App->render->camera.y = -600;

	App->gui->Start();

	to_end = false;

	ret = App->map->Load("New_Tilesete_Map.tmx");

	//test background
	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/hacker_background.png");

	LoadGuiElements();

	path_debug_tex = App->tex->Load("maps/path_tile.png");
	occupied_debug = App->tex->Load("maps/occupied_tile.png");
	occupied_by_entity_debug = App->tex->Load("maps/occupied_by_entity_tile.png");

	//App->audio->PlayMusic(App->scene->music_path2.c_str());
	inGame_song = App->audio->LoadMusic("audio/music/3_Music_Gameplay.ogg");
	App->audio->PlayMusic(inGame_song, 0.0f);
}

void GameplayScene::LoadGuiElements()
{
	// In-game menu

	// Back
	SDL_Rect background = { 780, 451, 514, 403 };
	in_game_background = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 380, 135, background, false, false, false, this, nullptr);


	// Continue Button
	SDL_Rect in_game_continue_button_size = { 0, 0, 158, 23 };
	SDL_Rect in_game_continue_button_idle = { 1, 0, 158, 23 };
	SDL_Rect in_game_continue_button_hover = { 178, 0, 158, 23 };
	SDL_Rect in_game_continue_button_clicked = { 356, 0, 158, 23 };

	in_game_continue_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 555, 241, false, true, false, this, in_game_background
		, &in_game_continue_button_idle, &in_game_continue_button_hover, &in_game_continue_button_clicked);


	// Options Button
	SDL_Rect in_game_options_button_size = { 0, 0, 133, 24 };
	SDL_Rect in_game_options_button_idle = { 1, 52, 133, 24 };
	SDL_Rect in_game_options_button_hover = { 178, 52, 133, 24 };
	SDL_Rect in_game_options_button_clicked = { 356, 52, 133, 24 };

	in_game_options_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 567, 281, false, true, false, this, in_game_background
		, &in_game_options_button_idle, &in_game_options_button_hover, &in_game_options_button_clicked);


	// Back to menu Button
	SDL_Rect in_game_back_to_menu_size = { 0, 0, 189, 23 };
	SDL_Rect in_game_back_to_menu_idle = { 0, 137, 189, 23 };
	SDL_Rect in_game_back_to_menu_hover = { 204, 137, 189, 23 };
	SDL_Rect in_game_back_to_menu_clicked = { 408, 137, 189, 23 };

	in_game_back_to_menu = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 541, 321, false, true, false, this, in_game_background
		, &in_game_back_to_menu_idle, &in_game_back_to_menu_hover, &in_game_back_to_menu_clicked);


	// Exit Button
	SDL_Rect in_game_exit_button_size = { 0, 0, 74, 23 };
	SDL_Rect in_game_exit_button_idle = { 1, 77, 74, 23 };
	SDL_Rect in_game_exit_button_hover = { 178, 77, 74, 23 };
	SDL_Rect in_game_exit_button_clicked = { 356, 77, 74, 23 };

	in_game_exit_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 596, 361, false, true, false, this, in_game_background
		, &in_game_exit_button_idle, &in_game_exit_button_hover, &in_game_exit_button_clicked);


	// Title
	SDL_Rect in_game_text_rect = { 0, 0, 100, 20 };
	_TTF_Font* in_game_font = App->font->Load("fonts/borgsquadcond.ttf", 50);
	std::string in_game_title_string = "Pause Menu";
	in_game_title_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 469, 165, in_game_text_rect, in_game_font, SDL_Color{ 255,255,0,0 }, false, false, false, this, in_game_background, &in_game_title_string);

	// Options Menu
	LoadInGameOptionsMenu();
	App->gui->SetElementsVisibility(in_game_options_parent, false);



	// HUD

	// Up Bar
	// Group Up
	SDL_Rect HUD_group_button_size = { 0, 0, 63, 38 };
	SDL_Rect HUD_group_button_idle = { 759, 48, 63, 38 };
	SDL_Rect HUD_group_button_hover = { 970, 48, 63, 38 };
	SDL_Rect HUD_group_button_clicked = { 1171, 48, 63, 38 };

	HUD_group_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 544, -4, true, true, false, this, nullptr
		, &HUD_group_button_idle, &HUD_group_button_hover, &HUD_group_button_clicked);

	// Pause
	SDL_Rect HUD_pause_button_size = { 0, 0, 63, 38 };
	SDL_Rect HUD_pause_button_idle = { 825, 48, 63, 38 };
	SDL_Rect HUD_pause_button_hover = { 1036, 48, 63, 38 };
	SDL_Rect HUD_pause_button_clicked = { 1237, 48, 63, 38 };

	HUD_pause_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 601, -4, true, true, false, this, nullptr
		, &HUD_pause_button_idle, &HUD_pause_button_hover, &HUD_pause_button_clicked);	
	
	// Play
	SDL_Rect HUD_play_button_size = { 0, 0, 63, 38 };
	SDL_Rect HUD_play_button_idle = { 1037, 3, 63, 38 };
	SDL_Rect HUD_play_button_hover = { 1104, 3, 63, 38 };
	SDL_Rect HUD_play_button_clicked = { 1171, 3, 63, 38 };

	HUD_play_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 601, -4, false, true, false, this, nullptr
		, &HUD_play_button_idle, &HUD_play_button_hover, &HUD_play_button_clicked);

	// Home 
	SDL_Rect HUD_home_button_size = { 0, 0, 63, 37 };
	SDL_Rect HUD_home_button_idle = { 890, 48, 63, 37 };
	SDL_Rect HUD_home_button_hover = { 1101, 48, 63, 37 };
	SDL_Rect HUD_home_button_clicked = { 1302, 48, 63, 37 };

	HUD_home_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 657, -4, true, true, false, this, nullptr
		, &HUD_home_button_idle, &HUD_home_button_hover, &HUD_home_button_clicked);




	//Down Bar

	//Resource Bar
	SDL_Rect HUD_resource_bar_size = { 884, 223, 188, 150 };

	HUD_resource_bar = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 1095, 574, HUD_resource_bar_size, true, true, false, this, nullptr);

	//Resources
	SDL_Rect HUD_electricity_resource_size = { 707, 54, 13, 25 };

	HUD_electricity_resource = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 1115, 604, HUD_electricity_resource_size, true, true, false, this, HUD_resource_bar);

	SDL_Rect HUD_data_resource_size = { 687, 54, 16, 25 };

	HUD_data_resource = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 1115, 634, HUD_data_resource_size, true, true, false, this, HUD_resource_bar);


	//Data Store

	SDL_Rect HUD_text_data_resource_rect = { 737, 54, 13, 25 };
	_TTF_Font* HUD_data_resource_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	HUD_data_resource_string = "0";
	HUD_data_resource_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 1145, 604, HUD_text_data_resource_rect, HUD_data_resource_font, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_townhall_bar, &HUD_data_resource_string);

	//Electricity Store

	SDL_Rect HUD_text_electricity_resource_rect = { 737, 54, 13, 25 };
	_TTF_Font* HUD_electricity_resource_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	HUD_electricity_resource_string = "0";
	HUD_electricity_resource_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 1145, 634, HUD_text_electricity_resource_rect, HUD_electricity_resource_font, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_townhall_bar, &HUD_electricity_resource_string);

	//Townhall Bar
	SDL_Rect HUD_townhall_bar_size = { 20, 209, 798, 160 };

	HUD_townhall_bar = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 309, 560, HUD_townhall_bar_size, false, true, false, this, nullptr);

	// Title Townhall
	SDL_Rect HUD_text_townhall_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_townhall_font = App->font->Load("fonts/borgsquadcond.ttf", 30);
	std::string HUD_title_townhall_string = "TOWNHALL";
	HUD_title_townhall = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 360, 582, HUD_text_townhall_rect, HUD_townhall_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_townhall_bar, &HUD_title_townhall_string);

	// Title Gatherer
	SDL_Rect HUD_text_gatherer_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_gatherer_font = App->font->Load("fonts/borgsquadcond.ttf", 30);
	std::string HUD_title_gatherer_string = "GATHERER";
	HUD_title_gatherer = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 370, 582, HUD_text_gatherer_rect, HUD_gatherer_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, nullptr, &HUD_title_gatherer_string);

	// HP Townhall


	// Description Townhall
	SDL_Rect HUD_text_townhall_descp_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_townhall_descp_font = App->font->Load("fonts/borgsquadcond.ttf", 12);
	std::string HUD_townhall_descp_string = "The main building of your base.";
	std::string HUD_townhall_descp_string2 = "If It gets destroyed,";
	std::string HUD_townhall_descp_string3 = "the game ends.";
	HUD_description_townhall = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 335, 635, HUD_text_townhall_descp_rect, HUD_townhall_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_townhall, &HUD_townhall_descp_string);
	HUD_description_townhall = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 362, 649, HUD_text_townhall_descp_rect, HUD_townhall_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_townhall, &HUD_townhall_descp_string2);
	HUD_description_townhall = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 379, 662, HUD_text_townhall_descp_rect, HUD_townhall_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_townhall, &HUD_townhall_descp_string3);

	// Description Gatherer
	SDL_Rect HUD_text_getherer_descp_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_gatherer_descp_font = App->font->Load("fonts/borgsquadcond.ttf", 12);
	std::string HUD_gatherer_descp_string = "Primary resource unit.";
	std::string HUD_gatherer_descp_string2 = "It doesn't attack. It is useful";
	std::string HUD_gatherer_descp_string3 = "for gathering resources.";
	HUD_description_gatherer = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 360, 635, HUD_text_getherer_descp_rect, HUD_gatherer_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_gatherer, &HUD_gatherer_descp_string);
	HUD_description_gatherer = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 337, 649, HUD_text_getherer_descp_rect, HUD_gatherer_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_gatherer, &HUD_gatherer_descp_string2);
	HUD_description_gatherer = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 355, 662, HUD_text_getherer_descp_rect, HUD_gatherer_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_gatherer, &HUD_gatherer_descp_string3);

	// Unit Townhall
	SDL_Rect HUD_unit_townhall_size = { 0, 0, 53, 50 };
	SDL_Rect HUD_unit_townhall_idle = { 1095, 140, 53, 50 };
	SDL_Rect HUD_unit_townhall_hover = { 1152, 140, 53, 50 };
	SDL_Rect HUD_unit_townhall_clicked = { 1207, 140, 53, 50 };

	HUD_unit_townhall = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 556, 610, false, true, false, this, HUD_townhall_bar
		, &HUD_unit_townhall_idle, &HUD_unit_townhall_hover, &HUD_unit_townhall_clicked);

	// Resources1
	HUD_parent_resources_unit_townhall_gatherer = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_townhall_res_unit_size = { 707, 54, 13, 25 };

	HUD_resources_unit_townhall_gatherer = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 880, 685, HUD_townhall_res_unit_size, false, true, false, this, HUD_parent_resources_unit_townhall_gatherer);

	SDL_Rect HUD_townhall_res2_unit_size = { 687, 54, 16, 25 };

	HUD_resources2_unit_townhall_gatherer = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 980, 685, HUD_townhall_res2_unit_size, false, true, false, this, HUD_parent_resources_unit_townhall_gatherer);

	// Price Gatherer Data
	SDL_Rect HUD_text_townhall_price_unit_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_townhall_price_unit_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_townhall_price_unit_string = "20";
	HUD_prices_unit_townhall_gatherer = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 900, 687, HUD_text_townhall_price_unit_rect, HUD_townhall_price_unit_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_townhall_gatherer, &HUD_townhall_price_unit_string);

	// Price Gatherer Electricity
	SDL_Rect HUD_text_townhall_price2_unit_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_townhall_price2_unit_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_townhall_price2_unit_string = "0";
	HUD_prices2_unit_townhall_gatherer = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 1000, 687, HUD_text_townhall_price2_unit_rect, HUD_townhall_price2_unit_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_townhall_gatherer, &HUD_townhall_price2_unit_string);

	// Resources2
	HUD_parent_resources_upgrade_unit_townhall_gatherer = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_townhall_res_upg_unit_size = { 707, 54, 13, 25 };

	HUD_resources_upgrade_unit_townhall_gatherer = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 880, 685, HUD_townhall_res_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer);

	SDL_Rect HUD_townhall_res2_upg_unit_size = { 687, 54, 16, 25 };

	HUD_resources2_upgrade_unit_townhall_gatherer = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 980, 685, HUD_townhall_res2_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer);

	// Price Upgrade Gatherer Data
	SDL_Rect HUD_text_townhall_price_unit_upgrade_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_townhall_price_unit_upgrade_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_townhall_price_unit_upgrade_string = "100";
	HUD_prices_unit_townhall_gatherer = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 900, 687, HUD_text_townhall_price_unit_upgrade_rect, HUD_townhall_price_unit_upgrade_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer, &HUD_townhall_price_unit_upgrade_string);

	// Price Upgrade Gatherer Electricity
	SDL_Rect HUD_text_townhall_price2_unit_upgrade_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_townhall_price2_unit_upgrade_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_townhall_price2_unit_upgrade_string = "50";
	HUD_prices2_unit_townhall_gatherer = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 1000, 687, HUD_text_townhall_price2_unit_upgrade_rect, HUD_townhall_price2_unit_upgrade_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer, &HUD_townhall_price2_unit_upgrade_string);

	// Resources3
	HUD_parent_resources_upgrade_townhall = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_townhall_res_upg_townhall_size = { 707, 54, 13, 25 };

	HUD_resources_upgrade_townhall = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 880, 685, HUD_townhall_res_upg_townhall_size, false, true, false, this, HUD_parent_resources_upgrade_townhall);

	SDL_Rect HUD_townhall_res2_upg_townhall_size = { 687, 54, 16, 25 };

	HUD_resources2_upgrade_townhall = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 980, 685, HUD_townhall_res2_upg_townhall_size, false, true, false, this, HUD_parent_resources_upgrade_townhall);

	// Price Upgrade Townhall Data
	SDL_Rect HUD_text_townhall_price_upgrade_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_townhall_price_upgrade_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_townhall_price_upgrade_string = "200";
	HUD_prices_upgrade_townhall = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 900, 687, HUD_text_townhall_price_upgrade_rect, HUD_townhall_price_upgrade_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_townhall, &HUD_townhall_price_upgrade_string);

	// Price Upgrade Townhall Electricity
	SDL_Rect HUD_text_townhall_price2_upgrade_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_townhall_price2_upgrade_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_townhall_price2_upgrade_string = "175";
	HUD_prices2_upgrade_townhall = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 1000, 687, HUD_text_townhall_price2_upgrade_rect, HUD_townhall_price2_upgrade_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_townhall, &HUD_townhall_price2_upgrade_string);


	// Upgrade Unit
	SDL_Rect HUD_unit_upgrade_townhall_size = { 0, 0, 50, 16 };
	SDL_Rect HUD_unit_upgrade_townhall_idle = { 899, 129, 50, 16 };
	SDL_Rect HUD_unit_upgrade_townhall_hover = { 899, 148, 50, 16 };
	SDL_Rect HUD_unit_upgrade_townhall_clicked = { 899, 167, 50, 16 };

	HUD_unit_upgrade_townhall_gatherer = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 556, 594, false, true, false, this, HUD_townhall_bar
		, &HUD_unit_upgrade_townhall_idle, &HUD_unit_upgrade_townhall_hover, &HUD_unit_upgrade_townhall_clicked);

	// Upgrade Building
	SDL_Rect HUD_upgrade_townhall_size = { 0, 0, 117, 24 };
	SDL_Rect HUD_upgrade_townhall_idle = { 960, 99, 117, 24 };
	SDL_Rect HUD_upgrade_townhall_hover = { 960, 125, 117, 24 };
	SDL_Rect HUD_upgrade_townhall_clicked = { 960, 154, 117, 24 };

	HUD_upgrade_townhall = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 975, 577, false, true, false, this, HUD_townhall_bar
		, &HUD_upgrade_townhall_idle, &HUD_upgrade_townhall_hover, &HUD_upgrade_townhall_clicked);




	//Barracks Bar
	SDL_Rect HUD_barracks_bar_size = { 20, 209, 798, 160 };

	HUD_barracks_bar = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 309, 560, HUD_barracks_bar_size, false, true, false, this, nullptr);

	// Title Barracks
	SDL_Rect HUD_text_barracks_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_barracks_font = App->font->Load("fonts/borgsquadcond.ttf", 30);
	std::string HUD_title_barracks_string = "BARRACKS";
	HUD_title_barracks = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 362, 582, HUD_text_barracks_rect, HUD_barracks_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_barracks_bar, &HUD_title_barracks_string);

	// Title Infantry
	SDL_Rect HUD_text_infantry_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_infantry_font = App->font->Load("fonts/borgsquadcond.ttf", 30);
	std::string HUD_title_infantry_string = "INFANTRY";
	HUD_title_infantry = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 370, 582, HUD_text_infantry_rect, HUD_infantry_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, nullptr, &HUD_title_infantry_string);

	// HP Barracks

	// Description Barracks
	SDL_Rect HUD_text_barracks_descp_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_barracks_descp_font = App->font->Load("fonts/borgsquadcond.ttf", 12);
	std::string HUD_barracks_descp_string = "Trains different military units";
	std::string HUD_barracks_descp_string2 = "depending on the number of";
	std::string HUD_barracks_descp_string3 = "resources you acquire.";
	HUD_description_barracks = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 329, 635, HUD_text_barracks_descp_rect, HUD_barracks_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_barracks, &HUD_barracks_descp_string);
	HUD_description_barracks = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 341, 649, HUD_text_barracks_descp_rect, HUD_barracks_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_barracks, &HUD_barracks_descp_string2);
	HUD_description_barracks = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 359, 662, HUD_text_barracks_descp_rect, HUD_barracks_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_barracks, &HUD_barracks_descp_string3);

	// Description Infantry
	SDL_Rect HUD_text_infranty_descp_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_infantry_descp_font = App->font->Load("fonts/borgsquadcond.ttf", 12);
	std::string HUD_infantry_descp_string = "Primary combat unit.";
	std::string HUD_infantry_descp_string2 = "It has a standard attck.";
	std::string HUD_infantry_descp_string3 = "Fight your enemies with these!";
	HUD_description_infantry = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 365, 635, HUD_text_infranty_descp_rect, HUD_infantry_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_infantry, &HUD_infantry_descp_string);
	HUD_description_infantry = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 347, 649, HUD_text_infranty_descp_rect, HUD_infantry_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_infantry, &HUD_infantry_descp_string2);
	HUD_description_infantry = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 329, 662, HUD_text_infranty_descp_rect, HUD_infantry_descp_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_infantry, &HUD_infantry_descp_string3);

	// Unit Barracks
	SDL_Rect HUD_unit_barracks_size = { 0, 0, 53, 50 };
	SDL_Rect HUD_unit_barracks_idle = { 1095, 90, 53, 50 };
	SDL_Rect HUD_unit_barracks_hover = { 1152, 90, 53, 50 };
	SDL_Rect HUD_unit_barracks_clicked = { 1207, 90, 53, 50 };

	HUD_unit_barracks = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 556, 610, false, true, false, this, HUD_barracks_bar
		, &HUD_unit_barracks_idle, &HUD_unit_barracks_hover, &HUD_unit_barracks_clicked);


	// Resources1
	HUD_parent_resources_unit_barracks_infantry = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_barracks_res_unit_size = { 707, 54, 13, 25 };

	HUD_resources_unit_barracks_infantry = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 880, 685, HUD_barracks_res_unit_size, false, true, false, this, HUD_parent_resources_unit_barracks_infantry);

	SDL_Rect HUD_barracks_res2_unit_size = { 687, 54, 16, 25 };

	HUD_resources2_unit_barracks_infantry = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 980, 685, HUD_barracks_res2_unit_size, false, true, false, this, HUD_parent_resources_unit_barracks_infantry);

	// Price Infantry Data
	SDL_Rect HUD_text_barracks_price_unit_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_barracks_price_unit_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_barracks_price_unit_string = "0";
	HUD_prices_unit_townhall_infantry = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 900, 687, HUD_text_barracks_price_unit_rect, HUD_barracks_price_unit_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_barracks_infantry, &HUD_barracks_price_unit_string);

	// Price Infantry Electricity
	SDL_Rect HUD_text_barracks_price2_unit_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_barracks_price2_unit_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_barracks_price2_unit_string = "10";
	HUD_prices2_unit_townhall_gatherer = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 1000, 687, HUD_text_barracks_price2_unit_rect, HUD_barracks_price2_unit_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_barracks_infantry, &HUD_barracks_price2_unit_string);

	// Resources2
	HUD_parent_resources_upgrade_unit_barracks_infantry = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_barracks_res_upg_unit_size = { 707, 54, 13, 25 };

	HUD_resources_upgrade_unit_barracks_infantry = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 880, 685, HUD_barracks_res_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry);

	SDL_Rect HUD_barracks_res2_upg_unit_size = { 687, 54, 16, 25 };

	HUD_resources2_upgrade_unit_barracks_infantry = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 980, 685, HUD_barracks_res2_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry);

	// Price Upgrade Infantry Data
	SDL_Rect HUD_text_barracks_price_unit_upgrade_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_barracks_price_unit_upgrade_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_barracks_price_unit_upgrade_string = "50";
	HUD_prices_upgrade_unit_barracks_infantry = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 900, 687, HUD_text_barracks_price_unit_upgrade_rect, HUD_barracks_price_unit_upgrade_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry, &HUD_barracks_price_unit_upgrade_string);

	// Price Upgrade Infantry Electricity
	SDL_Rect HUD_text_barracks_price2_unit_upgrade_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_barracks_price2_unit_upgrade_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_barracks_price2_unit_upgrade_string = "100";
	HUD_prices2_upgrade_unit_barracks_infantry = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 1000, 687, HUD_text_barracks_price2_unit_upgrade_rect, HUD_barracks_price2_unit_upgrade_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry, &HUD_barracks_price2_unit_upgrade_string);

	// Resources3
	HUD_parent_resources_upgrade_barracks = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_res_upg_barracks_size = { 707, 54, 13, 25 };

	HUD_resources_upgrade_barracks = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 880, 685, HUD_res_upg_barracks_size, false, true, false, this, HUD_parent_resources_upgrade_barracks);

	SDL_Rect HUD_res2_upg_barracks_size = { 687, 54, 16, 25 };

	HUD_resources2_upgrade_barracks = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 980, 685, HUD_res2_upg_barracks_size, false, true, false, this, HUD_parent_resources_upgrade_barracks);

	// Price Upgrade Barracks Data
	SDL_Rect HUD_text_barracks_price_upgrade_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_barracks_price_upgrade_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_barracks_price_upgrade_string = "170";
	HUD_prices_upgrade_barracks = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 900, 687, HUD_text_barracks_price_upgrade_rect, HUD_barracks_price_upgrade_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_barracks, &HUD_barracks_price_upgrade_string);

	// Price Upgrade Barracks Electricity
	SDL_Rect HUD_text_barracks_price2_upgrade_rect = { 0, 0, 100, 20 };
	_TTF_Font* HUD_barracks_price2_upgrade_font = App->font->Load("fonts/borgsquadcond.ttf", 20);
	std::string HUD_barracks_price2_upgrade_string = "120";
	HUD_prices2_upgrade_barracks = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 1000, 687, HUD_text_barracks_price2_upgrade_rect, HUD_barracks_price2_upgrade_font, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_barracks, &HUD_barracks_price2_upgrade_string);

	// Upgrade Unit
	SDL_Rect HUD_unit_upgrade_barracks_size = { 0, 0, 50, 16 };
	SDL_Rect HUD_unit_upgrade_barracks_idle = { 899, 129, 50, 16 };
	SDL_Rect HUD_unit_upgrade_barracks_hover = { 899, 148, 50, 16 };
	SDL_Rect HUD_unit_upgrade_barracks_clicked = { 899, 167, 50, 16 };

	HUD_unit_upgrade_barracks_infantry = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 556, 594, false, true, false, this, HUD_barracks_bar
		, &HUD_unit_upgrade_barracks_idle, &HUD_unit_upgrade_barracks_hover, &HUD_unit_upgrade_barracks_clicked);

	// Upgrade Building
	SDL_Rect HUD_upgrade_barracks_size = { 0, 0, 117, 24 };
	SDL_Rect HUD_upgrade_barracks_idle = { 960, 99, 117, 24 };
	SDL_Rect HUD_upgrade_barracks_hover = { 960, 125, 117, 24 };
	SDL_Rect HUD_upgrade_barracks_clicked = { 960, 154, 117, 24 };

	HUD_upgrade_barracks = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 975, 577, false, true, false, this, HUD_barracks_bar
		, &HUD_upgrade_barracks_idle, &HUD_upgrade_barracks_hover, &HUD_upgrade_barracks_clicked);


	// God_Mode
	SDL_Rect HUD_text_God = { 0, 0, 100, 20 };
	_TTF_Font* HUD_God_font = App->font->Load("fonts/borgsquadcond.ttf", 30);
	std::string HUD_God_string = "God Mode Activated";
	God_Mode_Activated = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 950, 2, HUD_text_God, HUD_God_font, SDL_Color{ 255,255,0,0 }, false, false, false, this, nullptr, &HUD_God_string);
}

void GameplayScene::LoadInGameOptionsMenu()
{
	// Options Menu
	SDL_Rect in_game_text_rect = { 0, 0, 100, 20 };
	_TTF_Font* in_game_font = App->font->Load("fonts/borgsquadcond.ttf", 50);
	_TTF_Font* in_game_font2 = App->font->Load("fonts/borgsquadcond.ttf", 30);
	in_game_options_parent = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	// Options
	std::string in_game_title_string = "Options Menu";
	in_game_options_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 445, 165, in_game_text_rect, in_game_font, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, in_game_options_parent, &in_game_title_string);

	//Music
	std::string in_game_music_string = "Music";
	in_game_music_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 487, 230, in_game_text_rect, in_game_font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, in_game_options_parent, &in_game_music_string);

	SDL_Rect in_game_thumb_rect = { 930,2,18,31 };
	SDL_Rect in_game_scrollbar_rect = { 743,3,180,15 };

	in_game_music_scrollbar = (UI_Scrollbar*)App->gui->CreateScrollbar(UI_ELEMENT::SCROLLBAR, 600, 235, in_game_scrollbar_rect, in_game_thumb_rect
		, iPoint(20, -7), in_game_scrollbar_rect, 20.0f, true, false, true, false, false, false);
	in_game_music_scrollbar->parent = in_game_options_parent;

	//SFX
	std::string sfx_string = "SFX";
	in_game_sfx_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 491, 264, in_game_text_rect, in_game_font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, in_game_options_parent, &sfx_string);
	in_game_sfx_scrollbar = (UI_Scrollbar*)App->gui->CreateScrollbar(UI_ELEMENT::SCROLLBAR, 600, 275, in_game_scrollbar_rect, in_game_thumb_rect
		, iPoint(20, -7), in_game_scrollbar_rect, 20.0f, true, false, false, false, false, false);
	in_game_sfx_scrollbar->parent = in_game_options_parent;

	//screen size
	std::string in_game_resolution_string = "screen";
	in_game_resolution_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 448, 301, in_game_text_rect, in_game_font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, in_game_options_parent, &in_game_resolution_string);

	//Remapping

	//Back button
	SDL_Rect in_game_back_button_size = { 0, 0, 45, 33 };
	SDL_Rect in_game_back_button_idle = { 0, 103, 45, 33 };
	SDL_Rect in_game_back_button_hover = { 57, 103, 45, 33 };
	SDL_Rect in_game_back_button_clicked = { 114, 103, 45, 33 };

	in_game_back_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 430, 445, true, true, false, this, in_game_options_parent
		, &in_game_back_button_idle, &in_game_back_button_hover, &in_game_back_button_clicked);
}

void GameplayScene::OnEventCall(UI* element, UI_EVENT ui_event)
{
	// In_game menu

	if (element == in_game_continue_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Continue
		if (App->pause)
		{
			App->pause = false;
		}

		App->gui->SetElementsVisibility(in_game_background, false);
		App->audio->PlayFx(App->gui->new_game_fx, 0);
	}

	if (element == in_game_options_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Options
		App->audio->PlayFx(App->gui->options_fx, 0);

		App->gui->SetElementsVisibility(in_game_continue_button, false);							// Deactivate Pause Menu
		App->gui->SetElementsVisibility(in_game_options_button, false);
		App->gui->SetElementsVisibility(in_game_exit_button, false);
		App->gui->SetElementsVisibility(in_game_back_to_menu, false);
		App->gui->SetElementsVisibility(in_game_title_text, false);

		App->gui->SetElementsVisibility(in_game_options_parent, true);
	}

	if (element == in_game_back_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui->back_fx, 0);

		App->gui->SetElementsVisibility(in_game_continue_button, true);							// Activate Pause menu
		App->gui->SetElementsVisibility(in_game_options_button, true);
		App->gui->SetElementsVisibility(in_game_exit_button, true);
		App->gui->SetElementsVisibility(in_game_back_to_menu, true);
		App->gui->SetElementsVisibility(in_game_title_text, true);

		App->gui->SetElementsVisibility(in_game_options_parent, false);
	}

	if (element == in_game_back_to_menu && ui_event == UI_EVENT::UNCLICKED)
	{
		if (App->pause)
		{
			App->pause = false;
		}

		// Back to menu
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_SCENE, 0.5f, true, 10, false, true);
		App->audio->PlayFx(App->gui->exit_fx, 0);
	}

	if (element == in_game_exit_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_SCENE, 0.5f, true, 10, false, true);

		// Exit
		escape = false;
	}


	// HUD

	if (element == HUD_group_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Group
		App->player->SelectAllEntities();
		App->audio->PlayFx(App->gui->standard_fx, 0);
	}

	if (element == HUD_pause_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Pause
		App->pause = !App->pause;
		App->audio->PlayFx(App->gui->standard_fx, 0);
		//App->gui->SetElementsVisibility(HUD_pause_button, false);			
		//App->gui->SetElementsVisibility(HUD_play_button, true);	
		//element->ui_event = UI_EVENT::IDLE;
	}
	
	//if (element == HUD_play_button && ui_event == UI_EVENT::UNCLICKED)
	//{
	//	// Play
	//	App->pause = false;
	//	App->audio->PlayFx(App->gui->standard_fx, 0);
	//	App->gui->SetElementsVisibility(HUD_play_button, false);
	//	App->gui->SetElementsVisibility(HUD_pause_button, true);
	//	
	//}

	if (element == HUD_home_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Home
		App->render->camera.x = 1550;
		App->render->camera.y = -600;

		App->audio->PlayFx(App->gui->standard_fx, 0);
	}


	// Townhall bar

	if (element == HUD_unit_townhall && ui_event == UI_EVENT::UNCLICKED)
	{
		// Recruit Unit
		UnitSpawn();
		App->audio->PlayFx(App->gui->recruit_fx, 0);
	}

	if (element == HUD_unit_townhall && ui_event == UI_EVENT::HOVER)
	{
		// Price to recruit
		App->gui->SetElementsVisibility(HUD_parent_resources_unit_townhall_gatherer, true);
		App->gui->SetElementsVisibility(HUD_title_townhall, false);
		App->gui->SetElementsVisibility(HUD_title_gatherer, true);
	}
	if (element == HUD_unit_townhall && ui_event == UI_EVENT::UNHOVER)
	{
		App->gui->SetElementsVisibility(HUD_parent_resources_unit_townhall_gatherer, false);
		App->gui->SetElementsVisibility(HUD_title_townhall, true);
		App->gui->SetElementsVisibility(HUD_title_gatherer, false);
	}

	if (element == HUD_unit_upgrade_townhall_gatherer && ui_event == UI_EVENT::UNCLICKED)
	{
		// Upgrade Unit
		UnitUpgrade();
		App->audio->PlayFx(App->gui->upgrade_fx, 0);
	}

	if (element == HUD_unit_upgrade_townhall_gatherer && ui_event == UI_EVENT::HOVER)
	{
		// Price to upgrade unit
		App->gui->SetElementsVisibility(HUD_parent_resources_upgrade_unit_townhall_gatherer, true);
	}
	if (element == HUD_unit_upgrade_townhall_gatherer && ui_event == UI_EVENT::UNHOVER)
	{
		App->gui->SetElementsVisibility(HUD_parent_resources_upgrade_unit_townhall_gatherer, false);
	}

	if (element == HUD_upgrade_townhall && ui_event == UI_EVENT::UNCLICKED)
	{
		// Upgrade Townhall
		BuildingUpgrade();
		App->audio->PlayFx(App->gui->upgrade_fx, 0);
	}

	if (element == HUD_upgrade_townhall && ui_event == UI_EVENT::HOVER)
	{
		// Price to upgrade townhall
		App->gui->SetElementsVisibility(HUD_parent_resources_upgrade_townhall, true);
	}
	if (element == HUD_upgrade_townhall && ui_event == UI_EVENT::UNHOVER)
	{
		App->gui->SetElementsVisibility(HUD_parent_resources_upgrade_townhall, false);
	}



	// Barracks Bar

	if (element == HUD_unit_barracks && ui_event == UI_EVENT::UNCLICKED)
	{
		// Recruit Unit
		UnitSpawn();
		App->audio->PlayFx(App->gui->recruit_fx, 0);
	}

	if (element == HUD_unit_barracks && ui_event == UI_EVENT::HOVER)
	{
		// Price to recruit
		App->gui->SetElementsVisibility(HUD_parent_resources_unit_barracks_infantry, true);
		App->gui->SetElementsVisibility(HUD_title_barracks, false);
		App->gui->SetElementsVisibility(HUD_title_infantry, true);
	}
	if (element == HUD_unit_barracks && ui_event == UI_EVENT::UNHOVER)
	{
		App->gui->SetElementsVisibility(HUD_parent_resources_unit_barracks_infantry, false);
		App->gui->SetElementsVisibility(HUD_title_barracks, true);
		App->gui->SetElementsVisibility(HUD_title_infantry, false);
	}


	if (element == HUD_unit_upgrade_barracks_infantry && ui_event == UI_EVENT::UNCLICKED)
	{
		// Upgrade Unit
		UnitUpgrade();
		App->audio->PlayFx(App->gui->upgrade_fx, 0);
	}

	if (element == HUD_unit_upgrade_barracks_infantry && ui_event == UI_EVENT::HOVER)
	{
		// Price to upgrade unit
		App->gui->SetElementsVisibility(HUD_parent_resources_upgrade_unit_barracks_infantry, true);
	}
	if (element == HUD_unit_upgrade_barracks_infantry && ui_event == UI_EVENT::UNHOVER)
	{
		App->gui->SetElementsVisibility(HUD_parent_resources_upgrade_unit_barracks_infantry, false);
	}

	if (element == HUD_upgrade_barracks && ui_event == UI_EVENT::UNCLICKED)
	{
		// Upgrade Barracks
		// Code to upgrade barracks
		BuildingUpgrade();
		App->audio->PlayFx(App->gui->upgrade_fx, 0);
	}

	if (element == HUD_upgrade_barracks && ui_event == UI_EVENT::HOVER)
	{
		// Price to upgrade townhall
		App->gui->SetElementsVisibility(HUD_parent_resources_upgrade_barracks, true);
	}
	if (element == HUD_upgrade_barracks && ui_event == UI_EVENT::UNHOVER)
	{
		App->gui->SetElementsVisibility(HUD_parent_resources_upgrade_barracks, false);
	}
}

void GameplayScene::AdjustVolumeWithScrollbar()
{
	// --- Audio Scrollbars
	if (in_game_music_scrollbar != nullptr)
	{
		float local_thumb_pos = in_game_music_scrollbar->GetThumbHitbox().x - in_game_music_scrollbar->GetHitbox().x;

		float offset = local_thumb_pos / in_game_music_scrollbar->GetHitbox().w;											// Value from 0.0f to 1.0f


		App->audio->volume = offset * 100;																					// Will make the offset a valid value to modify the volume.
	}

	if (in_game_sfx_scrollbar != nullptr)
	{
		float local_thumb_pos = in_game_sfx_scrollbar->GetThumbHitbox().x - in_game_sfx_scrollbar->GetHitbox().x;

		float start_offset = local_thumb_pos / in_game_sfx_scrollbar->GetHitbox().w;										// Value from 0.0f to 1.0f

		uint offset = floor(start_offset * 100);																			// Will make the offset a valid value to modify the volume.					

		App->audio->volume_fx = offset;
	}
}

void GameplayScene::ExecuteTransition()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::LOGO_SCENE, 0.5f, true, 8, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_SCENE, 0.5f, true, 10, false, true);
	}

	// No KP_3 because we are in the 3rd scene.

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->transition_manager->CreateSlide(SCENES::WIN_SCENE, 0.5f, true, true, false, false, Black);
		
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		App->transition_manager->CreateSlide(SCENES::LOSE_SCENE, 0.5f, true, true, false, false, Black);
	}
}

void GameplayScene::UnitDebugKeys()
{
	if (App->player->god_mode)
	{
		if (App->pathfinding->IsWalkable(iPoint(App->player->mouse_tile.x, App->player->mouse_tile.y)))
		{
			if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
			{
				(Gatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::GATHERER, App->player->mouse_tile.x, App->player->mouse_tile.y, 1);
			}
			if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
			{
				(Infantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::INFANTRY, App->player->mouse_tile.x, App->player->mouse_tile.y, 1);
			}
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			{
				(Enemy*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY, App->player->mouse_tile.x, App->player->mouse_tile.y, 1);
			}


			if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
			{
				(TownHall*)App->entity_manager->CreateEntity(ENTITY_TYPE::TOWNHALL, App->player->mouse_tile.x, App->player->mouse_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
			{
				(EnemyTownHall*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_TOWNHALL, App->player->mouse_tile.x, App->player->mouse_tile.y);
			}

			if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
			{
				(Barracks*)App->entity_manager->CreateEntity(ENTITY_TYPE::BARRACKS, App->player->mouse_tile.x, App->player->mouse_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
			{
				(EnemyBarracks*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_BARRACKS, App->player->mouse_tile.x, App->player->mouse_tile.y);
			}


			if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			{
				(Rock*)App->entity_manager->CreateEntity(ENTITY_TYPE::ROCK, App->player->mouse_tile.x, App->player->mouse_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
			{
				(Tree*)App->entity_manager->CreateEntity(ENTITY_TYPE::TREE, App->player->mouse_tile.x, App->player->mouse_tile.y, 1);
			}
			if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
			{
				App->entity_manager->resource_data += 300;
				App->entity_manager->resource_electricity += 300;
			}
		}
	}
}

void GameplayScene::PathfindingDebug()
{
	if (App->map->pathfinding_meta_debug)
	{
		static iPoint origin;
		static bool origin_selected = false;

		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->map->WorldToMap(p.x, p.y);

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			if (origin_selected == true)
			{
				App->pathfinding->CreatePath(origin, p);
				LOG("Tiles selected: (%d, %d) and (%d, %d)", origin.x, origin.y, p.x, p.y);
				origin_selected = false;
			}
			else
			{
				origin = p;
				origin_selected = true;
			}
		}
	}
}

void GameplayScene::DrawPathfindingDebug()
{
	if (App->map->pathfinding_meta_debug)
	{
		// Draw pathfinding debug tiles ------------------------------
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->map->WorldToMap(p.x, p.y);
		p = App->map->MapToWorld(p.x, p.y);

		App->render->Blit(path_debug_tex, p.x, p.y);								//Should we want it, we could make a separate texture called mouse_debug_tex so the tex at mouse pos and the tex at path tile are different.

		const std::vector<iPoint> path = App->pathfinding->GetLastPath();

		for (uint i = 0; i < path.size(); ++i)
		{
			iPoint pos = App->map->MapToWorld(path.at(i).x, path.at(i).y);		//Both work, reach a consensus on which to use.
			//iPoint pos = App->map->MapToWorld((*path)[i].x, (*path)[i].y);

			App->render->Blit(path_debug_tex, pos.x, pos.y);
		}
	}
}

// ------------------- ENTITY HUD METHODS -------------------

void GameplayScene::DebugHUDSpawn()
{
	if (App->player->building_selected != nullptr)
	{
		switch (App->player->building_selected->type)
		{
		case ENTITY_TYPE::TOWNHALL:
			App->gui->SetElementsVisibility(HUD_barracks_bar, false);
			if (!HUD_townhall_bar->isVisible)
			{
				App->audio->PlayFx(App->entity_manager->click_townhall_fx, 0);
				App->gui->SetElementsVisibility(HUD_townhall_bar, true);
			}

			break;

		case ENTITY_TYPE::ENEMY_TOWNHALL:

			break;

		case ENTITY_TYPE::BARRACKS:
			App->gui->SetElementsVisibility(HUD_townhall_bar, false);
			if (!HUD_barracks_bar->isVisible)
			{
				App->audio->PlayFx(App->entity_manager->click_barracks_fx, 0);
				App->gui->SetElementsVisibility(HUD_barracks_bar, true);

				if (HUD_townhall_bar->isVisible)
				{
					!HUD_barracks_bar->isVisible;
				}
			}

			break;

		case ENTITY_TYPE::ENEMY_BARRACKS:

			break;
		}
	}
	else
	{
		if (HUD_townhall_bar->isVisible)
		{
			App->gui->SetElementsVisibility(HUD_townhall_bar, false);
		}
		if (HUD_barracks_bar->isVisible)
		{
			App->gui->SetElementsVisibility(HUD_barracks_bar, false);
		}
	}
}

// ------------------- ENTITY SPAWN METHODS -------------------

void GameplayScene::UnitSpawn()
{
	if (App->player->building_selected != nullptr)
	{
		TownHall* townhall = nullptr;
		Barracks* barrack = nullptr;

		switch (App->player->building_selected->type)
		{
		case ENTITY_TYPE::TOWNHALL:

			if (CheckResources(20, 0))
			{
				townhall = (TownHall*)App->player->building_selected;
				townhall->GenerateUnit(ENTITY_TYPE::GATHERER, townhall->unit_level);
			}
			break;

		case ENTITY_TYPE::BARRACKS:
			if (CheckResources(0, 10))
			{
				barrack = (Barracks*)App->player->building_selected;
				barrack->GenerateUnit(ENTITY_TYPE::INFANTRY, barrack->unit_level);
			}
			break;
		}
	}
}

void GameplayScene::BuildingUpgrade()
{
	if (App->player->building_selected != nullptr)
	{
		TownHall* townhall = nullptr;
		Barracks* barrack = nullptr;

		switch (App->player->building_selected->type)
		{
		case ENTITY_TYPE::TOWNHALL:

			if (CheckResources(200, 175))
			{
				townhall = (TownHall*)App->player->building_selected;
				townhall->level++;
				townhall->LevelChanges();
			}
			break;

		case ENTITY_TYPE::BARRACKS:

			if (CheckResources(170, 120))
			{
				barrack = (Barracks*)App->player->building_selected;
				barrack->level++;
				barrack->LevelChanges();
			}
			break;
		}
	}

}



void GameplayScene::UnitUpgrade()
{
	if (App->player->building_selected != nullptr)
	{
		TownHall* townhall = nullptr;
		Barracks* barrack = nullptr;

		switch (App->player->building_selected->type)
		{
		case ENTITY_TYPE::TOWNHALL:
			if (CheckResources(100, 50))
			{
				townhall = (TownHall*)App->player->building_selected;
				townhall->unit_level++;
			}
			break;

		case ENTITY_TYPE::BARRACKS:
			if (CheckResources(50, 100))
			{
				barrack = (Barracks*)App->player->building_selected;
				barrack->unit_level++;
			}
			break;
		}
	}
}


//--------------- RESOURCE MANAGEMENT ---------------


bool GameplayScene::CheckResources(uint required_data, uint required_electricity)
{
	if ((required_data <= App->entity_manager->resource_data) && (required_electricity <= App->entity_manager->resource_electricity))
	{
		App->entity_manager->resource_data -= required_data;
		App->entity_manager->resource_electricity -= required_electricity;
		return true;
	}
	else
	{
		return false;
	}
}


// --------------- REVISE IF THEY ARE NEEDED ---------------
//bool Scene1::Load(pugi::xml_node& data)
//{
//	if (currentMap != data.child("currentMap").attribute("num").as_int())
//	{
//		LOG("Calling switch maps");
//		currentMap = data.child("currentMap").attribute("num").as_int();
//
//		//std::list<std::string>::iterator map_iterator = map_names.begin();
//
//		//std::advance(map_iterator, data.child("currentMap").attribute("num").as_int() );
//
//		//App->map->SwitchMaps( (*map_iterator) );
//	}
//	return true;
//}

//bool Scene1::Save(pugi::xml_node& data) const
//{
//	data.append_child("currentMap").append_attribute("num") = currentMap;
//	return true;
//}