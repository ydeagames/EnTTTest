#include "pch.h"
#include "Components.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PrimitiveRenderer::RenderInitialize(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity)
{
	m_model = GeometricPrimitive::CreateSphere(ctx.dr->GetD3DDeviceContext());
}

void PrimitiveRenderer::Render(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity)
{
	if (m_model)
		m_model->Draw(registry.get<Transform>(entity).GetMatrix(), ctx.view, ctx.projection);
}

void PrimitiveRenderer::RenderFinalize(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity)
{
	m_model.reset();
}

void MoveUpdater::Update(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity)
{
	registry.get<Transform>(entity).position.x += .1f;
}

void MoveDownUpdater::Update(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity)
{
	registry.get<Transform>(entity).position.y -= .1f;
}
