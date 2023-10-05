#include "scenes.hpp"
#include <utils/shapes.hpp>



bool Ballistic::is_boundary(uint x, uint y, uint z) const {
    return z <= 2u || DatasetScene::is_boundary(x, y, z);
}

Ballistic::~Ballistic() {
    DatasetScene::~DatasetScene();
}