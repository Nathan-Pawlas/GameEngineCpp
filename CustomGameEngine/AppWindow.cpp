#include "AppWindow.h"
#include <Windows.h>
#include "Vec3.h"
#include "Mat4.h"
#include "InputSystem.h"

struct vertex
{
	Vec3 position;
	Vec3 color;
	Vec3 color1;
};


__declspec(align(16))
struct constant
{
	Mat4 m_world;
	Mat4 m_view;
	Mat4 m_proj;
	unsigned int m_time;
};


AppWindow::AppWindow()
{
}

void AppWindow::update()
{
	// ------------- GET DELTA TIME --------------------
	constant cc;
	cc.m_time = ::GetTickCount();

	m_delta_pos += m_delta_time / 10.0f;
	if (m_delta_pos > 1.0f)
		m_delta_pos = 0;

	Mat4 temp;

	m_delta_scale += m_delta_time / 0.5f;

	cc.m_world.setIdentity();

	// ------------------- CAMERA MOVEMENT --------------
	Mat4 world_cam;
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

	// ----------------- SET PROJECTION PERSPECTIVE ------------
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	cc.m_view = world_cam;

	//Orthographic
	/*cc.m_proj.setOrthoLH
	(
		(width) / 300.0f,
		(height) / 300.0f,
		-4.0f,
		4.0f
	);*/

	//Perspective
	cc.m_proj.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);

	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}


AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{

	Window::onCreate();

	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);

	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	m_world_cam.setTranslation(Vec3(0, 0, -2));

	vertex vertex_list[] =
	{
		//FRONT FACE
		{Vec3(-0.5f,-0.5f,-0.5f),   Vec3(1,  0,  0),   Vec3(1,  0,  0)}, //v0
		{Vec3(-0.5f, 0.5f,-0.5f),   Vec3(0,  1,  0),   Vec3(0,  1,  0)}, //v1
		{Vec3( 0.5f, 0.5f,-0.5f),   Vec3(0,  0,  1),   Vec3(0,  0,  1)}, //v2
		{Vec3( 0.5f,-0.5f,-0.5f),   Vec3(1,  1,  0),   Vec3(1,  1,  0)}, //v3
		//BACK FACE
		{Vec3( 0.5f,-0.5f, 0.5f),   Vec3(1,  0,  1),   Vec3(1,  0,  1)}, //v4
		{Vec3( 0.5f, 0.5f, 0.5f),   Vec3(0,  1,  1),   Vec3(0,  1,  1)}, //v5
		{Vec3(-0.5f, 0.5f, 0.5f),   Vec3(0,  0,  1),   Vec3(0,  0,  1)}, //v6
		{Vec3(-0.5f,-0.5f, 0.5f),   Vec3(0,  1,  0),   Vec3(0,  1,  0)}  //v7
	};
	UINT size_vertex_list = ARRAYSIZE(vertex_list);

	// ----------------- INDEX BUFFER ------------------------------------
	unsigned int index_list[] =
	{
		//FRONT
		0,1,2,
		2,3,0,
		//BACK
		4,5,6,
		6,7,4,
		//TOP
		1,6,5,
		5,2,1,
		//BOTTOM
		7,0,3,
		3,4,7,
		//RIGHT
		3,2,5,
		5,4,3,
		//LEFT
		7,6,1,
		1,0,7
	};
	UINT size_index_list = ARRAYSIZE(index_list);
	m_ib = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(index_list, size_index_list);


	// ----------------- VERTEX SHADER -----------------------------------
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	m_vb = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(vertex_list, sizeof(vertex), size_vertex_list, shader_byte_code, size_shader);
	
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	// ------------------ PIXEL SHADER -----------------------------------
	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);

	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	// ----------------- CONSTANT BUFFER --------------------------------
	constant cc;
	cc.m_time = 0;

	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();

	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f, 0.4f, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);


	update();


	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	//SET DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);


	//SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_vb);
	//SET INDICES
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_ib);

	// FINALLY DRAW THE TRIANGLE
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(m_ib->getSizeIndexList(), 0, 0);
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
		m_cam_speed = 0.3f;
	}
}

void AppWindow::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_right = 0.0f;
	m_cam_speed = 0.05f;
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
