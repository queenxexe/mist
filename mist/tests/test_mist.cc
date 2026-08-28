#include <gtest/gtest.h>
#include <Math.hpp>
#include <physics/Physics.hpp>

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

		EXPECT_NEAR(fwd.x, 1.0f, 0.0001f);
		EXPECT_NEAR(fwd.y, 0.0f, 0.0001f);
		EXPECT_NEAR(fwd.z, 0.0f, 0.0001f);
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

		EXPECT_NEAR(fwd.x, 0.0f, 0.0001f);
		EXPECT_NEAR(fwd.y, 0.0f, 0.0001f);
		EXPECT_NEAR(fwd.z, -1.0f, 0.0001f);
	}

	{
		mist::Transform a;
		a.Rotate(glm::radians(90.0), glm::vec3(0,1,0));
		a.Rotate(glm::radians(90.0), glm::vec3(0,1,0));
		glm::vec3 fwd = a.Forward();

		EXPECT_NEAR(fwd.x, 0.0f, 0.0001f);
		EXPECT_NEAR(fwd.y, 0.0f, 0.0001f);
		EXPECT_NEAR(fwd.z, -1.0f, 0.0001f);
	}

	{
		mist::Transform a;
		a.Rotate(glm::radians(90.0), glm::vec3(0,0,1));
		a.Rotate(glm::radians(90.0), glm::vec3(0,0,1));
		glm::vec3 fwd = a.Forward();

		EXPECT_NEAR(fwd.x, 0.0f, 0.0001f);
		EXPECT_NEAR(fwd.y, 0.0f, 0.0001f);
		EXPECT_NEAR(fwd.z, 1.0f, 0.0001f);
	}
}

TEST(MistTest, tranformEuler) {
	{
		glm::quat q = mist::Transform::EulerToQuat(glm::vec3(0,0,0));
		EXPECT_NEAR(q.x, 0.0f, 0.0001f);
		EXPECT_NEAR(q.y, 0.0f, 0.0001f);
		EXPECT_NEAR(q.z, 0.0f, 0.0001f);
		EXPECT_NEAR(q.w, 1.0f, 0.0001f);
	}

	{
		glm::quat q = mist::Transform::EulerToQuat(glm::vec3(0,90,0));
		glm::vec3 fwd = q * glm::vec3(0.0f, 0.0f, 1.0f);
		EXPECT_NEAR(fwd.x, 1.0f, 0.0001f);
		EXPECT_NEAR(fwd.y, 0.0f, 0.0001f);
		EXPECT_NEAR(fwd.z, 0.0f, 0.0001f);
	}

	// No gimbal lock test
	{
		glm::vec3 euler = glm::vec3(30,45,60);
		glm::quat q = mist::Transform::EulerToQuat(euler);
		glm::vec3 result = mist::Transform::QuatToEuler(q);
		EXPECT_NEAR(result.x, euler.x, 0.0001f);
		EXPECT_NEAR(result.y, euler.y, 0.0001f);
		EXPECT_NEAR(result.z, euler.z, 0.0001f);
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
		EXPECT_NEAR(result.x, 10.0f, 0.0001f);
		EXPECT_NEAR(result.y, 20.0f, 0.0001f);
		EXPECT_NEAR(result.z, 30.0f, 0.0001f);
	}

	{
		mist::Transform a;
		a.scale = glm::vec3(2,3,4);
		glm::mat4 matrix = a.GetLocalToWorldMatrix();
		glm::vec4 result = matrix * glm::vec4(1,1,1,1);
		EXPECT_NEAR(result.x, 2.0f, 0.0001f);
		EXPECT_NEAR(result.y, 3.0f, 0.0001f);
		EXPECT_NEAR(result.z, 4.0f, 0.0001f);
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

		for (int row = 0; row < 4; ++row) {
			for (int col = 0; col < 4; ++col) {
				EXPECT_NEAR(result[row][col], glm::mat4(1.0f)[row][col], 0.0001f);
			}
		}
	}

	{
		mist::Transform a;
		a.position = glm::vec3(10, 5, -3);
		a.rotation = mist::Transform::EulerToQuat(glm::vec3(20,40,60));
		a.scale = glm::vec3(2,3,4);

		glm::vec4 localPoint(1,2,3,4);
		glm::vec4 worldPoint = a.GetLocalToWorldMatrix() * localPoint;
		glm::vec4 restoredPoint = a.GetWorldToLocalMatrix() * worldPoint;

		EXPECT_NEAR(restoredPoint.x, localPoint.x, 0.0001f);
		EXPECT_NEAR(restoredPoint.y, localPoint.y, 0.0001f);
		EXPECT_NEAR(restoredPoint.z, localPoint.z, 0.0001f);
		EXPECT_NEAR(restoredPoint.w, localPoint.w, 0.0001f);
	}
}