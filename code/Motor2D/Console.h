#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "Module.h"

struct _TTF_Font;

class UI;
class UI_Image;
class UI_Text;
class UI_InputBox;
class UI_Scrollbar;

class Command
{
public:
	Command(const char* command, Module* callback, int min_arg, int max_arg);
	
	std::string command;	//"map iso.tmx" --> The string is separated at the space and the first string will be the name of the command
							//and the second one are the number of arguments that the command has (speed would have 1 argument).
	uint n_arguments;

	int min_arg;
	int max_arg;

	Module* callback;
};

class Console : public Module
{
public:
	Console();

	~Console();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

public:
	void InitConsole();
	void CreateConsoleElements();
	void CreateConsoleCommands();
	void DrawBackgroundElement();
	bool ConsoleIsOpen();
	Command* CreateCommand(const char* command, Module* callback, int min_arg, int max_arg);
	void OnCommand(const char* command, const char* subCommand = nullptr);
	//void DrawConsoleElements();

public:
	UI_Image*			console_background;
	UI_Text*			console_output;
	UI_InputBox*		console_input;
	UI_Scrollbar*		console_scroll;

	iPoint				bg_position;
	SDL_Rect			bg_rect;
	SDL_Color			bg_colour;
	bool				bg_isVisible;
	bool				bg_isInteractible;
	bool				bg_isDraggable;

	iPoint				output_position;
	SDL_Rect			output_rect;
	std::string			output_font_path;
	_TTF_Font*			output_font;
	int					output_font_size;
	SDL_Color			output_font_colour;
	bool				output_isVisible;
	bool				output_isInteractible;
	bool				output_isDraggable;

	iPoint				input_position;
	SDL_Rect			input_rect;
	std::string			input_font_path;
	_TTF_Font*			input_font;
	int					input_font_size;
	SDL_Color			input_font_colour;
	SDL_Rect			cursor_rect;
	SDL_Color			cursor_colour;
	iPoint				input_text_offset;
	float				cursor_blinkFrequency;
	bool				input_isVisible;
	bool				input_isInteractible;
	bool				input_isDraggable;

	iPoint				scroll_position;
	SDL_Rect			scrollbar_rect;
	SDL_Rect			thumb_rect;
	iPoint				thumb_offset;
	SDL_Rect			drag_area;
	float				drag_factor;
	bool				drag_x_axis;
	bool				drag_y_axis;
	bool				inverted_scrolling;
	bool				scroll_isVisible;
	bool				scroll_isInteractible;
	bool				scroll_isDraggable;


	bool				runGame;
	bool				commandWasFound;
	std::list<Command*>	commands;
	std::list<Command*>	commandHistory;

private:
	pugi::xml_document	config_file;
	pugi::xml_node		console;

	// Console Strings
	const char*			command_list;
	const char*			enable_pause;
	const char*			disable_pause;
	const char*			enableFrameCap;
	const char*			disableFrameCap;
	const char*			resetFrameCap;
	const char*			FPS_30;
	const char*			FPS_45;
	const char*			FPS_60;
	const char*			FPS_90;
	const char*			FPS_120;

	const char*			enable_god_mode;
	const char*			disable_god_mode;
};

#endif // !__CONSOLE_H__