#ifndef SHADER_COMPUTE_DEFINE
#define SHADER_COMPUTE_DEFINE

#define NONE 3

#define STOP_TIME     0
#define STOP_DISTANCE 1

#define DISSOLVETEX     (1<<8)

cbuffer Particle_Init_Data : register(b0)
{
    unsigned int iNumInstance;
    unsigned int eParticleMode;
    bool bParticleLoop;
    bool bisSquare;

    float2 vStartSizeMin;
    float2 vStartSizeMax;

    float2 vSizeOverLife;
    float2 vPadding0;

    float3 vRotAxis;
    float fRotSpeed;

    bool bBillboardWithoutUp;
    float fStretchFactor;
    float2 vPadding1;

    float3 vLightDir;
    float fPadding2;

    float4 vLightAmbient;

    bool bHasTargetPoint;
    bool bTargetWorldOffSet;
    float2 vPadding2;

    float3 vTargetPointOffset;
    float fMargin;

    bool bFollowTargetPos;
    bool bTargetLifeZero;
    bool bXStretch;
    bool bYStretch;

    bool bStopParticle;
    unsigned int iStopType;
    float fStopTime;
    float fStopDistance;

    float fMaintainTime;
    bool bChangeDirAfterStop;
    float2 vPadding3;

    float3 vAfterDirMin;
    float fPadding4;
    float3 vAfterDirMax;
    float fPadding5;

    float fSpriteChangeTolerancel;
    float3 vPadding6;
};

cbuffer Common_Effect_Data : register(b1)
{
    unsigned int iTransformType = { 0 };
    unsigned int iTextureFlag = { 0 };

    bool bEffectLoop = { false };
    bool bLerpSize = { false };

    float fSpeedOverLifeTime = { 0.f };
    bool bEnableColorOverLife = { false };
    float2 Padding7 = { 0.f, 0.f };

    float3 vStartPosMin = { 0.f, 0.f, 0.f };
    float Padding8 = { 0.f };

    float3 vStartPosMax = { 0.f, 0.f, 0.f };
    float Padding9 = { 0.f };

    float4 vColorOverLife = { 0.f, 0.f, 0.f, 0.f };

    float3 vRotationOverLife = { 0.f, 0.f, 0.f };
    bool bRotationLocal = { false };


    float fTotalPlayTime = { 1.f };
    bool bDissolveReverse = false;
    float fDissolveSpeed = 0.f;
    float fDissolveStartTime = 0.f;

    bool bBaseSprite = { false };
    int iBaseSpriteMaxRow = { 0 };
    int iBaseSpriteMaxCol = { 0 };
    bool ChangeBaseSprite = { false };

    float fBaseSpriteChangeTime = { 0.f };
    bool bMaskSprite = { false };
    int iMaskSpriteMaxRow = { 0 };
    int iMaskSpriteMaxCol = { 0 };

    bool ChangeMaskSprite = { false };
    float fMaskSpriteChangeTime = { 0.f };
    float2 Padding10 = { 0.f, 0.f };
}

cbuffer Tick_Data : register(b2)
{
    float fTimeDelta = 0.f;
    float3 vTargetPoint = (0.f, 0.f, 0.f);
    bool bEndStop = false;
    bool bStoped = false;
    bool bChangeDir = false;
    bool bCurParticleLoop = false;
    int iDispatchX = 1;
    float3 vPadding11 = { 0.f, 0.f, 0.f };
    float4x4 AttachMatrix;
}


struct PARTICLE
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    
    float4 vColor;
    
    float3 vVelocity;
    float fLifeTime;
    
    int iSpriteIndex;
    float fDissolve;
    float fRotation;
    float fPadding;
};

struct Each_Desc
{
    float fSpeed;
    float3 vSpeedDir;
    
    float3 vPowerDir;
    float fStartDelay;
    
    float3 vRot;
    float fDissolveTime;

    float3 vAfterStop_Dir;
    float fMoveDist;
		
    float fSpriteChangeTime;
    float fSpriteTimeAcc;
    bool bDelayEnd;
    float fPadding;
};

RWStructuredBuffer<PARTICLE> g_Particle_Original : register(u0);
RWStructuredBuffer<PARTICLE> g_Particle_Current : register(u1);

