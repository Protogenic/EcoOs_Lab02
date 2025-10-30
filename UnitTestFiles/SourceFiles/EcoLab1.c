/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* System headers first to avoid SAL/annotation macro conflicts */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorC.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"

#define TEST_PASS "OK"
#define TEST_FAIL "ERROR"
#define TEST_SKIP "MISSING"

static void box_line(char c, int width) {
    int i;
    for (i = 0; i < width; ++i) putchar(c);
    putchar('\n');
}

static void box_title(const char* title) {
    int width = 76;
    int len = 0;
    const char* p = title;
    while (*p++) ++len;
    box_line('=', width);
    printf("|| %-*s ||\n", width - 6, title);
    box_line('=', width);
}

static void box_section(const char* label) {
    printf("\n--- %s ---\n", label);
}

static void print_array(const int32_t* arr, int size) {
    int i;
    putchar('[');
    for (i = 0; i < size; ++i) {
        printf("%d%s", arr[i], (i < size - 1) ? ", " : "");
    }
    printf("]\n");
}

static void print_array_long(const int64_t* arr, int size) {
    int i;
    putchar('[');
    for (i = 0; i < size; ++i) {
        printf("%lld%s", arr[i], (i < size - 1) ? ", " : "");
    }
    printf("]\n");
}

static void print_array_float(const float* arr, int size) {
    int i;
    putchar('[');
    for (i = 0; i < size; ++i) {
        printf("%.3f%s", arr[i], (i < size - 1) ? ", " : "");
    }
    printf("]\n");
}

static void print_array_double(const double* arr, int size) {
    int i;
    putchar('[');
    for (i = 0; i < size; ++i) {
        printf("%.6f%s", arr[i], (i < size - 1) ? ", " : "");
    }
    printf("]\n");
}

