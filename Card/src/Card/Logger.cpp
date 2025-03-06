#include "Logger.h"


namespace Card {
	std::shared_ptr<spdlog::logger> Logger::s_EngineLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

	/// <summary>
	/// Logger initialisation.
	/// Setting of the format {TIME, LOGNAME, MESSAGE}.
	/// Setting name of the loggers.
	/// Setting of global level to trace.
	/// </summary>
	void Logger::Init() {
		//time, namelogger, msg
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_EngineLogger = spdlog::stdout_color_mt("GAMEENGINE");
		s_EngineLogger->set_level(spdlog::level::trace);
		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}
