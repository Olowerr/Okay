#pragma once

#include "Keys.h"

namespace Okay
{
	class Input
	{
	public:
		friend class Window;

		static inline bool isKeyDown(Keys key);
		static inline bool isKeyPressed(Keys key);
		static inline bool isKeyReleased(Keys key);

	private:
		static inline void update();
		static inline void setKeyDown(Keys key);
		static inline void setKeyUp(Keys key);

		static inline bool keys[Keys::NUM_KEYS]{};
		static inline bool prevKeys[Keys::NUM_KEYS]{};
	};

	inline bool Input::isKeyDown(Keys key)	   { return Input::keys[key]; }
	inline bool Input::isKeyPressed(Keys key)  { return Input::keys[key] && !Input::prevKeys[key]; }
	inline bool Input::isKeyReleased(Keys key) { return !Input::keys[key] && Input::prevKeys[key]; }

	inline void Input::update() { memcpy(Input::prevKeys, Input::keys, size_t(Keys::NUM_KEYS)); }

	inline void Input::setKeyDown(Keys key) { if (key >= Keys::NUM_KEYS) return; keys[key] = true; }
	inline void Input::setKeyUp(Keys key)   { if (key >= Keys::NUM_KEYS) return; keys[key] = false; }
}