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

		static inline bool leftMouseDown();
		static inline bool leftMouseClicked();
		static inline bool leftMouseReleased();

		static inline bool rightMouseDown();
		static inline bool rightMouseClicked();
		static inline bool rightMouseReleased();

		static inline float getMouseXPos();
		static inline float getMouseYPos();

		static inline float getMouseXDelta();
		static inline float getMouseYDelta();

	private:
		static inline void update();
		static inline void setKeyDown(Key key);
		static inline void setKeyUp(Key key);

		static inline bool keys[Key::NUM_KEYS]{};
		static inline bool prevKeys[Key::NUM_KEYS]{};

		static inline bool mouseLeft = false, mouseRight = false;
		static inline bool prevMouseLeft = false, prevMouseRight = false;

		static inline float mouseXPos = 0.f, mouseYPos = 0.f;
		static inline float mousePrevXPos = 0.f, mousePrevYPos = 0.f;
		static inline float mouseXDelta = 0.f, mouseYDelta = 0.f;
	};

	inline bool Input::isKeyDown(Key key)	  { return Input::keys[key]; }
	inline bool Input::isKeyPressed(Key key)  { return Input::keys[key] && !Input::prevKeys[key]; }
	inline bool Input::isKeyReleased(Key key) { return !Input::keys[key] && Input::prevKeys[key]; }

	inline bool Input::leftMouseDown()		{ return Input::mouseLeft; }
	inline bool Input::leftMouseClicked()	{ return Input::mouseLeft && !Input::prevMouseLeft; }
	inline bool Input::leftMouseReleased()	{ return !Input::mouseLeft && Input::prevMouseLeft; }

	inline bool Input::rightMouseDown()		{ return Input::mouseRight; }
	inline bool Input::rightMouseClicked()	{ return Input::mouseRight && !Input::prevMouseRight; }
	inline bool Input::rightMouseReleased() { return !Input::mouseRight && Input::prevMouseRight; }

	inline float Input::getMouseXPos() { return Input::mouseXPos; }
	inline float Input::getMouseYPos() { return Input::mouseYPos; }

	inline float Input::getMouseXDelta() { return Input::mouseXDelta; }
	inline float Input::getMouseYDelta() { return Input::mouseYDelta; }

	inline void Input::setKeyDown(Key key) { if (key >= Key::NUM_KEYS) return; keys[key] = true;  }
	inline void Input::setKeyUp(Key key)   { if (key >= Key::NUM_KEYS) return; keys[key] = false; }

	inline void Input::update()
	{
		memcpy(Input::prevKeys, Input::keys, size_t(Key::NUM_KEYS));
		Input::prevMouseLeft = Input::mouseLeft;
		Input::prevMouseRight =	Input::mouseRight;

		Input::mouseXDelta = Input::mouseXPos - Input::mousePrevXPos;
		Input::mouseYDelta = Input::mouseYPos - Input::mousePrevYPos;
		Input::mousePrevXPos = Input::mouseXPos;
		Input::mousePrevYPos = Input::mouseYPos;
	}
}