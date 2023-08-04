#include "AppWindow.h"
#include <Windows.h>
#include "Vec3.h"
#include "Vec2.h"
#include "Mat4.h"
#include "InputSystem.h"
#include "Mesh.h"

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

void AppWindow::update()
{
	// ------------- GET DELTA TIME --------------------
	constant cc;
	//cc.m_time = ::GetTickCount();

	m_delta_pos += m_delta_time / 10.0f;
	if (m_delta_pos > 1.0f)
		m_delta_pos = 0;

	Mat4 temp;
	Mat4 m_light_rot;

	m_delta_scale += m_delta_time / 0.5f;

	cc.m_world.setIdentity();
	m_light_rot.setIdentity();

	// ------------------- LIGHT MATRIX -----------------
	m_light_rot_y += 0.785f * m_delta_time;
	m_light_rot.setRotationY(m_light_rot_y);
	cc.m_light_dir = m_light_rot.getZDirection();

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

	cc.m_camera_pos = new_pos;
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


	// ------------ RESOURCE RETRIEVAL ------------

	m_wood_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\brick.png");
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\suzanne.obj");
	m_sky_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\sphere.obj");

	// ------------ VERTEX DATA ---------------
	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	m_world_cam.setTranslation(Vec3(0, 0, -1));

	Vec3 position_list[] =
	{
		{ Vec3(-0.5f,-0.5f,-0.5f)},
		{ Vec3(-0.5f,0.5f,-0.5f) },
		{ Vec3(0.5f,0.5f,-0.5f) },
		{ Vec3(0.5f,-0.5f,-0.5f)},

		//BACK FACE
		{ Vec3(0.5f,-0.5f,0.5f) },
		{ Vec3(0.5f,0.5f,0.5f) },
		{ Vec3(-0.5f,0.5f,0.5f)},
		{ Vec3(-0.5f,-0.5f,0.5f) }
	};

	Vec2 texcoord_list[] =
	{
		{ Vec2(0.0f,0.0f) },
		{ Vec2(0.0f,1.0f) },
		{ Vec2(1.0f,0.0f) },
		{ Vec2(1.0f,1.0f) }
	};



	vertex vertex_list[] =
	{
		//X - Y - Z
		//FRONT FACE
		{ position_list[0],texcoord_list[1] },
		{ position_list[1],texcoord_list[0] },
		{ position_list[2],texcoord_list[2] },
		{ position_list[3],texcoord_list[3] },


		{ position_list[4],texcoord_list[1] },
		{ position_list[5],texcoord_list[0] },
		{ position_list[6],texcoord_list[2] },
		{ position_list[7],texcoord_list[3] },


		{ position_list[1],texcoord_list[1] },
		{ position_list[6],texcoord_list[0] },
		{ position_list[5],texcoord_list[2] },
		{ position_list[2],texcoord_list[3] },

		{ position_list[7],texcoord_list[1] },
		{ position_list[0],texcoord_list[0] },
		{ position_list[3],texcoord_list[2] },
		{ position_list[4],texcoord_list[3] },

		{ position_list[3],texcoord_list[1] },
		{ position_list[2],texcoord_list[0] },
		{ position_list[5],texcoord_list[2] },
		{ position_list[4],texcoord_list[3] },

		{ position_list[7],texcoord_list[1] },
		{ position_list[6],texcoord_list[0] },
		{ position_list[1],texcoord_list[2] },
		{ position_list[0],texcoord_list[3] }


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
		8,9,10,
		10,11,8,
		//BOTTOM
		12,13,14,
		14,15,12,
		//RIGHT
		16,17,18,
		18,19,16,
		//LEFT
		20,21,22,
		22,23,20
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
	//cc.m_time = 0;

	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
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


	update();

	drawMesh(m_mesh, m_vs, m_ps, m_cb, m_wood_tex);

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
		m_cam_speed = 0.03f;
	}
}

void AppWindow::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_right = 0.0f;
	m_cam_speed = 0.01f;
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
