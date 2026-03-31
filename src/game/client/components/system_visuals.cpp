#include "system_visuals.h"

#include <base/color.h>
#include <base/math.h>
#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>

#include <game/client/gameclient.h>
#include <game/client/components/controls.h>
#include <game/client/ui.h>
#include <game/client/prediction/entities/character.h>
#include <game/collision.h>
#include <game/gamecore.h>

#include <algorithm>
#include <vector>

void CSystemVisuals::InitDefaultHudPositions()
{
	// Initialize draggable HUD positions to their original fixed layout.
	const float WaterMarkW = 280.0f;
	const float WaterMarkH = 90.0f;
	const float WaterMarkMargin = 250.0f;

	const float FunctionListW = 220.0f;
	const float FunctionListBaseH = 40.0f;
	const float FunctionListRowH = 22.0f;
	const float FunctionListMargin = 10.0f;

	const float SpeedTabW = 150.0f;
	const float SpeedTabH = 60.0f;
	const float SpeedTabMargin = 10.0f;

	const int EnabledCount =
		(m_WaterMarkEnabled ? 1 : 0) +
		(m_SpeedTabEnabled ? 1 : 0) +
		(m_RainbowMeEnabled ? 1 : 0) +
		(m_PredictionEnabled ? 1 : 0);

	const float FunctionListH = FunctionListBaseH + EnabledCount * FunctionListRowH;

	const float ScreenW = Graphics()->ScreenWidth();
	const float ScreenH = Graphics()->ScreenHeight();

	m_WaterMarkPos = vec2(ScreenW - WaterMarkW - WaterMarkMargin, 10.0f);
	m_FunctionListPos = vec2(ScreenW - FunctionListW - FunctionListMargin, (ScreenH - FunctionListH) / 2.0f);
	m_SpeedTabPos = vec2(SpeedTabMargin, ScreenH - SpeedTabH - SpeedTabMargin);

	ClampHudPositions(ScreenW, ScreenH, FunctionListH);
}

void CSystemVisuals::ResetHudLayoutPositions()
{
	InitDefaultHudPositions();
}

void CSystemVisuals::OnInit()
{
	InitDefaultHudPositions();
	m_LayoutMousePos = vec2(Graphics()->ScreenWidth() * 0.5f, Graphics()->ScreenHeight() * 0.5f);
}

void CSystemVisuals::OnUpdate()
{
	// Apply Rainbow Me before any rendering (affects m_RenderInfo used by Players)
	if(m_RainbowMeEnabled)
		ApplyRainbowColors();

	HandleHudDragging();
}

void CSystemVisuals::OnWindowResize()
{
	const float FunctionListBaseH = 40.0f;
	const float FunctionListRowH = 22.0f;

	const int EnabledCount =
		(m_WaterMarkEnabled ? 1 : 0) +
		(m_SpeedTabEnabled ? 1 : 0) +
		(m_RainbowMeEnabled ? 1 : 0) +
		(m_PredictionEnabled ? 1 : 0);

	const float FunctionListH = FunctionListBaseH + EnabledCount * FunctionListRowH;

	const float ScreenW = Graphics()->ScreenWidth();
	const float ScreenH = Graphics()->ScreenHeight();
	ClampHudPositions(ScreenW, ScreenH, FunctionListH);
	m_LayoutMousePos.x = std::clamp(m_LayoutMousePos.x, 0.0f, std::max(0.0f, ScreenW - 1.0f));
	m_LayoutMousePos.y = std::clamp(m_LayoutMousePos.y, 0.0f, std::max(0.0f, ScreenH - 1.0f));
}

bool CSystemVisuals::OnCursorMove(float x, float y, IInput::ECursorType CursorType)
{
	(void)x;
	(void)y;
	(void)CursorType;
	return false;
}

void CSystemVisuals::OnRender()
{
	Graphics()->MapScreen(0, 0, Graphics()->ScreenWidth(), Graphics()->ScreenHeight());

	if(m_WaterMarkEnabled)
		RenderWaterMark();

	if(m_FunctionListEnabled)
		RenderFunctionList();

	if(m_SpeedTabEnabled)
		RenderSpeedTab();

	if(m_PredictionEnabled)
		RenderPrediction();
}

