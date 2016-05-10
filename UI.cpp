#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>

#include "UI.h"

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

bool isPureNumber(std::string str)
{
	for (char c : str)
	{
		if (c < '0' || c > '9')
			return false;
	}

	return true;
}

class GearParams
{
public:
	double module;
	int numWheelTeeth;
	int numPinionTeeth;
	double wheelThickness;
	double pinionThickness;

	bool isValid()
	{
		if (module > 0.0 && numWheelTeeth > 2 && numPinionTeeth > 2 && wheelThickness > 0.0 && pinionThickness > 0.0)
			return true;
		else
			return false;
	}

	bool extractValsFromInput(Ptr<CommandInputs> inputs)
	{
		Ptr<ValueCommandInput> moduleInput = inputs->itemById("module");
		Ptr<StringValueCommandInput> wheelTeethInput = inputs->itemById("numWheelTeeth");
		Ptr<StringValueCommandInput> pinionTeethInput = inputs->itemById("numPinionTeeth");
		Ptr<ValueCommandInput> wheelThicknessInput = inputs->itemById("wheelThickness");
		Ptr<ValueCommandInput> pinionThicknessInput = inputs->itemById("pinionThickness");

		if (!moduleInput || !wheelTeethInput || !pinionTeethInput || !wheelThicknessInput || !pinionThicknessInput)
			return false;

		module = moduleInput->value();

		numWheelTeeth = 0;
		std::string wheelTeethValue = wheelTeethInput->value();
		if (!wheelTeethValue.empty() && isPureNumber(wheelTeethValue))
		{
			numWheelTeeth = atoi(wheelTeethValue.c_str());
		}

		numPinionTeeth = 0;
		std::string pinionTeethValue = pinionTeethInput->value();
		if (!pinionTeethValue.empty() && isPureNumber(pinionTeethValue))
		{
			numPinionTeeth = atoi(pinionTeethValue.c_str());
		}

		wheelThickness = wheelThicknessInput->value();
		pinionThickness = pinionThicknessInput->value();

		return true;
	}
};

class OnExecuteEventHander : public CommandEventHandler
{
public:
	void notify(const Ptr<CommandEventArgs>& eventArgs) override
	{
		Ptr<Event> firingEvent = eventArgs->firingEvent();
		if (!firingEvent)
			return;

		Ptr<Command> command = firingEvent->sender();
		if (!command)
			return;

		Ptr<CommandInputs> inputs = command->commandInputs();
		if (!inputs)
			return;

		GearParams params;
		params.extractValsFromInput(inputs);

		Ptr<Application> app = Application::get();
		if (!app)
			return;

		Ptr<UserInterface> ui = app->userInterface();
		if (!ui)
			return;

		if (params.isValid())
		{
			// Make the gear!
		}
		else
		{
			ui->messageBox("Inputs are not valid!");
		}
	}
};

class OnValidateInputsHandler : public ValidateInputsEventHandler
{
public:
	void notify(const Ptr<ValidateInputsEventArgs>& eventArgs) override
	{
		Ptr<Event> firingEvent = eventArgs->firingEvent();
		if (!firingEvent)
			return;

		Ptr<Command> command = firingEvent->sender();
		if (!command)
			return;

		Ptr<CommandInputs> inputs = command->commandInputs();
		if (!inputs)
			return;

		GearParams params;
		params.extractValsFromInput(inputs);

		if(params.isValid())
			eventArgs->areInputsValid(true);
		else
			eventArgs->areInputsValid(false);
	}
};

// Setup the Dialog box to allow user to input params
class OnCommandCreatedEventHandler : public CommandCreatedEventHandler
{
public:
	void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
	{
		if (eventArgs)
		{
			Ptr<Command> cmd = eventArgs->command();
			if (cmd)
			{
				// Connect to the CommandExecuted event.
				Ptr<CommandEvent> onExec = cmd->execute();
				if (!onExec)
					return;
				bool isOk = onExec->add(&onExecuteHander_);
				if (!isOk)
					return;

				// Connect to the ValidateInputs event.
				Ptr<ValidateInputsEvent> onValidateInputs = cmd->validateInputs();
				if (!onValidateInputs)
					return;
				isOk = onValidateInputs->add(&onValidateInputsHandler_);
				if (!isOk)
					return;

				// Define the inputs.
				Ptr<CommandInputs> inputs = cmd->commandInputs();
				if (!inputs)
					return;

				// Module
				Ptr<ValueInput> moduleInitialVal = ValueInput::createByReal(1.0);
				if (!moduleInitialVal)
					return;
				inputs->addValueInput("module", "Module", "mm", moduleInitialVal);

				// Wheel Teeth
				inputs->addStringValueInput("numWheelTeeth", "Number of Wheel Teeth", "24");

				// Pinion Teeth
				inputs->addStringValueInput("numPinionTeeth", "Number of Pinion Teeth", "8");

				// Wheel Thickness
				Ptr<ValueInput> wheelThicknessVal = ValueInput::createByReal(1.0);
				if (!wheelThicknessVal)
					return;
				inputs->addValueInput("wheelThickness", "Wheel Thickness", "mm", wheelThicknessVal);

				// Pinion Thickness
				Ptr<ValueInput> pinionThicknessVal = ValueInput::createByReal(2.0);
				if (!pinionThicknessVal)
					return;
				inputs->addValueInput("pinionThickness", "Pinion Thickness", "mm", pinionThicknessVal);

			}
		}
	}
private:
	OnExecuteEventHander onExecuteHander_;
	OnValidateInputsHandler onValidateInputsHandler_;
} cmdCreated_;

bool CreateMenus(Ptr<UserInterface> ui)
{
	const std::string commandId = "CycloidalGearCommandId";
	const std::string commandName = "Create Cycloidal Gear";
	const std::string commandDescription = "Create a Cycloidal gear.";
	const std::string commandResources = "./resources";

	Ptr<Workspaces> workspaces = ui->workspaces();
	if (!workspaces)
		return false;

	Ptr<Workspace> modelingWorkspace = workspaces->itemById("FusionSolidEnvironment");
	if (!modelingWorkspace)
		return false;

	Ptr<ToolbarPanels> toolbarPanels = modelingWorkspace->toolbarPanels();
	if (!toolbarPanels)
		return false;

	Ptr<ToolbarPanel> toolbarPanel = toolbarPanels->itemById("SolidCreatePanel");
	if (!toolbarPanel)
		return false;

	Ptr<ToolbarControls> toolbarControls = toolbarPanel->controls();
	if (!toolbarControls)
		return false;

	Ptr<ToolbarControl> toolbarControl = toolbarControls->itemById(commandId);
	if (toolbarControl)
	{
		ui->messageBox("Cycloidal Gear command is already loaded.");
		adsk::terminate();
		return true;
	}
	else
	{
		Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
		if (!commandDefinitions)
			return false;

		Ptr<CommandDefinition> commandDefinition = commandDefinitions->itemById(commandId);
		if (!commandDefinition)
		{
			commandDefinition = commandDefinitions->addButtonDefinition(commandId, commandName, commandDescription, commandResources);
			if (!commandDefinition)
				return false;
		}

		Ptr<CommandCreatedEvent> commandCreatedEvent = commandDefinition->commandCreated();
		if (!commandCreatedEvent)
			return false;
		commandCreatedEvent->add(&cmdCreated_);
		toolbarControl = toolbarControls->addCommand(commandDefinition);
		if (!toolbarControl)
			return false;
		toolbarControl->isVisible(true);
	}

	return true;
}