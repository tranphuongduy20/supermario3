#include "Goomba.h"
#include "Brick.h"
Goomba::Goomba()
{
	tag = EntityType::GOOMBA;
	SetState(GOOMBA_STATE_WALKING);
}

void Goomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + GOOMBA_BBOX_WIDTH;

	if (state == GOOMBA_STATE_DIE || state == GOOMBA_STATE_DIE_FLY)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else
		bottom = y + GOOMBA_BBOX_HEIGHT;
	/*if (state != GOOMBA_STATE_DIE)
	{
		left = x;
		top = y;
		right = x + GOOMBA_BBOX_WIDTH;
		bottom = y + GOOMBA_BBOX_HEIGHT;
	}*/
	/*if (state == GOOMBA_STATE_DIE_FLY)
	{
		left = x;
		top = y;
		right = x + GOOMBA_BBOX_WIDTH;
		bottom = y + GOOMBA_BBOX_HEIGHT + 60;
	}*/
}

void Goomba::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	Entity::Update(dt, coObjects);

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 

	/*x += dx;
	y += dy;

	if (vx < 0 && x < 0) {
		x = 0; vx = -vx;
	}

	if (vx > 0 && x > 290) {
		x = 290; vx = -vx;
	}*/
	//if (state != GOOMBA_STATE_DIE)
		vy += 0.002 * dt;


	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != GOOMBA_STATE_DIE && state != GOOMBA_STATE_DIE_FLY)
	{
		//Disable Collider
		CalcPotentialCollisions(coObjects, coEvents);
	}

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

void Goomba::Render()
{
	int ani = GOOMBA_ANI_WALKING;
	if (state == GOOMBA_STATE_DIE) {
		ani = GOOMBA_ANI_DIE;
	}
	else if (state == GOOMBA_STATE_DIE_FLY)
		ani = GOOMBA_ANI_DIE_FLY;

	if (state == GOOMBA_STATE_WALKING)
		animationSet->at(ani)->Render(nx, x, y);
	else if (state == GOOMBA_STATE_DIE)
	{
		if (timerenderanidie == 0)
			timerenderanidie = GetTickCount64();
		if (GetTickCount64() - timerenderanidie < 200)
			animationSet->at(ani)->Render(nx, x, y);
	}
	else if (state == GOOMBA_STATE_DIE_FLY)
	{
			animationSet->at(ani)->Render(nx, x, y);
	}

	//animationSet->at(ani)->Render(nx, x, y);

	RenderBoundingBox();
}

void Goomba::SetState(int state)
{
	Entity::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE:
		y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;
		break;
	case GOOMBA_STATE_WALKING:
		vx = -GOOMBA_WALKING_SPEED;
		break;
	case GOOMBA_STATE_DIE_FLY:
		vx = -GOOMBA_WALKING_SPEED + 0.04f;
		vy = -0.35;
		break;
	}
}
