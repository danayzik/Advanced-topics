#pragma once
#include "ActionRequest.h"
#include <string>
#include <vector>


namespace UserCommon_206038929_314620071::ActionRequestUtils {

    inline const std::vector<ActionRequest> &getRotationalActions() {
        static const std::vector<ActionRequest> rotations = {
                ActionRequest::RotateRight45, ActionRequest::RotateRight90,
                ActionRequest::RotateLeft45, ActionRequest::RotateLeft90
        };
        return rotations;
    }

    inline bool isRotation(ActionRequest action) {
        switch (action) {
            case ActionRequest::RotateLeft45:
            case ActionRequest::RotateLeft90:
            case ActionRequest::RotateRight45:
            case ActionRequest::RotateRight90:
                return true;
            default:
                return false;
        }
    }

    inline int rotateActionToAngle(ActionRequest action) {
        switch (action) {
            case ActionRequest::RotateLeft45:
                return -45;
            case ActionRequest::RotateLeft90:
                return -90;
            case ActionRequest::RotateRight45:
                return 45;
            case ActionRequest::RotateRight90:
                return 90;
            default:
                return 0;
        }
    }

    inline ActionRequest angleToRotationAction(int angle) {
        switch (angle) {
            case 45:
                return ActionRequest::RotateRight45;
            case 90:
                return ActionRequest::RotateRight90;
            case -45:
                return ActionRequest::RotateLeft45;
            case -90:
                return ActionRequest::RotateLeft90;
            default:
                return ActionRequest::DoNothing;
        }
    }


    inline std::string actionToString(ActionRequest action) {
        switch (action) {
            case ActionRequest::MoveForward:
                return "MoveForward";
            case ActionRequest::MoveBackward:
                return "MoveBackward";
            case ActionRequest::RotateLeft45:
                return "RotateLeft45";
            case ActionRequest::RotateRight45:
                return "RotateRight45";
            case ActionRequest::RotateLeft90:
                return "RotateLeft90";
            case ActionRequest::RotateRight90:
                return "RotateRight90";
            case ActionRequest::Shoot:
                return "Shoot";
            case ActionRequest::DoNothing:
                return "DoNothing";
            case ActionRequest::GetBattleInfo:
                return "GetBattleInfo";
            default:
                return "UnknownAction";
        }
    }
}
