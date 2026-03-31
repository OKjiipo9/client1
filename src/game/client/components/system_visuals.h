#ifndef GAME_CLIENT_COMPONENTS_SYSTEM_VISUALS_H
#define GAME_CLIENT_COMPONENTS_SYSTEM_VISUALS_H

#include <base/vmath.h>
#include <game/client/component.h>
#include <vector>

class CSystemVisuals : public CComponent
{
public:
	// Visual features
	bool m_WaterMarkEnabled = true;
	bool m_FunctionListEnabled = false;
	bool m_FunctionListRainbow = false;
	bool m_SpeedTabEnabled = false;
	bool m_RainbowMeEnabled = false;
	bool m_PredictionEnabled = false;
	bool m_FireAssEnabled = false;
	bool m_GhostAccessoryEnabled = false;

	// Shared HUD theme for WaterMark / FunctionList / SpeedTab
	// 0 = Dark Violet (default), 1 = Minimal, 2 = Neon
	// 3 = Aurora, 4 = Cyber, 5 = Sunset, 6 = Midnight, 7 = Pastel
	int m_HudThemeStyle = 0;
	bool m_HudLayoutMode = false;

	// Prediction settings
	int m_PredictionDistance = 5;

	// Component interface
	virtual int Sizeof() const override { return sizeof(*this); }
	virtual void OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnWindowResize() override;
	virtual bool OnCursorMove(float x, float y, IInput::ECursorType CursorType) override;
	void ResetHudLayoutPositions();

private:
	void RenderWaterMark();
	void RenderFunctionList();
	void RenderSpeedTab();
	void RenderPrediction();
	void RenderFireAss();
	void RenderGhostAccessory();
	void ApplyRainbowColors();
	void HandleHudDragging();
	void HandleDummySwapBind();
	void InitDefaultHudPositions();
	bool MouseInsideRect(const vec2 &MousePos, const vec2 &RectPos, float RectW, float RectH) const;
	void ClampHudPositions(float ScreenW, float ScreenH, float FunctionListH);

	enum class EDragTarget
	{
		NONE = 0,
		WATERMARK,
		FUNCTIONLIST,
		SPEEDTAB,
	};

	// Draggable HUD positions (top-left corner) in GUI/screen coordinates.
	// Defaults mimic the original fixed placements.
	vec2 m_WaterMarkPos = vec2(0.0f, 10.0f);
	vec2 m_FunctionListPos = vec2(0.0f, 0.0f);
	vec2 m_SpeedTabPos = vec2(10.0f, 0.0f);

	// Drag state
	EDragTarget m_DragTarget = EDragTarget::NONE;
	vec2 m_DragOffset = vec2(0.0f, 0.0f); // mousePos - rectPos at drag start
	bool m_HasDragInProgress = false;
	bool m_ForcedAbsoluteMouseInLayout = false;
	vec2 m_LayoutMousePos = vec2(0.0f, 0.0f);
	bool m_LastLayoutMouseButton1 = false;

	float m_MaxSpeed = 0.0f;
	// Dummy swap bind: track previous key state to detect edges
	bool m_DummySwapBindPrev = false;
};

#endif
