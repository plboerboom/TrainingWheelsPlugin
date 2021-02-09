#include "DynamicGameSpeedPlugin.h"
#include "bakkesmod\wrappers\GameEvent\TutorialWrapper.h"
#include "bakkesmod\wrappers\GameObject\BallWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarWrapper.h"
#include "bakkesmod\wrappers\Engine\EngineTAWrapper.h"

BAKKESMOD_PLUGIN(DynamicGameSpeedPlugin, "Dynamic gamespeed plugin", "0.1", PLUGINTYPE_FREEPLAY)

void DynamicGameSpeedPlugin::onLoad()
{
	cvarManager->registerCvar("dgsp_initial_speed", "0.5", "gamespeed to start at", true, true, 0.1f, true, 2.0f, false);
	cvarManager->registerCvar("dgsp_final_speed", "1.0", "gamespeed to end at", true, true, 0.1f, true, 2.0f, false);
	cvarManager->registerCvar("dgsp_duration", "10.0", "minutes over which to increase from start to finish speed", true, true, 1.0f, true, 60.0f, false);

	cvarManager->registerNotifier("dgsp_start", [this](std::vector<std::string> params) {
		if (!gameWrapper->IsInFreeplay() && !gameWrapper->IsInCustomTraining() && !gameWrapper->IsInGame()) {
			cvarManager->log("dgsp not in freeplay or custom training");
			return;
		}

		float initialSpeed = cvarManager->getCvar("dgsp_initial_speed").getFloatValue();
		
		this->finalSpeed = cvarManager->getCvar("dgsp_final_speed").getFloatValue();
		this->duration = cvarManager->getCvar("dgsp_duration").getFloatValue() * 60;

		float speedDelta = this->finalSpeed - initialSpeed;
		float numIncrements = speedDelta / this->speedIncrement;
		this->period = this->duration / numIncrements;

		this->currentSpeed = initialSpeed;
		this->SetGameSpeed(this->currentSpeed);

		this->IncrementGameSpeed();

		}, "starts the plugin", PERMISSION_ALL);
}

void DynamicGameSpeedPlugin::onUnload()
{
}

void DynamicGameSpeedPlugin::IncrementGameSpeed()
{
	if (gameWrapper->IsPaused()) {
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->IncrementGameSpeed();
			}, this->period);
		return;
	}

	this->currentSpeed += this->speedIncrement;

	if (this->currentSpeed > this->finalSpeed) {
		return;
	} 

	this->SetGameSpeed(this->currentSpeed);

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		this->IncrementGameSpeed();
		}, this->period);
}

void DynamicGameSpeedPlugin::SetGameSpeed(float speed)
{
	cvarManager->executeCommand("sv_soccar_gamespeed " + std::to_string(speed), true);
}

