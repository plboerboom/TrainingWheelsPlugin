#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

class TrainingWheelsPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	float currentSpeed;
	float finalSpeed;
	float duration;
	float speedIncrement = 0.01f;
	float period;
	bool isPaused = false;
	bool shouldStop = false;
public:
	virtual void onLoad();
	virtual void onUnload();
private:
	void IncrementGameSpeed();
	void SetGameSpeed(float speed);
};