/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include "effects.h"

#include <algorithm>
#include <cmath>
#include <base/color.h>
#include <base/math.h>
#include <base/time.h>

#include <engine/demo.h>
#include <engine/shared/config.h>

#include <generated/client_data.h>

#include <game/client/components/damageind.h>
#include <game/client/components/flow.h>
#include <game/client/components/particles.h>
#include <game/client/components/sounds.h>
#include <game/client/gameclient.h>

CEffects::CEffects()
{
	m_Add5hz = false;
	m_Add50hz = false;
	m_Add100hz = false;
}

void CEffects::AirJump(vec2 Pos, float Alpha, float Volume)
{
	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_AIRJUMP;
	p.m_Pos = Pos + vec2(-6.0f, 16.0f);
	p.m_Vel = vec2(0.0f, -200.0f);
	p.m_LifeSpan = 0.5f;
	p.m_StartSize = 48.0f;
	p.m_EndSize = 0.0f;
	p.m_Rot = random_angle();
	p.m_Rotspeed = pi * 2.0f;
	p.m_Gravity = 500.0f;
	p.m_Friction = 0.7f;
	p.m_FlowAffected = 0.0f;
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);

	p.m_Pos = Pos + vec2(6.0f, 16.0f);
	GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);

	if(g_Config.m_ClSysAirJumpFx)
	{
		const float Intensity = std::clamp((float)g_Config.m_ClSysAirJumpFxIntensity / 100.0f, 0.0f, 2.0f);
		const ColorRGBA FxColor = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClSysAirJumpFxColor, true)).WithAlpha(Alpha);
		const int NumPuffs = (int)round_to_int(6.0f + 10.0f * Intensity);
		for(int i = 0; i < NumPuffs; i++)
		{
			p.SetDefault();
			p.m_Spr = SPRITE_PART_SMOKE;
			p.m_Pos = Pos + vec2(0.0f, 18.0f) + random_direction() * random_float(4.0f, 14.0f);
			p.m_Vel = vec2(0.0f, -1.0f) * random_float(200.0f, 450.0f) + random_direction() * random_float(80.0f, 180.0f);
			p.m_LifeSpan = random_float(0.25f, 0.45f);
			p.m_StartSize = random_float(18.0f, 30.0f) * (0.75f + 0.50f * Intensity);
			p.m_EndSize = 0.0f;
			p.m_Friction = 0.65f;
			p.m_Gravity = random_float(-200.0f);
			p.m_Collides = false;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = Alpha;
			p.m_EndAlpha = 0.0f;
			p.m_Color = FxColor;
			GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
		}
	}

	if(g_Config.m_SndGame)
		GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_PLAYER_AIRJUMP, Volume, Pos);
}

void CEffects::DamageIndicator(vec2 Pos, vec2 Dir, float Alpha)
{
	GameClient()->m_DamageInd.Create(Pos, Dir, Alpha);
}

void CEffects::PowerupShine(vec2 Pos, vec2 Size, float Alpha)
{
	if(!m_Add50hz)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_SLICE;
	p.m_Pos = Pos + vec2(random_float(-0.5f, 0.5f), random_float(-0.5f, 0.5f)) * Size;
	p.m_Vel = vec2(0.0f, 0.0f);
	p.m_LifeSpan = 0.5f;
	p.m_StartSize = 16.0f;
	p.m_EndSize = 0.0f;
	p.m_Rot = random_angle();
	p.m_Rotspeed = pi * 2.0f;
	p.m_Gravity = 500.0f;
	p.m_Friction = 0.9f;
	p.m_FlowAffected = 0.0f;
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
}

void CEffects::FreezingFlakes(vec2 Pos, vec2 Size, float Alpha)
{
	if(!m_Add5hz)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_SNOWFLAKE;
	p.m_Pos = Pos + vec2(random_float(-0.5f, 0.5f), random_float(-0.5f, 0.5f)) * Size;
	p.m_Vel = vec2(0.0f, 0.0f);
	p.m_LifeSpan = 1.5f;
	p.m_StartSize = random_float(0.5f, 1.5f) * 16.0f;
	p.m_EndSize = p.m_StartSize * 0.5f;
	p.m_UseAlphaFading = true;
	p.m_StartAlpha = 1.0f;
	p.m_EndAlpha = 0.0f;
	p.m_Rot = random_angle();
	p.m_Rotspeed = pi;
	p.m_Gravity = random_float(250.0f);
	p.m_Friction = 0.9f;
	p.m_FlowAffected = 0.0f;
	p.m_Collides = false;
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_EXTRA, &p);
}

void CEffects::SparkleTrail(vec2 Pos, float Alpha)
{
	if(!m_Add50hz)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_SPARKLE;
	p.m_Pos = Pos + random_direction() * random_float(40.0f);
	p.m_Vel = vec2(0.0f, 0.0f);
	p.m_LifeSpan = 0.5f;
	p.m_StartSize = 0.0f;
	p.m_EndSize = random_float(20.0f, 30.0f);
	p.m_UseAlphaFading = true;
	p.m_StartAlpha = Alpha;
	p.m_EndAlpha = std::min(0.2f, Alpha);
	p.m_Collides = false;
	GameClient()->m_Particles.Add(CParticles::GROUP_TRAIL_EXTRA, &p);
}

