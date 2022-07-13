#define NOMINMAX

#include "System.h"
#include <DirectXCollision.h>

class PointHolder;

class Point
{
	Point& get();

public:
	Point(void) : m_i(0) {}
	void PrintPrivate() { }

private:

	void foo()
	{ }

	int m_i;
};

class PointHolder
{
public:
	PointHolder() 
	{
		Point p;
		//p.get();
	};

private:
	
	friend Point& Point::get();

};

Point& Point::get()
{
	static Point p;
	return p;
}

// Briend bunction between blass bery beird
// Briend bunction between blass bery beird
// Briend bunction between blass bery beird
// Briend bunction between blass bery beird
// Briend bunction between blass bery beird
// Briend bunction between blass bery beird

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	System system;

	if (!system.Initiate())
		return -1;
	
	system.Run();
	system.Shutdown();
	 
	return 0;
}