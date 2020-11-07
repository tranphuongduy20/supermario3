﻿#include <algorithm>
#include <assert.h>
#include "debug.h"

#include "Player.h"
#include "Game.h"
#include "Brick.h"
#include "CBrick.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Mushroom.h"

Player::Player(float x, float y) : Entity()
{
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(ANIMATION_SET_PLAYER));
	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	SetState(MARIO_ANI_SMALL_IDLE);
	start_x = x;
	start_y = y;
	backup_JumpY = 0;
	isFly = false;
	isCheckCanFly = true;
	this->x = x;
	this->y = y;
	nx = 1;
	holdthing = nullptr;
}

Player* Player::instance = NULL;
Player* Player::GetInstance()
{
	if (instance == NULL)
		instance = new Player();
	return instance;
}

void Player::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	Entity::Update(dt);
#pragma region Xử lý vy
	vy += MARIO_GRAVITY * dt;
	//Check hightPlayer
	/*if (isJumping && backup_JumpY - y >= HIGHT_LEVER1 && isJumpHandle == false)
	{
		if (!isPressJump)
			vy = 0.09;
		isJumpHandle = true;
	}*/
	if (vy < 0 && !Game::GetInstance()->IsKeyDown(DIK_S))
	{
		vy += 0.0003 * dt;
	}

	if (vy > 0.04)
	{
		isJumping = true;
		isFly = false;
	}

#pragma endregion

#pragma region RUN
	if (isRun == true && vx != 0)
	{
		if (abs(vx) < MARIO_VMAX)
			vx += MARIO_SPEEEDUP * nx;
	}
#pragma endregion

#pragma region Xử lý va chạm
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	coEvents.clear();
	isWaittingPressBtn = GetTickCount() - startWalkingComplete <= MARIO_TYPE;
	if (isWaittingPressBtn) {
		//DebugOut(L"\n true - %d", GetTickCount() - startWalkingComplete);
	}
	//else DebugOut(L"\n false - %d", GetTickCount() - startWalkingComplete);

	if (isWaittingPressBtn) {
		if (walkingDirection != nx) {
			isWalkingComplete = true;
		}
		else
			isWalkingComplete = false;
	}
	else
		isWalkingComplete = false;

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	if (isWaitingForAni && animationSet->at(state)->IsOver()) {
		isWaitingForAni = false;
	}

	

	//turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

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

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.001f;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->obj->GetType() == EntityType::BRICK)
			{
				Brick* brick = dynamic_cast<Brick*>(e->obj);
				/*x += min_tx * dx + nx * 0.4f;
				y += min_ty * dy + ny * 0.001f;*/
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isGround = true;
						isJumping = false;
						isFly = false;
						isCheckCanFly = true;
						vy = 0;
					}
					else
						y += dy;
				}
				//if (e->ny < 0)
				//{
				//	isJumping = false;
				//}
			}
			else if (e->obj->GetType() == EntityType::CBRICK) // if e->obj is CBrick 
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				/*x += min_tx * dx + nx * 0.4f;
				y += min_ty * dy + ny * 0.001f;*/
				if (nx != 0) vx = 0;
				if (ny != 0) vy = 0;
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isGround = true;
						isJumping = false;
						//isFly = false;
						//vy = 0;
					}
					/*else
						y += dy;*/

				}
				if (e->ny > 0)
				{
					brick->SetState(CBRICK_STATE_COLLISION);
				}
			}
			else if (e->obj->GetType() == EntityType::GOOMBA) // if e->obj is CBrick 
			{
				Goomba* goomba = dynamic_cast<Goomba*>(e->obj);
				if (e->ny < 0)
				{
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
						isJumping = true;
					}
				}
				else if (e->nx != 0)
				{
					/*if (GetState() == MARIO_ANI_RACCOON_SPIN)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							goomba->SetState(GOOMBA_STATE_DIE_FLY);
							goomba->vx = -goomba->vx;
						}
					}*/
					if (untouchable == 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE || goomba->GetState() != GOOMBA_STATE_DIE_FLY)
						{
							if (level > MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_BIG;
								StartUntouchable();
							}
							else if (level == MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
							{
								SetState(MARIO_STATE_DIE);
								return;
							}
							/*if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);*/
						}
					}
				}
			}
			else if (e->obj->GetType() == EntityType::KOOPA)
			{
				Koopa* koopa = dynamic_cast<Koopa*>(e->obj);
				/*x += min_tx * dx + nx * 0.4f;
				y += min_ty * dy + ny * 0.001f;*/
				if (nx != 0) vx = 0;
				if (ny != 0) vy = 0;
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isJumping = false;
						if (koopa->GetState() != KOOPA_STATE_DIE)
						{
							koopa->SetState(KOOPA_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
					}
				}

				else if (e->nx != 0)
				{
					if (untouchable == 0)
					{
						if (koopa->GetState() != KOOPA_STATE_DIE)
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
						else
						{
							//isKick = true;
							//SetState(MARIO_STATE_KICK);
							e->obj->nx = nx;
							if (!Game::GetInstance()->IsKeyDown(DIK_A))
							{
								holdthing = nullptr;
								e->obj->SetState(KOOPA_STATE_TROOPA_SPIN);
							}
							else
							{
								holdthing = e->obj;
							}
						}
					}
					else
						x += dx;
				}
			}
			//else if (e->obj->GetType() == EntityType::MUSH) // if e->obj is CBrick 
			//{
			//	Mushroom* mush = dynamic_cast<Mushroom*>(e->obj);

			//	if (nx != 0) vx = 0;
			//	if (ny != 0) vy = 0;
			//	if (e->ny != 0)
			//	{
			//		if (e->ny == -1)
			//		{
			//			isGround = true;
			//			isJumping = false;
			//			//isFly = false;
			//			//vy = 0;
			//		}
			//		/*else
			//			y += dy;*/

			//	}
			//	if (e->ny > 0)
			//	{
			//		DebugOut(L"yyyyyyyyyyyyyyyyyyy \n");
			//		mush->SetState(MUSHROOM_STATE_WALKING);
			//	}
			//}
			//if (e->obj->GetType() == EntityType::ENEMY)
				//SetInjured(1);



			//else if (dynamic_cast<Goomba*>(e->obj)) // if e->obj is Goomba 
			//{
			//	Goomba* goomba = dynamic_cast<Goomba*>(e->obj);

			//	// jump on top >> kill Goomba and deflect a bit 

			//	if (e->ny < 0)
			//	{
			//		if (goomba->GetState() != GOOMBA_STATE_DIE)
			//		{
			//			goomba->SetState(GOOMBA_STATE_DIE);
			//			vy = -MARIO_JUMP_DEFLECT_SPEED;
			//		}
			//	}
			//	else if (e->nx != 0)
			//	{
			//		if (untouchable == 0)
			//		{
			//			if (goomba->GetState() != GOOMBA_STATE_DIE)
			//			{
			//				if (level > MARIO_LEVEL_SMALL)
			//				{
			//					level = MARIO_LEVEL_SMALL;
			//					StartUntouchable();
			//				}
			//				else if (level == MARIO_LEVEL_SMALL)
			//					SetState(MARIO_STATE_DIE);
			//			}
			//		}
			//	}
			//} // if Goomba
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];


	float k, l;
	GetPosition(k, l);
	if (!Game::GetInstance()->IsKeyDown(DIK_A))
	{
		holdthing = nullptr;
	}
	if (holdthing && Game::GetInstance()->IsKeyDown(DIK_A))
	{
		holdthing->SetPosition(nx == 1?k + nx * 10: k + nx * 13, /*holdthing->Gety*/ l - 20);
	}