RWStructuredBuffer<Each_Desc> g_Each_Desc_Original : register(u2);
RWStructuredBuffer<Each_Desc> g_Each_Desc_Current : register(u3);

float Hash(uint seed)
{
    uint state = seed * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return ((word >> 22u) ^ word);
}

float Random_float(uint seed)
{
    return Hash(seed) / 4294967295.f;
}

float3 random_float3_range(uint seed, float3 vMin, float3 vMax)
{
 
    return float3(
        vMin.x + (Random_float(seed) * (vMax.x - vMin.x)),
        vMin.y + (Random_float(seed + 1) * (vMax.y - vMin.y)),
        vMin.z + (Random_float(seed + 2) * (vMax.z - vMin.z))
    );
}

float4x4 CreateAxisRotMat(float3 vRotationAngle)
{
    float4x4 Identity = float4x4(
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1);
    
    float4x4 rotationX, rotationY, rotationZ;
    if (0 != vRotationAngle.x)
    {
        rotationX = float4x4(
                         1, 0, 0, 0,
                         0, cos(vRotationAngle.x), -sin(vRotationAngle.x), 0,
                         0, sin(vRotationAngle.x), cos(vRotationAngle.x), 0,
                         0, 0, 0, 1);
    }
    else
        rotationX = Identity;
    
    if (0 != vRotationAngle.y)
    {
        
        rotationY = float4x4(
                        cos(vRotationAngle.y), 0, sin(vRotationAngle.y), 0,
                        0, 1, 0, 0,
                        -sin(vRotationAngle.y), 0, cos(vRotationAngle.y), 0,
                        0, 0, 0, 1);
    }
    else
        rotationY = Identity;
    
    if (0 != vRotationAngle.z)
    {
        
        rotationZ = float4x4(
                        cos(vRotationAngle.z), -sin(vRotationAngle.z), 0, 0,
                        sin(vRotationAngle.z), cos(vRotationAngle.z), 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1);
    }
    else
        rotationZ = Identity;
    
    return mul(rotationZ, mul(rotationY, rotationX));
}


void Compute_StartDelay(int iIndex, uint Seed)
{
    if (0 <= g_Each_Desc_Current[iIndex].fStartDelay)
    {
        g_Particle_Current[iIndex].fLifeTime = 0.f;
        g_Each_Desc_Current[iIndex].fStartDelay -= fTimeDelta;
        
        if (0.f > g_Each_Desc_Current[iIndex].fStartDelay)
        {
            g_Particle_Original[iIndex].vTranslation = float4(random_float3_range(Seed, vStartPosMin, vStartPosMax), 1.f);
            g_Particle_Current[iIndex] = g_Particle_Original[iIndex];
            if (1 == iTransformType)    // FOLLOW
            {
                matrix ParticleMat;
                ParticleMat[0] = g_Particle_Original[iIndex].vRight;
                ParticleMat[1] = g_Particle_Original[iIndex].vUp;
                ParticleMat[2] = g_Particle_Original[iIndex].vLook;
                ParticleMat[3] = g_Particle_Original[iIndex].vTranslation;
                
                matrix BoneMat = AttachMatrix;
              
                g_Particle_Current[iIndex].vVelocity = mul(float4(g_Particle_Original[iIndex].vVelocity, 0.f), BoneMat);
                g_Each_Desc_Current[iIndex].vPowerDir = mul(float4(g_Each_Desc_Original[iIndex].vPowerDir, 0.f), BoneMat);

                g_Particle_Current[iIndex].vRight = mul(g_Particle_Original[iIndex].vRight, AttachMatrix);
                g_Particle_Current[iIndex].vUp = mul(g_Particle_Original[iIndex].vUp, AttachMatrix);
                g_Particle_Current[iIndex].vLook = mul(g_Particle_Original[iIndex].vLook, AttachMatrix);
                g_Particle_Current[iIndex].vTranslation = mul(g_Particle_Original[iIndex].vTranslation, AttachMatrix);

            }
            g_Each_Desc_Current[iIndex].bDelayEnd = true;
            return;
        }
        
        g_Each_Desc_Current[iIndex].bDelayEnd = false;
        return;
    }
   
    g_Each_Desc_Current[iIndex].bDelayEnd = true;
    return;
}

