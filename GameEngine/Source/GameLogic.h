
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GameAudio.h"
#include "PhysicalAction.h"

#include "InputManager.h"
#include "RenderSwapFrame.h"
/** Processes the logic of the game. Started by the Core Engine and manipulates
    the GameDataModel to be rendered for the next frame.
 */
class GameLogic : public Thread
{
public:
	GameLogic(GameAudio & gameAudio) : Thread("GameLogic"), gameAudio(gameAudio)
    {
        //inputManager = new InputManager();
		gamePaused = true;
    }
    
	~GameLogic()
    {
        gameModelCurrentFrame = nullptr;
	}

	/** Sets whether or not the game is paused.
	*/
	void setPaused(bool paused)
	{
		gamePaused = paused;
	}

	/* Gets the current pause/play state of the game*/
	bool isPaused() {
		return gamePaused;
	}

    /** Sets the GameModel current frame being processed for logic, and the
        GameModel swap frame that will be swapped with the GameView to be rendered.
     */
	void setGameModel(GameModel * curentFrame)
    {
		gameModelCurrentFrame = curentFrame;
	}
    

	/** Sets the Render swap frame that will be processed for logic before it
	is sent to the GameView to be rendered.
	*/
	void setRenderSwapFrame(RenderSwapFrame * swapFrame)
	{
		renderSwapFrame = swapFrame;
	}

	/** Returns the GameModel swap frame that the GameLogic is currently
	processing.
	*/
	RenderSwapFrame * getRenderSwapFrame()
	{
		return renderSwapFrame;
	}

    /** Sets the WaitableEvent for the GameLogic to signal the CoreEngine when
        it is done processing.
     */
	void setCoreEngineWaitable (WaitableEvent * waitable)
    {
		coreEngineWaitable = waitable;
	}

    /** Sets the WaitableEvent for the GameLogic to wait until the CoreEngine
        has signaled it to go after the swap frames have been swapped.
     */
	void setLogicWaitable (WaitableEvent * waitable)
    {
		logicWaitable = waitable;

	}

	/* Sets the InputManager to match the values of the CoreEngine 
		InputManager
	*/
	void registerInputManager(InputManager* inputManager) {
		this->inputManager = inputManager;
	}

private:


