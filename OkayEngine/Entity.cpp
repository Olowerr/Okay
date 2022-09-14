#include "Entity.h"
#include "Scripts.h"

template ScriptCameraMovement& Entity::GetScript<ScriptCameraMovement>();

template<typename T>
T& Entity::GetScript()
{
	return GetComponent<CompScript>().GetScript<T>();
}