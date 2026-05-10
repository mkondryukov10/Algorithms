#include <stdio.h>
#include <math.h>

#define MAXN 100
#define EPS 1e-9

void swap_rows(double a[MAXN][MAXN], int n, int r1, int r2) {
    for (int j = 0; j < n; j++) {
        double temp = a[r1][j];
        a[r1][j] = a[r2][j];
        a[r2][j] = temp;
    }
}

int main() {
    int n;
    double a[MAXN][MAXN];

    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%lf", &a[i][j]);
        }
    }

    double det = 1.0;
    int sign = 1;
    int zero_det = 0;

    for (int k = 0; k < n && zero_det == 0; k++) {
        if (fabs(a[k][k]) < EPS) {
            int found = -1;
            for (int i = k + 1; i < n; i++) {
                if (fabs(a[i][k]) > EPS && found == -1) {
                    found = i;
                }
            }
            if (found == -1) {
                det = 0.0;
                zero_det = 1;
            } else {
                swap_rows(a, n, k, found);
                sign = -sign;
            }
        }
        if (zero_det == 0) {
            for (int i = k + 1; i < n; i++) {
                double c = a[i][k] / a[k][k];

                for (int j = k; j < n; j++) {
                    a[i][j] = a[i][j] - c * a[k][j];
                }
            }
        }
    }
    if (zero_det == 0) {
        for (int i = 0; i < n; i++) {
            det = det * a[i][i];
        }
        det = det * sign;
    }
    printf("%.10lf\n", det);
    return 0;
}