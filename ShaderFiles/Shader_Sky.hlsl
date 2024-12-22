#include "Shader_Defines.hlsli"

matrix g_CamViewMatrixInv, g_CamProjMatrixInv;
float4 g_vCamPosition;

// Sky
float4  g_vTopColor = float4(0.f, 0.4f, 0.9f, 1.f);
float4  g_vBottomColor = float4(0.2f, 0.3f, 0.8f, 1.f);;

// Horizon
float3 horizon_color = float3(0.f, 0.7f, 0.8f);
float horizon_blur = 0.05f;

// Sun
float3 sun_color = float3(10.f, 8.f, 1.f);
float3 light_direction = float3(0.f, -1.f, 1.f);
float sun_size = 2.f;
float sun_blur = 10.f;

// Cloud
float3 clouds_edge_color = float3(0.8, 0.8, 0.98);
float3 clouds_top_color = float3(1, 1, 1);
float3 clouds_middle_color = float3(0.92, 0.92, 0.98);
float3 clouds_bottom_color = float3(0.83, 0.83, 0.94);
float  clouds_speed = 2.f;
float  clouds_direction = 0.2f;
float  clouds_scale = 1.f;
float  clouds_cutoff = 0.3f;
float  clouds_fuzziness = 0.5f;
float  clouds_weight = 0.0f;
float  clouds_blur = 0.25f;

float overwritten_time = 0.f;

float g_fColorStrength = 1.f;

#define PI 3.14159265359

float cubic_lerp(float a, float b, float c, float d, float t)
{
    float p = d - c - (a - b);
    return t * t * t * p + t * t * (a - b - p) + t * (c - a) + b;
}

float ping_pong(float t)
{
    t -= floor(t * 0.5) * 2.0;
    return t < 1.0 ? t : 2.0 - t;
}

int hash(int seed, int x_primed, int y_primed)
{
    return (seed ^ x_primed ^ y_primed) * 0x27d4eb2d;
}

float val_coord(int seed, int x_primed, int y_primed)
{
    int h = hash(seed, x_primed, y_primed);
    h *= h;
    h ^= h << 19;
    return float(h) * (1.0 / 2147483648.0);
}

float single_value_cubic(int seed, float x, float y)
{
    int x1 = (int) floor(x);
    int y1 = (int) floor(y);

    float xs = x - float(x1);
    float ys = y - float(y1);

    x1 *= 501125321; // PRIME_X
    y1 *= 1136930381; // PRIME_Y
    int x0 = x1 - 501125321;
    int y0 = y1 - 1136930381;
    int x2 = x1 + 501125321;
    int y2 = y1 + 1136930381;
    int x3 = x1 + (501125321 << 1);
    int y3 = y1 + (1136930381 << 1);

    return cubic_lerp(
        cubic_lerp(val_coord(seed, x0, y0), val_coord(seed, x1, y0), val_coord(seed, x2, y0), val_coord(seed, x3, y0), xs),
        cubic_lerp(val_coord(seed, x0, y1), val_coord(seed, x1, y1), val_coord(seed, x2, y1), val_coord(seed, x3, y1), xs),
        cubic_lerp(val_coord(seed, x0, y2), val_coord(seed, x1, y2), val_coord(seed, x2, y2), val_coord(seed, x3, y2), xs),
        cubic_lerp(val_coord(seed, x0, y3), val_coord(seed, x1, y3), val_coord(seed, x2, y3), val_coord(seed, x3, y3), xs),
        ys) * (1.0 / (1.5 * 1.5));
}

float gen_fractal_ping_pong(float2 pos, int seed, float frequency)
{
    const float FRACTAL_BOUNDING = 1.0 / 1.75;
    const int OCTAVES = 5;
    const float PING_PONG_STRENGTH = 2.0;
    const float WEIGHTED_STRENGTH = 0.0;
    const float GAIN = 0.5;
    const float LACUNARITY = 2.0;

    float x = pos.x * frequency;
    float y = pos.y * frequency;
    float sum = 0.0;
    float amp = FRACTAL_BOUNDING;
    for (int i = 0; i < OCTAVES; i++)
    {
        float noise = ping_pong((single_value_cubic(seed++, x, y) + 1.0) * PING_PONG_STRENGTH);
        sum += (noise - 0.5) * 2.0 * amp;
        amp *= lerp(1.0, noise, WEIGHTED_STRENGTH);
        x *= LACUNARITY;
        y *= LACUNARITY;
        amp *= GAIN;
    }
    return sum * 0.5 + 0.5;
}

float sphere_intersect(float3 view_dir, float3 sphere_pos, float radius)
{
    float b = dot(-sphere_pos, view_dir);
    float c = dot(-sphere_pos, -sphere_pos) - radius * radius;
    float h = b * b - c;
    return h < 0.0 ? -1.0 : -b - sqrt(h);
}


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = vector(In.vPosition, 1.f);
    Out.vPosition.z = 1.f;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