#pragma endregion
}

//void Player::SetInjured(int dame)
//{
//	if (isImmortaling)
//		return;
//	health -= dame;
//	gunDam -= dame;
//G
//	StartUntouchable();
//	immortalTimer->Start();
//	isImmortaling = true;
//}

void Player::Render()
{
	int ani = -1;
	if (isDie == true) {
		ani = MARIO_ANI_SMALL_DIE;
	}
	else if (level == MARIO_LEVEL_BIG)
	{
		if (isCrouch == true) {
			ani = MARIO_ANI_BIG_CROUCH;
		}
		else if (isWalkingComplete == true && vy == 0) 
{
			ani = MARIO_ANI_BIG_SKID;
		}
		else
		{
			if (isJumping == false)
			{
				if (vx == 0)
					ani = MARIO_ANI_BIG_IDLE;
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
					ani = MARIO_ANI_BIG_WALKING;
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
					ani = MARIO_ANI_BIG_WALK_RUN;
				else if (MARIO_VMAX < abs(vx))
					ani = MARIO_ANI_BIG_RUN;
			}
			else
			{
				if (vy > 0)
					ani = MARIO_ANI_BIG_JUMP_DOWN;
				else
					ani = MARIO_ANI_BIG_JUMP_UP;
			}
		}
	}
	else if (level == MARIO_LEVEL_SMALL)
	{
		if (isWalkingComplete == true && vy ==0) {
			ani = MARIO_ANI_SMALL_SKID;
		}
		else if (isKick == true) {
			ani = MARIO_ANI_SMALL_KICK;
		}
		else {
			if (isJumping == false)
			{
				if (vx == 0)
					ani = MARIO_ANI_SMALL_IDLE;
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
					ani = MARIO_ANI_SMALL_WALKING;
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
					ani = MARIO_ANI_SMALL_WALK_RUN;
				else if (MARIO_VMAX < abs(vx))
					ani = MARIO_ANI_SMALL_RUN;
			}
			else
				ani = MARIO_ANI_SMALL_JUMP;
		}

	}
	else if (level == MARIO_LEVEL_RACCOON)
	{
		if (isFly)
		{
			ani = MARIO_ANI_RACCOON_FLY;
		}
		else if (isCrouch)
			ani = MARIO_ANI_RACCOON_CROUCH;
		else if (isSpin)
			ani = MARIO_ANI_RACCOON_SPIN;
		else if (isWalkingComplete && vy == 0)
			ani = MARIO_ANI_RACCOON_SKID;
		else
		{
			if (isJumping == false)
			{
				if (vx == 0)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_IDLE;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_WALKING;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_RUN;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else if (MARIO_VMAX < abs(vx))
				{
					ani = MARIO_ANI_RACCOON_SPEEDUP;
				}
			}
			else
			{
				if (vy > MARIO_VY_TO_CONFIRM_JUM)
					ani = MARIO_ANI_RACCOON_JUMP_DOWN;
				else
					ani = MARIO_ANI_RACCOON_JUMP_UP;
			}
		}
	}
	else if (level == MARIO_LEVEL_FIRE)
	{
		if (isCrouch == true) {
			ani = MARIO_ANI_FIRE_CROUCH;
		}
		else if (isWalkingComplete == true && vy == 0) {
			ani = MARIO_ANI_FIRE_SKID;
		}
		else
		{
			if (isJumping == false)
			{
				if (vx == 0)
					ani = MARIO_ANI_FIRE_IDLE;
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
					ani = MARIO_ANI_FIRE_WALKING;
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
					ani = MARIO_ANI_FIRE_WALK_RUN;
				else if (MARIO_VMAX < abs(vx))
					ani = MARIO_ANI_FIRE_RUN;
			}
			else
			{
				if (vy > 0)
					ani = MARIO_ANI_FIRE_JUMP_DOWN;
				else
					ani = MARIO_ANI_FIRE_JUMP_UP;
			}
		}
	}
	int alpha = 255;
	if (untouchable) alpha = 128;
	animationSet->at(ani)->Render(nx, x, y, alpha);
	if (animationSet->at(MARIO_ANI_RACCOON_SPIN_SINGLE)->GetCurrentFrame() == 3)
		isAttack = false;
	//DebugOut(L"[vx] %f \n", vx);
	RenderBoundingBox();
}

