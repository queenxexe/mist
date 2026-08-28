#include <gtest/gtest.h>
#include <Math.hpp>
#include <physics/Physics.hpp>
#include <components/Camera.hpp>
#include <renderer/vulkan/VulkanHelper.hpp>

void ExpectMatrixNear(const glm::mat4& a, const glm::mat4& b, float tolerance = 0.0001f) {
	for (int column = 0; column < 4; ++column) {
		for (int row = 0; row < 4; ++row) {
			EXPECT_NEAR(
				a[column][row],
				b[column][row],
				tolerance
			);
		}
	}
}

void ExpectNearVec(const glm::vec3& a, const glm::vec3& b, float tolerance = 0.0001f) {
	EXPECT_NEAR(a.x, b.x, tolerance);
	EXPECT_NEAR(a.y, b.y, tolerance);
	EXPECT_NEAR(a.z, b.z, tolerance);
}

void ExpectNearVec(const glm::vec4& a, const glm::vec4& b, float tolerance = 0.0001f) {
	EXPECT_NEAR(a.x, b.x, tolerance);
	EXPECT_NEAR(a.y, b.y, tolerance);
	EXPECT_NEAR(a.z, b.z, tolerance);
	EXPECT_NEAR(a.w, b.w, tolerance);
}

void ExpectNearQuat(const glm::quat& a, const glm::quat& b, float tolerance = 0.0001f) {
	EXPECT_NEAR(a.x, b.x, tolerance);
	EXPECT_NEAR(a.y, b.y, tolerance);
	EXPECT_NEAR(a.z, b.z, tolerance);
	EXPECT_NEAR(a.w, b.w, tolerance);
}