void CEffects::SmokeTrail(vec2 Pos, vec2 Vel, float Alpha, float TimePassed)
{
	if(!m_Add50hz && TimePassed < 0.001f)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_SMOKE;
	p.m_Pos = Pos;
	p.m_Vel = Vel + random_direction() * 50.0f;
	p.m_LifeSpan = random_float(0.5f, 1.0f);
	p.m_StartSize = random_float(12.0f, 20.0f);
	p.m_EndSize = 0.0f;
	p.m_Friction = 0.7f;
	p.m_Gravity = random_float(-500.0f);
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_PROJECTILE_TRAIL, &p, TimePassed);
}

void CEffects::SkidTrail(vec2 Pos, vec2 Vel, int Direction, float Alpha, float Volume)
{
	if(m_Add100hz)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SMOKE;
		p.m_Pos = Pos + vec2(-Direction * 6.0f, 12.0f);
		p.m_Vel = vec2(-Direction * 100.0f * length(Vel), -50.0f) + random_direction() * 50.0f;
		p.m_LifeSpan = random_float(0.5f, 1.0f);
		p.m_StartSize = random_float(24.0f, 36.0f);
		p.m_EndSize = 0.0f;
		p.m_Friction = 0.7f;
		p.m_Gravity = random_float(-500.0f);
		p.m_Color = ColorRGBA(0.75f, 0.75f, 0.75f, Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
	if(g_Config.m_SndGame)
	{
		int64_t Now = time();
		if(Now - m_SkidSoundTimer > time_freq() / 10)
		{
			m_SkidSoundTimer = Now;
			GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_PLAYER_SKID, Volume, Pos);
		}
	}
}

void CEffects::BulletTrail(vec2 Pos, float Alpha, float TimePassed)
{
	if(!m_Add100hz && TimePassed < 0.001f)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_BALL;
	p.m_Pos = Pos;
	p.m_LifeSpan = random_float(0.25f, 0.5f);
	p.m_StartSize = 8.0f;
	p.m_EndSize = 0.0f;
	p.m_Friction = 0.7f;
	p.m_Color.a *= Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_PROJECTILE_TRAIL, &p, TimePassed);

	if(g_Config.m_ClSysBulletTrailFx)
	{
		const float Intensity = std::clamp((float)g_Config.m_ClSysBulletTrailFxIntensity / 100.0f, 0.0f, 2.0f);
		const ColorRGBA FxColor = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClSysBulletTrailFxColor, true)).WithAlpha(Alpha);
		const int Num = (int)round_to_int(1.0f + 2.0f * Intensity);
		for(int i = 0; i < Num; i++)
		{
			p.SetDefault();
			p.m_Spr = SPRITE_PART_SMOKE;
			p.m_Pos = Pos + random_direction() * random_float(2.0f, 8.0f);
			p.m_Vel = random_direction() * random_float(20.0f, 80.0f);
			p.m_LifeSpan = random_float(0.15f, 0.25f) + 0.05f * Intensity;
			p.m_StartSize = random_float(10.0f, 16.0f) * (0.75f + 0.50f * Intensity);
			p.m_EndSize = 0.0f;
			p.m_Friction = 0.75f;
			p.m_Gravity = random_float(-100.0f);
			p.m_Collides = false;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = Alpha;
			p.m_EndAlpha = 0.0f;
			p.m_Color = FxColor.WithAlpha(Alpha * 0.6f);
			GameClient()->m_Particles.Add(CParticles::GROUP_PROJECTILE_TRAIL, &p, TimePassed);
		}
	}
}

void CEffects::PlayerSpawn(vec2 Pos, float Alpha, float Volume)
{
	for(int i = 0; i < 32; i++)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SHELL;
		p.m_Pos = Pos;
		p.m_Vel = random_direction() * (std::pow(random_float(), 3) * 600.0f);
		p.m_LifeSpan = random_float(0.3f, 0.6f);
		p.m_StartSize = random_float(64.0f, 96.0f);
		p.m_EndSize = 0.0f;
		p.m_Rot = random_angle();
		p.m_Rotspeed = random_float();
		p.m_Gravity = random_float(-400.0f);
		p.m_Friction = 0.7f;
		p.m_Color = ColorRGBA(0xb5 / 255.0f, 0x50 / 255.0f, 0xcb / 255.0f, Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
	if(g_Config.m_SndGame)
		GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_PLAYER_SPAWN, Volume, Pos);
}

