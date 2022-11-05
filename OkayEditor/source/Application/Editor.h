#pragma once
#include <Engine/Application/Application.h>

class Editor : public Application
{
public:
	Editor(std::string_view scene);
	~Editor();

	// Inherited via Application
	virtual void run() override;

private:

};