void CSystemVisuals::ApplyRainbowColors()
{
	int LocalClientId = GameClient()->m_Snap.m_LocalClientId;
	if(LocalClientId < 0 || LocalClientId >= MAX_CLIENTS)
		return;

	CGameClient::CClientData *pLocalClient = &GameClient()->m_aClients[LocalClientId];
	if(!pLocalClient || !pLocalClient->m_Active)
		return;

	m_RainbowHue += 0.001f;
	if(m_RainbowHue >= 1.0f)
		m_RainbowHue = 0.0f;

	ColorHSLA RainbowHSL(m_RainbowHue, 1.0f, 0.5f, 1.0f);
	ColorRGBA RainbowRGB = color_cast<ColorRGBA>(RainbowHSL);

	CTeeRenderInfo *pRenderInfo = &pLocalClient->m_RenderInfo;
	pRenderInfo->m_ColorBody = RainbowRGB;
	pRenderInfo->m_ColorFeet = RainbowRGB;

	int r = (int)(RainbowRGB.r * 255.0f);
	int g = (int)(RainbowRGB.g * 255.0f);
	int b = (int)(RainbowRGB.b * 255.0f);
	int PackedColor = (r << 16) | (g << 8) | b;

	pLocalClient->m_UseCustomColor = 1;
	pLocalClient->m_ColorBody = PackedColor;
	pLocalClient->m_ColorFeet = PackedColor;
}

bool CSystemVisuals::MouseInsideRect(const vec2 &MousePos, const vec2 &RectPos, float RectW, float RectH) const
{
	return MousePos.x >= RectPos.x && MousePos.x <= (RectPos.x + RectW) &&
	       MousePos.y >= RectPos.y && MousePos.y <= (RectPos.y + RectH);
}

void CSystemVisuals::ClampHudPositions(float ScreenW, float ScreenH, float FunctionListH)
{
	const float WaterMarkW = 280.0f;
	const float WaterMarkH = 90.0f;
	const float FunctionListW = 220.0f;
	const float SpeedTabW = 150.0f;
	const float SpeedTabH = 60.0f;

	const float MaxWaterMarkX = std::max(0.0f, ScreenW - WaterMarkW);
	const float MaxWaterMarkY = std::max(0.0f, ScreenH - WaterMarkH);
	const float MaxFunctionListX = std::max(0.0f, ScreenW - FunctionListW);
	const float MaxFunctionListY = std::max(0.0f, ScreenH - FunctionListH);
	const float MaxSpeedTabX = std::max(0.0f, ScreenW - SpeedTabW);
	const float MaxSpeedTabY = std::max(0.0f, ScreenH - SpeedTabH);

	m_WaterMarkPos.x = std::clamp(m_WaterMarkPos.x, 0.0f, MaxWaterMarkX);
	m_WaterMarkPos.y = std::clamp(m_WaterMarkPos.y, 0.0f, MaxWaterMarkY);

	m_FunctionListPos.x = std::clamp(m_FunctionListPos.x, 0.0f, MaxFunctionListX);
	m_FunctionListPos.y = std::clamp(m_FunctionListPos.y, 0.0f, MaxFunctionListY);

	m_SpeedTabPos.x = std::clamp(m_SpeedTabPos.x, 0.0f, MaxSpeedTabX);
	m_SpeedTabPos.y = std::clamp(m_SpeedTabPos.y, 0.0f, MaxSpeedTabY);
}

void CSystemVisuals::HandleHudDragging()
{
	// Layout mode was removed; keep HUD at static/clamped positions.
	if(m_ForcedAbsoluteMouseInLayout)
	{
		Input()->MouseModeRelative();
		m_ForcedAbsoluteMouseInLayout = false;
	}
	m_DragTarget = EDragTarget::NONE;
	m_HasDragInProgress = false;
	m_LastLayoutMouseButton1 = false;
}

