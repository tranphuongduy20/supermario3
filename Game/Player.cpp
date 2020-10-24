#include <algorithm>
#include <assert.h>
#include "debug.h"

#include "Player.h"
#include "Game.h"

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
	if (isJumping && backup_JumpY - y >= HIGHT_LEVER1 && isJumpHandle == false)
	{
		if (!isPressJump)
			vy = 0.03;
		isJumpHandle = true;
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
		DebugOut(L"\n true - %d", GetTickCount() - startWalkingComplete);
	}
	else DebugOut(L"\n false - %d", GetTickCount() - startWalkingComplete);

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
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->obj->GetType() == EntityType::BRICK)
			{
				x += min_tx * dx + nx * 0.4f;
				y += min_ty * dy + ny * 0.001f;
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
			//if (e->obj->GetType() == EntityType::ENEMY)
				//SetInjured(1);
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
#pragma endregion
}

//void Player::SetInjured(int dame)
//{
//	if (isImmortaling)
//		return;
//	health -= dame;
//	gunDam -= dame;
//
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
		else if (isWalkingComplete == true) {
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
		if (isWalkingComplete == true) {
			ani = MARIO_ANI_SMALL_SKID;
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
			//if (vy < 0.04)
			//{
			//	/*if (isCheckCanFly == true)
			//		ani = MARIO_ANI_RACCOON_FLY;
			//	else*/
			//		ani = MARIO_ANI_RACCOON_JUMP_DOWN;
			//}
			//else
			//{
			//	ani = MARIO_ANI_RACCOON_JUMP_DOWN;
			//	isCheckCanFly = false;
			//}
		}
		else if (isCrouch)
			ani = MARIO_ANI_RACCOON_CROUCH;
		else if (isSpin)
			ani = MARIO_ANI_RACCOON_SPIN;
		else if (isWalkingComplete)
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
		else if (isWalkingComplete == true) {
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
	//RenderBoundingBox();
}

void Player::SetState(int state)
{
	Entity::SetState(state);

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		isCrouch = false;
		nx = 1;
		if (isRun == true)
			return;
		vx = MARIO_WALKING_SPEED;
		break;
	case MARIO_STATE_WALKING_LEFT:
		isCrouch = false;
		nx = -1;
		if (isRun == true)
			return;
		vx = -MARIO_WALKING_SPEED;
		break;
	case MARIO_STATE_IDLE:
		isSpin = false;
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
			if ((abs(vx) > MARIO_VMAX || isFly == true) && vy <0.01 && isCheckCanFly == true)
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
	}
}

void Player::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;

	if (level == MARIO_LEVEL_BIG)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_SMALL)
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_RACCOON)
	{
		right = x + MARIO_RACCOON_BBOX_WIDTH;
		bottom = y + MARIO_RACCOON_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_FIRE)
	{
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

