#pragma once
namespace nsYMEngine
{
	namespace nsGeometries
	{
        namespace
        {
            constexpr float kTOL = 1.0e-10f;
            constexpr int kMax = 100;
        }

        template<int n>
        int EigenJacobiMethod(float a[], float x[])
        {
            int    i, j, k, m, count, status;
            float amax, amax0, theta, co, si, co2, si2, cosi, pi = 4.0f * atan(1.0f);
            float aii, aij, ajj, aik, ajk;

            //   ‰Šú’lİ’è
            for (i = 0; i < n; i++) {
                for (j = 0; j < n; j++) {
                    if (i == j)  x[n * i + j] = 1.0f; else  x[n * i + j] = 0.0f;
                }
            }

            //   ”½•œŒvZ
            count = 0;  status = 9;
            while (count <= kMax) {
                //  ”ñ‘ÎŠp—v‘f‚ÌÅ‘å’l‚ğ’Tõ
                amax = 0.0;
                for (k = 0; k < n - 1; k++) {
                    for (m = k + 1; m < n; m++) {
                        amax0 = fabs(a[n * k + m]);
                        if (amax0 > amax) { i = k;  j = m, amax = amax0; }
                    }
                }
                //  û‘©”»’è
                if (amax <= kTOL) { status = 0;   break; }
                else {
                    aii = a[n * i + i];   aij = a[n * i + j];   ajj = a[n * j + j];
                    //   ‰ñ“]Šp“xŒvZ
                    if (fabs(aii - ajj) < kTOL) {
                        theta = 0.25f * pi * aij / fabs(aij);
                    }
                    else {
                        theta = 0.5f * atan(2.0f * aij / (aii - ajj));
                    }
                    co = cos(theta); si = sin(theta); co2 = co * co; si2 = si * si; cosi = co * si;

                    //   ‘Š—•ÏŠ·s—ñ
                    a[n * i + i] = co2 * aii + 2.0f * cosi * aij + si2 * ajj;
                    a[n * j + j] = si2 * aii - 2.0f * cosi * aij + co2 * ajj;
                    a[n * i + j] = 0.0f;    a[n * j + i] = 0.0f;
                    for (k = 0; k < n; k++) {
                        if (k != i && k != j) {
                            aik = a[n * k + i];            ajk = a[n * k + j];
                            a[n * k + i] = co * aik + si * ajk;  a[n * i + k] = a[n * k + i];
                            a[n * k + j] = -si * aik + co * ajk;  a[n * j + k] = a[n * k + j];
                        }
                    }

                    //   ŒÅ—LƒxƒNƒgƒ‹
                    for (k = 0; k < n; k++) {
                        aik = x[n * k + i];   ajk = x[n * k + j];
                        x[n * k + i] = co * aik + si * ajk;
                        x[n * k + j] = -si * aik + co * ajk;
                    }
                    count++;
                }
            }
            return status;
        }
	}
}