void CSystemVisuals::RenderWaterMark()
{
	const float Width = 250.0f;
	const float Height = 72.0f;

	float ScreenWidth = Graphics()->ScreenWidth();
	m_WaterMarkPos.x = std::clamp(m_WaterMarkPos.x, 0.0f, std::max(0.0f, ScreenWidth - Width));
	m_WaterMarkPos.y = std::clamp(m_WaterMarkPos.y, 0.0f, std::max(0.0f, Graphics()->ScreenHeight() - Height));
	float x = m_WaterMarkPos.x;
	float y = m_WaterMarkPos.y;

	float Time = Client()->LocalTime();
	float Hue = fmod(Time * 0.1f, 1.0f);

	ColorHSLA GlowHSL(Hue, 0.55f, 0.67f, 1.0f);
	ColorRGBA GlowRGBA = color_cast<ColorRGBA>(GlowHSL);
	ColorRGBA BorderRGBA(GlowRGBA.r, GlowRGBA.g, GlowRGBA.b, 0.42f);
	ColorRGBA BgRGBA(0.07f, 0.09f, 0.13f, 0.54f);
	ColorRGBA ShineRGBA(0.95f, 0.98f, 1.0f, 0.12f);
	ColorRGBA AccentRGBA(GlowRGBA.r, GlowRGBA.g, GlowRGBA.b, 0.26f);
	const char *pTitle = "system client";
	const char *pEdition = "Liquid Glass";

	// Outer frame
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(BorderRGBA.r, BorderRGBA.g, BorderRGBA.b, BorderRGBA.a * 0.85f);
	IGraphics::CQuadItem QuadBorder(x - 1.5f, y - 1.5f, Width + 3.0f, Height + 3.0f);
	Graphics()->QuadsDrawTL(&QuadBorder, 1);
	Graphics()->SetColor(GlowRGBA.r, GlowRGBA.g, GlowRGBA.b, 0.12f);
	IGraphics::CQuadItem QuadGlow(x - 6.0f, y - 6.0f, Width + 12.0f, Height + 12.0f);
	Graphics()->QuadsDrawTL(&QuadGlow, 1);
	Graphics()->QuadsEnd();

	// Background
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(BgRGBA.r, BgRGBA.g, BgRGBA.b, BgRGBA.a);
	IGraphics::CQuadItem QuadBg(x, y, Width, Height);
	Graphics()->QuadsDrawTL(&QuadBg, 1);
	Graphics()->SetColor(AccentRGBA.r, AccentRGBA.g, AccentRGBA.b, AccentRGBA.a);
	IGraphics::CQuadItem QuadAccent(x, y + Height * 0.46f, Width, Height * 0.54f);
	Graphics()->QuadsDrawTL(&QuadAccent, 1);
	Graphics()->QuadsEnd();

	// Soft top highlight
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(ShineRGBA.r, ShineRGBA.g, ShineRGBA.b, ShineRGBA.a);
	IGraphics::CQuadItem QuadGradient(x, y, Width, Height * 0.45f);
	Graphics()->QuadsDrawTL(&QuadGradient, 1);
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, 0.08f);
	IGraphics::CQuadItem QuadShineBand(x + 8.0f, y + 9.0f, Width - 16.0f, 10.0f);
	Graphics()->QuadsDrawTL(&QuadShineBand, 1);
	Graphics()->SetColor(BorderRGBA.r, BorderRGBA.g, BorderRGBA.b, 0.22f);
	IGraphics::CQuadItem QuadLine(x + 10.0f, y + Height - 5.0f, Width - 20.0f, 1.5f);
	Graphics()->QuadsDrawTL(&QuadLine, 1);
	Graphics()->QuadsEnd();

	// Title
	float TextX = x + 12.0f;
	float TextY = y + 11.0f;

	TextRender()->TextColor(0.96f, 0.98f, 1.0f, 1.0f);
	TextRender()->Text(TextX, TextY, 18.0f, pTitle);

	// Edition / active label
	TextRender()->TextColor(0.75f, 0.82f, 0.9f, 1.0f);
	TextRender()->Text(x + 12.0f, y + 34.0f, 10.0f, pEdition);

	TextRender()->TextColor(GlowRGBA.r, GlowRGBA.g, GlowRGBA.b, 0.96f);
	TextRender()->Text(x + 12.0f, y + 50.0f, 11.0f, "ACTIVE");

	TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void CSystemVisuals::RenderFunctionList()
{
	const float Width = 230.0f;
	float ScreenWidth = Graphics()->ScreenWidth();
	float ScreenHeight = Graphics()->ScreenHeight();

	int EnabledCount = 0;
	if(m_WaterMarkEnabled) EnabledCount++;
	if(m_SpeedTabEnabled) EnabledCount++;
	if(m_RainbowMeEnabled) EnabledCount++;
	if(m_PredictionEnabled) EnabledCount++;

	if(EnabledCount == 0 && !m_HudLayoutMode)
		return;

	const int DisplayCount = std::max(EnabledCount, 1);
	float Height = 38.0f + DisplayCount * 20.0f;
	m_FunctionListPos.x = std::clamp(m_FunctionListPos.x, 0.0f, std::max(0.0f, ScreenWidth - Width));
	m_FunctionListPos.y = std::clamp(m_FunctionListPos.y, 0.0f, std::max(0.0f, ScreenHeight - Height));
	float x = m_FunctionListPos.x;
	float y = m_FunctionListPos.y;

	float Time = Client()->LocalTime();
	float Hue = fmod(Time * 0.11f + 0.18f, 1.0f);
	ColorHSLA GlowHSL(Hue, 0.52f, 0.66f, 1.0f);
	ColorRGBA GlowRGBA = color_cast<ColorRGBA>(GlowHSL);
	ColorRGBA BorderRGBA(GlowRGBA.r, GlowRGBA.g, GlowRGBA.b, 0.40f);
	ColorRGBA LineRGBA(GlowRGBA.r, GlowRGBA.g, GlowRGBA.b, 0.28f);
	ColorRGBA TitleBaseRGBA(0.95f, 0.98f, 1.0f, 1.0f);
	ColorRGBA ItemBaseRGBA(0.87f, 0.93f, 0.99f, 1.0f);

	// Rainbow override
	if(m_FunctionListRainbow)
	{
		ColorHSLA BorderColor(Hue, 0.92f, 0.65f, 1.0f);
		ColorRGBA RainbowRGBA = color_cast<ColorRGBA>(BorderColor);
		BorderRGBA = ColorRGBA(RainbowRGBA.r, RainbowRGBA.g, RainbowRGBA.b, 0.5f);
		LineRGBA = ColorRGBA(RainbowRGBA.r, RainbowRGBA.g, RainbowRGBA.b, 0.35f);
		TitleBaseRGBA = ColorRGBA(RainbowRGBA.r, RainbowRGBA.g, RainbowRGBA.b, 1.0f);
		ItemBaseRGBA = ColorRGBA(RainbowRGBA.r, RainbowRGBA.g, RainbowRGBA.b, 1.0f);
	}

	// Border
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(BorderRGBA.r, BorderRGBA.g, BorderRGBA.b, BorderRGBA.a);
	IGraphics::CQuadItem QuadBorder(x - 1.5f, y - 1.5f, Width + 3.0f, Height + 3.0f);
	Graphics()->QuadsDrawTL(&QuadBorder, 1);
	Graphics()->QuadsEnd();

	// Background
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(0.07f, 0.09f, 0.13f, 0.52f);
	IGraphics::CQuadItem Quad(x, y, Width, Height);
	Graphics()->QuadsDrawTL(&Quad, 1);
	Graphics()->SetColor(GlowRGBA.r, GlowRGBA.g, GlowRGBA.b, 0.16f);
	IGraphics::CQuadItem QuadBottomTint(x, y + Height * 0.50f, Width, Height * 0.50f);
	Graphics()->QuadsDrawTL(&QuadBottomTint, 1);
	Graphics()->QuadsEnd();

	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, 0.10f);
	IGraphics::CQuadItem QuadShine(x + 8.0f, y + 8.0f, Width - 16.0f, 10.0f);
	Graphics()->QuadsDrawTL(&QuadShine, 1);
	Graphics()->SetColor(BorderRGBA.r, BorderRGBA.g, BorderRGBA.b, 0.16f);
	IGraphics::CQuadItem QuadSoftLine(x + 8.0f, y + Height - 6.0f, Width - 16.0f, 1.0f);
	Graphics()->QuadsDrawTL(&QuadSoftLine, 1);
	Graphics()->QuadsEnd();

	const char *pTitle = "Active Functions";
	float TitleX = x + 12.0f;
	float TitleY = y + 8.0f;

	if(m_FunctionListRainbow)
	{
		for(int i = 0; i < (int)str_length(pTitle); i++)
		{
			float CharHue = fmod(Hue + i * 0.05f, 1.0f);
			ColorHSLA CharColor(CharHue, 1.0f, 0.7f, 1.0f);
			ColorRGBA CharRGBA = color_cast<ColorRGBA>(CharColor);
			TextRender()->TextColor(CharRGBA);

			char aChar[2] = {pTitle[i], 0};
			TextRender()->Text(TitleX, TitleY, 15.0f, aChar);
			TitleX += TextRender()->TextWidth(15.0f, aChar);
		}
	}
	else
	{
		TextRender()->TextColor(TitleBaseRGBA);
		TextRender()->Text(TitleX, TitleY, 15.0f, pTitle);
	}

	// Separator line
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(LineRGBA.r, LineRGBA.g, LineRGBA.b, LineRGBA.a);
	IGraphics::CQuadItem Line(x + 10, y + 28, Width - 20, 1);
	Graphics()->QuadsDrawTL(&Line, 1);
	Graphics()->QuadsEnd();

	float ListY = y + 34.0f;
	int ItemIndex = 0;

	auto AddItem = [&](const char *pName) {
		if(m_FunctionListRainbow)
		{
			float ItemHue = fmod(Hue + ItemIndex * 0.10f, 1.0f);
			ColorHSLA ItemColor(ItemHue, 1.0f, 0.7f, 1.0f);
			ColorRGBA ItemRGBA = color_cast<ColorRGBA>(ItemColor);
			TextRender()->TextColor(ItemRGBA);
		}
		else
		{
			TextRender()->TextColor(ItemBaseRGBA);
		}
		TextRender()->Text(x + 14.0f, ListY, 11.0f, pName);
		ListY += 20.0f;
		ItemIndex++;
	};

	if(m_HudLayoutMode)
	{
		AddItem("[Layout Mode] Drag this block");
	}
	else
	{
		if(m_WaterMarkEnabled) AddItem("[+] WaterMark");
		if(m_SpeedTabEnabled) AddItem("[+] Speed Tab");
		if(m_RainbowMeEnabled) AddItem("[+] Rainbow Me");
		if(m_PredictionEnabled) AddItem("[+] Prediction");
	}

	TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void CSystemVisuals::RenderSpeedTab()
{
	const float Width = 170.0f;
	const float Height = 68.0f;

	float ScreenW = Graphics()->ScreenWidth();
	float ScreenH = Graphics()->ScreenHeight();
	m_SpeedTabPos.x = std::clamp(m_SpeedTabPos.x, 0.0f, std::max(0.0f, ScreenW - Width));
	m_SpeedTabPos.y = std::clamp(m_SpeedTabPos.y, 0.0f, std::max(0.0f, ScreenH - Height));
	float x = m_SpeedTabPos.x;
	float y = m_SpeedTabPos.y;

	float Time = Client()->LocalTime();
	float Hue = fmod(Time * 0.11f + 0.28f, 1.0f);
	ColorHSLA GlowHSL(Hue, 0.52f, 0.67f, 1.0f);
	ColorRGBA GlowRGBA = color_cast<ColorRGBA>(GlowHSL);
	ColorRGBA BorderRGBA(GlowRGBA.r, GlowRGBA.g, GlowRGBA.b, 0.42f);
	ColorRGBA BgRGBA(0.07f, 0.09f, 0.13f, 0.53f);
	ColorRGBA TitleRGBA(0.95f, 0.98f, 1.0f, 1.0f);
	ColorRGBA ValueRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	ColorRGBA MaxRGBA(0.78f, 0.86f, 0.95f, 1.0f);

	// Border outline
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(BorderRGBA.r, BorderRGBA.g, BorderRGBA.b, BorderRGBA.a);
	IGraphics::CQuadItem QuadBorder(x - 1.5f, y - 1.5f, Width + 3.0f, Height + 3.0f);
	Graphics()->QuadsDrawTL(&QuadBorder, 1);
	Graphics()->QuadsEnd();

	// Background
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(BgRGBA.r, BgRGBA.g, BgRGBA.b, BgRGBA.a);
	IGraphics::CQuadItem Quad(x, y, Width, Height);
	Graphics()->QuadsDrawTL(&Quad, 1);
	Graphics()->SetColor(GlowRGBA.r, GlowRGBA.g, GlowRGBA.b, 0.17f);
	IGraphics::CQuadItem QuadBottomTint(x, y + Height * 0.50f, Width, Height * 0.50f);
	Graphics()->QuadsDrawTL(&QuadBottomTint, 1);
	Graphics()->QuadsEnd();

	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, 0.11f);
	IGraphics::CQuadItem QuadShine(x + 8.0f, y + 8.0f, Width - 16.0f, 9.0f);
	Graphics()->QuadsDrawTL(&QuadShine, 1);
	Graphics()->SetColor(BorderRGBA.r, BorderRGBA.g, BorderRGBA.b, 0.16f);
	IGraphics::CQuadItem QuadSoftLine(x + 8.0f, y + Height - 7.0f, Width - 16.0f, 1.0f);
	Graphics()->QuadsDrawTL(&QuadSoftLine, 1);
	Graphics()->QuadsEnd();

	float Speed = 0.0f;
	if(GameClient()->m_Snap.m_pLocalCharacter)
	{
		vec2 Vel = vec2(GameClient()->m_Snap.m_pLocalCharacter->m_VelX / 256.0f,
			GameClient()->m_Snap.m_pLocalCharacter->m_VelY / 256.0f);
		Speed = length(Vel);
		if(Speed > m_MaxSpeed)
			m_MaxSpeed = Speed;
	}

	TextRender()->TextColor(TitleRGBA);
	TextRender()->Text(x + 10.0f, y + 7.0f, 12.0f, "Speed");

	char aBuf[64];
	str_format(aBuf, sizeof(aBuf), "%.1f u/s", Speed);
	TextRender()->TextColor(ValueRGBA);
	TextRender()->Text(x + 10.0f, y + 27.0f, 17.0f, aBuf);

	str_format(aBuf, sizeof(aBuf), "Max: %.1f", m_MaxSpeed);
	TextRender()->TextColor(MaxRGBA);
	TextRender()->Text(x + 10.0f, y + 50.0f, 10.0f, aBuf);

	TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void CSystemVisuals::RenderPrediction()
{
	if(!GameClient()->m_Snap.m_pLocalCharacter)
		return;

	int LocalClientId = GameClient()->m_Snap.m_LocalClientId;
	if(LocalClientId < 0)
		return;

	CCharacter *pChar = GameClient()->m_PredictedWorld.GetCharacterById(LocalClientId);
	if(!pChar)
		return;

	vec2 StartPos = pChar->Core()->m_Pos;
	vec2 Vel = pChar->Core()->m_Vel;

	CNetObj_PlayerInput *pInput = &GameClient()->m_Controls.m_aInputData[g_Config.m_ClDummy];
	int Direction = pInput->m_Direction;

	// Don't show prediction if player is standing still
	float VelLength = length(Vel);
	if(Direction == 0 && VelLength < 0.1f && pChar->IsGrounded())
		return;

	const CTuningParams *pTuning = GameClient()->GetTuning(g_Config.m_ClDummy);
	CCollision *pCollision = Collision();
	if(!pTuning || !pCollision)
		return;

	// Constants
	const float TileSize = 32.0f;
	const float MaxDistance = (float)m_PredictionDistance * TileSize;
	const int MaxSteps = 150;

	// Simulation state
	vec2 SimPos = StartPos;
	vec2 SimVel = Vel;

	// Store trajectory points
	std::vector<vec2> TrajectoryPoints;
	std::vector<float> Distances;
	TrajectoryPoints.push_back(StartPos);
	Distances.push_back(0.0f);

	// Simulate physics manually
	for(int Step = 0; Step < MaxSteps; Step++)
	{
		// Get current grounded state
		bool IsGrounded = pCollision->CheckPoint(SimPos.x, SimPos.y + 28.0f);

		// Apply movement
		float MaxSpeed = IsGrounded ? pTuning->m_GroundControlSpeed : pTuning->m_AirControlSpeed;
		float Accel = IsGrounded ? pTuning->m_GroundControlAccel : pTuning->m_AirControlAccel;
		float Friction = IsGrounded ? pTuning->m_GroundFriction : pTuning->m_AirFriction;

		// Apply direction
		if(Direction < 0)
			SimVel.x = std::clamp(SimVel.x - Accel, -MaxSpeed, MaxSpeed);
		else if(Direction > 0)
			SimVel.x = std::clamp(SimVel.x + Accel, -MaxSpeed, MaxSpeed);

		// Apply friction
		SimVel.x *= Friction;

		// Apply gravity
		if(!IsGrounded)
			SimVel.y += pTuning->m_Gravity;
		else
			SimVel.y = 0.0f;

		// Update position
		SimPos += SimVel;

		// Collision response (no rewind; prevents jitter "tearing")
		// Check ground
		if(pCollision->CheckPoint(SimPos.x, SimPos.y + 28.0f))
			SimVel.y = 0.0f;

		// Check walls
		if(pCollision->CheckPoint(SimPos.x + 14.0f, SimPos.y) || pCollision->CheckPoint(SimPos.x - 14.0f, SimPos.y))
			SimVel.x = 0.0f;

		// Check ceiling
		if(pCollision->CheckPoint(SimPos.x, SimPos.y - 28.0f))
			SimVel.y = 0.0f;

		// Check distance
		float Dist = distance(StartPos, SimPos);
		if(Dist > MaxDistance)
			break;

		// Add point every 2 steps for smooth line
		if(Step % 2 == 0)
		{
			TrajectoryPoints.push_back(SimPos);
			Distances.push_back(Dist);
		}
	}

	// Need at least 2 points
	if(TrajectoryPoints.size() < 2)
		return;

	// Determine base color from settings
	ColorRGBA BaseColor = color_cast<ColorRGBA>(ColorHSLA(m_PredictionColor, true));

	// Half-width in world units for thickness
	float HalfWidth = static_cast<float>(m_PredictionThickness);

	// Switch to world-space coordinates so the prediction is drawn as part of the map,
	// not locked to the camera/HUD plane.
	Graphics()->MapScreenToInterface(
		GameClient()->m_Camera.m_Center.x,
		GameClient()->m_Camera.m_Center.y,
		GameClient()->m_Camera.m_Zoom);

	Graphics()->TextureClear();
	Graphics()->QuadsBegin();

	for(size_t i = 0; i < TrajectoryPoints.size() - 1; i++)
	{
		float Progress = Distances[i] / MaxDistance;
		float Alpha = BaseColor.a * (1.0f - Progress * 0.35f);
		Graphics()->SetColor(BaseColor.r, BaseColor.g, BaseColor.b, Alpha);

		vec2 A = TrajectoryPoints[i];
		vec2 B = TrajectoryPoints[i + 1];
		vec2 Dir = B - A;
		float Len = length(Dir);
		if(Len < 0.001f)
			continue;
		Dir /= Len;

		// Perpendicular offset for line thickness
		vec2 Out = vec2(Dir.y, -Dir.x) * HalfWidth;
		IGraphics::CFreeformItem Freeform(A - Out, A + Out, B - Out, B + Out);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);
	}

	Graphics()->QuadsEnd();

	// Restore screen-space coordinate system for subsequent HUD rendering
	Graphics()->MapScreen(0, 0, Graphics()->ScreenWidth(), Graphics()->ScreenHeight());
}

