#pragma once

#include "Entity.h"
#include "debug.h"

#define KOOPA_WALKING_SPEED 0.03f;

#define KOOPA_BBOX_WIDTH		16
#define KOOPA_BBOX_HEIGHT		26
#define KOOPA_BBOX_HEIGHT_DIE	15

#define KOOPA_STATE_WALKING			100
#define KOOPA_STATE_DIE				200
#define KOOPA_STATE_TROOPA_SPIN		300

#define KOOPA_ANI_WALKING		0
#define KOOPA_ANI_DIE			1
#define KOOPA_ANI_TROOPA_SPIN	2


class Koopa : public Entity
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	virtual void Render();

public:
	Koopa();
	virtual void SetState(int state);
};

