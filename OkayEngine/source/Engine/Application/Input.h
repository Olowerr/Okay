#pragma once

namespace Okay
{
	class Input
	{
	public:
		friend class Window;

		static inline bool isKeyDown(int key);
		static inline bool isKeyPressed(int key);
		static inline bool isKeyReleased(int key);

	private:
		static inline void update();
		static inline void setKeyDown(int key);
		static inline void setKeyUp(int key);

		static inline bool keys[256]{};
		static inline bool prevKeys[256]{};
	};

	inline bool Input::isKeyDown(int key)	  { return Input::keys[key]; }
	inline bool Input::isKeyPressed(int key)  { return Input::keys[key] && !Input::prevKeys[key]; }
	inline bool Input::isKeyReleased(int key) { return !Input::keys[key] && Input::prevKeys[key]; }

	inline void Input::update() { memcpy(Input::prevKeys, Input::keys, 256ull); }

	inline void Input::setKeyDown(int key) { keys[key] = true; }
	inline void Input::setKeyUp(int key)   { keys[key] = false; }
}