void CSystemVisuals::RenderFireAss()
{
	int LocalClientId = GameClient()->m_Snap.m_LocalClientId;
	if(LocalClientId < 0)
		return;

	const CNetObj_Character *pChar = &GameClient()->m_Snap.m_aCharacters[LocalClientId].m_Cur;
	if(!pChar)
		return;

	vec2 PlayerPos = vec2(pChar->m_X, pChar->m_Y);
	int Direction = pChar->m_Direction;

	vec2 CameraOffset = GameClient()->m_Camera.m_Center;
	float ScreenWidth = Graphics()->ScreenWidth();
	float ScreenHeight = Graphics()->ScreenHeight();

	vec2 ScreenPos;
	ScreenPos.x = (PlayerPos.x - CameraOffset.x) + ScreenWidth / 2.0f;
	ScreenPos.y = (PlayerPos.y - CameraOffset.y) + ScreenHeight / 2.0f;

	vec2 FirePos = ScreenPos;
	FirePos.x -= Direction * 22.0f;
	FirePos.y += 12.0f;

	float Time = Client()->LocalTime();

	Graphics()->TextureClear();

	Graphics()->QuadsBegin();
	float CorePulse = 0.7f + sin(Time * 10.0f) * 0.3f;
	Graphics()->SetColor(1.0f, 0.15f, 0.0f, CorePulse * 0.6f);
	IGraphics::CQuadItem OuterCore(FirePos.x - 20.0f, FirePos.y - 20.0f, 40.0f, 40.0f);
	Graphics()->QuadsDrawTL(&OuterCore, 1);
	Graphics()->SetColor(1.0f, 0.5f, 0.0f, CorePulse * 0.8f);
	IGraphics::CQuadItem MidCore(FirePos.x - 14.0f, FirePos.y - 14.0f, 28.0f, 28.0f);
	Graphics()->QuadsDrawTL(&MidCore, 1);
	Graphics()->SetColor(1.0f, 0.95f, 0.7f, CorePulse);
	IGraphics::CQuadItem InnerCore(FirePos.x - 8.0f, FirePos.y - 8.0f, 16.0f, 16.0f);
	Graphics()->QuadsDrawTL(&InnerCore, 1);
	Graphics()->QuadsEnd();

	for(int i = 0; i < 20; i++)
	{
		float ParticlePhase = fmod(Time * 9.0f + i * 0.2f, 1.0f);
		float RiseHeight = ParticlePhase * 45.0f * (1.0f + sin(Time * 3.0f + i) * 0.2f);
		float Alpha = (1.0f - ParticlePhase) * (0.7f - ParticlePhase * 0.3f);
		float Size = (8.0f - i * 0.1f) * (1.0f - ParticlePhase * 0.7f) * (1.0f + sin(Time * 12.0f + i * 1.5f) * 0.2f);
		float Turbulence = sin(Time * 6.0f + i * 2.5f) * (8.0f - ParticlePhase * 6.0f);

		vec2 FlamePos = FirePos;
		FlamePos.y -= RiseHeight;
		FlamePos.x += Turbulence;

		float ColorPhase = ParticlePhase;
		float R = 1.0f, G = 0.3f + ColorPhase * 0.8f, B = ColorPhase * 0.3f;

		Graphics()->QuadsBegin();
		Graphics()->SetColor(R, G, B, Alpha);
		IGraphics::CQuadItem Flame(FlamePos.x - Size * 0.7f, FlamePos.y - Size * 1.2f, Size * 1.4f, Size * 2.4f);
		Graphics()->QuadsDrawTL(&Flame, 1);
		Graphics()->QuadsEnd();
	}
}

