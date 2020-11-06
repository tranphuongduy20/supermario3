#include "Koopa.h"
#include "Brick.h"

Koopa::Koopa()
{
	tag = EntityType::KOOPA;
	SetState(KOOPA_STATE_WALKING);
}

void Koopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	
	right = x + KOOPA_BBOX_WIDTH;
	bottom = y + KOOPA_BBOX_HEIGHT+1;
	/*if (state == KOOPA_STATE_DIE)
		bottom = y + KOOPA_BBOX_HEIGHT_DIE;
	else
		bottom = y + KOOPA_BBOX_HEIGHT;*/
	if (state == KOOPA_STATE_DIE || state == KOOPA_STATE_TROOPA_SPIN)
		top = y + 10;
	else
		top = y;
	//else
	//	bottom = y + KOOPA_BBOX_HEIGHT;
}

void Koopa::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	Entity::Update(dt);


	// Simple fall down
	DebugOut(L"SOPT:%d \n", state);
	if(state != KOOPA_STATE_DIE) vy += 0.002 * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();


	CalcPotentialCollisions(coObjects, coEvents);
	
	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 

		// block every object first!
		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;

		//if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;


		//
		// Collision logic with other objects
		//
		//DebugOut(L"SOPT:%d \n", coEvents.size());
		/*if (coEvents.size() >= 3 )
			vx *= -1;*/
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<Brick*>(e->obj))
				if (e->nx != 0)vx = -1 * vx;
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void Koopa::Render()
{
	int ani = KOOPA_ANI_WALKING;
	if (state == KOOPA_STATE_DIE) {
		ani = KOOPA_ANI_DIE;
	}
	else if (state == KOOPA_STATE_TROOPA_SPIN) {
		ani = KOOPA_ANI_TROOPA_SPIN;
	}
	if (vx > 0)
		nx = 1;
	else nx = -1;


	/*else if (vx > 0) ani = KOOPAS_ANI_WALKING_RIGHT;
	else if (vx <= 0) ani = KOOPAS_ANI_WALKING_LEFT;*/

	animationSet->at(ani)->Render(nx, x, y);

	RenderBoundingBox();
}

void Koopa::SetState(int state)
{
	Entity::SetState(state);
	switch (state)
	{
	case KOOPA_STATE_DIE:
		y += (KOOPA_BBOX_HEIGHT - KOOPA_BBOX_HEIGHT_DIE)/2 -5;
		vx = 0;
		vy = 0;
		break;
	case KOOPA_STATE_WALKING:
		vx = KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_TROOPA_SPIN:
		vx = -nx * 0.1;
		break;
	}
}
