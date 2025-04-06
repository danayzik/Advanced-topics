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