#include "pch.h"
#include "EngineDLLInterface.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "XMLArchive.h"

static bool CompareTransforms(const Transform& ta, const Transform& tb)
{
	return ta.getLocalPosition() == tb.getLocalPosition() &&
		ta.getLocalRotation() == tb.getLocalRotation() &&
		ta.getLocalScale() == tb.getLocalScale() &&
		ta.getGlobalPosition() == tb.getGlobalPosition() &&
		ta.getGlobalScale() == tb.getGlobalScale();
}

TEST(Serialization, BasicXMLArchiveTest) {
	Scn::Scene s;
	EntityReg& r = s.entities.GetReg();
	Entity e = r.create();
	r.emplace<MeshComponent>(e);

	r.emplace<Transform>(e);
	Transform& t = r.get<Transform>(e);
	t.setLocalPosition({ 420, 69, 42 });
	t.setLocalRotation({ 1,2,3 });
	Entity e2 = r.create();

	r.emplace<Transform>(e2);
	Transform& t1 = r.get<Transform>(e2);
	t1.setLocalPosition({ 123, 456, 789 });
	t1.setLocalRotation({ 42, 0, 21 });

	XMLArchive ar("test.xml", true);
	s.entities.Serialize(ar);
	ar.Test();
	ar.Store();

	XMLArchive ar2("test.xml", false);
	Scn::Scene s2;
	s2.entities.Serialize(ar2);

	auto v = s2.entities.GetReg().view<entt::entity>();
	size_t numEnts = 0;
	for (Entity e : v)
	{
		++numEnts;
	}

	auto view = s2.entities.GetReg().view<Transform>();
	EVec<Transform> transforms;
	for (entt::entity entity : view) {
		Transform& t = view.get<Transform>(entity);
		transforms.push_back(t);
	}
	auto view2 = s2.entities.GetReg().view<MeshComponent>();

	EVec<MeshComponent> mesh;
	for (entt::entity entity : view2)
	{
		MeshComponent& t = view2.get<MeshComponent>(entity);
		mesh.push_back(t);
	}

	
	EXPECT_EQ(mesh.size(), 1);
	EXPECT_EQ(numEnts, 2);
	ASSERT_EQ(transforms.size(), 2);
	EXPECT_TRUE(CompareTransforms(transforms[0], t));
	EXPECT_TRUE(CompareTransforms(transforms[1], t1));

}