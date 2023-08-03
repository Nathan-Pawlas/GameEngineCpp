#pragma once
#include <d3d11.h>
#include "RenderSystem.h"
#include "Prerequisites.h"

class GraphicsEngine
{
public:
	GraphicsEngine();
	bool init();
	bool release();
	~GraphicsEngine();
	RenderSystem* getRenderSystem();
	static GraphicsEngine* get();

private:
	RenderSystem* m_render_system = nullptr;
};