	void run()
    {
        // Set time the very first time GameLogic runs
		currentTime = Time::currentTimeMillis();
		int64 checkTime = 0;

		Level& currLevel = gameModelCurrentFrame->getCurrentLevel();
		// Main Logic loop
		while (!threadShouldExit())
        {



			// ADD: If level changed, update the current level
				// currentLevel = gameModelCurrentFrame->getCurrentLevel();



			// Wait for CoreEngine to signal() this loop
			logicWaitable->wait();

			if (gamePaused) {
				currentTime = Time::currentTimeMillis();
				
			}

			// Calculate time
			newTime = Time::currentTimeMillis();
			deltaTime = newTime - currentTime;
			currentTime = newTime;
			checkTime += deltaTime;

			//locks in the commands for this iteration
			inputManager->getCommands(newCommands);

			for (auto & command : newCommands)
			{
				switch (command)
				{
				
					case GameCommand::Player1MoveUp:
						if (!isPaused()) {
							currLevel.getPlayer(0)->moveUp();
						}

						break;
					case GameCommand::Player1MoveDown:
						if (!isPaused()) {
							currLevel.getPlayer(0)->moveDown();
						}

						break;
					case GameCommand::Player1MoveLeft:
						if (!isPaused()) {
							currLevel.getPlayer(0)->moveLeft();
							if (!currLevel.getPlayer(0)->getIsAnimating()) {
								currLevel.getPlayer(0)->setAnimationStartTime(currentTime);
								currLevel.getPlayer(0)->setLeftAnimation(true);
								currLevel.getPlayer(0)->setIsAnimating(true);
							}
						}

						break;
					case GameCommand::Player1MoveRight:
						if (!isPaused()) {
							currLevel.getPlayer(0)->moveRight();

							if (!currLevel.getPlayer(0)->getIsAnimating()) {
								currLevel.getPlayer(0)->setAnimationStartTime(currentTime);
								currLevel.getPlayer(0)->setLeftAnimation(false);
								currLevel.getPlayer(0)->setIsAnimating(true);
							}
						}
							
							
						break;
					//Player 2 commands
					case GameCommand::Player2MoveUp:
						if (!isPaused()) {
							currLevel.getPlayer(1)->moveUp();
						}
						break;
					case GameCommand::Player2MoveDown:
						if (!isPaused()) {
							currLevel.getPlayer(1)->moveDown();
						}
						break;
					case GameCommand::Player2MoveLeft:
						if (!isPaused()) {
							currLevel.getPlayer(1)->moveLeft();
						}
						break;
					case GameCommand::Player2MoveRight:
						if (!isPaused()) {
							currLevel.getPlayer(1)->moveRight();
						}
						break;
					case GameCommand::reset:
						if (!isPaused()) {
							currLevel.getPlayer(0)->reset();
							currLevel.getPlayer(1)->reset();
						}
						break;
				}
				
				
				
			}


			if ((oldCommands.contains(GameCommand::Player1MoveRight) && !newCommands.contains(GameCommand::Player1MoveRight)) ||
				(oldCommands.contains(GameCommand::Player1MoveLeft) && !newCommands.contains(GameCommand::Player1MoveLeft)) ||
				newCommands.contains(GameCommand::Player1MoveLeft) && newCommands.contains(GameCommand::Player1MoveRight)) {

				if (!newCommands.contains(GameCommand::Player1MoveLeft) && !newCommands.contains(GameCommand::Player1MoveRight) ||
					newCommands.contains(GameCommand::Player1MoveLeft) && newCommands.contains(GameCommand::Player1MoveRight)) {
					currLevel.getPlayer(0)->setIsAnimating(false);

				}


			}
			

			oldCommands = newCommands;
			
            //Only do these things if the game is not paused
			if (!gamePaused) {
				// Process Physics
				currLevel.processWorldPhysics(deltaTime);

				// Play Audio
				// If any new collisions occur, play the specified collision audio
				for (auto & object : currLevel.getGameObjects())
				{
					if (object->getCanimate()) {
						if (object->getIsAnimating()) {
							object->updateAnimationCurrentTime(currentTime);
						}
					}

					if (object->getPhysicsProperties().hasNewCollisions())
					{
						//                    File * audioFile = object->getAudioFileForAction(PhysicalAction::collsion);
						//                 
						//                     If audio file was not in the map, do nothing
						//                    if (audioFile != nullptr)
						//                    {
						//                        gameAudio.playAudioFile(*audioFile, false);
						//                    }
						gameAudio.playAudioFile(object->getAudioFile(), false);

					}
				}
			}
            
            // Update the GameModel
			//Update the number of DrawableObjects in the RenderSwapFrame
			renderSwapFrame->setDrawableObjectsLength(currLevel.getNumGameObjects());

			for (int i = 0; i < currLevel.getGameObjects().size(); i++)
			{
				renderSwapFrame->setDrawableObjectVertices(currLevel.getGameObjects()[i]->getVertices(), i);

				renderSwapFrame->setDrawableObjectTexture(currLevel.getGameObjects()[i]->getTexture(),i);
			}
            // Maybe actions are triggered here ???
            // IMPLEMENT . . .


			// Notify CoreEngine logic is done
			coreEngineWaitable->signal();
		}
	}

    GameAudio & gameAudio;
	GameModel* gameModelCurrentFrame;
	RenderSwapFrame* renderSwapFrame;
	WaitableEvent* logicWaitable;
	WaitableEvent* coreEngineWaitable;

	//input handling
	InputManager* inputManager;
	Array<GameCommand> newCommands;
	Array<GameCommand> oldCommands;


	int64 newTime;
	int64 currentTime;
	int64 deltaTime;
	int64 gameLoopTime;

	//Physics World
	WorldPhysics world;

	bool gamePaused;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GameLogic)
};
