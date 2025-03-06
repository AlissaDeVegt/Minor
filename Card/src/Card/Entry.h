#pragma once

#ifdef CARD_PLATFORM_WINDOWS

extern Card::Application* Card::CreateApplication();


int main(int argc, char** argv) {
	Card::Logger::Init();
	CARD_ENGINE_TRACE("Live long and prosper says the logger. Because vulkan works.");
	CARD_CLIENT_TRACE("Starting logger... yes im not a nerd.");

	auto app = Card::CreateApplication();
	app->Run();
	delete app;
}

#endif