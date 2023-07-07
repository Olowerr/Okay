#pragma once

#include <Engine/Script/ScriptBehaviour.h>
#include <glm/glm.hpp>

class EditorCamera : public Okay::ScriptBehaviour
{
public:
	EditorCamera(Okay::Entity entity);
	~EditorCamera();

	inline void setSelectedEntity(Okay::Entity entity);

	void start() override;
	void update() override;

private:
	static inline const float SKIP_DURATION = 0.01f;
	static inline const float PADDING = 5.f;
	static inline const float FOCUS_TIME = 0.1f;

	Okay::Entity focusedEntity;
	glm::vec3 lerpStartPos;
	bool lerpToFocus;
	float focusLerpT;

	float rotSkipTimer;
	bool skip = false;

	float targetDist;
	glm::vec3 targetPos;

};

inline void EditorCamera::setSelectedEntity(Okay::Entity entity) 
{
	if (entity != getEntity())
		focusedEntity = entity; 
}