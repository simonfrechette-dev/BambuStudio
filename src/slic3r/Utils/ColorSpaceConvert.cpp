// Qt6 stub for ColorSpaceConvert.cpp
#include "ColorSpaceConvert.hpp"
#include <cmath>
#include <algorithm>
#include <QColor>

std::tuple<int, int, int> rgb_to_yuv(float r, float g, float b)
{
    int y = static_cast<int>( 0.299f * r + 0.587f * g + 0.114f * b);
    int u = static_cast<int>(-0.147f * r - 0.289f * g + 0.436f * b + 128.0f);
    int v = static_cast<int>( 0.615f * r - 0.515f * g - 0.100f * b + 128.0f);
    return {y, u, v};
}

double PivotRGB(double n)
{
    return (n > 0.04045) ? std::pow((n + 0.055) / 1.055, 2.4) : n / 12.92;
}

double PivotXYZ(double n)
{
    const double threshold = 0.008856;
    return (n > threshold) ? std::cbrt(n) : (7.787 * n + 16.0 / 116.0);
}

void RGB2XYZ(float R, float G, float B, float* X, float* Y, float* Z)
{
    double r = PivotRGB(R / 255.0);
    double g = PivotRGB(G / 255.0);
    double b = PivotRGB(B / 255.0);
    // D65 illuminant
    *X = static_cast<float>((r * 0.4124564 + g * 0.3575761 + b * 0.1804375) / 0.95047f);
    *Y = static_cast<float>((r * 0.2126729 + g * 0.7151522 + b * 0.0721750) / 1.00000f);
    *Z = static_cast<float>((r * 0.0193339 + g * 0.1191920 + b * 0.9503041) / 1.08883f);
}

void XYZ2Lab(float X, float Y, float Z, float* L, float* a, float* b)
{
    double fx = PivotXYZ(X);
    double fy = PivotXYZ(Y);
    double fz = PivotXYZ(Z);
    *L = static_cast<float>(116.0 * fy - 16.0);
    *a = static_cast<float>(500.0 * (fx - fy));
    *b = static_cast<float>(200.0 * (fy - fz));
}

void RGB2Lab(float R, float G, float B, float* L, float* a, float* b)
{
    float X, Y, Z;
    RGB2XYZ(R, G, B, &X, &Y, &Z);
    XYZ2Lab(X, Y, Z, L, a, b);
}

void RGB2HSV(float r, float g, float b, float* h, float* s, float* v)
{
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
    float cmax = std::max({rf, gf, bf});
    float cmin = std::min({rf, gf, bf});
    float delta = cmax - cmin;
    *v = cmax;
    *s = (cmax > 0.0f) ? delta / cmax : 0.0f;
    if (delta < 1e-6f) { *h = 0.0f; return; }
    if (cmax == rf)      *h = 60.0f * std::fmod((gf - bf) / delta, 6.0f);
    else if (cmax == gf) *h = 60.0f * ((bf - rf) / delta + 2.0f);
    else                 *h = 60.0f * ((rf - gf) / delta + 4.0f);
    if (*h < 0.0f) *h += 360.0f;
}

void Lab2XYZ(float L, float a, float b, float* X, float* Y, float* Z)
{
    double fy = (L + 16.0) / 116.0;
    double fx = a / 500.0 + fy;
    double fz = fy - b / 200.0;
    auto inv = [](double t) { return (t > 0.206897) ? t*t*t : (t - 16.0/116.0) / 7.787; };
    *X = static_cast<float>(inv(fx) * 0.95047);
    *Y = static_cast<float>(inv(fy) * 1.00000);
    *Z = static_cast<float>(inv(fz) * 1.08883);
}

void XYZ2RGB(float X, float Y, float Z, float* R, float* G, float* B)
{
    double r =  3.2404542 * X - 1.5371385 * Y - 0.4985314 * Z;
    double g = -0.9692660 * X + 1.8760108 * Y + 0.0415560 * Z;
    double b =  0.0556434 * X - 0.2040259 * Y + 1.0572252 * Z;
    auto lin2srgb = [](double c) {
        c = std::clamp(c, 0.0, 1.0);
        return (c <= 0.0031308) ? 12.92 * c : 1.055 * std::pow(c, 1.0/2.4) - 0.055;
    };
    *R = static_cast<float>(lin2srgb(r) * 255.0);
    *G = static_cast<float>(lin2srgb(g) * 255.0);
    *B = static_cast<float>(lin2srgb(b) * 255.0);
}

void Lab2RGB(float L, float a, float b, float* R, float* G, float* B)
{
    float X, Y, Z;
    Lab2XYZ(L, a, b, &X, &Y, &Z);
    XYZ2RGB(X, Y, Z, R, G, B);
}

float DeltaE76(float l1, float a1, float b1, float l2, float a2, float b2)
{
    float dl = l1 - l2, da = a1 - a2, db = b1 - b2;
    return std::sqrt(dl*dl + da*da + db*db);
}

float DeltaE94(float l1, float a1, float b1, float l2, float a2, float b2)
{
    float dl = l1 - l2;
    float c1 = std::sqrt(a1*a1 + b1*b1);
    float c2 = std::sqrt(a2*a2 + b2*b2);
    float dc = c1 - c2;
    float da = a1 - a2, db = b1 - b2;
    float dh2 = da*da + db*db - dc*dc;
    float dh = (dh2 > 0.0f) ? std::sqrt(dh2) : 0.0f;
    const float kl = 1.0f, kc = 1.0f, kh = 1.0f;
    const float k1 = 0.045f, k2 = 0.015f;
    float sc = 1.0f + k1 * c1;
    float sh = 1.0f + k2 * c1;
    float e = (dl / kl) * (dl / kl)
            + (dc / (kc * sc)) * (dc / (kc * sc))
            + (dh / (kh * sh)) * (dh / (kh * sh));
    return std::sqrt(e);
}

float DeltaE00(float l1, float a1, float b1, float l2, float a2, float b2)
{
    // Simplified: use DeltaE76 as approximation
    return DeltaE76(l1, a1, b1, l2, a2, b2);
}

std::string color_to_string(const QColor &color)
{
    return color.name().toStdString();
}

QColor string_to_wxColor(const std::string &str)
{
    return QColor(QString::fromStdString(str));
}