void Update_Sprite(int iIndex)
{
    if (true == bBaseSprite && true == ChangeBaseSprite)
    {
        g_Each_Desc_Current[iIndex].fSpriteTimeAcc += fTimeDelta;
        if (g_Each_Desc_Current[iIndex].fSpriteTimeAcc >= g_Each_Desc_Current[iIndex].fSpriteChangeTime)
        {
            g_Each_Desc_Current[iIndex].fSpriteTimeAcc = 0.f;
            ++g_Particle_Current[iIndex].iSpriteIndex;
            if (iBaseSpriteMaxCol * iBaseSpriteMaxRow <= g_Particle_Current[iIndex].iSpriteIndex)
                g_Particle_Current[iIndex].iSpriteIndex = 0;
        }

    }
    
    if (true == bMaskSprite && true == ChangeMaskSprite)
    {
        g_Each_Desc_Current[iIndex].fSpriteTimeAcc += fTimeDelta;
        if (g_Each_Desc_Current[iIndex].fSpriteTimeAcc >= g_Each_Desc_Current[iIndex].fSpriteChangeTime)
        {
            g_Each_Desc_Current[iIndex].fSpriteTimeAcc = 0.f;
            ++g_Particle_Current[iIndex].iSpriteIndex;
            if (iMaskSpriteMaxCol * iMaskSpriteMaxRow <= g_Particle_Current[iIndex].iSpriteIndex)
                g_Particle_Current[iIndex].iSpriteIndex = 0;
        }
    }
}

void Update_Dissolve(int iIndex)
{
    if (0 == (iTextureFlag & DISSOLVETEX))
        return;
    
    g_Each_Desc_Current[iIndex].fDissolveTime += fTimeDelta;
    
    if (fDissolveStartTime > g_Each_Desc_Current[iIndex].fDissolveTime)
        return;
    
    g_Particle_Current[iIndex].fDissolve += fDissolveSpeed * fTimeDelta;
}

void Update_Position(int iIndex)
{
    float4 vCurPos = {0.f,0.f,0.f,1.f};
    
    if (true == bStopParticle && false == bEndStop)
    {
        if (true == bStoped && STOP_TIME == iStopType)
            return;
        
        if (STOP_DISTANCE == iStopType && g_Each_Desc_Current[iIndex].fMoveDist >= fStopDistance)
            return;
        
        vCurPos = g_Particle_Current[iIndex].vTranslation;
    }
    
    if (true == bHasTargetPoint)
    {
        
        float4x4 TransfomrMat = float4x4(g_Particle_Current[iIndex].vRight
                                                , g_Particle_Current[iIndex].vUp
                                                , g_Particle_Current[iIndex].vLook
                                                , g_Particle_Current[iIndex].vTranslation);
        
        vector vDir = float4(vTargetPoint, 1.f) - g_Particle_Current[iIndex].vTranslation;
        
        if (true == bChangeDirAfterStop && true == bEndStop)
            vDir = float4(g_Each_Desc_Original[iIndex].vAfterStop_Dir, 0.f);
        
        if (fMargin < length(vDir) || (true == bChangeDirAfterStop && true == bEndStop))
        {
            TransfomrMat[3] += mul(normalize(vDir), g_Each_Desc_Current[iIndex].fSpeed * fTimeDelta);
        }
        else
        {
            if (true == bTargetLifeZero)
                g_Particle_Current[iIndex].fLifeTime = 0.f;
            else
            {
                if (true == bChangeDirAfterStop || false == bEndStop)
                    return;
            }
        }
        
        g_Particle_Current[iIndex].vTranslation = TransfomrMat[3];
        
        if (STOP_DISTANCE == iStopType && false == bEndStop)
            g_Each_Desc_Current[iIndex].fMoveDist += length(TransfomrMat[3] - vCurPos);
        return;
    }
    
    float3 vVelocity = g_Particle_Current[iIndex].vVelocity;
    
    g_Particle_Current[iIndex].vTranslation += float4(mul(g_Particle_Current[iIndex].vVelocity, fTimeDelta), 0.f);
    
    if (STOP_DISTANCE == iStopType && false == bEndStop)
        g_Each_Desc_Current[iIndex].fMoveDist += length(g_Particle_Current[iIndex].vTranslation - vCurPos);
}
//
void Update_Rotation(int iIndex)
{
    if (0.f == fRotSpeed)
        return;
    
    if (NONE != eParticleMode)
    {
        g_Particle_Current[iIndex].fRotation += fRotSpeed * fTimeDelta;
    }
    else
    {
        vector vRight, vUp, vLook, vAxis;
        if (true == bRotationLocal)
        {
            vRight = g_Particle_Current[iIndex].vRight;
            vUp = g_Particle_Current[iIndex].vUp;
            vLook = g_Particle_Current[iIndex].vLook;
        }
        else
        {
            vRight = float4(1.f, 0.f, 0.f, 0.f);
            vUp = float4(0.f, 1.f, 0.f, 0.f);
            vLook = float4(0.f, 0.f, 1.f, 0.f);
        }
     
        row_major float4x4 RotMat = CreateAxisRotMat(mul(vRotAxis, radians(fRotSpeed) * fTimeDelta));
        
        g_Particle_Current[iIndex].vRight = mul(vRight, RotMat);
        g_Particle_Current[iIndex].vUp = mul(vUp, RotMat);
        g_Particle_Current[iIndex].vLook = mul(vLook, RotMat);
    }

}

