#pragma once
#include "Entity.h"
#define LEAF_WALKING_SPEED 0.04f;

#define LEAF_BBOX_WIDTH		16
#define LEAF_BBOX_HEIGHT	16


#define LEAF_STATE_WALKING	100

#define LEAF_ANI_WALKING		0

class Leaf : public Entity
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	virtual void Render();

public:
	Leaf();
	virtual void SetState(int state);
};

