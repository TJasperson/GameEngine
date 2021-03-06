/*

Lists the properties of the currently selected unit
*/
#pragma once

#include "CoreEngine.h"
#include "InspectorUpdater.h"
#include "GameObjectType.h"
#include <string>   
#include "ComboBoxPropertyComponent.h"
#include "FilenamePropertyComponent.h"
#include "Speed.h"

class ObjectInspector : public Component, public InspectorUpdater,
public TextPropertyComponent::Listener, public Value::Listener, public FilenameComponentListener
{
public:
	ObjectInspector()
    {
		addAndMakeVisible(propertyPanel);
		levelToWin.addListener(this);
		selectedObj = NULL;
	}
    
	~ObjectInspector() {}

	void setCoreEngine (CoreEngine* engine)
    {
		coreEngine = engine;
	}
        
	void setSelectedObject (GameObject* obj)
    {
		selectedObj = obj;
		updateObj();
	}
    
    /** Sets the selected objects a the inspector should evaluate.
        
        IMPLEMENT FEATURE LATER: The Object Inspector should adapt so allow you
        to set data for multiple objects at once. Right now it just grabs the
        first available one.
     */
    void setSelectedObjects (Array<GameObject *> gameObjects)
    {
        if (!gameObjects.isEmpty())
        {
            selectedObj = gameObjects[0];
            updateObj();
        }
        else
        {
            selectedObj = nullptr;
            updateObj();
        }
    }
        
    
        
        
	// JUCE GUI Callbacks ======================================================
	void paint(Graphics& g) override {
		//g.fillAll(Colours::coral);
		//g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
	}

	void resized() override
	{
		//scrollBar.setBounds(getLocalBounds());

		propertyPanel.setBounds(getLocalBounds());
	}

	void textPropertyComponentChanged(TextPropertyComponent * component) override
	{
		if (component->getName() == "Name:") {
			selectedObj->setName(component->getText());
			updateInspectorsChangeBroadcaster->sendSynchronousChangeMessage();
		}
        else if (component->getName() == "Texture:")
        {

			File textureFile;
			if (component->getText().isEmpty()) {
				textureFile = File(File::getCurrentWorkingDirectory().getFullPathName() + "/textures/default.png");
			}
			else {
				textureFile = File(component->getText());
			}
			selectedObj->getRenderableObject().animationProperties.setIdleTexture(textureFile);
			updateInspectorsChangeBroadcaster->sendSynchronousChangeMessage();
		}
        // NOTE: MUST BE ELSE IF for all other "component" checks because, when
        // the inspectors are updated synchronously, this one is also updated
        // synchronously, therefore the TextPropertyComponent pointer that
        // originally triggered this callback my now be null, so we cannot check
        // this condition without causing a crash
	}

	void valueChanged(Value &value) override
	{
		if (value.refersToSameSourceAs(objPhysicsXCap)) {
			switch ((int)objPhysicsXCap.getValue()) {
			case 1:
				selectedObj->setMoveSpeed(Speed::SLOW);
				break;
			case 2:
				selectedObj->setMoveSpeed(Speed::MED);
				break;
			case 3:
				selectedObj->setMoveSpeed(Speed::FAST);
				break;
			}

		}

		else if (value.refersToSameSourceAs(objPhysicsYCap)) {
			switch ((int)objPhysicsYCap.getValue()) {
			case 1:
				selectedObj->setJumpSpeed(Speed::SLOW);
				break;
			case 2:
				selectedObj->setJumpSpeed(Speed::MED);
				break;
			case 3:
				selectedObj->setJumpSpeed(Speed::FAST);
				break;
			}

		}

		else if (value.refersToSameSourceAs(comboValue)) {

			switch ((int)comboValue.getValue()) {
			case 1:
				selectedObj->getRenderableObject().animationProperties.setAnimationSpeed(Speed::SLOW);
				break;
			case 2:
				selectedObj->getRenderableObject().animationProperties.setAnimationSpeed(Speed::MED);
				break;
			case 3:
				selectedObj->getRenderableObject().animationProperties.setAnimationSpeed(Speed::FAST);
				break;
			}
		}

		else if (value.refersToSameSourceAs(stateComboValue)) {

			switch ((int)stateComboValue.getValue()) {
			case 1:
				selectedObj->getPhysicsProperties().setIsStatic(true);
				break;
			case 2:
				selectedObj->getPhysicsProperties().setIsStatic(false);
				break;
			}
		}

		else if (value.refersToSameSourceAs(aiState)) {

			switch ((int)aiState.getValue()) {
			case 1:
				((EnemyObject*)(selectedObj))->changeAI(EnemyObject::NONE);
				break;
			case 2:
				((EnemyObject*)(selectedObj))->changeAI(EnemyObject::GROUNDPATROL);
				break;
			case 3:
				((EnemyObject*)(selectedObj))->changeAI(EnemyObject::JUMPPATROL);
				break;
			case 4:
				((EnemyObject*)(selectedObj))->changeAI(EnemyObject::SCAREDAF);
				break;
			case 5:
				((EnemyObject*)(selectedObj))->changeAI(EnemyObject::CHASE);
				break;
			}
		}else if (value.refersToSameSourceAs(playerLives)) {
			selectedObj->setLives(value.getValue());
		}else if (value.refersToSameSourceAs(levelToWin)) {
			((GoalPointObject*)selectedObj)->setToWin();
			updateInspectorsChangeBroadcaster->sendSynchronousChangeMessage();
		}else if (value.refersToSameSourceAs(levelGoTo)) {
			((GoalPointObject*)selectedObj)->setLevelToGoTo(levelGoTo.getValue());
			updateInspectorsChangeBroadcaster->sendSynchronousChangeMessage();
		}

		else if (value.refersToSameSourceAs(Scale)) {

			float scale = (float)value.getValue();
			selectedObj->setScale(scale, scale);
		}

	}

	void filenameComponentChanged(FilenameComponent *fileComponentThatHasChanged) {
		if (fileComponentThatHasChanged->getName() == "Animation Directory") {
			selectedObj->getRenderableObject().animationProperties.setAnimationTextures(fileComponentThatHasChanged->getCurrentFile());
			updateInspectorsChangeBroadcaster->sendSynchronousChangeMessage();
		}

		else if (fileComponentThatHasChanged->getName() == "Choose Idle Texture") {
			selectedObj->getRenderableObject().animationProperties.setIdleTexture(fileComponentThatHasChanged->getCurrentFile());
			updateInspectorsChangeBroadcaster->sendSynchronousChangeMessage();
		}

		else if (fileComponentThatHasChanged->getName() == "Choose Collision Audio") {
			selectedObj->mapAudioFileToPhysicalAction(fileComponentThatHasChanged->getCurrentFile(), PhysicalAction::collsion);
			updateInspectorsChangeBroadcaster->sendSynchronousChangeMessage();
		}

		else if (fileComponentThatHasChanged->getName() == "Choose Death Audio") {
			selectedObj->mapAudioFileToPhysicalAction(fileComponentThatHasChanged->getCurrentFile(), PhysicalAction::death);
			updateInspectorsChangeBroadcaster->sendSynchronousChangeMessage();
		}

	}

private:

	void updateObj()
	{
		propertyPanel.clear();
		objPhysicsProperties.clear();
		objAudioProperties.clear();
		objHudProperties.clear();
		objMovementProperties.clear();
		objTextureProperties.clear();
		objParticularProperties.clear();

		//add Level Physics
		//create additional functions in desired objects to make it easier to retrieve information.
		//add sections to physics menu
		if (selectedObj != NULL)
		{
			switch (selectedObj->getObjType())
			{
			case Generic:
				break;
			case Player:	//player
				addMovementProperties();
				addHudProperties();
				break;
			case Enemy:	
				addMovementProperties();
				addEnemyProperties();
                break;
			case Checkpoint:
				addGoalPointProperties();				
				break;
			case Collectable:
				break;
			}
			addPhysicalProperties();
			addTextureProperties();
			addAudioProperties();


		}


	}

	//specialized properties
	void addGoalPointProperties() {


		levelToWin.removeListener(this);
		levelToWin.setValue(var(((GoalPointObject*)selectedObj)->getToWin()));
		BooleanPropertyComponent* toWinFlag = new BooleanPropertyComponent(levelToWin, "Transition", "Win Game?");
		objParticularProperties.add(toWinFlag);
		
		if (!((GoalPointObject*)selectedObj)->getToWin()) {
			GoalPointObject* goal = ((GoalPointObject*)selectedObj);
			levelGoTo.setValue(var(goal->getLevelToGoTo()));
			ComboBoxPropertyComponent* combo = new ComboBoxPropertyComponent(levelGoTo, "Next Level:");
			combo->setTextWhenNothingSelected("Choose next Level");
			combo->addItem("1", 1);
			for (int i = 1; i < coreEngine->getGameModel().getNumLevels(); i++) {
				combo->addItem(String(i + 1), i + 1);
			}
			combo->setSelectedId(goal->getLevelToGoTo(), NotificationType::dontSendNotification);
			objParticularProperties.add(combo);
		}
		
		levelToWin.addListener(this);
		levelGoTo.addListener(this);

		propertyPanel.addSection("Goal Attributes", objParticularProperties);
	}
	void addEnemyProperties() {
		aiState.setValue(var(((EnemyObject*)selectedObj)->getAIState()));
		ComboBoxPropertyComponent* combo = new ComboBoxPropertyComponent(aiState, "AI:");
		combo->setTextWhenNothingSelected("Choose AI Type");
		combo->addItem("Chase", 5);
		combo->addItem("Flee", 4);
		combo->addItem("Jump & Patrol", 3);
		combo->addItem("Patrol", 2);
		combo->addItem("Do Nothing", 1);
		combo->setSelectedId(((EnemyObject*)selectedObj)->getAIState() + 1, dontSendNotification);
		aiState.addListener(this);
		objParticularProperties.add(combo);
		
		propertyPanel.addSection("Enemy Attributes", objParticularProperties);
	}
	
	//base physics properties
	void addPhysicalProperties() {
		//Scale.setValue(var((float)selectedObj->getScale().x));
		//Scale.addListener(this);
		//SliderPropertyComponent* slider = new SliderPropertyComponent(Scale, "scale:", 1, 10, 1);
		//objPhysicsProperties.add(slider);


		stateComboValue.setValue(var(selectedObj->getPhysicsProperties().getIsStatic()));
		ComboBoxPropertyComponent* combo = new ComboBoxPropertyComponent(stateComboValue, "Physics:");
		combo->setTextWhenNothingSelected("Choose if active");
		combo->addItem("No", 1);
		combo->addItem("Yes", 2);
		combo->setSelectedId(selectedObj->getPhysicsProperties().getIsStatic() ? 1 : 2, NotificationType::dontSendNotification);
		stateComboValue.addListener(this);
		objPhysicsProperties.add(combo);

		//Add HUD properties to panel
		propertyPanel.addSection("Physical Attributes", objPhysicsProperties);
	}

	void addHudProperties()
	{
		playerLives.setValue(var(selectedObj->getLives()));
		SliderPropertyComponent* playerHealthValue = new SliderPropertyComponent(playerLives, "Lives:", 0, 10, 1);
		playerLives.addListener(this);
		objHudProperties.add(playerHealthValue);

		//Add HUD properties to panel
		propertyPanel.addSection("HUD Properties", objHudProperties);
	}
	void addMovementProperties() {
		objPhysicsXCap.setValue(var(selectedObj->getMoveSpeed()));
		ComboBoxPropertyComponent* combo = new ComboBoxPropertyComponent(objPhysicsXCap, "Move Speed:");
		combo->setTextWhenNothingSelected("Choose Move Speed");
		combo->addItem("Fast", 3);
		combo->addItem("Medium", 2);
		combo->addItem("Slow", 1);
		combo->setSelectedId(selectedObj->getMoveSpeed() + 1, dontSendNotification);
		objPhysicsXCap.addListener(this);
		objMovementProperties.add(combo);

		objPhysicsYCap.setValue(var(selectedObj->getJumpSpeed()));
		combo = new ComboBoxPropertyComponent(objPhysicsYCap, "Jump Height:");
		combo->setTextWhenNothingSelected("Choose Jump Height");
		combo->addItem("High", 3);
		combo->addItem("Medium", 2);
		combo->addItem("Low", 1);
		combo->setSelectedId(selectedObj->getJumpSpeed() + 1, dontSendNotification);
		objPhysicsYCap.addListener(this);
		objMovementProperties.add(combo);

		propertyPanel.addSection("Movement", objMovementProperties);
	}
	void addAudioProperties()
	{
		// Get file already associated with selected object
		File * collisionAudioFile = selectedObj->getAudioFileForAction(PhysicalAction::collsion);

		FilenamePropertyComponent* collisionAudio = new FilenamePropertyComponent("Choose Collision Audio", (collisionAudioFile == nullptr) ? File() : *collisionAudioFile, false, false, false, "", "", "Select a file");
		collisionAudio->addListener(this);
		objAudioProperties.add(collisionAudio);

		File * deathAudioFile = selectedObj->getAudioFileForAction(PhysicalAction::death);

		FilenamePropertyComponent* deathAudio = new FilenamePropertyComponent("Choose Death Audio", (deathAudioFile == nullptr) ? File() : *deathAudioFile, false, false, false, "", "", "Select a file");
		deathAudio->addListener(this);
		objAudioProperties.add(deathAudio);

		// Add Object Audio
		propertyPanel.addSection("Audio", objAudioProperties);
	}

	//base Graphical properties
	void addTextureProperties()
	{
		//Set objName to be the name of the selected object, and create its TextPropertyComponent
		objName.setValue(var(selectedObj->getName()));
		TextPropertyComponent* objNameText = new TextPropertyComponent(objName, "Name:", 40, false);
		objNameText->addListener(this);

		objTextureProperties.add(objNameText);
	
		//Note that this is the custom ComboBoxPropertyComponent JUCE docs
		comboValue.setValue(var((selectedObj->getRenderableObject()).animationProperties.getAnimationSpeed()));
		ComboBoxPropertyComponent* combo = new ComboBoxPropertyComponent(comboValue, "Animation Speed:");
		combo->setTextWhenNothingSelected("Choose Speed");
		combo->addItem("Fast", 3);
		combo->addItem("Normal", 2);
		combo->addItem("Slow", 1);
		combo->setSelectedId((selectedObj->getRenderableObject()).animationProperties.getAnimationSpeed() + 1, dontSendNotification);
		comboValue.addListener(this);
		objTextureProperties.add(combo);

		FilenamePropertyComponent* filename = new FilenamePropertyComponent("Choose Idle Texture", selectedObj->getRenderableObject().animationProperties.getIdleTexture(), false, false, false, "", "", "Select a file");
		filename->addListener(this);
		objTextureProperties.add(filename);

		FilenamePropertyComponent* animationDirectory = new FilenamePropertyComponent("Animation Directory", selectedObj->getRenderableObject().animationProperties.getAnimationTextureDirectory(), false, true, false, "", "", "Select a Dir");
		animationDirectory->addListener(this);
		objTextureProperties.add(animationDirectory);

		propertyPanel.addSection("Textures", objTextureProperties);
	}


	CoreEngine* coreEngine;
	GameObject* selectedObj;

	PropertyPanel propertyPanel;


	//updated GUI
	Array<PropertyComponent *> objMovementProperties;
	Array<PropertyComponent *> objHudProperties;
	Array<PropertyComponent *> objAudioProperties;
	Array<PropertyComponent *> objTextureProperties;
	Array<PropertyComponent *> objParticularProperties;
	Array<PropertyComponent *> objPhysicsProperties;

	Value objName, objPhysicsXCap, objPhysicsYCap,
		   objPhysicsDensity, comboValue, stateComboValue, aiState,
			playerLives, levelGoTo,levelToWin, Scale;


	ScopedPointer<FilenameComponent> chooseFile;

};