void Player::SetState(int state)
{
	Entity::SetState(state);

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		isCrouch = false;
		//isKick = false;
		nx = 1;
		if (isRun == true)
			return;
		vx = MARIO_WALKING_SPEED;
		break;
	case MARIO_STATE_WALKING_LEFT:
		isCrouch = false;
		//isKick = false;
		nx = -1;
		if (isRun == true)
			return;
		vx = -MARIO_WALKING_SPEED;
		break;
	case MARIO_STATE_IDLE:
		isSpin = false;
		//isKick = false;
		isRun = false;
		isPressJump = false;
		isCrouch = false;
		if (vx > 0) {
			vx -= MARIO_WALKING_ACC * dt;
			if (vx < 0)
				vx = 0;
		}
		else if (vx < 0) {
			vx += MARIO_WALKING_ACC * dt;
			if (vx > 0)
				vx = 0;
		}
		//vx = 0;
		break;
	case MARIO_STATE_JUMP:
		isPressJump = true;
		isCrouch == true;
		if (level == MARIO_LEVEL_RACCOON)
		{
			if ((abs(vx) > MARIO_VMAX || isFly == true) && vy < 0.01 && isCheckCanFly == true)
			{
				isCrouch = false;
				isFly = true;
				vy = -MARIO_VY_FLY;
			}
			else
			{
				if (isJumping == true)
				{
					if (vy < 0)
						return;
					else
						vy = -MARIO_STOP_FALLING;
				}
				else
				{
					isJumpHandle = false;
					isJumping = true;
					vy = -MARIO_JUMP_SPEED_YMAX;
					backup_JumpY = y;
				}
			}
		}
		else
		{
			if (isJumping == true)
				return;
			else
			{
				isJumpHandle = false;
				isJumping = true;
				vy = -MARIO_JUMP_SPEED_YMAX;
				backup_JumpY = y;
			}
		}
		break;
	case MARIO_STATE_CROUCH:
		isCrouch = true;
		vx = 0;
		break;
	case MARIO_STATE_SPIN:
		isSpin = true;
		break;
	case MARIO_STATE_DIE:
		isDie = true;
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_KICK:
		isKick = true;
		break;
	}
}

void Player::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (level == MARIO_LEVEL_BIG)
	{
		left = x;
		top = y;
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_SMALL)
	{
		left = x;
		top = y;
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_RACCOON)
	{
		left = x;
		top = y;
		right = x + MARIO_RACCOON_BBOX_WIDTH -3;
		bottom = y + MARIO_RACCOON_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_FIRE)
	{
		left = x;
		top = y;
		right = x + MARIO_FIRE_BBOX_WIDTH;
		bottom = y + MARIO_FIRE_BBOX_HEIGHT;
	}
}

void Player::ResetBIG()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	nx = 1;
}

void Player::ResetRACCOON()
{
	SetState(MARIO_ANI_RACCOON_IDLE);
	SetLevel(MARIO_LEVEL_RACCOON);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	nx = 1;
}

void Player::ResetFIRE()
{
	SetState(MARIO_ANI_FIRE_IDLE);
	SetLevel(MARIO_LEVEL_FIRE);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	nx = 1;
}

