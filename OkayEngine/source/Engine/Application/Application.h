#pragma once
#include <string>

class Application
{
public:
	Application(std::string_view appName);
	virtual ~Application();

	virtual void run() = 0;

protected:
	// Window

private:
	std::string appName;
};