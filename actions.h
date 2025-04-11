#pragma once


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
//Name does not suggest it can return MoveForward
inline Action getFirstRotationAction(Direction current, Direction target) {
    int diff = static_cast<int>(target) - static_cast<int>(current);
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    if (diff == 0) return MoveForward;
    if (diff > 0) {
        return (diff >= 90) ? Action::RotateRight90 : Action::RotateRight45;
    } else {
        return (-diff >= 90) ? Action::RotateLeft90 : Action::RotateLeft45;
    }
}