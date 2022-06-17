// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};
cbuffer NoiseMapBuffer : register (b1)
{
    float frequency;
    float amplitude;
    float offset;
    float excess1;
}
struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};
int FastFloor(double x)
{
    return x > 0 ? (int)x : (int)x - 1;
}
double Dot(int g[3], double x, double y, double z)
{
    return g[0] * x + g[1] * y + g[2] * z;
}
double Mix(double a, double b, double t)
{
    return (1 - t) * a + t * b;
}
double Fade(double t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}
double Dot(int g[3], double x, double y) {
    return g[0] * x + g[1] * y;
}
#define NOISE_SIMPLEX_1_DIV_289 0.00346020761245674740484429065744f
float mod289(float x) {
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float2 mod289(float2 x) {
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float3 mod289(float3 x) {
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float4 mod289(float4 x) {
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}


// ( x*34.0 + 1.0 )*x =
// x*x*34.0 + x
float permute(float x) {
    return mod289(
        x * x * 34.0 + x
    );
}

float3 permute(float3 x) {
    return mod289(
        x * x * 34.0 + x
    );
}

float4 permute(float4 x) {
    return mod289(
        x * x * 34.0 + x
    );
}



float taylorInvSqrt(float r) {
    return 1.79284291400159 - 0.85373472095314 * r;
}

float4 taylorInvSqrt(float4 r) {
    return 1.79284291400159 - 0.85373472095314 * r;
}
float snoise(float2 v)
{
    const float4 C = float4(
        0.211324865405187, // (3.0-sqrt(3.0))/6.0
        0.366025403784439, // 0.5*(sqrt(3.0)-1.0)
        -0.577350269189626, // -1.0 + 2.0 * C.x
        0.024390243902439  // 1.0 / 41.0
        );

    // First corner
    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);

    // Other corners
        // float2 i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
        // Lex-DRL: afaik, step() in GPU is faster than if(), so:
        // step(x, y) = x <= y

        // Optimization hint Dolkar
        //int xLessEqual = step(x0.x, x0.y); // x <= y ?
        //int2 i1 =
        //    int2(1, 0) * (1 - xLessEqual) // x > y
        //    + int2(0, 1) * xLessEqual // x <= y
        //;
        //float4 x12 = x0.xyxy + C.xxzz;
        //x12.xy -= i1;

    float4 x12 = x0.xyxy + C.xxzz;
    int2 i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    x12.xy -= i1;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    float3 p = permute(
        permute(
            i.y + float3(0.0, i1.y, 1.0)
        ) + i.x + float3(0.0, i1.x, 1.0)
    );

    float3 m = max(
        0.5 - float3(
            dot(x0, x0),
            dot(x12.xy, x12.xy),
            dot(x12.zw, x12.zw)
            ),
        0.0
    );
    m = m * m;
    m = m * m;

    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

    // Compute final noise value at P
    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}
double Noise(double x, double y, double z) {

    int grad3[12][3] = { {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
    {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
    {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1} };
    int p[256] = { 151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
    int perm[512];


    for (int i = 0; i < 512; i++)
    {
        perm[i] = p[i & 255];
    }


    int X = FastFloor(x);
    int Y = FastFloor(y);
    int Z = FastFloor(z);

    // Get relative xyz coordinates of point within that cell
    x = x - X;
    y = y - Y;
    z = z - Z;

    // Wrap the integer cells at 255 (smaller integer period can be introduced here)
    X = X & 255;
    Y = Y & 255;
    Z = Z & 255;
    int gi000 = perm[X + perm[Y + perm[Z]]] % 12;
    int gi001 = perm[X + perm[Y + perm[Z + 1]]] % 12;
    int gi010 = perm[X + perm[Y + 1 + perm[Z]]] % 12;
    int gi011 = perm[X + perm[Y + 1 + perm[Z + 1]]] % 12;
    int gi100 = perm[X + 1 + perm[Y + perm[Z]]] % 12;
    int gi101 = perm[X + 1 + perm[Y + perm[Z + 1]]] % 12;
    int gi110 = perm[X + 1 + perm[Y + 1 + perm[Z]]] % 12;
    int gi111 = perm[X + 1 + perm[Y + 1 + perm[Z + 1]]] % 12;

    // Calculate noise contributions from each of the eight corners
    double n000 = Dot(grad3[gi000], x, y, z);
    double n100 = Dot(grad3[gi100], x - 1, y, z);
    double n010 = Dot(grad3[gi010], x, y - 1, z);
    double n110 = Dot(grad3[gi110], x - 1, y - 1, z);
    double n001 = Dot(grad3[gi001], x, y, z - 1);
    double n101 = Dot(grad3[gi101], x - 1, y, z - 1);
    double n011 = Dot(grad3[gi011], x, y - 1, z - 1);
    double n111 = Dot(grad3[gi111], x - 1, y - 1, z - 1);
    // Compute the fade curve value for each of x, y, z
    double u = Fade(x);
    double v = Fade(y);
    double w = Fade(z);
    // Interpolate along x the contributions from each of the corners
    double nx00 = Mix(n000, n100, u);
    double nx01 = Mix(n001, n101, u);
    double nx10 = Mix(n010, n110, u);
    double nx11 = Mix(n011, n111, u);
    // Interpolate the four results along y
    double nxy0 = Mix(nx00, nx10, v);
    double nxy1 = Mix(nx01, nx11, v);
    // Interpolate the two last results along z
    double nxyz = Mix(nxy0, nxy1, w);

    return nxyz;
}
double SimplexNoise(double x, double y) {

    int grad3[12][3] = { {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
    {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
    {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1} };
    int p[256] = { 151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
    int perm[512];


    for (int i = 0; i < 512; i++)
    {
        perm[i] = p[i & 255];
    }


    double n0, n1, n2; // Noise contributions from the three corners
    // Skew the input space to determine which simplex cell we're in
    double F2 = 0.5 * (sqrt(3.0) - 1.0);
    double s = (x + y) * F2; // Hairy factor for 2D
    i = FastFloor(x + s);
    int j = FastFloor(y + s);
    double G2 = (3.0 - sqrt(3.0)) / 6.0;
    double t = (i + j) * G2;
    double X0 = i - t; // Unskew the cell origin back to (x,y) space
    double Y0 = j - t;
    double x0 = x - X0; // The x,y distances from the cell origin
    double y0 = y - Y0;
    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    else { i1 = 0; j1 = 1; } // upper triangle, YX order: (0,0)->(0,1)->(1,1)
    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6
    double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    double y1 = y0 - j1 + G2;
    double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
    double y2 = y0 - 1.0 + 2.0 * G2;
    // Work out the hashed gradient indices of the three simplex corners
    int ii = i & 255;
    int jj = j & 255;
    int gi0 = perm[ii + perm[jj]] % 12;
    int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
    int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;
    // Calculate the contribution from the three corners
    double t0 = 0.5 - x0 * x0 - y0 * y0;
    if (t0 < 0) n0 = 0.0;
    else {
        t0 *= t0;
        n0 = t0 * t0 * Dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
    }
    double t1 = 0.5 - x1 * x1 - y1 * y1;
    if (t1 < 0) n1 = 0.0;
    else {
        t1 *= t1;
        n1 = t1 * t1 * Dot(grad3[gi1], x1, y1);
    }
    double t2 = 0.5 - x2 * x2 - y2 * y2;
    if (t2 < 0) n2 = 0.0;
    else {
        t2 *= t2;
        n2 = t2 * t2 * Dot(grad3[gi2], x2, y2);
    }
    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 70.0 * (n0 + n1 + n2);
}
float4 main(InputType input) : SV_TARGET
{
	float4	textureColor;
    float3	lightDir;
    float	lightIntensity;
    float4	color;

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
	color = saturate(color);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex/5);
    float2 pos = float2((input.position3D.z * frequency) + offset, (input.position3D.x * frequency) + offset);
    //float noise = snoise(pos)*amplitude;
    float noise = textureColor;
    float4 noiseMap = float4(lerp(float3(0, 0, 1), float3(1, 0, 0), noise),1);
    float4 tempMap = float4(lerp(float3(1, 1, 1), float3(0, 0, 1), noise), 1);
 
    return (padding == 0 ? textureColor  : tempMap);
   
}

