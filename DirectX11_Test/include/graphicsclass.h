#pragma once

#include <windows.h>
#include "d3dclass.h"
#include "camera.h"
#include "model.h"
#include "colorshader.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
    GraphicsClass();
    GraphicsClass(const GraphicsClass&);
    ~GraphicsClass();

    bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
    void Shutdown();
    bool Frame();

private:
    bool Render();

private:
	D3DClass* m_Direct3D;
	Camera* m_camera;
	Model* m_model;
	ColorShader* m_colorShader;
};