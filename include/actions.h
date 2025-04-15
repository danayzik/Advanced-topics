#pragma once
#include "direction.h"

enum Action {
    MoveForward,
    MoveBackward,
    RotateLeft45,
    RotateRight45,
    RotateLeft90,
    RotateRight90,
    Shoot,
    NoAction
};

inline bool isRotation(Action action){
    switch (action) {
        case RotateLeft45:
        case RotateLeft90:
        case RotateRight45:
        case RotateRight90:
            return true;
        default:
            return false;
    }
}

inline int rotateActionToAngle(Action action){
    switch (action) {
        case RotateLeft45:
            return -45;
        case RotateLeft90:
            return -90;
        case RotateRight45:
            return 45;
        case RotateRight90:
            return 90;
        default:
            return 0;
    }
}
inline Action angleToRotationAction(int angle){
    switch(angle){
        case 45:
            return RotateRight45;
        case 90:
            return RotateRight90;
        case -45:
            return RotateLeft45;
        case -90:
            return RotateLeft90;
        default:
            return NoAction;
    }
}


inline Action getFirstRotationAction(Direction current, Direction target) {
    int angle = getDirectionDiff(current, target);
    if (angle == 0) return NoAction;
    if(-90 <= angle && angle <= 90){
        return angleToRotationAction(angle);
    }
    else if(angle < -90){
        return RotateLeft90;
    }
    else{
        return RotateRight90;
    }
}

inline const char* actionToString(Action action) {
    switch (action) {
        case MoveForward:     return "MoveForward";
        case MoveBackward:    return "MoveBackward";
        case RotateLeft45:    return "RotateLeft45";
        case RotateRight45:   return "RotateRight45";
        case RotateLeft90:    return "RotateLeft90";
        case RotateRight90:   return "RotateRight90";
        case Shoot:           return "Shoot";
        case NoAction:        return "NoAction";
        default:              return "UnknownAction";
    }
}