TEST(MistTest, collisionDetectionTest) {
	mist::Physics physics;

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(3, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(3, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 5, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,-1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(1, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 1) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(3, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(3, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 3, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 3, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 3, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 3, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}
}

TEST(MistTest, transformEqual) {
	{
		mist::Transform a;
		mist::Transform b;
		EXPECT_TRUE(a.IsEqual(b));
	}

	{
		mist::Transform a(glm::vec3(0,0,0));
		mist::Transform b(glm::vec3(1,0,0));
		EXPECT_FALSE(a.IsEqual(b));
	}

	{
		mist::Transform a;
		mist::Transform b;
		a.Rotate(glm::radians(45.0), glm::vec3(0,1,0));
		EXPECT_FALSE(a.IsEqual(b));
	}

	{
		mist::Transform a;
		mist::Transform b;
		a.Rotate(glm::radians(45.0), glm::vec3(0,1,0));
		b.Rotate(glm::radians(45.0), glm::vec3(0,1,0));
		EXPECT_TRUE(a.IsEqual(b));
	}

	{
		mist::Transform a;
		mist::Transform b;
		a.scale = glm::vec3(1, 2, 1);
		EXPECT_FALSE(a.IsEqual(b));
	}

	{
		mist::Transform a;
		mist::Transform b;
		a.scale = glm::vec3(1, 2, 1);
		b.scale = glm::vec3(1, 2, 1);
		EXPECT_TRUE(a.IsEqual(b));
	}
}

TEST(MistTest, transformRotate) {
	{
		mist::Transform a;
		a.Rotate(glm::radians(90.0), glm::vec3(0,1,0));
		glm::vec3 fwd = a.Forward();
		ExpectNearVec(fwd, glm::vec3(1,0,0));
	}

	{
		mist::Transform a;
		glm::quat initRot = a.rotation;
		a.Rotate(0.0, glm::vec3(1,0,0));
		EXPECT_EQ(a.rotation, initRot);
	}

	{
		mist::Transform a;
		glm::quat initRot = a.rotation;
		a.Rotate(0.0, glm::vec3(0,1,0));
		EXPECT_EQ(a.rotation, initRot);
	}

	{
		mist::Transform a;
		glm::quat initRot = a.rotation;
		a.Rotate(0.0, glm::vec3(0,0,1));
		EXPECT_EQ(a.rotation, initRot);
	}

	{
		mist::Transform a;
		a.Rotate(glm::radians(90.0), glm::vec3(1,0,0));
		a.Rotate(glm::radians(90.0), glm::vec3(1,0,0));
		glm::vec3 fwd = a.Forward();
		ExpectNearVec(fwd, glm::vec3(0,0,-1));
	}

	{
		mist::Transform a;
		a.Rotate(glm::radians(90.0), glm::vec3(0,1,0));
		a.Rotate(glm::radians(90.0), glm::vec3(0,1,0));
		glm::vec3 fwd = a.Forward();
		ExpectNearVec(fwd, glm::vec3(0,0,-1));
	}

	{
		mist::Transform a;
		a.Rotate(glm::radians(90.0), glm::vec3(0,0,1));
		a.Rotate(glm::radians(90.0), glm::vec3(0,0,1));
		glm::vec3 fwd = a.Forward();
		ExpectNearVec(fwd, glm::vec3(0,0,1));
	}
}

TEST(MistTest, tranformEuler) {
	{
		glm::quat q = mist::Transform::EulerToQuat(glm::vec3(0,0,0));
		ExpectNearQuat(q, glm::quat(1,0,0,0));
	}

	{
		glm::quat q = mist::Transform::EulerToQuat(glm::vec3(0,90,0));
		glm::vec3 fwd = q * glm::vec3(0.0f, 0.0f, 1.0f);
		ExpectNearVec(fwd, glm::vec3(1,0,0));
	}

	// No gimbal lock test
	{
		glm::vec3 euler = glm::vec3(30,45,60);
		glm::quat q = mist::Transform::EulerToQuat(euler);
		glm::vec3 result = mist::Transform::QuatToEuler(q);
		ExpectNearVec(euler, result);
	}

	// Gimbal lock test (caused by having 90)
	{
		glm::vec3 euler = glm::vec3(30,90,60);
		glm::quat original = mist::Transform::EulerToQuat(euler);
		glm::vec3 convertedEuler = mist::Transform::QuatToEuler(original);
		glm::quat converted = mist::Transform::EulerToQuat(convertedEuler);
		EXPECT_NEAR(glm::abs(glm::dot(original, converted)), 1.0f, 0.0001f);
	}
}

TEST(MistTest, transformDirs) {
	mist::Transform a;
	EXPECT_EQ(a.Left(), 	glm::vec3(1,0,0));
	EXPECT_EQ(a.Right(), 	glm::vec3(-1,0,0));
	EXPECT_EQ(a.Up(), 		glm::vec3(0,1,0));
	EXPECT_EQ(a.Down(), 	glm::vec3(0,-1,0));
	EXPECT_EQ(a.Forward(), 	glm::vec3(0,0,1));
	EXPECT_EQ(a.Backward(), glm::vec3(0,0,-1));
}

TEST(MistTest, transformLTW) {
	{
		mist::Transform a;
		glm::mat4 matrix = a.GetLocalToWorldMatrix();
		EXPECT_EQ(matrix, glm::mat4(1.0));
	}

	{
		mist::Transform a;
		a.position = glm::vec3(10,20,30);
		glm::mat4 matrix = a.GetLocalToWorldMatrix();
		glm::vec4 result = matrix * glm::vec4(0,0,0,1);
		ExpectNearVec(result, glm::vec3(10,20,30));
	}

	{
		mist::Transform a;
		a.scale = glm::vec3(2,3,4);
		glm::mat4 matrix = a.GetLocalToWorldMatrix();
		glm::vec4 result = matrix * glm::vec4(1,1,1,1);
		ExpectNearVec(result, glm::vec3(2,3,4));
	}
}

TEST(MistTest, transformWTL) {
	{
		mist::Transform a;
		a.position = glm::vec3(10,20,30);
		a.rotation = mist::Transform::EulerToQuat(glm::vec3(30,40,60));
		a.scale = glm::vec3(2,3,4);

		glm::mat4 ltw = a.GetLocalToWorldMatrix();
		glm::mat4 wtl = a.GetWorldToLocalMatrix();
		glm::mat4 result = ltw * wtl;

		ExpectMatrixNear(glm::mat4(1.0f), result);
	}

	{
		mist::Transform a;
		a.position = glm::vec3(10, 5, -3);
		a.rotation = mist::Transform::EulerToQuat(glm::vec3(20,40,60));
		a.scale = glm::vec3(2,3,4);

		glm::vec4 localPoint(1,2,3,4);
		glm::vec4 worldPoint = a.GetLocalToWorldMatrix() * localPoint;
		glm::vec4 restoredPoint = a.GetWorldToLocalMatrix() * worldPoint;
		ExpectNearVec(restoredPoint, localPoint);
	}
}

TEST(MistTest, cameraEquals) {
	{
		mist::Transform aTransform;
		mist::Camera a(aTransform);
		mist::Transform bTransform;
		mist::Camera b(bTransform);
		EXPECT_TRUE(a.IsEqual(b));
	}

	{
		mist::Transform aTransform;
		mist::Camera a(aTransform);
		mist::Transform bTransform;
		mist::Camera b(bTransform);
		b.SetPerspectiveCamera(1920, 1080, 90, 1, 1000);
		EXPECT_FALSE(a.IsEqual(b));
	}

	{
		mist::Transform aTransform;
		mist::Camera a(aTransform);
		mist::Transform bTransform;
		mist::Camera b(bTransform);
		a.SetPerspectiveCamera(1920, 1080, 90, 1, 1000);
		b.SetPerspectiveCamera(1920, 1080, 90, 1, 1000);
		EXPECT_TRUE(a.IsEqual(b));
	}

	{
		mist::Transform aTransform;
		mist::Camera a(aTransform);
		mist::Transform bTransform;
		mist::Camera b(bTransform);
		a.SetOrthographicCamera(1920, 1080, 10, 1, 1000);
		b.SetPerspectiveCamera(1920, 1080, 90, 1, 1000);
		EXPECT_FALSE(a.IsEqual(b));
	}
}

TEST(MistTest, perspectiveCamera) {
	const float width = 1920;
	const float height = 1080;
	const float near = 0.1f;
	const float far = 1000.0f;
	const float fov = 60.0f;

	mist::Transform transform;
	mist::Camera cam(transform);
	cam.SetPerspectiveCamera(width, height, fov, near, far);
	glm::mat4 expected = glm::perspectiveLH_ZO(glm::radians(fov), width / height, near, far);
	EXPECT_EQ(cam.GetProjectionMatrix(), expected);
}

TEST(MistTest, perspectiveRatio) {
	const float width = 1920;
	const float height = 1080;
	const float near = 0.1f;
	const float far = 1000.0f;
	const float fov = 60.0f;

	mist::Transform transform;
	mist::Camera cam(transform);
	cam.SetPerspectiveCamera(width, height, fov, near, far);
	glm::mat4 wide = cam.GetProjectionMatrix();
	cam.SetPerspectiveCamera(height, width, fov, near, far);
	glm::mat4 tall = cam.GetProjectionMatrix();
	EXPECT_NE(wide, tall);
}

TEST(MistTest, orthographicCamera) {
	const float width = 1920;
	const float height = 1080;
	const float near = -1;
	const float far = 1;
	const float size = 10;

	mist::Transform transform;
	mist::Camera cam(transform);
	cam.SetOrthographicCamera(width, height, size, near, far);

	float aspect = width / height;
	float left = -size * aspect * 0.5f;
	float right = size * aspect * 0.5f;
	float bottom = -size * 0.5f;
	float top = size * 0.5f;
	glm::mat4 expected = glm::orthoLH_ZO(left, right, bottom, top, near, far);

	EXPECT_EQ(cam.GetProjectionMatrix(), expected);
}

TEST(MistTest, viewportResize) {
	{
		mist::Transform transform;
		mist::Camera cam(transform);
		cam.SetPerspectiveCamera(800, 600, 60, 0.1f, 1000);
		glm::mat4 original = cam.GetProjectionMatrix();
		cam.SetViewportSize(1920, 1080);
		glm::mat4 adjusted = cam.GetProjectionMatrix();
		EXPECT_NE(original, adjusted);
	}

	{
		mist::Transform transform;
		mist::Camera cam(transform);
		cam.SetOrthographicCamera(800, 600, 10, -1, 1);
		glm::mat4 original = cam.GetProjectionMatrix();
		cam.SetViewportSize(1920, 1080);
		glm::mat4 adjusted = cam.GetProjectionMatrix();
		EXPECT_NE(original, adjusted);
	}
}

TEST(MistTest, viewMatrix) {
	{
		mist::Transform transform;
		mist::Camera cam(transform);
		glm::mat4 expected = glm::lookAtLH(
			glm::vec3(0,0,0),
			glm::vec3(0,0,1),
			glm::vec3(0,1,0)
		);
		EXPECT_EQ(cam.GetViewMatrix(), expected);
	}

	{
		mist::Transform transform;
		mist::Camera cam(transform);
		glm::mat4 original = cam.GetViewMatrix();
		transform.position += glm::vec3(10,20,30);
		glm::mat4 moved = cam.GetViewMatrix();
		EXPECT_NE(original, moved);
	}

	{
		mist::Transform transform;
		mist::Camera cam(transform);
		glm::mat4 original = cam.GetViewMatrix();
		transform.Rotate(glm::radians(90.0), glm::vec3(0,1,0));
		glm::mat4 moved = cam.GetViewMatrix();
		EXPECT_NE(original, moved);
	}
}

TEST(MistTest, viewProjectionMatrix) {
	mist::Transform transform;
	mist::Camera cam(transform);
	cam.SetPerspectiveCamera(1920, 1080, 60, 0.1f, 1000);
	EXPECT_EQ(cam.GetViewProjectionMatrix(), cam.GetProjectionMatrix() * cam.GetViewMatrix());
}

#if DEBUG
// Rejections are based on MIST_ASSERT which are stripped in release builds
TEST(MistTest, cameraRejections) {
	{
		mist::Transform transform;
		mist::Camera cam(transform);
		EXPECT_DEATH(cam.SetPerspectiveCamera(1920, 1080, 60, 100, 10), "");
	}

	{
		mist::Transform transform;
		mist::Camera cam(transform);
		EXPECT_DEATH(cam.SetPerspectiveCamera(0, 1080, 60, 0.1f, 1000), "");
	}

	{
		mist::Transform transform;
		mist::Camera cam(transform);
		EXPECT_DEATH(cam.SetPerspectiveCamera(1920, 0, 60, 0.1f, 1000), "");
	}

	{
		mist::Transform transform;
		mist::Camera cam(transform);
		EXPECT_DEATH(cam.SetOrthographicCamera(1920, 1080, 10, 1, -1), "");
	}

	{
		mist::Transform transform;
		mist::Camera cam(transform);
		EXPECT_DEATH(cam.SetOrthographicCamera(0, 1080, 10, -1, 1), "");
	}

	{
		mist::Transform transform;
		mist::Camera cam(transform);
		EXPECT_DEATH(cam.SetOrthographicCamera(1920, 0, 10, -1, 1), "");
	}
}
#endif

TEST(MistTest, vulkanProjectionFlip) {
	{
		mist::Transform transform;
		mist::Camera cam(transform);
		cam.SetPerspectiveCamera(1920, 1080, 60, 0.1f, 1000);

		glm::mat4 expected = cam.GetProjectionMatrix();
		expected[1][1] *= -1;

		glm::mat4 flipped = mist::VulkanHelper::GetFlippedProjectionMatrix(cam.GetProjectionMatrix());
		ExpectMatrixNear(expected, flipped);
	}

	{
		mist::Transform transform;
		mist::Camera cam(transform);
		cam.SetPerspectiveCamera(1920, 1080, 60, 0.1f, 1000);

		glm::mat4 expected = cam.GetProjectionMatrix();
		expected[1][1] *= -1;
		expected *= cam.GetViewMatrix();

		glm::mat4 result = mist::VulkanHelper::GetFlippedViewProjectionMatrix(cam);
		ExpectMatrixNear(expected, result);
	}
}