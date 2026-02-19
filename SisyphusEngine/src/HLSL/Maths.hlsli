#define PI 3.14159265359


// 일반적인 수학
///////////////////////////////////////////////////////////////

float3 mod289(float3 x)
{
    return x - floor(x * (1.0f / 289.0f)) * 289.0f;
} // mod289


float2 mod289(float2 x)
{
    return x - floor(x * (1.0f / 289.0f)) * 289.0f;
} // mod289


float3 permute(float3 x)
{
    return mod289(((x * 34.0f) + 1.0f) * x);
} // permute


float rand(float3 p)
{
    return frac(sin(dot(p, float3(12.9898, 78.233, 45.164))) * 43758.5453123);
} // rand


float hash11(float p)
{
    p = frac(p * 0.1031);
    p *= p + 33.33;
    p *= p + p;
    return frac(p);
} // hash11


float hash21(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.x + p3.y) * p3.z);
} // hash21


float hash21(float2 p, float2 scape, float offset)
{
    float h = dot(p, scape);
    return frac(sin(h) * offset);
} // hash21


// 오일러 각을 이용한 3x3 회전 행렬 생성 (카메라/객체 회전용)
// 인자: ang (pitch, yaw, roll)
float3x3 fromEuler(float3 ang)
{
    float2 a1 = float2(sin(ang.x), cos(ang.x));
    float2 a2 = float2(sin(ang.y), cos(ang.y));
    float2 a3 = float2(sin(ang.z), cos(ang.z));
    
    float3x3 m;
    m[0] = float3(a1.y * a3.y + a1.x * a2.x * a3.x, a1.y * a2.x * a3.x + a3.y * a1.x, -a2.y * a3.x);
    m[1] = float3(-a2.y * a1.x, a1.y * a2.y, a2.x);
    m[2] = float3(a3.y * a1.x * a2.x + a1.y * a3.x, a1.x * a3.x - a1.y * a3.y * a2.x, a2.y * a3.y);
    return m;
} // fromEuler


// SDF(Signed DistanceFunction, or Signed Distance Field)
////////////////////////////////////////////////////////////

// 구 SDF
float sdSphere(float3 p, float radius)
{
    return length(p) - radius;
} // sdSphere


// 캡슐 SDF
float sdCapsule(float3 p, float3 a, float3 b, float r)
{
    float3 ab = b - a;
    float3 ap = p - a;
    float t = dot(ab, ap) / dot(ab, ab);
    t = clamp(t, 0.0f, 1.0f);
    float3 c = a + t * ab;
    return length(p - c) - r;
} // sdCapsule


// 도넛 SDF
float sdTorus(float3 p, float2 r)
{
    float x = length(p.xz) - r.x;
    return length(float2(x, p.y)) - r.y;
} // sdTorus


// 십자형 SDF
float sdCross(float3 p, float s)
{
    float da = max(abs(p.x), abs(p.y));
    float db = max(abs(p.y), abs(p.z));
    float dc = max(abs(p.z), abs(p.x));
    return min(da, min(db, dc)) - s;
} // sdCross

// etc

// 2D 회전 행렬 생성
float2x2 rotate2D(float a)
{
    float s = sin(a);
    float c = cos(a);
    return float2x2(c, -s, s, c);
} // rotate2D


// 부드러운 스텝 함수
float nextStep(float t, float len, float smo)
{
    float tt = (t + smo) % len;
    float stp = floor((t + smo) / len) - 1.0f;
    return smoothstep(0.0f, smo, tt) + stp;
} // nextStep

