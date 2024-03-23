#pragma once

#ifdef CARD_PLATFORM_WINDOWS

extern Card::Application* Card::CreateApplication();

int main(int args, char** argv) {
	printf("card engine\n");

	auto app = Card::CreateApplication();
	app->Run();
	delete app;
}

#endif