#pragma once
#include "cell.h"

public class GameEntity{
protected:
    Cell* cell;
public:
    inline Cell* getCell(){
        return cell;
    }
    inline void setCell(Cell* cell){
        this->cell = cell;
    }
};

