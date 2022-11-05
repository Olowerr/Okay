#pragma once
#include <Engine/Application/Application.h>

class Editor : public Application
{
public:
	Editor();
	~Editor();

	// Inherited via Application
	virtual void run() override;

private:

};