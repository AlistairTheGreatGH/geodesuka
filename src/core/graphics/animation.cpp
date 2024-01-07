#include <geodesuka/core/graphics/animation.h>

#include <string>
#include <vector>
#include <map>

#include <geodesuka/core/math.h>

// using namespace std;

namespace geodesuka::core::graphics {

	using namespace math;

    animation::node_anim::node_anim() {}

	mat4<float> animation::node_anim::operator[](double aTime) const {
		mat4<float> T;
		mat4<float> R;
		mat4<float> S;

		// Calculates interpolated translation matrix
		{
			float p = 0.0f;
			float t = aTime;
			vec3<float> T1, T2, Tf;
			for (size_t i = 0; i < this->PositionKey.size() - 1; i++) {
				float t1 = this->PositionKey[i].Time;
				float t2 = this->PositionKey[i + 1].Time;
				if ((aTime >= t1) && (aTime <= t2)) {
					p = ((t - t1) / (t2 - t1));
					T1 = this->PositionKey[i].Value;
					T2 = this->PositionKey[i + 1].Value;
					break;
				}
			}
			Tf = (1.0f - p) * T1 + p * T2;
			T = mat4<float>(
				1.0f, 0.0f, 0.0f, Tf.x,
				0.0f, 1.0f, 0.0f, Tf.y,
				0.0f, 0.0f, 1.0f, Tf.z,
				0.0f, 0.0f, 0.0f, 1.0f
			);
		}

		// Calculates interpoted quaternion.
		{
			float p = 0.0f;
			float t = aTime;
			quaternion<float> Q1, Q2, Qf;
			for (size_t i = 0; i < this->RotationKey.size() - 1; i++) {
				float t1 = this->RotationKey[i].Time;
				float t2 = this->RotationKey[i + 1].Time;
				if ((aTime >= t1) && (aTime <= t2)) {
					p = ((t - t1) / (t2 - t1));
					Q1 = this->RotationKey[i].Value;
					Q2 = this->RotationKey[i + 1].Value;
					break;
				}
			}
			float Theta = std::acos(Q1.x*Q2.x + Q1.y*Q2.y + Q1.z*Q2.z + Q1.t*Q2.t);
			Qf = ((std::sin((1.0f - p) * Theta) * Q1 + std::sin(p * Theta) * Q2) / std::sin(Theta));
			R = rotation(Qf);
		}

		// Calculates interpolated scaling matrix
		{
			float p = 0.0f;
			float t = aTime;
			vec3<float> S1, S2, Sf;
			for (size_t i = 0; i < this->ScalingKey.size() - 1; i++) {
				float t1 = this->ScalingKey[i].Time;
				float t2 = this->ScalingKey[i + 1].Time;
				if ((aTime >= t1) && (aTime <= t2)) {
					p = ((t - t1) / (t2 - t1));
					S1 = this->ScalingKey[i].Value;
					S2 = this->ScalingKey[i + 1].Value;
					break;
				}
			}
			Sf = (1.0f - p) * S1 + p * S2;
			T = mat4<float>(
				Sf.x, 0.0f, 0.0f, 0.0f,
				0.0f, Sf.y, 0.0f, 0.0f,
				0.0f, 0.0f, Sf.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			);
		}

		// Order matters, scaling is applied first, then the object is rotated, then translated.
		return T * R * S;
	}

	animation::animation() {
		this->Name = "";
		this->Weight = 0.0f;
		this->Duration = 0.0f;
		this->TicksPerSecond = 0.0f;
	}

	animation::node_anim animation::operator[](std::string aNodeName) {
		return this->NodeAnimMap[aNodeName];
	}

}