void CSystemVisuals::RenderGhostAccessory()
{
	int LocalClientId = GameClient()->m_Snap.m_LocalClientId;
	if(LocalClientId < 0)
		return;

	const CNetObj_Character *pChar = &GameClient()->m_Snap.m_aCharacters[LocalClientId].m_Cur;
	if(!pChar)
		return;

	CGameClient::CClientData *pLocalClient = &GameClient()->m_aClients[LocalClientId];
	if(!pLocalClient || !pLocalClient->m_Active)
		return;

	vec2 PlayerPos = vec2(pChar->m_X, pChar->m_Y);

	vec2 CameraOffset = GameClient()->m_Camera.m_Center;
	float ScreenWidth = Graphics()->ScreenWidth();
	float ScreenHeight = Graphics()->ScreenHeight();

	vec2 ScreenPos;
	ScreenPos.x = (PlayerPos.x - CameraOffset.x) + ScreenWidth / 2.0f;
	ScreenPos.y = (PlayerPos.y - CameraOffset.y) + ScreenHeight / 2.0f;

	float Time = Client()->LocalTime();
	float FloatOffset = sin(Time * 2.0f) * 8.0f;
	float HorizontalDrift = cos(Time * 1.5f) * 4.0f;

	vec2 GhostPos = ScreenPos;
	GhostPos.x += 45.0f + HorizontalDrift;
	GhostPos.y -= 35.0f + FloatOffset;

	float GhostAlpha = 0.4f + sin(Time * 3.0f) * 0.15f;

	Graphics()->TextureClear();

	Graphics()->QuadsBegin();
	float AuraPulse = 0.3f + sin(Time * 4.0f) * 0.2f;
	Graphics()->SetColor(0.6f, 0.8f, 1.0f, AuraPulse * 0.3f);
	IGraphics::CQuadItem OuterAura(GhostPos.x - 40.0f, GhostPos.y - 40.0f, 80.0f, 80.0f);
	Graphics()->QuadsDrawTL(&OuterAura, 1);
	Graphics()->SetColor(0.7f, 0.9f, 1.0f, AuraPulse * 0.5f);
	IGraphics::CQuadItem MidAura(GhostPos.x - 30.0f, GhostPos.y - 30.0f, 60.0f, 60.0f);
	Graphics()->QuadsDrawTL(&MidAura, 1);
	Graphics()->QuadsEnd();

	Graphics()->QuadsBegin();
	Graphics()->SetColor(0.8f, 0.95f, 1.0f, GhostAlpha * 0.6f);
	IGraphics::CQuadItem GhostBody(GhostPos.x - 20.0f, GhostPos.y - 20.0f, 40.0f, 40.0f);
	Graphics()->QuadsDrawTL(&GhostBody, 1);
	Graphics()->QuadsEnd();

	Graphics()->QuadsBegin();
	float EyeGlow = 0.8f + sin(Time * 8.0f) * 0.2f;
	Graphics()->SetColor(0.3f, 0.7f, 1.0f, GhostAlpha * EyeGlow);
	IGraphics::CQuadItem LeftEye(GhostPos.x - 10.0f, GhostPos.y - 5.0f, 6.0f, 6.0f);
	Graphics()->QuadsDrawTL(&LeftEye, 1);
	IGraphics::CQuadItem RightEye(GhostPos.x + 4.0f, GhostPos.y - 5.0f, 6.0f, 6.0f);
	Graphics()->QuadsDrawTL(&RightEye, 1);
	Graphics()->QuadsEnd();

	for(int i = 0; i < 12; i++)
	{
		float OrbitAngle = (Time * 1.5f + i * (3.14159f * 2.0f / 12.0f));
		float OrbitRadius = 25.0f + sin(Time * 3.0f + i) * 5.0f;

		vec2 ParticlePos = GhostPos;
		ParticlePos.x += cos(OrbitAngle) * OrbitRadius;
		ParticlePos.y += sin(OrbitAngle) * OrbitRadius;

		float ParticleSize = 3.0f + sin(Time * 4.0f + i) * 1.5f;
		float ParticleAlpha = (0.4f + sin(Time * 2.0f + i * 0.5f) * 0.2f) * GhostAlpha;

		Graphics()->QuadsBegin();
		Graphics()->SetColor(0.6f, 0.9f, 1.0f, ParticleAlpha);
		IGraphics::CQuadItem Particle(ParticlePos.x - ParticleSize, ParticlePos.y - ParticleSize, ParticleSize * 2.0f, ParticleSize * 2.0f);
		Graphics()->QuadsDrawTL(&Particle, 1);
		Graphics()->QuadsEnd();
	}
}
