#include "Lab2.h"

Lab2::Lab2() {

}

void Lab2::init()
{
	texture = engine->getRender()->getTextureManager()->loadTexture("bin/textures/DoomTexture.jpg", true, true);

	cubeGeom = engine->getRender()->getGeometryManager()->addCube();
	for (int i = 0; i < 100; i++)
	{
		Material* mat = engine->getRender()->getMaterialManager()->addMaterial();
		mat->getData().shadeModel = SHADE_MODEL_PHONG;
		mat->getData().diffuse.x = 1.0 * rand() / RAND_MAX;
		mat->getData().diffuse.y = 1.0 * rand() / RAND_MAX;
		mat->getData().diffuse.z = 1.0 * rand() / RAND_MAX;

		mat->getData().specular.x = 1.0 * rand() / RAND_MAX;
		mat->getData().specular.y = 1.0 * rand() / RAND_MAX;
		mat->getData().specular.z = 1.0 * rand() / RAND_MAX;

		mat->getData().ph = 30;

		mat->setTexture(texture, MaterialTextureType_Color);
		materials.push_back(mat);
	}

	const int W = 10;
	const int H = 10;
	const int D = 10;

	for (int x = -W /2; x < W/2; x++)
		for (int y = -H / 2; y < H / 2; y++)
			for (int z = -D / 2; z < D / 2; z++)
			{
				Mesh* mesh = engine->getRender()->getMeshManager()->addMesh(cubeGeom, materials[rand() % materials.size()]);
				mesh->setPosition({ 3.0f * x, 3.0f * y, 3.0f * z });
				//mesh->setRotation({ 1.0f * rand(), 1.0f * rand(), 1.0f * rand() });
				//mesh->setScale({ 0.01, 0.01, 0.01 });
				meshes.push_back(mesh);
			}

	engine->getRender()->getLightManager()->addSphereLight({ 10, 10, 10 }, {-20, 10, 5}, 20);
	engine->getRender()->getLightManager()->addSphereLight({ 0, 0, 15 }, {+20, -10, -5}, 20);
	light = engine->getRender()->getLightManager()->addSphereLight({ 0, 13, 0 }, {10, 0, 0}, 20);
}

void Lab2::update() {
	float t = engine->getTimer()->getTimeFromAppStart();
	float rotationRad = 10;
	light->setPosition({rotationRad * cosf(t), 0, rotationRad * sinf(t)});
}

Lab2::~Lab2() {
}


