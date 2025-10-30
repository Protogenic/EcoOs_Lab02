#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include <stdio.h>

static void print_separator() {
    printf("===============================================================================\n");
}

static void print_header(const char* title) {
    printf("\n");
    print_separator();
    printf("| %-75s |\n", title);
    print_separator();
}

static void print_array(const char* prefix, const int32_t* arr, int size) {
    int i;
    printf("%s[", prefix);
    for (i = 0; i < size; i++) {
        printf("%d", arr[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

static void print_array_long(const char* prefix, const int64_t* arr, int size) {
    int i;
    printf("%s[", prefix);
    for (i = 0; i < size; i++) {
        printf("%lld", arr[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

static void print_array_float(const char* prefix, const float* arr, int size) {
    int i;
    printf("%s[", prefix);
    for (i = 0; i < size; i++) {
        printf("%.2f", arr[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

static void print_array_double(const char* prefix, const double* arr, int size) {
    int i;
    printf("%s[", prefix);
    for (i = 0; i < size; i++) {
        printf("%.2lf", arr[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

static void print_array_longdouble(const char* prefix, const long double* arr, int size) {
    int i;
    printf("%s[", prefix);
    for (i = 0; i < size; i++) {
        printf("%.2Lf", arr[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

static int is_sorted_int32(const int32_t* arr, int size) {
    int i;
    for (i = 1; i < size; i++) {
        if (arr[i-1] > arr[i]) return 0;
    }
    return 1;
}

static int is_sorted_int64(const int64_t* arr, int size) {
    int i;
    for (i = 1; i < size; i++) {
        if (arr[i-1] > arr[i]) return 0;
    }
    return 1;
}

static int is_sorted_float(const float* arr, int size) {
    int i;
    for (i = 1; i < size; i++) {
        if (arr[i-1] > arr[i]) return 0;
    }
    return 1;
}

static int is_sorted_double(const double* arr, int size) {
    int i;
    for (i = 1; i < size; i++) {
        if (arr[i-1] > arr[i]) return 0;
    }
    return 1;
}

static int is_sorted_longdouble(const long double* arr, int size) {
    int i;
    for (i = 1; i < size; i++) {
        if (arr[i-1] > arr[i]) return 0;
    }
    return 1;
}

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoLab1* pIEcoLab1 = 0;


    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 || pISys == 0) {
            printf("ERROR: Failed to create system interface.\n");
            goto Release;
        }
    }

    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        printf("ERROR: Failed to get interface bus.\n");
        goto Release;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0,
        &IID_IEcoMemoryAllocator1, (void**)&pIMem);
    if (result != 0 || pIMem == 0) {
        printf("ERROR: Failed to get memory manager.\n");
        goto Release;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0,
        &IID_IEcoLab1, (void**)&pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        printf("ERROR: Failed to get IEcoLab1 interface.\n");
        goto Release;
    }
    /* Demonstrate inclusion: query calculator interfaces from CEcoLab1 */
    {
        IEcoCalculatorX* pIX = 0;
        IEcoCalculatorY* pIY = 0;
        int16_t rqx = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pIX);
        int16_t rqy = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**)&pIY);
        if (rqx == 0 && pIX != 0) {
            int32_t s = pIX->pVTbl->Addition(pIX, 3, 5);
            printf("CalculatorX Addition(3,5) = %d\n", s);
            pIX->pVTbl->Release(pIX);
        } else {
            printf("WARNING: IEcoCalculatorX not available.\n");
        }
        if (rqy == 0 && pIY != 0) {
            int32_t m = pIY->pVTbl->Multiplication(pIY, 4, 7);
            printf("CalculatorY Multiplication(4,7) = %d\n", m);
            pIY->pVTbl->Release(pIY);
        } else {
            printf("WARNING: IEcoCalculatorY not available.\n");
        }
    }

    print_header("TESTING COMB SORT FOR int32_t");

    {
        int32_t arr[8] = {64, 34, 25, 12, 22, 11, 90, 5};
        print_array("Test 1 - Before sorting: ", arr, 8);
        pIEcoLab1->pVTbl->csort_int(pIEcoLab1, arr, 8);
        print_array("Test 1 - After sorting:  ", arr, 8);
        printf("Result: %s\n\n", is_sorted_int32(arr, 8) ? "SUCCESS" : "FAILED");
    }

    {
        int32_t arr[6] = {1, 2, 3, 4, 5, 6};
        print_array("Test 2 - Before sorting: ", arr, 6);
        pIEcoLab1->pVTbl->csort_int(pIEcoLab1, arr, 6);
        print_array("Test 2 - After sorting:  ", arr, 6);
        printf("Result: %s\n\n", is_sorted_int32(arr, 6) ? "SUCCESS" : "FAILED");
    }

    {
        int32_t arr[7] = {7, 6, 5, 4, 3, 2, 1};
        print_array("Test 3 - Before sorting: ", arr, 7);
        pIEcoLab1->pVTbl->csort_int(pIEcoLab1, arr, 7);
        print_array("Test 3 - After sorting:  ", arr, 7);
        printf("Result: %s\n\n", is_sorted_int32(arr, 7) ? "SUCCESS" : "FAILED");
    }

    {
        int32_t arr[5] = {5, 5, 5, 5, 5};
        print_array("Test 4 - Before sorting: ", arr, 5);
        pIEcoLab1->pVTbl->csort_int(pIEcoLab1, arr, 5);
        print_array("Test 4 - After sorting:  ", arr, 5);
        printf("Result: %s\n\n", is_sorted_int32(arr, 5) ? "SUCCESS" : "FAILED");
    }

    {
        int32_t arr[9] = {-5, 8, -12, 3, -7, 15, -2, 0, -1};
        print_array("Test 5 - Before sorting: ", arr, 9);
        pIEcoLab1->pVTbl->csort_int(pIEcoLab1, arr, 9);
        print_array("Test 5 - After sorting:  ", arr, 9);
        printf("Result: %s\n\n", is_sorted_int32(arr, 9) ? "SUCCESS" : "FAILED");
    }

    print_header("TESTING COMB SORT FOR int64_t");

    {
        int64_t arr[6] = {9223372036854775807LL, -9223372036854775808LL, 1000000000LL, -1000000000LL, 0LL, 42LL};
        print_array_long("Test 1 - Before sorting: ", arr, 6);
        pIEcoLab1->pVTbl->csort_long(pIEcoLab1, arr, 6);
        print_array_long("Test 1 - After sorting:  ", arr, 6);
        printf("Result: %s\n\n", is_sorted_int64(arr, 6) ? "SUCCESS" : "FAILED");
    }

    {
        int64_t arr[7] = {5000000000LL, 2000000000LL, 8000000000LL, 1000000000LL, 9000000000LL};
        print_array_long("Test 2 - Before sorting: ", arr, 5);
        pIEcoLab1->pVTbl->csort_long(pIEcoLab1, arr, 5);
        print_array_long("Test 2 - After sorting:  ", arr, 5);
        printf("Result: %s\n\n", is_sorted_int64(arr, 5) ? "SUCCESS" : "FAILED");
    }

    {
        int64_t arr[4] = {1000LL, 2000LL, 3000LL, 4000LL};
        print_array_long("Test 3 - Before sorting: ", arr, 4);
        pIEcoLab1->pVTbl->csort_long(pIEcoLab1, arr, 4);
        print_array_long("Test 3 - After sorting:  ", arr, 4);
        printf("Result: %s\n\n", is_sorted_int64(arr, 4) ? "SUCCESS" : "FAILED");
    }

    {
        int64_t arr[6] = {7777777777LL, 7777777777LL, 7777777777LL, 7777777777LL, 7777777777LL, 7777777777LL};
        print_array_long("Test 4 - Before sorting: ", arr, 6);
        pIEcoLab1->pVTbl->csort_long(pIEcoLab1, arr, 6);
        print_array_long("Test 4 - After sorting:  ", arr, 6);
        printf("Result: %s\n\n", is_sorted_int64(arr, 6) ? "SUCCESS" : "FAILED");
    }

    {
        int64_t arr[8] = {0LL, -1LL, 1LL, -2LL, 2LL, -3LL, 3LL, 0LL};
        print_array_long("Test 5 - Before sorting: ", arr, 8);
        pIEcoLab1->pVTbl->csort_long(pIEcoLab1, arr, 8);
        print_array_long("Test 5 - After sorting:  ", arr, 8);
        printf("Result: %s\n\n", is_sorted_int64(arr, 8) ? "SUCCESS" : "FAILED");
    }

    print_header("TESTING COMB SORT FOR float");

    {
        float arr[7] = {3.14f, 1.41f, 2.71f, 1.73f, 0.57f, 4.67f, 1.61f};
        print_array_float("Test 1 - Before sorting: ", arr, 7);
        pIEcoLab1->pVTbl->csort_float(pIEcoLab1, arr, 7);
        print_array_float("Test 1 - After sorting:  ", arr, 7);
        printf("Result: %s\n\n", is_sorted_float(arr, 7) ? "SUCCESS" : "FAILED");
    }

    {
        float arr[6] = {-3.5f, 2.1f, -1.8f, 4.2f, -0.5f, 1.9f};
        print_array_float("Test 2 - Before sorting: ", arr, 6);
        pIEcoLab1->pVTbl->csort_float(pIEcoLab1, arr, 6);
        print_array_float("Test 2 - After sorting:  ", arr, 6);
        printf("Result: %s\n\n", is_sorted_float(arr, 6) ? "SUCCESS" : "FAILED");
    }

    {
        float arr[5] = {2.5f, 2.5f, 2.5f, 2.5f, 2.5f};
        print_array_float("Test 3 - Before sorting: ", arr, 5);
        pIEcoLab1->pVTbl->csort_float(pIEcoLab1, arr, 5);
        print_array_float("Test 3 - After sorting:  ", arr, 5);
        printf("Result: %s\n\n", is_sorted_float(arr, 5) ? "SUCCESS" : "FAILED");
    }
    {
        float arr[6] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
        print_array_float("Test 4 - Before sorting: ", arr, 6);
        pIEcoLab1->pVTbl->csort_float(pIEcoLab1, arr, 6);
        print_array_float("Test 4 - After sorting:  ", arr, 6);
        printf("Result: %s\n\n", is_sorted_float(arr, 6) ? "SUCCESS" : "FAILED");
    }

    {
        float arr[8] = {0.001f, 0.0001f, 0.01f, 0.1f, 0.00001f, 0.000001f, 0.0000001f, 0.00000001f};
        print_array_float("Test 5 - Before sorting: ", arr, 8);
        pIEcoLab1->pVTbl->csort_float(pIEcoLab1, arr, 8);
        print_array_float("Test 5 - After sorting:  ", arr, 8);
        printf("Result: %s\n\n", is_sorted_float(arr, 8) ? "SUCCESS" : "FAILED");
    }

    print_header("TESTING COMB SORT FOR double");

    {
        double arr[7] = {3.14159265359, 2.71828182846, 1.41421356237, 1.73205080757, 0.57721566490, 2.23606797750, 1.61803398875};
        print_array_double("Test 1 - Before sorting: ", arr, 7);
        pIEcoLab1->pVTbl->csort_double(pIEcoLab1, arr, 7);
        print_array_double("Test 1 - After sorting:  ", arr, 7);
        printf("Result: %s\n\n", is_sorted_double(arr, 7) ? "SUCCESS" : "FAILED");
    }

    {
        double arr[6] = {1234567.89, 9876543.21, 5555555.55, 1111111.11, 9999999.99, 3333333.33};
        print_array_double("Test 2 - Before sorting: ", arr, 6);
        pIEcoLab1->pVTbl->csort_double(pIEcoLab1, arr, 6);
        print_array_double("Test 2 - After sorting:  ", arr, 6);
        printf("Result: %s\n\n", is_sorted_double(arr, 6) ? "SUCCESS" : "FAILED");
    }

    {
        double arr[5] = {-9.87654, -1.23456, -7.65432, -3.45678, -5.67890};
        print_array_double("Test 3 - Before sorting: ", arr, 5);
        pIEcoLab1->pVTbl->csort_double(pIEcoLab1, arr, 5);
        print_array_double("Test 3 - After sorting:  ", arr, 5);
        printf("Result: %s\n\n", is_sorted_double(arr, 5) ? "SUCCESS" : "FAILED");
    }

    {
        double arr[6] = {42.42, 42.42, 42.42, 42.42, 42.42, 42.42};
        print_array_double("Test 4 - Before sorting: ", arr, 6);
        pIEcoLab1->pVTbl->csort_double(pIEcoLab1, arr, 6);
        print_array_double("Test 4 - After sorting:  ", arr, 6);
        printf("Result: %s\n\n", is_sorted_double(arr, 6) ? "SUCCESS" : "FAILED");
    }
    {
        double arr[8] = {0.0, -0.1, 0.1, -0.2, 0.2, -0.3, 0.3, 0.0};
        print_array_double("Test 5 - Before sorting: ", arr, 8);
        pIEcoLab1->pVTbl->csort_double(pIEcoLab1, arr, 8);
        print_array_double("Test 5 - After sorting:  ", arr, 8);
        printf("Result: %s\n\n", is_sorted_double(arr, 8) ? "SUCCESS" : "FAILED");
    }

    print_header("TESTING COMB SORT FOR long double");

    {
        long double arr[6] = {3.1415926535897932384626433832795L, 2.7182818284590452353602874713527L, 1.4142135623730950488016887242097L, 1.7320508075688772935274463415059L, 0.5772156649015328606065120900824L, 2.2360679774997896964091736687313L};
        print_array_longdouble("Test 1 - Before sorting: ", arr, 6);
        pIEcoLab1->pVTbl->csort_longdouble(pIEcoLab1, arr, 6);
        print_array_longdouble("Test 1 - After sorting:  ", arr, 6);
        printf("Result: %s\n\n", is_sorted_longdouble(arr, 6) ? "SUCCESS" : "FAILED");
    }

    {
        long double arr[5] = {123456789012345678.123456789L, 987654321098765432.987654321L, 555555555555555555.555555555L, 111111111111111111.111111111L, 999999999999999999.999999999L};
        print_array_longdouble("Test 2 - Before sorting: ", arr, 5);
        pIEcoLab1->pVTbl->csort_longdouble(pIEcoLab1, arr, 5);
        print_array_longdouble("Test 2 - After sorting:  ", arr, 5);
        printf("Result: %s\n\n", is_sorted_longdouble(arr, 5) ? "SUCCESS" : "FAILED");
    }

    {
        long double arr[4] = {-9.876543210987654321L, -1.234567890123456789L, -7.654321098765432109L, -3.456789012345678901L};
        print_array_longdouble("Test 3 - Before sorting: ", arr, 4);
        pIEcoLab1->pVTbl->csort_longdouble(pIEcoLab1, arr, 4);
        print_array_longdouble("Test 3 - After sorting:  ", arr, 4);
        printf("Result: %s\n\n", is_sorted_longdouble(arr, 4) ? "SUCCESS" : "FAILED");
    }

    {
        long double arr[5] = {7.777777777777777777L, 7.777777777777777777L, 7.777777777777777777L, 7.777777777777777777L, 7.777777777777777777L};
        print_array_longdouble("Test 4 - Before sorting: ", arr, 5);
        pIEcoLab1->pVTbl->csort_longdouble(pIEcoLab1, arr, 5);
        print_array_longdouble("Test 4 - After sorting:  ", arr, 5);
        printf("Result: %s\n\n", is_sorted_longdouble(arr, 5) ? "SUCCESS" : "FAILED");
    }

    {
        long double arr[7] = {0.000000000000000001L, 0.000000000000000002L, 0.000000000000000003L, 0.000000000000000004L, 0.000000000000000005L, 0.000000000000000006L, 0.000000000000000007L};
        print_array_longdouble("Test 5 - Before sorting: ", arr, 7);
        pIEcoLab1->pVTbl->csort_longdouble(pIEcoLab1, arr, 7);
        print_array_longdouble("Test 5 - After sorting:  ", arr, 7);
        printf("Result: %s\n\n", is_sorted_longdouble(arr, 7) ? "SUCCESS" : "FAILED");
    }
    result = 0;

Release:
    if (pIBus != 0) pIBus->pVTbl->Release(pIBus);
    if (pIMem != 0) pIMem->pVTbl->Release(pIMem);
    if (pIEcoLab1 != 0) pIEcoLab1->pVTbl->Release(pIEcoLab1);
    if (pISys != 0) pISys->pVTbl->Release(pISys);

    
    return result;
}