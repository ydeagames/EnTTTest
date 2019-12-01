#pragma once
#include "Component.h"
#include "Components.h"

using Components = ECS::ComponentManager<
	entt::registry,
	std::tuple<
		Transform,
		MoveUpdater,
		MoveDownUpdater,
		PrimitiveRenderer,
		UpdateRenderer,
		CameraComponent
	>,
	std::tuple<
	>,
	std::tuple<
		Updatable,
		Renderable
	>
>;
