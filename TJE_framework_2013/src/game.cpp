#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

Mesh* spitfire = NULL;
Mesh* terrain = NULL;
Texture* tex_terrain = NULL;
Texture* tex_spitfire = NULL;
Matrix44 m_player;

float time_elapsed;
float time;
const float UA = 1500.0; // 10^5 km

float angle = 0;

Game::Game()
{
	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl

	keystate = NULL;
	mouse_locked = false;
}

//Here we have already GL working, so we can create meshes and textures
void Game::init(void)
{
	
	std::cout << "initiating game..." << std::endl;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glEnable( GL_BLEND );


	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0,250,250),Vector3(0,0,0), Vector3(0,1,0)); //position the camera and point to 0,0,0
	camera->setPerspective(70,WINDOW_WIDTH/(float)WINDOW_HEIGHT,0.1,10000); //set the projection, we want to be perspective

	spitfire = new Mesh();
	spitfire->loadASE("spitfire/spitfire.ASE");

	tex_spitfire = new Texture();
	tex_spitfire->load("spitfire/spitfire_color_spec.tga");
	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	m_player.setTranslation(0,400,0);
	//m_player.rotateLocal(90*DEG2RAD, Vector3(1,0,0));

	terrain = new Mesh();
	terrain->loadASE("terrain/terrain.Ase");
	tex_terrain = new Texture();
	tex_terrain->load("terrain/terrain.tga");

}


//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	camera->center = m_player * Vector3();
    camera->eye = (camera->eye*0.75)+(m_player*Vector3(0,15,3)*0.25); 
    camera->eye = ((camera->eye- camera->center).normalize()*10) + camera->center;
    camera->up = camera->up * 0.9 + m_player.rotateVector(Vector3(0,0,1))*0.1;
	
	/*
	camera->center = m_player * Vector3(0,1,0);
    camera->eye = (camera->eye*0.75)+(m_player*Vector3(15,0,3)*0.25); 
    camera->eye = ((camera->eye- camera->center).normalize()*10) + camera->center;
    camera->up = camera->up * 0.9 + m_player.rotateVector(Vector3(0,0,1))*0.1;
	*/
	//Put the camera matrices on the stack
	camera->set();
	
	Matrix44 m_terrain;

	glPushMatrix();
	m_player.set();
	glColor3f(1,1,1);
		tex_spitfire->bind();
	
	spitfire->render();
	tex_spitfire->unbind();
	glPopMatrix();

	glPushMatrix();
	m_terrain.setRotation(90*DEG2RAD, Vector3(1,0,0));
	m_terrain.set();
	tex_terrain->bind();
	terrain->render();
	tex_terrain->unbind();
	glPopMatrix();


	//swap between front buffer and back buffer
	SDL_GL_SwapBuffers();
}

void Game::update(double seconds_elapsed)
{
	time_elapsed = (float) seconds_elapsed;
	time += (float)seconds_elapsed;

	//to navigate with the mouse fixed in the middle
	
	if (mouse_locked)
	{
		SDL_WarpMouse(WINDOW_WIDTH*0.5, WINDOW_HEIGHT*0.5); //put the mouse back in the middle of the screen
		this->mouse_position.x = WINDOW_WIDTH*0.5;
		this->mouse_position.y = WINDOW_HEIGHT*0.5;
	}

	
	double speed = seconds_elapsed * 1000; //the speed is defined by the seconds_elapsed so it goes constant

	//mouse input to rotate the cam
	
	//if ((mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	//{
		//m.rotateLocal(speed * mouse_delta.x * 0.001, Vector3(0,0,-1));
		//m.rotateLocal(speed * mouse_delta.y * 0.001, Vector3(-1,0,0));
		m_player.rotateLocal((mouse_position.y-WINDOW_HEIGHT/2)/WINDOW_HEIGHT*0.02, Vector3(-1,0,0));
		m_player.rotateLocal((mouse_position.x-WINDOW_WIDTH/2)/WINDOW_WIDTH*0.02, Vector3(0,1,0));
	//}
	

	//async input to move the camera around
	if(keystate[SDLK_LSHIFT])
		speed *= 10;
	if (keystate[SDLK_w]) m_player.traslateLocal(0,-speed*0.1,0);
	//if (keystate[SDLK_s]) m.traslateLocal(0,speed,0);
	if (keystate[SDLK_a]) m_player.rotateLocal(2*time_elapsed, Vector3(0,-1,0));
	if (keystate[SDLK_d]) m_player.rotateLocal(-2*time_elapsed, Vector3(0,-1,0));

	angle += seconds_elapsed * 10;
}

//Keyboard event handler (sync input)
void Game::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0);
	}
}


void Game::onMouseButton( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE)
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}
