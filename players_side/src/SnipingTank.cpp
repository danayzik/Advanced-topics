#include "SnipingTank.h"


SnipingTank::SnipingTank(const FullBattleInfo &battleInfo) {
    targetCoords = getClosestEnemyInLineOfSight(battleInfo);
}

ActionRequest SnipingTank::getAction(const FullBattleInfo &battleInfo) {
    bool shouldDodge = hasShellMovingTowardsTank(battleInfo, battleInfo.getMyTank());
    const auto& myTank = battleInfo.getMyTank();
    Coordinates myCoords = myTank.getCoords();
    Direction dir = myTank.getDirection().value();
    Coordinates coordsInFront = DirectionUtils::nextCoordinate(dir, myCoords, battleInfo.getRows(), battleInfo.getCols());
    auto& cellInFront = battleInfo.getCell(coordsInFront);
    bool canMoveForward = cellInFront.isPassableForTank();
    if(canMoveForward && shouldDodge)
        return ActionRequest::MoveForward;
    std::optional<ActionRequest> rotateOpt = rotationTowardsTarget(battleInfo);
    if(rotateOpt.has_value())
        return rotateOpt.value();
    if(myTank.canFire())
        return ActionRequest::Shoot;
    return ActionRequest::GetBattleInfo;
}



void SnipingTank::update(const FullBattleInfo &battleInfo) {
    targetCoords = getClosestEnemyInLineOfSight(battleInfo);
}

std::optional<ActionRequest> SnipingTank::rotationTowardsTarget(const FullBattleInfo &battleInfo) {
    if(!targetCoords.has_value())
        return std::nullopt;
    return rotationTowardsCoords(battleInfo.getMyTank(), targetCoords.value());
}



