#pragma once

#include "Keys.h"

namespace Okay
{
	class Input
	{
	public:
		friend class Window;

		static inline bool isKeyDown(Key key);
		static inline bool isKeyPressed(Key key);
		static inline bool isKeyReleased(Key key);

	private:
		static inline void update();
		static inline void setKeyDown(Key key);
		static inline void setKeyUp(Key key);

		static inline bool keys[Key::NUM_KEYS]{};
		static inline bool prevKeys[Key::NUM_KEYS]{};
	};

	inline bool Input::isKeyDown(Key key)	   { return Input::keys[key]; }
	inline bool Input::isKeyPressed(Key key)  { return Input::keys[key] && !Input::prevKeys[key]; }
	inline bool Input::isKeyReleased(Key key) { return !Input::keys[key] && Input::prevKeys[key]; }

	inline void Input::update() { memcpy(Input::prevKeys, Input::keys, size_t(Key::NUM_KEYS)); }

	inline void Input::setKeyDown(Key key) { if (key >= Key::NUM_KEYS) return; keys[key] = true; }
	inline void Input::setKeyUp(Key key)   { if (key >= Key::NUM_KEYS) return; keys[key] = false; }
}