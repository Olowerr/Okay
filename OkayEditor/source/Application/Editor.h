#pragma once
#include <Engine/Application/Application.h>
#include <Engine/Application/Window.h>

class Editor : public Application
{
public:
	Editor();
	~Editor();

	// Inherited via Application
	virtual void run() override;

private:
	int q = 0;
};