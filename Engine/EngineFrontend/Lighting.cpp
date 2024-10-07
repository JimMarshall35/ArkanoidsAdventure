#include "Lighting.h"
#include "Scene.h"
#include "PointLightComponent.h"
#include "TransformComponent.h"
#include <algorithm>

int Light::GetPointLights(PointLightComponent* outPLC, glm::vec3* outPositions, size_t maxRequested)
{
	struct PL
	{
		PointLightComponent plc;
		glm::vec3 pos;
		float distanceFromCam;
	};
	Scn::Scene& scn = Scn::GetScene();
	EntityReg& reg = scn.entities.GetReg();
	Transform& t1 = reg.get<Transform>(scn.activeCameraAntity);
	auto v = scn.entities.GetReg().view<const PointLightComponent, Transform>();
	EVec<PL> lights;
	v.each([&](const PointLightComponent& plc, Transform& t) { 
		lights.push_back({ 
			plc,
			t.getGlobalPosition(),
			glm::distance(t.getGlobalPosition(), t1.getGlobalPosition())
		});
	});
	std::sort(lights.begin(), lights.end(), [](const PL& p1, const PL& p2) { return p1.distanceFromCam < p2.distanceFromCam; });

	int numOutput = 0;
	for (int i = 0; i < maxRequested && i < lights.size(); i++)
	{
		*outPLC++ = lights[i].plc;
		*outPositions++ = lights[i].pos;
		++numOutput;
	}
	return numOutput;
}
