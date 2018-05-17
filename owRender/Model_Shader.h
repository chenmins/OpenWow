#pragma once

#include "technique.h"

class Model_Shader : public Technique
{
    int32 gBones;
    int32  gColor;
    int32  gIsAnimated;
    int32  gDiffuseTexture;
    int32  gSpecularTexture;
    int32  gBlendMode;
    int32  gTextureWeight;
    int32  gBillboard;
    int32  gTextureAnimEnable;
    int32  gTextureAnimMatrix;

public:
	Model_Shader(RenderDevice* _RenderDevice) : Technique(_RenderDevice, "shaders/Models/Model")
    {
        gBones = getLocation("gBones");
        gColor = getLocation("gColor");
        gIsAnimated = getLocation("gIsAnimated");
        gDiffuseTexture = getLocation("gDiffuseTexture");
        gSpecularTexture = getLocation("gSpecularTexture");
        gBlendMode = getLocation("gBlendMode");
        gTextureWeight = getLocation("gTextureWeight");
        gBillboard = getLocation("gBillboard");
        gTextureAnimEnable = getLocation("gTextureAnimEnable");
        gTextureAnimMatrix = getLocation("gTextureAnimMatrix");
    }

	// Colors

	inline void SetColor(vec4 _color)
	{
		setVec4(gColor, _color);
	}

    inline void SetAnimated(int32 animated)
    {
        setInt(gIsAnimated, animated);
    }

    inline void SetBones(vector<mat4>& _bones)
    {
        if (_bones.empty())
        {
            fail1();
        }

        m_Shader->setShaderConst(gBones, CONST_FLOAT44, _bones.data(), _bones.size());
    }

	// Textures
 
	inline void SetDiffuseTexture(int TextureUnit)
	{
		setTexture(gDiffuseTexture, TextureUnit);
	}

	inline void SetSpecularTexture(int TextureUnit)
	{
		setTexture(gSpecularTexture, TextureUnit);
	}

	// Blending & Alpha

	inline void SetBlendMode(int _blendMode)
	{
		setInt(gBlendMode, _blendMode);
	}

	// R_Texture Weight
	inline void SetTextureWeight(float _weight)
	{
		setFloat(gTextureWeight, _weight);
	}


	// Billboard
	inline void SetBillboard(int _flag)
	{
		setInt(gBillboard, _flag);
	}

	// TextureAnim
	inline void SetTextureAnimEnable(int _flag)
	{
		setInt(gTextureAnimEnable, _flag);
	}

	inline void SetTextureAnimMatrix(mat4 _matrix)
	{
		setMat4(gTextureAnimMatrix, _matrix);
	}
};
