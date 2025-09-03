#pragma once
#include "constants.h"


class StateManager {
private:
	GizmoTool activeTool;

public:

	StateManager() : activeTool(GizmoTool::None) {}

	void setMoveTool() {
		if (activeTool != GizmoTool::move) activeTool = GizmoTool::move;
	}

	void setRotateTool() {
		if (activeTool != GizmoTool::rotate) activeTool = GizmoTool::rotate;
	}

	void setNoneTool() {
		if (activeTool != GizmoTool::None) activeTool = GizmoTool::None;
	}

	GizmoTool getActiveTool() { return activeTool; }
};