static void print_array_longdouble(const long double* arr, int size) {
    int i;
    putchar('[');
    for (i = 0; i < size; ++i) {
        printf("%.6Lf%s", arr[i], (i < size - 1) ? ", " : "");
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

static void print_test_result(const char* test_desc, const char* result, const char* details) {
    printf("  %-50s [%s] %s\n", test_desc, result, details);
}

/* Тестирование отдельных компонентов калькулятора */
void test_calculator_components(IEcoInterfaceBus1* pIBus) {
    IEcoCalculatorX* pICalcB = 0;
    IEcoCalculatorX* pICalcC = 0;
    IEcoCalculatorY* pICalcD = 0;
    IEcoCalculatorY* pICalcE = 0;
    int16_t result;
    int32_t op_result;
    
    box_title("INDIVIDUAL CALCULATOR COMPONENTS TEST (B, C, D, E)");
    
    /* ===== КОМПОНЕНТ B (IEcoCalculatorX) ===== */
    box_section("Component B - IEcoCalculatorX (Containment)");
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, 0, 
                                          &IID_IEcoCalculatorX, (void**)&pICalcB);
    if (result == 0 && pICalcB != 0) {
        print_test_result("Component B acquisition", TEST_PASS, "Successfully obtained");
        
        op_result = pICalcB->pVTbl->Addition(pICalcB, 100, 50);
        if (op_result == 150) {
            print_test_result("Component B: Addition (100 + 50)", TEST_PASS, "Result: 150");
        } else {
            printf("  Component B: Addition (100 + 50)               [%s] Expected: 150, Got: %d\n", 
                   TEST_FAIL, op_result);
        }
        
        op_result = pICalcB->pVTbl->Subtraction(pICalcB, 100, 50);
        if (op_result == 50) {
            print_test_result("Component B: Subtraction (100 - 50)", TEST_PASS, "Result: 50");
        } else {
            printf("  Component B: Subtraction (100 - 50)            [%s] Expected: 50, Got: %d\n", 
                   TEST_FAIL, op_result);
        }
        
        pICalcB->pVTbl->Release(pICalcB);
    } else {
        print_test_result("Component B acquisition", TEST_FAIL, "Failed to obtain component");
        printf("  Component B: Addition (100 + 50)               [%s] Component unavailable\n", TEST_SKIP);
        printf("  Component B: Subtraction (100 - 50)            [%s] Component unavailable\n", TEST_SKIP);
    }
    
    /* ===== КОМПОНЕНТ C (IEcoCalculatorX) ===== */
    box_section("Component C - IEcoCalculatorX (Containment)");
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorC, 0, 
                                          &IID_IEcoCalculatorX, (void**)&pICalcC);
    if (result == 0 && pICalcC != 0) {
        print_test_result("Component C acquisition", TEST_PASS, "Successfully obtained");
        
        op_result = pICalcC->pVTbl->Addition(pICalcC, 200, 75);
        if (op_result == 275) {
            print_test_result("Component C: Addition (200 + 75)", TEST_PASS, "Result: 275");
        } else {
            printf("  Component C: Addition (200 + 75)               [%s] Expected: 275, Got: %d\n", 
                   TEST_FAIL, op_result);
        }
        
        op_result = pICalcC->pVTbl->Subtraction(pICalcC, 200, 75);
        if (op_result == 125) {
            print_test_result("Component C: Subtraction (200 - 75)", TEST_PASS, "Result: 125");
        } else {
            printf("  Component C: Subtraction (200 - 75)            [%s] Expected: 125, Got: %d\n", 
                   TEST_FAIL, op_result);
        }
        
        pICalcC->pVTbl->Release(pICalcC);
    } else {
        print_test_result("Component C acquisition", TEST_FAIL, "Failed to obtain component");
        printf("  Component C: Addition (200 + 75)               [%s] Component unavailable\n", TEST_SKIP);
        printf("  Component C: Subtraction (200 - 75)            [%s] Component unavailable\n", TEST_SKIP);
    }
    
    /* ===== КОМПОНЕНТ D (IEcoCalculatorY) ===== */
    box_section("Component D - IEcoCalculatorY (Aggregation)");
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorD, 0, 
                                          &IID_IEcoCalculatorY, (void**)&pICalcD);
    if (result == 0 && pICalcD != 0) {
        print_test_result("Component D acquisition", TEST_PASS, "Successfully obtained");
        
        op_result = pICalcD->pVTbl->Multiplication(pICalcD, 12, 8);
        if (op_result == 96) {
            print_test_result("Component D: Multiplication (12 * 8)", TEST_PASS, "Result: 96");
        } else {
            printf("  Component D: Multiplication (12 * 8)           [%s] Expected: 96, Got: %d\n", 
                   TEST_FAIL, op_result);
        }
        
        op_result = pICalcD->pVTbl->Division(pICalcD, 96, 8);
        if (op_result == 12) {
            print_test_result("Component D: Division (96 / 8)", TEST_PASS, "Result: 12");
        } else {
            printf("  Component D: Division (96 / 8)                 [%s] Expected: 12, Got: %d\n", 
                   TEST_FAIL, op_result);
        }
        
        pICalcD->pVTbl->Release(pICalcD);
    } else {
        print_test_result("Component D acquisition", TEST_FAIL, "Failed to obtain component");
        printf("  Component D: Multiplication (12 * 8)           [%s] Component unavailable\n", TEST_SKIP);
        printf("  Component D: Division (96 / 8)                 [%s] Component unavailable\n", TEST_SKIP);
    }
    
    /* ===== КОМПОНЕНТ E (IEcoCalculatorY) ===== */
    box_section("Component E - IEcoCalculatorY (Aggregation)");
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorE, 0, 
                                          &IID_IEcoCalculatorY, (void**)&pICalcE);
    if (result == 0 && pICalcE != 0) {
        print_test_result("Component E acquisition", TEST_PASS, "Successfully obtained");
        
        op_result = pICalcE->pVTbl->Multiplication(pICalcE, 15, 6);
        if (op_result == 90) {
            print_test_result("Component E: Multiplication (15 * 6)", TEST_PASS, "Result: 90");
        } else {
            printf("  Component E: Multiplication (15 * 6)           [%s] Expected: 90, Got: %d\n", 
                   TEST_FAIL, op_result);
        }
        
        op_result = pICalcE->pVTbl->Division(pICalcE, 90, 6);
        if (op_result == 15) {
            print_test_result("Component E: Division (90 / 6)", TEST_PASS, "Result: 15");
        } else {
            printf("  Component E: Division (90 / 6)                 [%s] Expected: 15, Got: %d\n", 
                   TEST_FAIL, op_result);
        }
        
        pICalcE->pVTbl->Release(pICalcE);
    } else {
        print_test_result("Component E acquisition", TEST_FAIL, "Failed to obtain component");
        printf("  Component E: Multiplication (15 * 6)           [%s] Component unavailable\n", TEST_SKIP);
        printf("  Component E: Division (90 / 6)                 [%s] Component unavailable\n", TEST_SKIP);
    }
    
    printf("\n=== COMPONENT TEST SUMMARY ===\n");
    printf("Individual components B, C, D, E: NEED REGISTRATION\n");
    printf("These components must be registered in the interface bus\n");
    printf("to enable individual testing outside of IEcoLab1 context.\n\n");
}

