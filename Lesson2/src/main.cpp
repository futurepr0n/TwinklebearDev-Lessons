#include <string>
#include <iostream>
#if defined(_MSC_VER)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

/**
* Lesson 2: Don't Put Everything in Main
*/
//Screen attributes
const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

/**
* Log an SDL error with some error message to the output stream of our choice
* @param os The output stream to write the message too
* @param msg The error message to write, format will be msg error: SDL_GetError()
*/
void logSDLError(std::ostream &os, const std::string &msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}
/**
* Loads a BMP image into a texture on the rendering device
* @param file The BMP image file to load
* @param ren The renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren){
	SDL_Texture *texture = nullptr;
	//Load the image
	SDL_Surface *loadedImage = SDL_LoadBMP(file.c_str());
	//If the loading went ok, convert to texture and return the texture
	if (loadedImage != nullptr){
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);
		SDL_FreeSurface(loadedImage);
		//Make sure converting went ok too
		if (texture == nullptr)
			logSDLError(std::cout, "CreateTextureFromSurface");
	}
	else
		logSDLError(std::cout, "LoadBMP");

	return texture;
}
/*
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param x The x coordinate to draw too
* @param y The y coordinate to draw too
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw too
*/
void renderTexture(int x, int y, SDL_Texture *tex, SDL_Renderer *rend){
	//First we must create an SDL_Rect for the position of the image, as SDL
	//won't accept raw coordinates as the image's position
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	//We also need to query the texture to get its width and height to use
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(rend, tex, NULL, &pos);
}

int main(int argc, char** argv){
	//Start up SDL and make sure it went ok
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}

	//Setup our window and renderer
	SDL_Window *window = SDL_CreateWindow("Lesson 2", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr){
		logSDLError(std::cout, "CreateWindow");
		return 2;
	}
	
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr){
		logSDLError(std::cout, "CreateRenderer");
		return 3;
	}

	//The textures we'll be using
	SDL_Texture *background = loadTexture("../res/Lesson2/background.bmp", renderer);
	SDL_Texture *image = loadTexture("../res/Lesson2/image.bmp", renderer);
	//Make sure it went ok
	if (background == nullptr || image == nullptr)
		return 4;

	//Clear the window
	SDL_RenderClear(renderer);

	//Get the width and height from the texture so we know how much to move x,y by
	//to tile it correctly
	int bW, bH;
	SDL_QueryTexture(background, NULL, NULL, &bW, &bH);
	//We want to tile our background so draw it 4 times
	renderTexture(0, 0, background, renderer);
	renderTexture(bW, 0, background, renderer);
	renderTexture(0, bH, background, renderer);
	renderTexture(bW, bH, background, renderer);

	//Draw our image in the center of the window
	//We also need its width so query it as well
	int iW, iH;
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
	int x = SCREEN_WIDTH / 2 - iW / 2;
	int y = SCREEN_HEIGHT / 2 - iH / 2;
	renderTexture(x, y, image, renderer);

	//Update the screen
	SDL_RenderPresent(renderer);
	SDL_Delay(2000);

	//Destroy the various items
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	
	return 0;
}
