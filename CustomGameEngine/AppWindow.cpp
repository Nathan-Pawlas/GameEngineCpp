#include "AppWindow.h"
#include <Windows.h>
#include "Vec3.h"
#include "Vec2.h"
#include "Mat4.h"
#include "InputSystem.h"
#include "Mesh.h"
#include <stdlib.h>

int status = 0;

struct vertex
{
	Vec3 position;
	Vec2 texcoord;
};


__declspec(align(16))
struct constant
{
	Mat4 m_world;
	Mat4 m_view;
	Mat4 m_proj;
	Vec4 m_light_dir;
	Vec4 m_camera_pos;
};


AppWindow::AppWindow()
{
}


AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{

	Window::onCreate();

	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);

	m_world_cam.setTranslation(Vec3(0, 0, -1));

	// ------------ RESOURCE RETRIEVAL ------------

	m_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\brick.png");
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\statue.obj");

	m_sky_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\sky.jpg");
	m_sky_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\sphere.obj");

	// ------------ WINDOW ---------------
	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	// ----------------- VERTEX SHADER -----------------------------------
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	// ------------------ PIXEL SHADER -----------------------------------
	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SkyBoxShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_sky_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	// ----------------- CONSTANT BUFFER --------------------------------
	constant cc;
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
	m_sky_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();

	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0.2f, 0.3f, 0.5f, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	//COMPUTE TRANSFORM MATRICES
	update();

	//RENDER MODELS AND SKYBOX
	GraphicsEngine::get()->getRenderSystem()->setRasterizerState(false);
	drawMesh(m_mesh, m_vs, m_ps, m_cb, m_tex);

	GraphicsEngine::get()->getRenderSystem()->setRasterizerState(true);
	drawMesh(m_sky_mesh, m_vs, m_sky_ps, m_sky_cb, m_sky_tex);

	m_swap_chain->present(true);


	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void AppWindow::onKeyDown(int key)
{
	if (key == 'W')
	{
		m_forward = 1.0f;
	}
	else if (key == 'S')
	{
		m_forward = -1.0f;
	}
	else if (key == 'A')
	{
		m_right = -1.0f;
	}
	else if (key == 'D')
	{
		m_right = 1.0f;
	}
	else if (key == VK_SHIFT)
	{
		m_cam_speed = 0.08f;
	}
	else if (key == 'Q')
	{
		exit(status);
	}
}

void AppWindow::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_right = 0.0f;
	m_cam_speed = 0.03f;
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	m_rot_x += (mouse_pos.y - (height / 2)) * m_delta_time*0.1f;
	m_rot_y += (mouse_pos.x - (width / 2)) * m_delta_time*0.1f;

	InputSystem::get()->setCursorPosition(Point(width / 2.0f, height / 2.0f));
}

void AppWindow::onLeftMouseDown(const Point& mouse_pos)
{
	m_scale += 0.25f;
}

void AppWindow::onRightMouseDown(const Point& mouse_pos)
{
	m_scale -= 0.25f;
}

void AppWindow::onLeftMouseUp(const Point& mouse_pos)
{
}

void AppWindow::onRightMouseUp(const Point& mouse_pos)
{
}

void AppWindow::update()
{
	updateCamera();
	updateModel();
	updateSkybox();
}

void AppWindow::updateModel()
{
	constant cc;

	Mat4 m_light_rot;

	m_light_rot.setIdentity();
	 
	m_light_rot_y += 0.785f * m_delta_time;
	m_light_rot.setRotationY(m_light_rot_y);

	cc.m_world.setIdentity();
	cc.m_view = m_view;
	cc.m_proj = m_proj_view;
	cc.m_camera_pos = m_world_cam.getTranslation();
	cc.m_light_dir = m_light_rot.getZDirection();
	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

void AppWindow::updateCamera()
{
	// ------------------- CAMERA MOVEMENT --------------
	Mat4 world_cam, temp;
	world_cam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	world_cam *= temp;


	Vec3 new_pos = m_world_cam.getTranslation() + (world_cam.getZDirection() * (m_forward * m_cam_speed));

	new_pos = new_pos + (world_cam.getXDirection() * (m_right * m_cam_speed));

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse(); //Turns Camera Matrix Into View Matrix

	m_view = world_cam;

	// ----------------- SET PROJECTION PERSPECTIVE ------------
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);


	//Orthographic
	/*cc.m_proj.setOrthoLH
	(
		(width) / 300.0f,
		(height) / 300.0f,
		-4.0f,
		4.0f
	);*/

	//Perspective
	m_proj_view.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);
}

void AppWindow::updateSkybox()
{
	constant cc;

	cc.m_world.setIdentity();
	cc.m_world.setScale(Vec3(100.0f, 100.0f, 100.0f));
	cc.m_world.setTranslation(m_world_cam.getTranslation());
	cc.m_view = m_view;
	cc.m_proj = m_proj_view;
	cc.m_camera_pos = m_world_cam.getTranslation();
	m_sky_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

void AppWindow::drawMesh(const MeshPtr& mesh, const VertexShaderPtr& vs, const PixelShaderPtr& ps, ConstantBufferPtr& cb, const TexturePtr& tex)
{
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(vs, cb);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(ps, cb);

	//SET DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(ps);

	//TEXTURES
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(ps, tex);

	//SET THE VERTICES OF THE Mesh TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(mesh->getVertexBuffer());
	//SET INDICES
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(mesh->getIndexBuffer());

	// FINALLY DRAW THE TRIANGLE
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);
}
