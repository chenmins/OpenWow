#pragma once

class PipelineWorldTransformation
{
public:
	PipelineWorldTransformation() 
	{ 
		Clear();
	}
	inline void Clear()
	{
		worldTransformation = glm::mat4(1.0f);
	}

	inline void Translate(float x, float y, float z);
	inline void Translate(cvec3 _translate);

	inline void RotateX(float _yaw);
	inline void RotateY(float _pitch);
	inline void RotateZ(float _roll);
	inline void Rotate(float _yaw, float _pitch, float _roll);
	inline void Rotate(cvec3 _rotation);

	inline void Scale(float _scale);
	inline void Scale(float _scaleX, float _scaleY, float _scaleZ);
	inline void Scale(cvec3 _scale);

	inline void Quat(cQuaternion _quat);
	inline void Quat(float w, float x, float y, float z);

	inline void Mult(cmat4 mat);

	inline const mat4& GetWorld()
	{
		return worldTransformation;
	}

private:
	mat4 worldTransformation;
};

#include "PipelineWorldTransformation.inl"