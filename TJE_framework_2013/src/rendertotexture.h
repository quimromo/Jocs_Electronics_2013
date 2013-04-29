/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This allows to render the scene to a texture instead of the screen, it is used to do special FX.
*/

#ifndef RENDER_TO_TEXTURE
#define RENDER_TO_TEXTURE

#include <string>
#include <map>
#include "includes.h"
#include "framework.h"


class RenderToTexture
{
public:
	int width;
	int height;
	bool generate_mipmaps;

	GLuint fbo;
	GLuint depthbuffer;
	GLuint texture_id;

	static bool init();

	RenderToTexture();
	~RenderToTexture();

	bool create(int width, int height, bool generate_mipmaps = false);
	void bind();
	void unbind();

	void bindTexture();
	void generateMipmaps();
};

#endif