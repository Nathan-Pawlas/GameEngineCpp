#include "AppWindow.h"
#include <Windows.h>
#include "Vec3.h"
#include "Mat4.h"


struct vertex
{
	Vec3 position;
	Vec3 position1;
	Vec3 color;
	Vec3 color1;
};

__declspec(align(16))
struct constant
{
	Mat4 m_world;
	Mat4 m_view;
	Mat4 m_projection;
	float m_angle;
};


AppWindow::AppWindow()
{
}

void AppWindow::updateQuadPosition()
{
}


AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	Window::onCreate();
	GraphicsEngine::get()->init();
	m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	vertex list[] =
	{
		{Vec3(-0.5f, -0.5f, 0.0f),   Vec3(-0.32f, -0.11f,  0),     Vec3(1,  0,  0),   Vec3(0,  1,  1)},
		{Vec3(-0.5f,  0.5f, 0.0f),   Vec3(-0.11f,  0.78f,  0),     Vec3(0,  1,  0),   Vec3(1,  0,  0)},
		{Vec3(0.5f, -0.5f, 0.0f),    Vec3( 0.75f, -0.73f,  0),     Vec3(0,  0,  1),   Vec3(0,  1,  0)},
		{Vec3(0.5f,  0.5f, 0.0f),    Vec3( 0.88f,  0.77f,  0),     Vec3(1,  1,  0),   Vec3(0,  0,  1)}
	};

	m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(list);


	// ----------------- VERTEX SHADER -----------------------------------
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl","vsmain",&shader_byte_code,&size_shader);

	m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();

	// ------------------ PIXEL SHADER -----------------------------------
	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();


	// ----------------- CONSTANT BUFFER --------------------------------
	constant cc;
	cc.m_angle = 0;

	m_cb = GraphicsEngine::get()->createConstantBuffer();
	m_cb->load(&cc, sizeof(constant));

}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f, 0.4f, 1);

	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	unsigned long new_time = 0;
	if (m_old_time)
		new_time = ::GetTickCount() - m_old_time;
	m_delta_time = new_time / 1000.0f;
	m_old_time = ::GetTickCount();
	m_angle += 1.57f * m_delta_time;
	constant cc;
	cc.m_angle = m_angle;
	m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);

	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	//SET SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(m_ps);

	//SET  VERTICES
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);

	// DRAW
	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);
	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_swap_chain->release();
	m_vs->release();
	m_ps->release();
	GraphicsEngine::get()->release();
}