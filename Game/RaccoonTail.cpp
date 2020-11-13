#include "RaccoonTail.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Define.h"
#include "Brick.h"
#include "Game.h"
#include "Player.h"


RaccoonTail::RaccoonTail()
{
	/*x = position.x;
	y = position.y;
	this->nx = nx;*/
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(1));
	timeDelay = 0;
	timeDelayMax = TIME_DELAY_MAX;
}

void RaccoonTail::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	if (!isAttack)
		return;
	timeDelay += dt;
	if (timeDelay >= timeDelayMax)
	{
		isAttack = false;
		timeDelay = 0;
		return;
	}
	Entity::Update(dt);
	/*this->x = playerPos.x;
	this->y = playerPos.y;
	this->nx = playerNx;*/
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	CalcPotentialCollisions(coObjects, coEvents);

	for (int i = 0; i < coObjects->size(); i++)
	{
		//if (coObjects->at(i)->GetType() != Type::BRICK/* && coObjects->at(i)->GetType() != Type::GROUND*/)
		//{
		//	if (IsAABB(coObjects->at(i)))
		//	{
		//		if (coObjects->at(i)->GetState() != STATE_DESTROYED && coObjects->at(i)->GetState() != EState::DIE_BY_CRUSH && coObjects->at(i)->GetState() != EState::DIE_BY_ATTACK)
		//		{
		//			CEnemy* enemy = dynamic_cast<CEnemy*>(coObjects->at(i));
		//			//can cap nhat nx cua enemy khi quat duoi
		//			//enemy->nx = this->nx;
		//			float ax, ay;
		//			float bx, by;
		//			GetPosition(ax, ay);//mario
		//			enemy->GetPosition(bx, by);
		//			if ((bx - ax) > 0)
		//				enemy->nx = 1;
		//			else
		//				enemy->nx = -1;
		//			enemy->DieByAttack();
		//			vy = -0.2;
		//		}
		//		state = STATE_DESTROYED;
		//	}
		//}
	}

	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block every object first!
		y += min_ty * dy + ny * 0.4f;
		x += min_tx * dx + nx * 0.4f;

		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
		}
	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

RaccoonTail::~RaccoonTail()
{
}

void RaccoonTail::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isAttack)
	{
		left = x;
		top = y;
		right = left + TAIL_BB_WIDTH;
		bottom = right + TAIL_BB_HEIGHT;
	}
}

void RaccoonTail::Render()
{
	RenderBoundingBox();
}

void RaccoonTail::Attack(float posX, float posY)
{
	x = posX;
	y = posY;
	isAttack = true;
}
