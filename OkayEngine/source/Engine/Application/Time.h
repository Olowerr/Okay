#pragma once
#include <chrono>

namespace Okay
{
	class Time
	{
	public:
		Time() = delete;

		static inline float getDT();
		static inline void setTimeScale(float timeScale);
		static inline float getUpTime();

		static inline float getApplicationUpTime();
		static inline float getApplicationDT();

		static inline void measure();
		static inline void start();

	private:
		static inline std::chrono::time_point<std::chrono::system_clock> frameStart;
		static inline std::chrono::duration<float> dtApp;
		static inline std::chrono::duration<float> upTimeApp;
		static inline std::chrono::duration<float> upTime;
		static inline float timeScale = 1.f;
	};

	
	inline float Time::getDT() { return Time::dtApp.count() * Time::timeScale; }
	inline void Time::setTimeScale(float timeScale) { Time::timeScale = timeScale; }

	inline float Time::getApplicationDT() { return Time::dtApp.count(); }
	inline float Time::getUpTime() { return Time::upTime.count(); }
	
	inline void Time::measure()
	{
		Time::dtApp = std::chrono::system_clock::now() - Time::frameStart;
		Time::frameStart = std::chrono::system_clock::now();
		Time::upTime += Time::dtApp * Time::timeScale;
		Time::upTimeApp += Time::dtApp;
	}

	inline void Time::start()
	{
		Time::dtApp = std::chrono::duration<float>(0.f);
		Time::upTimeApp = std::chrono::duration<float>(0.f);
		Time::upTime = std::chrono::duration<float>(0.f);
		Time::frameStart = std::chrono::system_clock::now();
	}
}