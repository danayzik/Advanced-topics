#include "SnipingTank.h"


SnipingTank::SnipingTank() {}


//Action priority:
// Dodge if needs to -> rotate towards target if possible -> shoot if possible -> Get info
ActionRequest SnipingTank::getAction(const FullBattleInfo &battleInfo) {
    const auto& myTank = battleInfo.getMyTank();
    std::optional<ActionRequest> dodgeAction = dodgingAction(battleInfo, myTank);
    if(dodgeAction.has_value())
        return dodgeAction.value();
    acquireTarget(battleInfo);
    std::optional<ActionRequest> rotateOpt = rotationTowardsTarget(battleInfo);
    if(rotateOpt.has_value())
        return rotateOpt.value();
    Coordinates myCoords = myTank.getCoords();
    bool targetIsInDirection = areColinear(myCoords, targetCoords.value(), battleInfo.getRows(), battleInfo.getCols());

    if(myTank.canFire() && targetIsInDirection)
        return ActionRequest::Shoot;
    Coordinates coordsInFront = DirectionUtils::nextCoordinate(myTank.getDirection().value(), myCoords, battleInfo.getRows(), battleInfo.getCols());
    auto& cellInFront = battleInfo.getCell(coordsInFront);
    bool canMoveForward = cellInFront.isPassableForTank();
    if(canMoveForward)
        return ActionRequest::MoveForward;
    if(myTank.canFire())
        return ActionRequest::Shoot;
    return ActionRequest::RotateLeft45;

}


void SnipingTank::update(const FullBattleInfo &battleInfo) {
    acquireTarget(battleInfo);
}



std::optional<ActionRequest> SnipingTank::rotationTowardsTarget(const FullBattleInfo &battleInfo) {
    return rotationTowardsCoords(battleInfo.getMyTank(), targetCoords.value());
}

void SnipingTank::acquireTarget(const FullBattleInfo &battleInfo) {
    targetCoords = getExposedCoordinates(battleInfo, battleInfo.getMyTank(), battleInfo.getEnemyTanksCoordinates());
    if(!targetCoords.has_value())
        targetCoords = getClosestCoordinates(battleInfo, battleInfo.getEnemyTanksCoordinates());
}




