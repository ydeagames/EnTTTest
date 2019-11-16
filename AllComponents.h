#pragma once
#include "Serialize.h"
#include "Components.h"

using Components = ECS::ComponentManager<
	std::tuple<
		Transform,
		MoveUpdater,
		MoveDownUpdater,
		PrimitiveRenderer,
		UpdateRenderer
	>,
	std::tuple<
		Updatable,
		Renderable
	>,
	std::tuple<
	>
>;