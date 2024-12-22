
#include "Shader_Compute_Defines.hlsli"

/* 16Byte Padding Required */
//StructuredBuffer<>

[numthreads(32, 32, 1)]
void CS_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    
    uint index = DTid.x + (DTid.y * (32 * iDispatchX));
    uint seed = DTid.x + DTid.y * 32 + DTid.z * 32 * 32;
    
    if (index > (iNumInstance * 2) - 1)
        return;
    
    if (index < iNumInstance)
    {
        Compute_StartDelay(index, seed);
        
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    if (index < iNumInstance)
    {
        if (false == g_Each_Desc_Current[index].bDelayEnd || 0 >= g_Particle_Current[index].fLifeTime)
            return;
        
        Update_Sprite(index);
        Update_Dissolve(index);
        Update_Color(index);
    }
    else
    {
        index -= iNumInstance;
        if (false == g_Each_Desc_Current[index].bDelayEnd || 0 >= g_Particle_Current[index].fLifeTime)
            return;
        
        if (true == bChangeDir)
        {
            g_Particle_Current[index].vVelocity = mul(g_Each_Desc_Current[index].vAfterStop_Dir, g_Each_Desc_Current[index].fSpeed);
            g_Each_Desc_Current[index].vSpeedDir = g_Each_Desc_Current[index].vAfterStop_Dir;
        }
        
        Update_Position(index);
        Update_Rotation(index);
        Update_Size(index);
        Update_Speed(index);
        ComputeLifeTime(index);
    }
    

    




}