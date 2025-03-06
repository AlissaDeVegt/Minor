#pragma once

#include "Base.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

namespace Card {
	class CARD_API Logger
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; };

	private:
		static std::shared_ptr<spdlog::logger> s_EngineLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};

}

#define CARD_ENGINE_ERROR(...)			Card::Logger::GetEngineLogger()->error(__VA_ARGS__)
#define CARD_ENGINE_WARN(...)			Card::Logger::GetEngineLogger()->warn(__VA_ARGS__)
#define CARD_ENGINE_INFO(...)			Card::Logger::GetEngineLogger()->info(__VA_ARGS__)
#define CARD_ENGINE_TRACE(...)			Card::Logger::GetEngineLogger()->trace(__VA_ARGS__)


#define CARD_CLIENT_ERROR(...)			Card::Logger::GetClientLogger()->error(__VA_ARGS__)
#define CARD_CLIENT_WARN(...)  			Card::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define CARD_CLIENT_INFO(...)  			Card::Logger::GetClientLogger()->info(__VA_ARGS__)
#define CARD_CLIENT_TRACE(...)			Card::Logger::GetClientLogger()->trace(__VA_ARGS__)
