#include "TrainingWheelsPlugin.h"
#include "bakkesmod\wrappers\GameEvent\TutorialWrapper.h"
#include "bakkesmod\wrappers\GameObject\BallWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarWrapper.h"
#include "bakkesmod\wrappers\Engine\EngineTAWrapper.h"

BAKKESMOD_PLUGIN(TrainingWheelsPlugin, "Training wheels plugin", "0.1", PLUGINTYPE_FREEPLAY)

void TrainingWheelsPlugin::onLoad()
{
	cvarManager->registerCvar("training_wheels_initial_gamespeed", "0.5", "gamespeed to start at", true, true, 0.1f, true, 2.0f, false);
	cvarManager->registerCvar("training_wheels_target_gamespeed", "1.0", "gamespeed to end at", true, true, 0.1f, true, 2.0f, false);
	cvarManager->registerCvar("training_wheels_duration", "10.0", "minutes over which to increase from initial to target speed", true, true, 1.0f, true, 60.0f, false);

	cvarManager->registerNotifier("training_wheels_start", [this](std::vector<std::string> params) {
		if (!gameWrapper->IsInFreeplay() && !gameWrapper->IsInCustomTraining() && !gameWrapper->IsInGame()) {
			cvarManager->log("not in freeplay or custom training");
			return;
		}

		float initialSpeed = cvarManager->getCvar("training_wheels_initial_gamespeed").getFloatValue();
		
		this->finalSpeed = cvarManager->getCvar("training_wheels_target_gamespeed").getFloatValue();
		this->duration = cvarManager->getCvar("training_wheels_duration").getFloatValue() * 60;

		float speedDelta = this->finalSpeed - initialSpeed;
		float numIncrements = speedDelta / this->speedIncrement;
		this->period = this->duration / numIncrements;

		this->currentSpeed = initialSpeed;
		this->SetGameSpeed(this->currentSpeed);

		this->IncrementGameSpeed();

		}, "starts the change from initial to target values over duration", PERMISSION_ALL);

	cvarManager->registerNotifier("training_wheels_stop", [this](std::vector<std::string> params) {
		this->shouldStop = true;
		}, "starts the change from initial to target values over duration", PERMISSION_ALL);

	cvarManager->registerNotifier("training_wheels_pause", [this](std::vector<std::string> params) {
		this->isPaused = !this->isPaused;
		}, "toggles the paused state of training wheels", PERMISSION_ALL);
}

void TrainingWheelsPlugin::onUnload()
{
}

void TrainingWheelsPlugin::IncrementGameSpeed()
{
	if (gameWrapper->IsPaused() || this->isPaused) {
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->IncrementGameSpeed();
			}, this->period);
		return;
	}

	this->currentSpeed += this->speedIncrement;

	if (this->shouldStop || this->currentSpeed > this->finalSpeed) {
		return;
	} 

	this->SetGameSpeed(this->currentSpeed);

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		this->IncrementGameSpeed();
		}, this->period);
}

void TrainingWheelsPlugin::SetGameSpeed(float speed)
{
	cvarManager->executeCommand("sv_soccar_gamespeed " + std::to_string(speed), true);
}