void Update_Color(int iIndex)
{
    if (true == bEnableColorOverLife)
    {
        float fLerpRatio = 1.f - (g_Particle_Current[iIndex].fLifeTime 
        / g_Particle_Original[iIndex].fLifeTime);
        
        g_Particle_Current[iIndex].vColor = lerp(g_Particle_Original[iIndex].vColor, vColorOverLife, fLerpRatio);
    }
}

void Update_Size(int iIndex)
{
    if (false == bLerpSize)
        return;
    
    float fOriginalSizeX = length(g_Particle_Original[iIndex].vRight);
    float fOriginalSizeY = length(g_Particle_Original[iIndex].vUp);
    
    float fLerpRatio = 1.f - (g_Particle_Current[iIndex].fLifeTime 
                        / g_Particle_Original[iIndex].fLifeTime);
    
    float fCurSizeX = lerp(fOriginalSizeX, vSizeOverLife.x, fLerpRatio);
    float fCurSizeY = lerp(fOriginalSizeY, vSizeOverLife.y, fLerpRatio);
    
    g_Particle_Current[iIndex].vRight = mul(normalize(g_Particle_Current[iIndex].vRight), fCurSizeX);
    g_Particle_Current[iIndex].vUp = mul(normalize(g_Particle_Current[iIndex].vUp), fCurSizeY);
}

void Update_Speed(int iIndex)
{
    if (true == bStopParticle && true == bEndStop)
    {
        if (true == bStoped && STOP_TIME == iStopType)
            return;
        
        if (STOP_DISTANCE == iStopType && g_Each_Desc_Current[iIndex].fMoveDist >= fStopDistance)
            return;
    }
    
    if (true == bHasTargetPoint)
    {
        float4 vDir = float4(vTargetPoint, 1.f) - g_Particle_Current[iIndex].vTranslation;
        
        if (true == bChangeDirAfterStop && true == bEndStop)
        {
            vDir = float4(g_Each_Desc_Original[iIndex].vAfterStop_Dir, 0.f);
        }
        
        g_Particle_Current[iIndex].vVelocity = mul(normalize(vDir), g_Each_Desc_Current[iIndex].fSpeed);
        return;
    }
    
    g_Particle_Current[iIndex].vVelocity += mul(g_Each_Desc_Current[iIndex].vPowerDir, fTimeDelta);
}

void ComputeLifeTime(int iIndex)
{
    g_Particle_Current[iIndex].fLifeTime -= fTimeDelta;
    
    if (0.f >= g_Particle_Current[iIndex].fLifeTime)
    {
        g_Particle_Current[iIndex].fLifeTime = 0.f;
        
        if (true == bCurParticleLoop)
        {
            g_Each_Desc_Current[iIndex] = g_Each_Desc_Original[iIndex];
        }
    }
}

#endif