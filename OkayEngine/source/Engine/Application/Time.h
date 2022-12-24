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
		static inline float getInternalDT();

		static inline void measure();
		static inline void start();

	private:
		static inline std::chrono::time_point<std::chrono::system_clock> frameStart;
		static inline std::chrono::duration<float> deltaTime;
		static inline float timeScale = 1.f;
	};

	
	inline float Time::getDT() { return Time::deltaTime.count() * Time::timeScale; }
	inline void Time::setTimeScale(float timeScale) { Time::timeScale = timeScale; }
	inline float Time::getInternalDT() { return Time::deltaTime.count(); }
	
	inline void Time::measure()
	{
		Time::deltaTime = std::chrono::system_clock::now() - Time::frameStart;
		Time::frameStart = std::chrono::system_clock::now();
	}

	inline void Time::start()
	{
		Time::deltaTime = std::chrono::duration<float>(0.f);
		Time::frameStart = std::chrono::system_clock::now();
	}
}