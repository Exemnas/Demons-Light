// Beginning Game Programming, Third Edition
// MyGame.cpp

#include "MyDirectX.h"
#include <sstream>
using namespace std;

const string APPTITLE = "Demon's Light!           Hold SPACE for instructions.";
const int SCREENW = 1200;
const int SCREENH = 1100;

SPRITE seth, darkkeeper;
LPDIRECT3DTEXTURE9 imgseth = NULL;
LPDIRECT3DTEXTURE9 imgdarkkeeper = NULL;
LPDIRECT3DSURFACE9 surface = NULL;
LPDIRECT3DTEXTURE9 imgdarkness_energy = NULL;
LPDIRECT3DSURFACE9 background = NULL;

LPD3DXFONT font;

D3DCOLOR color = D3DCOLOR_XRGB(255,242,0);
float scale = 0.001f;
float r = 0;
float s = 1.0f;
int spritex = 10;
int spritey = 750;
int energyx = spritex += 200;
int energyy = spritey -= 100;
const int BUFFERW = SCREENW * 2;
const int BUFFERH = SCREENH * 2;
double scrollx = 0, scrolly = 0;

int frame = 0, columns, width, height;
int startframe, endframe, starttime = 0, delay;
int space = 0;
int score = 0;

CSound *sound_bounce = NULL;


