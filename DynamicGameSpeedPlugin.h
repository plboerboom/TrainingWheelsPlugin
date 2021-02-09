#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

class DynamicGameSpeedPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	float currentSpeed;
	float finalSpeed;
	float duration;
	float speedIncrement = 0.01f;
	float period;
public:
	virtual void onLoad();
	virtual void onUnload();
	void IncrementGameSpeed();
	void SetGameSpeed(float speed);
};