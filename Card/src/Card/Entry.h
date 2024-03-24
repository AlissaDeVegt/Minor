#pragma once

#ifdef CARD_PLATFORM_WINDOWS

extern Card::Application* Card::CreateApplication();

int main(int args, char** argv) {
	printf("we love testing the engine\n");
	//Card::Logging::Init();

	auto app = Card::CreateApplication();
	app->Run();
	delete app;
}

#endif