/* Демонстрация механизмов включения и агрегирования */
void test_component_integration(IEcoLab1* pIEcoLab1) {
    IEcoUnknown* pUnk = 0;
    IEcoCalculatorX* pIX = 0;
    IEcoCalculatorY* pIY = 0;
    IEcoCalculatorX* pIX_from_other = 0;
    IEcoCalculatorY* pIY_from_other = 0;
    IEcoLab1* pLab1_from_other = 0;
    IEcoUnknown* pUnk_from_other = 0;
    int16_t result;
    int32_t op_result;
    
    box_title("COMPONENT INTEGRATION VALIDATION");
    
    /* Direct Interface Access through IEcoLab1 */
    printf("\n--- Interface Access via IEcoLab1 ---\n");
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoUnknown, (void**)&pUnk);
    if (result == 0 && pUnk != 0) {
        print_test_result("IEcoUnknown interface", TEST_PASS, "Successfully obtained");
    } else {
        print_test_result("IEcoUnknown interface", TEST_FAIL, "Acquisition failed");
    }
    
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pIX);
    if (result == 0 && pIX != 0) {
        print_test_result("IEcoCalculatorX interface", TEST_PASS, "Successfully obtained");
    } else {
        print_test_result("IEcoCalculatorX interface", TEST_FAIL, "Acquisition failed");
    }
    
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**)&pIY);
    if (result == 0 && pIY != 0) {
        print_test_result("IEcoCalculatorY interface", TEST_PASS, "Successfully obtained");
    } else {
        print_test_result("IEcoCalculatorY interface", TEST_FAIL, "Acquisition failed");
    }

    /* Arithmetic Operations through obtained interfaces */
    printf("\n--- Arithmetic Operations ---\n");
    if (pIX != 0) {
        op_result = pIX->pVTbl->Addition(pIX, 25, 7);
        if (op_result == 32) {
            print_test_result("Addition (25 + 7)", TEST_PASS, "Correct result: 32");
        } else {
            print_test_result("Addition (25 + 7)", TEST_FAIL, "Wrong computation");
        }
        
        op_result = pIX->pVTbl->Subtraction(pIX, 25, 7);
        if (op_result == 18) {
            print_test_result("Subtraction (25 - 7)", TEST_PASS, "Correct result: 18");
        } else {
            print_test_result("Subtraction (25 - 7)", TEST_FAIL, "Wrong computation");
        }
    } else {
        print_test_result("IEcoCalculatorX operations", TEST_SKIP, "Interface unavailable");
    }
    
    if (pIY != 0) {
        op_result = pIY->pVTbl->Multiplication(pIY, 25, 7);
        if (op_result == 175) {
            print_test_result("Multiplication (25 * 7)", TEST_PASS, "Correct result: 175");
        } else {
            print_test_result("Multiplication (25 * 7)", TEST_FAIL, "Wrong computation");
        }
        
        op_result = pIY->pVTbl->Division(pIY, 25, 5);
        if (op_result == 5) {
            print_test_result("Division (25 / 5)", TEST_PASS, "Correct result: 5");
        } else {
            print_test_result("Division (25 / 5)", TEST_FAIL, "Wrong computation");
        }
    } else {
        print_test_result("IEcoCalculatorY operations", TEST_SKIP, "Interface unavailable");
    }

    /* Cross-Interface References through IEcoLab1 interfaces */
    printf("\n--- Cross-Interface References ---\n");
    
    /* From IEcoCalculatorX */
    if (pIX != 0) {
        result = pIX->pVTbl->QueryInterface(pIX, &IID_IEcoCalculatorY, (void**)&pIY_from_other);
        if (result == 0 && pIY_from_other != 0) {
            print_test_result("IEcoCalculatorY from IEcoCalculatorX", TEST_PASS, "Cross-reference valid");
            pIY_from_other->pVTbl->Release(pIY_from_other);
        } else {
            print_test_result("IEcoCalculatorY from IEcoCalculatorX", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pIX->pVTbl->QueryInterface(pIX, &IID_IEcoLab1, (void**)&pLab1_from_other);
        if (result == 0 && pLab1_from_other != 0) {
            print_test_result("IEcoLab1 from IEcoCalculatorX", TEST_PASS, "Cross-reference valid");
            pLab1_from_other->pVTbl->Release(pLab1_from_other);
        } else {
            print_test_result("IEcoLab1 from IEcoCalculatorX", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pIX->pVTbl->QueryInterface(pIX, &IID_IEcoUnknown, (void**)&pUnk_from_other);
        if (result == 0 && pUnk_from_other != 0) {
            print_test_result("IEcoUnknown from IEcoCalculatorX", TEST_PASS, "Cross-reference valid");
            pUnk_from_other->pVTbl->Release(pUnk_from_other);
        } else {
            print_test_result("IEcoUnknown from IEcoCalculatorX", TEST_FAIL, "Cross-reference broken");
        }
    }
    
    /* From IEcoCalculatorY */
    if (pIY != 0) {
        result = pIY->pVTbl->QueryInterface(pIY, &IID_IEcoCalculatorX, (void**)&pIX_from_other);
        if (result == 0 && pIX_from_other != 0) {
            print_test_result("IEcoCalculatorX from IEcoCalculatorY", TEST_PASS, "Cross-reference valid");
            pIX_from_other->pVTbl->Release(pIX_from_other);
        } else {
            print_test_result("IEcoCalculatorX from IEcoCalculatorY", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pIY->pVTbl->QueryInterface(pIY, &IID_IEcoLab1, (void**)&pLab1_from_other);
        if (result == 0 && pLab1_from_other != 0) {
            print_test_result("IEcoLab1 from IEcoCalculatorY", TEST_PASS, "Cross-reference valid");
            pLab1_from_other->pVTbl->Release(pLab1_from_other);
        } else {
            print_test_result("IEcoLab1 from IEcoCalculatorY", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pIY->pVTbl->QueryInterface(pIY, &IID_IEcoUnknown, (void**)&pUnk_from_other);
        if (result == 0 && pUnk_from_other != 0) {
            print_test_result("IEcoUnknown from IEcoCalculatorY", TEST_PASS, "Cross-reference valid");
            pUnk_from_other->pVTbl->Release(pUnk_from_other);
        } else {
            print_test_result("IEcoUnknown from IEcoCalculatorY", TEST_FAIL, "Cross-reference broken");
        }
    }
    
    /* From IEcoUnknown */
    if (pUnk != 0) {
        result = pUnk->pVTbl->QueryInterface(pUnk, &IID_IEcoCalculatorX, (void**)&pIX_from_other);
        if (result == 0 && pIX_from_other != 0) {
            print_test_result("IEcoCalculatorX from IEcoUnknown", TEST_PASS, "Cross-reference valid");
            pIX_from_other->pVTbl->Release(pIX_from_other);
        } else {
            print_test_result("IEcoCalculatorX from IEcoUnknown", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pUnk->pVTbl->QueryInterface(pUnk, &IID_IEcoCalculatorY, (void**)&pIY_from_other);
        if (result == 0 && pIY_from_other != 0) {
            print_test_result("IEcoCalculatorY from IEcoUnknown", TEST_PASS, "Cross-reference valid");
            pIY_from_other->pVTbl->Release(pIY_from_other);
        } else {
            print_test_result("IEcoCalculatorY from IEcoUnknown", TEST_FAIL, "Cross-reference broken");
        }
        
        result = pUnk->pVTbl->QueryInterface(pUnk, &IID_IEcoLab1, (void**)&pLab1_from_other);
        if (result == 0 && pLab1_from_other != 0) {
            print_test_result("IEcoLab1 from IEcoUnknown", TEST_PASS, "Cross-reference valid");
            pLab1_from_other->pVTbl->Release(pLab1_from_other);
        } else {
            print_test_result("IEcoLab1 from IEcoUnknown", TEST_FAIL, "Cross-reference broken");
        }
    }

    /* Cleanup */
    if (pUnk != 0) pUnk->pVTbl->Release(pUnk);
    if (pIX != 0) pIX->pVTbl->Release(pIX);
    if (pIY != 0) pIY->pVTbl->Release(pIY);
	printf("\n");
}

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    char_t* name = 0;
    char_t* copyName = 0;
    /* Указатель на тестируемый интерфейс */
    IEcoLab1* pIEcoLab1 = 0;

    /* Проверка и создание системного интрефейса */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 && pISys == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
            goto Release;
        }
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
#ifdef ECO_LIB
    /* Регистрация статического компонента для работы со списком */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorB, (IEcoUnknown*)GetIEcoComponentFactoryPtr_AE202E543CE54550899603BD70C62565);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorC, (IEcoUnknown*)GetIEcoComponentFactoryPtr_4828F6552E4540E78121EBD220DC360E);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorD, (IEcoUnknown*)GetIEcoComponentFactoryPtr_3A8E44677E82475CB4A3719ED8397E61);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorE, (IEcoUnknown*)GetIEcoComponentFactoryPtr_872FEF1DE3314B87AD44D1E7C232C2F0);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
#endif
    /* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 || pIMem == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
        goto Release;
    }

    /* Выделение блока памяти */
    name = (char_t *)pIMem->pVTbl->Alloc(pIMem, 10);

    /* Заполнение блока памяти */
    pIMem->pVTbl->Fill(pIMem, name, 'a', 9);

    /* Получение тестируемого интерфейса */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        /* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }

	// run_performance_tests(pIEcoLab1, name, copyName);
    test_component_integration(pIEcoLab1);

    /* Освобождение блока памяти */
    pIMem->pVTbl->Free(pIMem, name);

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) {
        pIEcoLab1->pVTbl->Release(pIEcoLab1);
    }

    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}