bool Game_Init(HWND window)
{
    //initialize Direct3D
    if (!Direct3D_Init(window, SCREENW, SCREENH, false)) {MessageBox(0, "Error initializing Direct3D","ERROR",0); return false;}
	DirectInput_Init(window);
	if (!DirectSound_Init(window)) return false;

	font = MakeFont("Arial", 24);

	//load background
	LPDIRECT3DSURFACE9 image = NULL;
	image = LoadSurface("cityscape.bmp");
	if (!image) return false;

	//set properties for sprites
	seth.x = 10;
	seth.y = 750;
	seth.width = seth.height = 200;
	seth.frame = 0;
	seth.starttime = 0;
	darkkeeper.x = 2000;
	darkkeeper.y = 400;
	darkkeeper.width = 250;
	darkkeeper.height = 156;
	darkkeeper.velx = 1.5f;
	

	//create background
	HRESULT bresult = d3ddev->CreateOffscreenPlainSurface(BUFFERW, BUFFERH, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &background, NULL);

	//copy image to upper left corner of background
	RECT source_rect = {0, 0, SCREENW, SCREENH};
	RECT dest_ul = {0, 0, SCREENW, SCREENH};
	d3ddev->StretchRect(image, &source_rect, background, &dest_ul, D3DTEXF_NONE);

	//copy image to upper right corner of background
	RECT dest_ur = {SCREENW, 0, SCREENW*2, SCREENH};
	d3ddev->StretchRect(image, &source_rect, background, &dest_ur, D3DTEXF_NONE);

	//copy image to lower left corner
	RECT dest_ll = {0, SCREENH, SCREENW*2, SCREENH*2};
	d3ddev->StretchRect(image, &source_rect, background, &dest_ll, D3DTEXF_NONE);

	//copy image to lower right corner
	RECT dest_lr = {SCREENW, SCREENH, SCREENW*2, SCREENH*2};
	d3ddev->StretchRect(image, &source_rect, background, &dest_lr, D3DTEXF_NONE);

	//get pointer to the back buffer
	d3ddev->GetBackBuffer(0,0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	
	//remove scratch image
	image->Release();

    //initialize DirectInput
    if (!DirectInput_Init(window))
    {
        MessageBox(0, "Error initializing DirectInput","ERROR",0);
        return false;
    }

	sound_bounce = LoadSound("step.wav");

    //load SPRITE
    imgseth = LoadTexture("SpriteSheet2.bmp", color);
	imgdarkkeeper = LoadTexture("darkkeeper.bmp", D3DCOLOR_XRGB(255,242,0));
	imgdarkness_energy = LoadTexture("energy.png", D3DCOLOR_XRGB(0,0,0));



    return true;
}

void Game_Run(HWND window)
{
    //make sure the Direct3D device is valid
    if (!d3ddev) return;

    //update input devices
    DirectInput_Update();

    //clear the scene
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,100), 1.0f, 0);

	//Keep scrolling within boundary
	if (scrolly < 0)
		scrolly = BUFFERH - SCREENH;
	if (scrolly > BUFFERH - SCREENH)
		scrolly = 0;
	if (scrollx < 0)
		scrollx = BUFFERH - SCREENW;
	if (scrollx > BUFFERH - SCREENW)
		scrollx = 0;
	
	//Move darkkeeper
	darkkeeper.x -= darkkeeper.velx;
	if (darkkeeper.x < -1000) {darkkeeper.x = 2000; darkkeeper.y = (float)( rand() % SCREENH - 200);}

	

    //start rendering
	if (d3ddev->BeginScene())
    {
		RECT source_rect = {scrollx, scrolly, scrollx + SCREENW, scrolly + SCREENH};
		RECT dest_rect = {0, 0, SCREENW, SCREENH};

		d3ddev->StretchRect(background, &source_rect, backbuffer, &dest_rect, D3DTEXF_NONE);

		//start drawing
        spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

		std::ostringstream oss;
		oss << score;
		FontPrint(font, 100, -20, oss.str());

		//set animation properties
		columns = 4;
		width = height = 200;
		s += scale;
		if (s < 0.1 || s > 1.25f) scale *= -1;

		//animate and draw the sprite
		Sprite_Animate(seth.frame, 0, 1, 1, seth.starttime, 200);
		Sprite_Transform_Draw(imgseth, seth.x, seth.y, seth.width, seth.height, seth.frame, seth.columns, 0, s, D3DCOLOR_XRGB(255,255,255));
		Sprite_Transform_Draw(imgdarkkeeper, darkkeeper.x, darkkeeper.y, darkkeeper.width, darkkeeper.height, darkkeeper.frame, darkkeeper.columns, 0, s, D3DCOLOR_XRGB(255,255,255));

        //stop drawing
        spriteobj->End();

		//stop rendering
        d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
    }

	//collision
	if (Collision(seth, darkkeeper)){PlaySound(sound_bounce); score++;
	darkkeeper.x = 2000; darkkeeper.y = (float)( rand() % SCREENH - 200 );
	}
	

    //Escape key ends program
	//KEY esc
	if (KEY_DOWN(VK_ESCAPE)) gameover = true;

	//Story
	//KEY S
	if (KEY_DOWN(0x53))MessageBox(0, "Hey", "Menu", 0);

	
	
	//MOVEMENT
	//KEY right arrow/left arrow
	if (KEY_DOWN(VK_RIGHT)){ spritex += 1.0f; scrollx += 1; seth.x += 1.0f;}
	else if (KEY_DOWN(VK_LEFT)){ spritex -= 1.0f; scrollx -= 1; seth.x -= 1.0f;}
	if (KEY_DOWN(VK_UP)){spritey -= 2.0f; seth.y -= 2.0f;}
	else if (KEY_DOWN(VK_DOWN)){spritey += 2.0f; seth.y += 2.0f;}


	//movement limiter
	if (spritex < 0) spritex = 0;
	if (spritex > SCREENW - 500) spritex = SCREENW - 500;

	if (spritey < 0) spritey = 0;
	if (spritey > SCREENH - 500) spritey = SCREENH - 500;

	if (seth.x < 0) seth.x = 0;
	if (seth.x > SCREENW - 500) seth.x = SCREENW - 500;
	
	if (seth.y < 0) seth.y = 0;
	if (seth.y > SCREENH - 400) seth.y = SCREENH - 400;
		

	//Credits
	//KEY enter
	if (KEY_DOWN(0x43)){MessageBox(0, "Created, Directed, and Produced by Steven Adcock.", "Credits", 0);}

	//Menu
	//KEY space bar
	if (KEY_DOWN(VK_SPACE)){MessageBox(0, "Arrow keys = movement   C = Credits", "Menu", 0);}
	
        
	DirectInput_Update();

}

void Game_End()
{
    //free memory and shut down
    imgseth->Release();

	DirectInput_Shutdown();
    Direct3D_Shutdown();
}