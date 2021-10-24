#pragma
enum LogLevel
{
	Info = 0,
	Warning = 1,
	Error = 2,
};
void log(const char* message, LogLevel level = LogLevel::Info);