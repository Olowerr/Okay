#pragma once

#include <Engine/Script/ScriptBehaviour.h>
#include <glm/glm.hpp>

class EditorCamera : public Okay::ScriptBehaviour
{
public:
	EditorCamera(Okay::Entity entity);
	~EditorCamera();

	void start() override;
	void update() override;

private:
	static inline const float SKIP_DURATION = 0.01f;
	static inline const float PADDING = 5.f;

	float rotSkipTimer;
	bool skip = false;

	float targetDist;
	glm::vec3 targetPos;

};