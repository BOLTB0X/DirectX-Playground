
// Bayer Dithering (8x8) - 밴딩 현상 제거용
float hashBasedDither2D(float2 a)
{
    a = floor(a);
    return frac(dot(a, float2(0.5, a.y * 0.75)));
} // hashBasedDither2D


#define DITHER2(a) hashBasedDither2D(a)
#define DITHER4(a) (DITHER2(.5*(a))*.25 + DITHER2(a))
#define DITHER8(a) (DITHER4(.5*(a))*.25 + DITHER2(a))
#define DITHER16(a) (DITHER8(.5*(a))*.25 + DITHER2(a))