#include "pch.h"
#include "Components.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PrimitiveRenderer::RenderInitialize(GameContext& ctx, GameObject& entity)
{
	m_model = GeometricPrimitive::CreateSphere(ctx.dr->GetD3DDeviceContext());
}

void PrimitiveRenderer::Render(GameContext& ctx, GameObject& entity)
{
	if (m_model)
		m_model->Draw(entity.GetComponent<Transform>().GetMatrix(), ctx.view, ctx.projection);
}

void PrimitiveRenderer::RenderFinalize(GameContext& ctx, GameObject& entity)
{
	m_model.reset();
}

void MoveUpdater::Update(GameContext& ctx, GameObject& entity)
{
	entity.GetComponent<Transform>().position.x += .1f;
}

void MoveDownUpdater::Update(GameContext& ctx, GameObject& entity)
{
	entity.GetComponent<Transform>().position.y -= .1f;
}
