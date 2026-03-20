#pragma once
#include "incl.h"
#define LOG(t, f, ...) g_logger.send(log_color::Grey, t, f, __VA_ARGS__)

namespace pure
{
	enum class log_color : u16 
	{
		Red = FOREGROUND_RED,
		Green = FOREGROUND_GREEN,
		Blue = FOREGROUND_BLUE,
		Intensify = FOREGROUND_INTENSITY,
		White = Red | Green | Blue | Intensify,
		Grey = Intensify,
		LightRed = Red | Intensify,
		LightGreen = Green | Intensify,
		LightBlue = Blue | Intensify
	};

	class logger final
	{
	public:
		logger() = default;
		~logger() = default;
	public:
		void create() 
		{
			m_path /= "pure";

			if (!fs::exists(m_path))
				fs::create_directory(m_path);

			m_path /= "log.txt";
			m_file.open(m_path.string(), std::ios::out | std::ios::trunc);

			if (!AttachConsole(GetCurrentProcessId()))
				AllocConsole();

			SetConsoleTitleA("pure");
			SetConsoleCP(CP_UTF8);
			SetConsoleOutputCP(CP_UTF8);
			m_con.open("CONOUT$");
			freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		};
		void destroy() 
		{
			FreeConsole();
			fclose(stdout);
			m_path.clear();
			m_con.close();
			m_file.close();
		}
	public:
		template <typename ...arguments>
		void send(log_color color, std::string type, std::string fmt, arguments... args)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<u16>(color));
			std::string msg = std::vformat(fmt, std::make_format_args(args...));
			m_con << type << " | " << msg << std::endl;
			m_file << type << " | " << msg << std::endl;
		}
	private:
		fs::path m_path = std::getenv("appdata");
		std::ofstream m_con{};
		std::ofstream m_file{};
	};
	inline logger g_logger{};
}