void CEffects::PlayerDeath(vec2 Pos, int ClientId, float Alpha)
{
	ColorRGBA BloodColor(1.0f, 1.0f, 1.0f);

	if(ClientId >= 0)
	{
		// Use m_RenderInfo.m_CustomColoredSkin instead of m_UseCustomColor
		// m_UseCustomColor says if the player's skin has a custom color (value sent from the client side)

		// m_RenderInfo.m_CustomColoredSkin Defines if in the context of the game the color is being customized,
		// Using this value if the game is teams (red and blue), this value will be true even if the skin is with the normal color.
		// And will use the team body color to create player death effect instead of tee color
		if(GameClient()->Client()->IsSixup())
		{
			if(GameClient()->m_aClients[ClientId].m_RenderInfo.m_aSixup[g_Config.m_ClDummy].m_aUseCustomColors[protocol7::SKINPART_BODY])
			{
				BloodColor = GameClient()->m_aClients[ClientId].m_RenderInfo.m_aSixup[g_Config.m_ClDummy].m_aColors[protocol7::SKINPART_BODY];
			}
			else
			{
				BloodColor = GameClient()->m_aClients[ClientId].m_RenderInfo.m_aSixup[g_Config.m_ClDummy].m_BloodColor;
			}
		}
		else
		{
			if(GameClient()->m_aClients[ClientId].m_RenderInfo.m_CustomColoredSkin)
			{
				BloodColor = GameClient()->m_aClients[ClientId].m_RenderInfo.m_ColorBody;
			}
			else
			{
				BloodColor = GameClient()->m_aClients[ClientId].m_RenderInfo.m_BloodColor;
			}
		}
	}

	for(int i = 0; i < 64; i++)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SPLAT01 + (rand() % 3);
		p.m_Pos = Pos;
		p.m_Vel = random_direction() * (random_float(0.1f, 1.1f) * 900.0f);
		p.m_LifeSpan = random_float(0.3f, 0.6f);
		p.m_StartSize = random_float(24.0f, 40.0f);
		p.m_EndSize = 0.0f;
		p.m_Rot = random_angle();
		p.m_Rotspeed = random_float(-0.5f, 0.5f) * pi;
		p.m_Gravity = 800.0f;
		p.m_Friction = 0.8f;
		p.m_Color = BloodColor.Multiply(random_float(0.75f, 1.0f)).WithAlpha(0.75f * Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
}

void CEffects::Confetti(vec2 Pos, float Alpha)
{
	ColorRGBA Red(1.0f, 0.4f, 0.4f);
	ColorRGBA Green(0.4f, 1.0f, 0.4f);
	ColorRGBA Blue(0.4f, 0.4f, 1.0f);
	ColorRGBA Yellow(1.0f, 1.0f, 0.4f);
	ColorRGBA Cyan(0.4f, 1.0f, 1.0f);
	ColorRGBA Magenta(1.0f, 0.4f, 1.0f);

	ColorRGBA aConfettiColors[] = {Red, Green, Blue, Yellow, Cyan, Magenta};

	// powerful confettis
	for(int i = 0; i < 32; i++)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SPLAT01 + (rand() % 3);
		p.m_Pos = Pos;
		p.m_Vel = direction(-0.5f * pi + random_float(-0.2f, 0.2f)) * random_float(0.01f, 1.0f) * 2000.0f;
		p.m_LifeSpan = random_float(1.0f, 1.2f);
		p.m_StartSize = random_float(12.0f, 24.0f);
		p.m_EndSize = 0.0f;
		p.m_Rot = random_angle();
		p.m_Rotspeed = random_float(-0.5f, 0.5f) * pi;
		p.m_Gravity = -700.0f;
		p.m_Friction = 0.6f;
		ColorRGBA c = aConfettiColors[(rand() % std::size(aConfettiColors))];
		p.m_Color = c.WithMultipliedAlpha(0.75f * Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}

	// broader confettis
	for(int i = 0; i < 32; i++)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SPLAT01 + (rand() % 3);
		p.m_Pos = Pos;
		p.m_Vel = direction(-0.5f * pi + random_float(-0.8f, 0.8f)) * random_float(0.01f, 1.0f) * 1500.0f;
		p.m_LifeSpan = random_float(0.8f, 1.0f);
		p.m_StartSize = random_float(12.0f, 24.0f);
		p.m_EndSize = 0.0f;
		p.m_Rot = random_angle();
		p.m_Rotspeed = random_float(-0.5f, 0.5f) * pi;
		p.m_Gravity = -700.0f;
		p.m_Friction = 0.6f;
		ColorRGBA c = aConfettiColors[(rand() % std::size(aConfettiColors))];
		p.m_Color = c.WithMultipliedAlpha(0.75f * Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
}

void CEffects::Explosion(vec2 Pos, float Alpha)
{
	// add to flow
	for(int y = -8; y <= 8; y++)
		for(int x = -8; x <= 8; x++)
		{
			if(x == 0 && y == 0)
				continue;

			float a = 1 - (length(vec2(x, y)) / length(vec2(8.0f, 8.0f)));
			GameClient()->m_Flow.Add(Pos + vec2(x, y) * 16.0f, normalize(vec2(x, y)) * 5000.0f * a, 10.0f);
		}

	// add the explosion
	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_EXPL01;
	p.m_Pos = Pos;
	p.m_LifeSpan = 0.4f;
	p.m_StartSize = 150.0f;
	p.m_EndSize = 0.0f;
	p.m_Rot = random_angle();
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);

	// Nudge position slightly to edge of closest tile so the
	// smoke doesn't get stuck inside the tile.
	if(Collision()->CheckPoint(Pos))
	{
		const vec2 DistanceToTopLeft = Pos - vec2(round_truncate(Pos.x / 32), round_truncate(Pos.y / 32)) * 32;

		vec2 CheckOffset;
		CheckOffset.x = (DistanceToTopLeft.x > 16.0f ? 32.0f : -1.0f);
		CheckOffset.y = (DistanceToTopLeft.y > 16.0f ? 32.0f : -1.0f);
		CheckOffset -= DistanceToTopLeft;

		for(vec2 Mask : {vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f)})
		{
			const vec2 NewPos = Pos + CheckOffset * Mask;
			if(!Collision()->CheckPoint(NewPos))
			{
				Pos = NewPos;
				break;
			}
		}
	}

	// add the smoke
	for(int i = 0; i < 24; i++)
	{
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SMOKE;
		p.m_Pos = Pos;
		p.m_Vel = random_direction() * (random_float(1.0f, 1.2f) * 1000.0f);
		p.m_LifeSpan = random_float(0.5f, 0.9f);
		p.m_StartSize = random_float(32.0f, 40.0f);
		p.m_EndSize = 0.0f;
		p.m_Gravity = random_float(-800.0f);
		p.m_Friction = 0.4f;
		p.m_Color = ColorRGBA(1.0f, 1.0f, 1.0f).Multiply(random_float(0.5f, 0.75f)).WithAlpha(Alpha);
		p.m_StartAlpha = p.m_Color.a;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
}

void CEffects::HammerHit(vec2 Pos, float Alpha, float Volume)
{
	// base hit
	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_HIT01;
	p.m_Pos = Pos;
	p.m_LifeSpan = 0.3f;
	p.m_StartSize = 120.0f;
	p.m_EndSize = 0.0f;
	p.m_Rot = random_angle();
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);

	// enhanced visual layer (system client)
	if(g_Config.m_ClSysHammerHitFx)
	{
		const float Intensity = std::clamp((float)g_Config.m_ClSysHammerHitFxIntensity / 100.0f, 0.0f, 2.0f);
		const int Style = std::clamp(g_Config.m_ClSysHammerHitFxStyle, 0, 5);
		ColorRGBA FxColor = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClSysHammerHitFxColor, true)).WithAlpha(Alpha);
		switch(Style)
		{
		case 1: // Storm
			FxColor = FxColor.Multiply(1.10f);
			break;
		case 2: // Nova
			FxColor = ColorRGBA(1.0f, 0.72f, 0.36f, Alpha);
			break;
		case 3: // Void
			FxColor = ColorRGBA(0.62f, 0.42f, 1.0f, Alpha);
			break;
		case 4: // Frost
			FxColor = ColorRGBA(0.58f, 0.90f, 1.0f, Alpha);
			break;
		case 5: // Inferno
			FxColor = ColorRGBA(1.0f, 0.38f, 0.12f, Alpha);
			break;
		default: // Impact
			break;
		}

		// Effect 1: double expanding shockwave rings
		{
			const float OuterLife = 0.20f + 0.10f * Intensity + (Style == 3 ? 0.06f : 0.0f) + (Style == 4 ? 0.03f : 0.0f);
			const float InnerLife = 0.16f + 0.08f * Intensity + (Style == 1 ? 0.03f : 0.0f);

			// Outer ring (main)
			p.SetDefault();
			p.m_Spr = SPRITE_PART_HIT01;
			p.m_Pos = Pos;
			p.m_LifeSpan = OuterLife;
			p.m_StartSize = 48.0f + 14.0f * Intensity;
			p.m_EndSize = 210.0f + 55.0f * Intensity + (Style == 2 ? 40.0f : 0.0f) + (Style == 5 ? 20.0f : 0.0f);
			p.m_Rot = random_angle();
			p.m_Rotspeed = (0.80f + 0.55f * Intensity) * pi;
			p.m_Collides = false;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = Alpha;
			p.m_EndAlpha = 0.0f;
			p.m_Color = FxColor;
			GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);

			// Inner ring (faster / opposite spin)
			p.SetDefault();
			p.m_Spr = SPRITE_PART_HIT01;
			p.m_Pos = Pos;
			p.m_LifeSpan = InnerLife;
			p.m_StartSize = 30.0f + 10.0f * Intensity;
			p.m_EndSize = 130.0f + 35.0f * Intensity + (Style == 1 ? 25.0f : 0.0f);
			p.m_Rot = random_angle();
			p.m_Rotspeed = -(1.15f + 0.40f * Intensity) * pi;
			p.m_Collides = false;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = Alpha * (0.95f - 0.05f * Intensity);
			p.m_EndAlpha = 0.0f;
			p.m_Color = FxColor.Multiply(1.0f);
			GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);

			// Impact flash core (white-hot)
			p.SetDefault();
			p.m_Spr = SPRITE_PART_HIT01;
			p.m_Pos = Pos;
			p.m_LifeSpan = 0.07f + 0.02f * Intensity;
			p.m_StartSize = 18.0f + 8.0f * Intensity;
			p.m_EndSize = 0.0f;
			p.m_Rot = random_angle();
			p.m_Rotspeed = (1.8f + 0.6f * Intensity) * pi;
			p.m_Collides = false;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = Alpha * (Style == 2 ? 0.95f : 0.85f);
			p.m_EndAlpha = 0.0f;
			p.m_Color = ColorRGBA(1.0f, 1.0f, 1.0f).WithAlpha(Alpha * 0.85f);
			GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);
		}

		// Effect 2: lightning-like sparks burst
		{
			const int NumSparks = (int)round_to_int(18.0f + 16.0f * Intensity + (Style == 1 ? 8.0f : 0.0f) + (Style == 2 ? 5.0f : 0.0f) + (Style == 5 ? 8.0f : 0.0f));
			for(int i = 0; i < NumSparks; i++)
			{
				p.SetDefault();
				p.m_Spr = SPRITE_PART_SMOKE;
				p.m_Pos = Pos + random_direction() * random_float(5.0f, 22.0f);

				vec2 Dir = random_direction();
				// Bias some sparks a bit upward for a more "electric" look.
				if(i % 3 == 0)
					Dir.y = std::abs(Dir.y);

				const float StyleSpeedMul = Style == 3 ? 0.85f : (Style == 1 ? 1.18f : (Style == 5 ? 1.25f : 1.0f));
				const float Speed = random_float(700.0f, 1500.0f) * (0.70f + 0.40f * Intensity) * StyleSpeedMul;
				p.m_Vel = Dir * Speed;

				p.m_LifeSpan = random_float(0.16f, 0.30f) + 0.03f * Intensity;
				p.m_StartSize = random_float(12.0f, 24.0f) * (0.65f + 0.60f * Intensity);
				p.m_EndSize = 0.0f;
				if(Style == 4)
					p.m_Gravity = random_float(-80.0f, 160.0f);
				else
					p.m_Gravity = random_float(Style == 3 ? 220.0f : 520.0f, Style == 3 ? 650.0f : 1050.0f);
				p.m_Friction = 0.50f;
				p.m_Collides = false;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha;
				p.m_EndAlpha = 0.0f;

				// Slight per-spark color variance, but keep base hue.
				p.m_Color = FxColor.Multiply(random_float(0.75f, Style == 2 ? 1.25f : 1.05f));
				GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
			}

			// A few bright glints on top of the sparks
			const int NumGlints = (int)round_to_int(4.0f + 3.0f * Intensity + (Style == 2 ? 3.0f : 0.0f));
			for(int i = 0; i < NumGlints; i++)
			{
				p.SetDefault();
				p.m_Spr = SPRITE_PART_HIT01;
				p.m_Pos = Pos + random_direction() * random_float(4.0f, 18.0f);
				p.m_Vel = random_direction() * random_float(180.0f, 520.0f);
				p.m_LifeSpan = random_float(0.10f, 0.18f);
				p.m_StartSize = random_float(10.0f, 20.0f) * (0.80f + 0.40f * Intensity);
				p.m_EndSize = 0.0f;
				p.m_Rot = random_angle();
				p.m_Rotspeed = random_float(2.0f, 4.0f) * pi;
				p.m_Collides = false;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha * (Style == 3 ? 0.80f : 0.95f);
				p.m_EndAlpha = 0.0f;
				p.m_Color = ColorRGBA(1.0f, 1.0f, 1.0f).WithAlpha(Alpha);
				GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);
			}
		}

		// Effect 3: afterglow + optional ember trail
		{
			const int NumAfterglow = (int)round_to_int(10.0f + 10.0f * Intensity + (Style == 3 ? 6.0f : 0.0f) + (Style == 4 ? 4.0f : 0.0f));
			for(int i = 0; i < NumAfterglow; i++)
			{
				p.SetDefault();
				p.m_Spr = SPRITE_PART_SMOKE;
				p.m_Pos = Pos + random_direction() * random_float(0.0f, 10.0f);
				p.m_Vel = random_direction() * random_float(40.0f, 260.0f) * (0.55f + 0.45f * Intensity);
				p.m_LifeSpan = random_float(0.22f, 0.42f) + 0.04f * Intensity;
				p.m_StartSize = random_float(16.0f, 34.0f) * (0.55f + 0.55f * Intensity);
				p.m_EndSize = 0.0f;
				if(Style == 4)
					p.m_Gravity = random_float(-260.0f, -40.0f);
				else
					p.m_Gravity = random_float(Style == 3 ? -60.0f : 120.0f, Style == 3 ? 220.0f : 420.0f);
				p.m_Friction = 0.70f;
				p.m_Collides = false;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha * 0.85f;
				p.m_EndAlpha = 0.0f;
				p.m_Color = FxColor.WithAlpha(Alpha * (Style == 3 ? 0.58f : 0.70f)).Multiply(random_float(0.85f, Style == 2 ? 1.25f : 1.1f));
				GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
			}

			if(g_Config.m_ClSysHammerHitTrail)
			{
				// Ember trail (slightly longer and dimmer than sparks)
				const int NumEmbers = (int)round_to_int(14.0f + 16.0f * Intensity + (Style == 1 ? 8.0f : 0.0f));
				for(int i = 0; i < NumEmbers; i++)
				{
					p.SetDefault();
					p.m_Spr = SPRITE_PART_SMOKE;
					p.m_Pos = Pos + random_direction() * random_float(4.0f, 18.0f);
					p.m_Vel = random_direction() * random_float(260.0f, 720.0f) * (0.65f + 0.35f * Intensity);
					p.m_LifeSpan = random_float(0.24f, 0.50f);
					p.m_StartSize = random_float(10.0f, 18.0f) * (0.80f + 0.45f * Intensity);
					p.m_EndSize = 0.0f;
					p.m_Friction = 0.75f;
					p.m_Gravity = random_float(Style == 3 ? -40.0f : 140.0f, Style == 3 ? 320.0f : 520.0f);
					p.m_Collides = false;
					p.m_UseAlphaFading = true;
					p.m_StartAlpha = Alpha * 0.70f;
					p.m_EndAlpha = 0.0f;
					p.m_Color = FxColor.WithAlpha(Alpha * 0.55f);
					GameClient()->m_Particles.Add(CParticles::GROUP_PROJECTILE_TRAIL, &p);
				}
			}
		}

		// Effect 4: directional blades (new layer for stronger hit readability)
		{
			const int BladeCount = 4 + (int)round_to_int(4.0f * Intensity) + (Style == 1 ? 2 : 0);
			for(int i = 0; i < BladeCount; i++)
			{
				const float Angle = (2.0f * pi * i) / (float)std::max(1, BladeCount) + random_float(-0.25f, 0.25f);
				vec2 Dir = vec2(cos(Angle), sin(Angle));
				p.SetDefault();
				p.m_Spr = SPRITE_PART_HIT01;
				p.m_Pos = Pos + Dir * random_float(2.0f, 10.0f);
				p.m_Vel = Dir * random_float(260.0f, 780.0f) * (0.70f + 0.50f * Intensity);
				p.m_LifeSpan = random_float(0.10f, 0.22f);
				p.m_StartSize = random_float(14.0f, 32.0f) * (0.8f + 0.40f * Intensity);
				p.m_EndSize = 0.0f;
				p.m_Rot = Angle;
				p.m_Rotspeed = random_float(-2.5f, 2.5f) * pi;
				p.m_Friction = 0.62f;
				p.m_Gravity = random_float(40.0f, 260.0f);
				p.m_Collides = false;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha * (Style == 3 ? 0.70f : 0.95f);
				p.m_EndAlpha = 0.0f;
				p.m_Color = FxColor.Multiply(random_float(0.85f, 1.2f));
				GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);
			}
		}

		// Effect 5: style-specific secondary burst.
		if(Style == 4 || Style == 5)
		{
			const int NumSecondary = (int)round_to_int(8.0f + 8.0f * Intensity);
			for(int i = 0; i < NumSecondary; i++)
			{
				p.SetDefault();
				p.m_Spr = SPRITE_PART_SPLAT01 + (i % 3);
				p.m_Pos = Pos + random_direction() * random_float(3.0f, 14.0f);
				p.m_Vel = random_direction() * random_float(180.0f, 540.0f) * (0.75f + 0.45f * Intensity);
				p.m_LifeSpan = random_float(0.16f, 0.32f);
				p.m_StartSize = random_float(10.0f, 24.0f);
				p.m_EndSize = 0.0f;
				p.m_Collides = false;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha * 0.85f;
				p.m_EndAlpha = 0.0f;
				if(Style == 4)
				{
					p.m_Gravity = random_float(-280.0f, -20.0f);
					p.m_Color = ColorRGBA(0.75f, 0.95f, 1.0f, Alpha);
				}
				else
				{
					p.m_Gravity = random_float(260.0f, 760.0f);
					p.m_Color = ColorRGBA(1.0f, 0.45f, 0.18f, Alpha);
				}
				GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
			}
		}

		// Effect 6: rotating rune halo (completely new ornamental ring).
		{
			const int RuneCount = 6 + (int)round_to_int(4.0f * Intensity);
			const float RingRadius = 30.0f + 28.0f * Intensity;
			for(int i = 0; i < RuneCount; i++)
			{
				const float BaseAngle = (2.0f * pi * i) / (float)std::max(1, RuneCount);
				const float SpinSign = (i % 2 == 0) ? 1.0f : -1.0f;
				vec2 Dir = vec2(cos(BaseAngle), sin(BaseAngle));
				p.SetDefault();
				p.m_Spr = SPRITE_PART_HIT01;
				p.m_Pos = Pos + Dir * RingRadius;
				p.m_Vel = vec2(-Dir.y, Dir.x) * random_float(120.0f, 280.0f) * SpinSign;
				p.m_LifeSpan = random_float(0.18f, 0.34f) + 0.04f * Intensity;
				p.m_StartSize = random_float(8.0f, 20.0f) * (0.8f + 0.35f * Intensity);
				p.m_EndSize = 0.0f;
				p.m_Rot = BaseAngle;
				p.m_Rotspeed = random_float(1.4f, 3.0f) * pi * SpinSign;
				p.m_Collides = false;
				p.m_Friction = 0.78f;
				p.m_Gravity = (Style == 4) ? random_float(-180.0f, 40.0f) : random_float(40.0f, 260.0f);
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha * (Style == 3 ? 0.58f : 0.75f);
				p.m_EndAlpha = 0.0f;
				p.m_Color = FxColor.Multiply(random_float(0.9f, 1.25f));
				GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);
			}
		}

		// Effect 7: impact dust dome + recoil sparks (new heavy feel layer).
		{
			const int NumDust = (int)round_to_int(16.0f + 20.0f * Intensity + (Style == 5 ? 8.0f : 0.0f));
			for(int i = 0; i < NumDust; i++)
			{
				p.SetDefault();
				p.m_Spr = SPRITE_PART_SMOKE;
				vec2 Dir = random_direction();
				Dir.y = std::abs(Dir.y); // push dome upward from impact
				p.m_Pos = Pos + vec2(random_float(-10.0f, 10.0f), random_float(-4.0f, 6.0f));
				p.m_Vel = Dir * random_float(130.0f, 420.0f) * (0.85f + 0.55f * Intensity);
				p.m_LifeSpan = random_float(0.22f, 0.46f);
				p.m_StartSize = random_float(12.0f, 30.0f) * (0.75f + 0.45f * Intensity);
				p.m_EndSize = 0.0f;
				p.m_Friction = 0.68f;
				p.m_Gravity = random_float(220.0f, 680.0f);
				p.m_Collides = false;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha * 0.60f;
				p.m_EndAlpha = 0.0f;
				if(Style == 5)
					p.m_Color = ColorRGBA(1.0f, 0.42f, 0.12f, Alpha * 0.70f);
				else if(Style == 4)
					p.m_Color = ColorRGBA(0.72f, 0.95f, 1.0f, Alpha * 0.66f);
				else
					p.m_Color = FxColor.WithAlpha(Alpha * 0.55f).Multiply(random_float(0.7f, 1.0f));
				GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
			}

			const int NumRecoil = (int)round_to_int(8.0f + 10.0f * Intensity);
			for(int i = 0; i < NumRecoil; i++)
			{
				p.SetDefault();
				p.m_Spr = SPRITE_PART_SPLAT01 + (i % 3);
				p.m_Pos = Pos + random_direction() * random_float(1.0f, 8.0f);
				p.m_Vel = random_direction() * random_float(220.0f, 680.0f) * (0.7f + 0.4f * Intensity);
				p.m_LifeSpan = random_float(0.10f, 0.20f);
				p.m_StartSize = random_float(8.0f, 18.0f) * (0.8f + 0.3f * Intensity);
				p.m_EndSize = 0.0f;
				p.m_Friction = 0.60f;
				p.m_Gravity = random_float(80.0f, 380.0f);
				p.m_Collides = false;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha * 0.75f;
				p.m_EndAlpha = 0.0f;
				p.m_Color = ColorRGBA(1.0f, 1.0f, 1.0f, Alpha * 0.85f);
				GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
			}
		}

		// Effect 8: helix burst (new) - opposite rotating spiral streams.
		{
			const int HelixSteps = (int)round_to_int(10.0f + 10.0f * Intensity);
			for(int i = 0; i < HelixSteps; i++)
			{
				const float t = (float)i / (float)std::max(1, HelixSteps - 1);
				const float AngleA = t * 5.2f * pi;
				const float AngleB = -t * 5.2f * pi;
				const float Radius = 6.0f + t * (34.0f + 22.0f * Intensity);

				for(int Branch = 0; Branch < 2; Branch++)
				{
					const float A = Branch == 0 ? AngleA : AngleB;
					const vec2 Dir = vec2(cos(A), sin(A));
					p.SetDefault();
					p.m_Spr = SPRITE_PART_HIT01;
					p.m_Pos = Pos + Dir * Radius;
					p.m_Vel = Dir * random_float(120.0f, 340.0f) + vec2(-Dir.y, Dir.x) * random_float(80.0f, 220.0f);
					p.m_LifeSpan = random_float(0.14f, 0.28f);
					p.m_StartSize = random_float(7.0f, 14.0f) * (0.9f + 0.30f * Intensity);
					p.m_EndSize = 0.0f;
					p.m_Rot = A;
					p.m_Rotspeed = random_float(-2.2f, 2.2f) * pi;
					p.m_Friction = 0.70f;
					p.m_Gravity = random_float(40.0f, 260.0f);
					p.m_Collides = false;
					p.m_UseAlphaFading = true;
					p.m_StartAlpha = Alpha * (0.55f + 0.25f * (1.0f - t));
					p.m_EndAlpha = 0.0f;
					p.m_Color = FxColor.Multiply(Branch == 0 ? 1.15f : 0.85f);
					GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);
				}
			}
		}

		// Effect 9: star fracture (new) - 8-direction shard blast with long streaks.
		{
			for(int Arm = 0; Arm < 8; Arm++)
			{
				const float ArmAngle = (2.0f * pi * Arm) / 8.0f;
				const vec2 ArmDir = vec2(cos(ArmAngle), sin(ArmAngle));
				const int ShardsPerArm = (int)round_to_int(2.0f + 2.0f * Intensity);
				for(int j = 0; j < ShardsPerArm; j++)
				{
					p.SetDefault();
					p.m_Spr = SPRITE_PART_SPLAT01 + (j % 3);
					p.m_Pos = Pos + ArmDir * random_float(2.0f, 8.0f);
					p.m_Vel = ArmDir * random_float(320.0f, 980.0f) * (0.75f + 0.45f * Intensity) + random_direction() * random_float(25.0f, 90.0f);
					p.m_LifeSpan = random_float(0.12f, 0.24f);
					p.m_StartSize = random_float(8.0f, 22.0f) * (0.85f + 0.40f * Intensity);
					p.m_EndSize = 0.0f;
					p.m_Rot = ArmAngle;
					p.m_Rotspeed = random_float(-3.5f, 3.5f) * pi;
					p.m_Friction = 0.58f;
					p.m_Gravity = random_float(120.0f, 560.0f);
					p.m_Collides = false;
					p.m_UseAlphaFading = true;
					p.m_StartAlpha = Alpha * 0.86f;
					p.m_EndAlpha = 0.0f;
					p.m_Color = ColorRGBA(1.0f, 1.0f, 1.0f, Alpha * 0.9f);
					GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
				}
			}
		}

		// Effect 10: shock columns (new) - vertical up/down jets at impact core.
		{
			const int JetCount = (int)round_to_int(6.0f + 8.0f * Intensity);
			for(int i = 0; i < JetCount; i++)
			{
				const float Side = (i % 2 == 0) ? -1.0f : 1.0f;
				const float XOff = Side * random_float(2.0f, 18.0f);

				// Upward jet
				p.SetDefault();
				p.m_Spr = SPRITE_PART_SMOKE;
				p.m_Pos = Pos + vec2(XOff, random_float(-2.0f, 6.0f));
				p.m_Vel = vec2(random_float(-40.0f, 40.0f), -random_float(280.0f, 760.0f) * (0.8f + 0.45f * Intensity));
				p.m_LifeSpan = random_float(0.16f, 0.34f);
				p.m_StartSize = random_float(10.0f, 24.0f) * (0.8f + 0.35f * Intensity);
				p.m_EndSize = 0.0f;
				p.m_Friction = 0.72f;
				p.m_Gravity = random_float(40.0f, 260.0f);
				p.m_Collides = false;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha * 0.70f;
				p.m_EndAlpha = 0.0f;
				p.m_Color = FxColor.Multiply(random_float(0.85f, 1.20f));
				GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);

				// Downward rebound jet
				p.SetDefault();
				p.m_Spr = SPRITE_PART_SMOKE;
				p.m_Pos = Pos + vec2(XOff * 0.6f, random_float(-3.0f, 3.0f));
				p.m_Vel = vec2(random_float(-35.0f, 35.0f), random_float(120.0f, 420.0f) * (0.75f + 0.35f * Intensity));
				p.m_LifeSpan = random_float(0.12f, 0.24f);
				p.m_StartSize = random_float(8.0f, 18.0f) * (0.75f + 0.30f * Intensity);
				p.m_EndSize = 0.0f;
				p.m_Friction = 0.76f;
				p.m_Gravity = random_float(220.0f, 620.0f);
				p.m_Collides = false;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = Alpha * 0.50f;
				p.m_EndAlpha = 0.0f;
				p.m_Color = FxColor.Multiply(0.7f);
				GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
			}
		}
	}

	if(g_Config.m_SndGame)
		GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_HAMMER_HIT, Volume, Pos);
}

void CEffects::OnRender()
{
	float Speed = 1.0f;
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		Speed = DemoPlayer()->BaseInfo()->m_Speed;

	const int64_t Now = time();
	auto UpdateClock = [&](bool &Add, int64_t &LastUpdate, int Frequency) {
		Add = Now - LastUpdate > time_freq() / ((float)Frequency * Speed);
		if(Add)
			LastUpdate = Now;
	};
	UpdateClock(m_Add5hz, m_LastUpdate5hz, 5);
	UpdateClock(m_Add50hz, m_LastUpdate50hz, 50);
	UpdateClock(m_Add100hz, m_LastUpdate100hz, 100);

	if(m_Add50hz)
		GameClient()->m_Flow.Update();
}
