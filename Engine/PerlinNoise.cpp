#include "pch.h"
#include "PerlinNoise.h"




PerlinNoise::~PerlinNoise()
{
}

void PerlinNoise::Initialize()
{
    for (int i = 0; i < 512; i++) perm[i] = p[i & 255];
}



int PerlinNoise::FastFloor(double x) {
    return x > 0 ? (int)x : (int)x - 1;
}
double PerlinNoise::Dot(int g[], double x, double y, double z) {
    return g[0] * x + g[1] * y + g[2] * z;
}
double PerlinNoise::Dot(int g[], double x, double y)
{
    return g[0] * x + g[1] * y;
}
double PerlinNoise::Mix(double a, double b, double t) {
    return (1 - t) * a + t * b;
}
double PerlinNoise::Fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}
double PerlinNoise::Noise(double x, double y, double z) {
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

    // The gradients of each corner are now:
    // g000 = grad3[gi000];
    // g001 = grad3[gi001];
    // g010 = grad3[gi010];
    // g011 = grad3[gi011];
    // g100 = grad3[gi100];
    // g101 = grad3[gi101];
    // g110 = grad3[gi110];
    // g111 = grad3[gi111];
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

double PerlinNoise::SimplexNoise(double x, double y)
{
    double n0, n1, n2; // Noise contributions from the three corners
// Skew the input space to determine which simplex cell we're in
    double F2 = 0.5 * (sqrt(3.0) - 1.0);
    double s = (x + y) * F2; // Hairy factor for 2D
    int i = FastFloor(x + s);
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
}









