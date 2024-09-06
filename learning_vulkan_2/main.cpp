#include "app.h"

int main(int argc, char** argv)
{
	App* hridizaApp = new App(640, 480, true);

	hridizaApp->run();
	delete hridizaApp;

	return 0;
}