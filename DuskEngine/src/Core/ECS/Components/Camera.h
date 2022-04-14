#pragma once

#include "glm/glm.hpp"
#include "rttr/registration.h"
#include "visit_struct/visit_struct.hpp"

namespace DuskEngine
{
	struct Camera	
	{
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 viewProjectionMatrix;

		bool main = true;
	};
}

VISITABLE_STRUCT(DuskEngine::Camera, main);

//RTTR_REGISTRATION
//{
//	rttr::registration::class_<DuskEngine::Camera>("Camera").
//		property("main", &DuskEngine::Camera::main);
//}