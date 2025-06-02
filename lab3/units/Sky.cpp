#include "Sky.h"

Sky::Sky() {

}
void Sky::init() {
	texture = engine->getRender()->getTextureManager()->loadCubeTexture("bin/cubemaps/gym/cubemap");
	engine->getRender()->setSky(texture);
}
void Sky::update() {
}

Sky::~Sky() {
}

