//#include "RaccoonTail.h"
//#include "Math.h"
//#include "Entity.h"
//#define MARIO_BIG_BBOX_WIDTH			44
//#define MARIO_BIG_BBOX_HEIGHT			76
//
//RaccoonTail::RaccoonTail() : Entity() {
//	SetLayer(ObjectLayers::ProjectTile);
//	SetTrigger(true);
//	SetActive(false);
//	SetDynamic(true);
//	SetName("Blade");
//	SetCollider(-MARIO_BIG_BBOX_WIDTH, 0, MARIO_BIG_BBOX_WIDTH, MARIO_BIG_BBOX_HEIGHT / 2);
//}
//
//void RaccoonTail::Render() {
//}
//
////void RaccoonTail::AddToScene(Scene scene) {
////	scene->AddObject(this);
////}
//
//void RaccoonTail::OnCollisionEnter(std::vector<CCollisionEvent> collisions) {
//
//}
//
//void RaccoonTail::OnTriggerEnter(std::vector<CCollisionEvent> collisions) {
//}
//
//
//void RaccoonTail::OnOverlapped(Entity obj) {
//	obj->OnBeingHorizontalCollided(HorizontalCollisionEvent(HorizontalCollideAction::Spin, Math::Sign(obj->GetPosition().x - GetPosition().x)));
//}