float4 PS_MAIN_SKY(PS_IN In) : SV_TARGET0
{
    float4 vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = 1.f;
    vWorldPos.w = 1.f;

    vWorldPos = mul(vWorldPos, g_CamProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_CamViewMatrixInv);
    vWorldPos = vWorldPos / vWorldPos.w;
    
    float3 EYEDIR = normalize(vWorldPos.xyz - g_vCamPosition.xyz);
    
    float time = overwritten_time;
    
    // SKY
    float _eyedir_y = abs(sin(EYEDIR.y * PI * 0.5));

    // 기본 하늘 색상 설정
    float3 _sky_color = lerp(g_vBottomColor, g_vTopColor, _eyedir_y);
    _sky_color = lerp(_sky_color, float3(0.0, 0.0, 0.0), clamp((0.7 - clouds_cutoff) * clouds_weight, 0.0, 1.0));

    // 최종 하늘 색상
    float3 COLOR = _sky_color;

    // HORIZON
    float _horizon_amount = 0.0;
    if (EYEDIR.y < 0.0)
    {
        _horizon_amount = clamp(abs(EYEDIR.y) / horizon_blur, 0.0, 1.0);
        float3 _horizon_color = horizon_color;
        _horizon_color = lerp(_horizon_color, float3(0.0, 0.0, 0.0), (1.0 - clouds_cutoff) * clouds_weight * 0.7);
        COLOR = lerp(COLOR, _horizon_color, _horizon_amount);
    }

    // SUN
    float _sun_distance = 0.0;
    float3 lightDirection = normalize(light_direction);
   
    _sun_distance = distance(EYEDIR, lightDirection);
    float _sun_size = sun_size + cos(lightDirection.y * PI) * sun_size * 0.25;
    float _sun_amount = clamp((1.0 - _sun_distance / _sun_size) / sun_blur, 0.0, 1.0);
    if (_sun_amount > 0.0)
    {
        float3 _sun_color = sun_color;
        _sun_amount *= 1.0 - _horizon_amount;
        if (_sun_color.r > 1.0 || _sun_color.g > 1.0 || _sun_color.b > 1.0)
            _sun_color *= _sun_amount;
        COLOR = lerp(COLOR, _sun_color, _sun_amount);
    }
   
    // CLOUDS
    if (EYEDIR.y > 0.0)
    {
        float2 _sky_uv = EYEDIR.xz / sqrt(EYEDIR.y);
        
        float _clouds_speed = time * clouds_speed * 0.01;
        float _sin_x = sin(clouds_direction * PI * 2.0);
        float _cos_y = cos(clouds_direction * PI * 2.0);

        float2 _clouds_movement = float2(_sin_x, _cos_y) * _clouds_speed;

        float _noise_top = gen_fractal_ping_pong((_sky_uv + _clouds_movement) * clouds_scale, 0, 0.5);
        _clouds_movement = float2(_sin_x * 0.97, _cos_y * 1.07) * _clouds_speed * 0.89;

        float _noise_middle = gen_fractal_ping_pong((_sky_uv + _clouds_movement) * clouds_scale, 1, 0.75);
        _clouds_movement = float2(_sin_x * 1.01, _cos_y * 0.89) * _clouds_speed * 0.79;

        float _noise_bottom = gen_fractal_ping_pong((_sky_uv + _clouds_movement) * clouds_scale, 2, 1.0);

        _noise_bottom = smoothstep(clouds_cutoff, clouds_cutoff + clouds_fuzziness, _noise_bottom);
        _noise_middle = smoothstep(clouds_cutoff, clouds_cutoff + clouds_fuzziness, _noise_middle + _noise_bottom * 0.2) * 1.1;
        _noise_top = smoothstep(clouds_cutoff, clouds_cutoff + clouds_fuzziness, _noise_top + _noise_middle * 0.4) * 1.2;
        float _clouds_amount = clamp(_noise_top + _noise_middle + _noise_bottom, 0.0, 1.0);

        _clouds_amount *= clamp(abs(EYEDIR.y) / clouds_blur, 0.0, 1.0);

        float3 _clouds_color = lerp(float3(0.0, 0.0, 0.0), clouds_top_color, _noise_top);
        _clouds_color = lerp(_clouds_color, clouds_middle_color, _noise_middle);
        _clouds_color = lerp(_clouds_color, clouds_bottom_color, _noise_bottom);
        _clouds_color = lerp(clouds_edge_color, _clouds_color, _noise_top);
      //  _clouds_color = lerp(_clouds_color, saturate(sun_color), pow(1.0 - clamp(_sun_distance, 0.0, 1.0), 5.0));
        _clouds_color = lerp(_clouds_color, float3(0.0, 0.0, 0.0), clouds_weight * 0.9);
        COLOR = lerp(COLOR, _clouds_color, _clouds_amount);
    }

    return float4(COLOR * g_fColorStrength, 1.0);
 
}

technique11 DefaultTechnique
{
    pass Sky //0
    {
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Sky, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SKY();
        ComputeShader = NULL;
    }
}