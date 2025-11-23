# Отчет по лабораторной работе №2
## Включение и агрегирование компонентов калькулятора

---

## 1. Общая постановка задачи

В рамках данной лабораторной работы необходимо было расширить ранее созданный компонент `Eco.Lab1`, добавив в него функциональность калькулятора посредством использования приемов **включения (containment)** и **агрегирования (aggregation)** компонентов. 

Компонент должен предоставлять интерфейсы `IEcoCalculatorX` (операции сложения и вычитания) и `IEcoCalculatorY` (операции умножения и деления), используя внутренние компоненты `Eco.CalculatorA`, `Eco.CalculatorB`, `Eco.CalculatorD` и `Eco.CalculatorE`.

---

## 2. Архитектура компонентов

### 2.1. Структура компонентов калькулятора

В системе присутствуют следующие компоненты:

- **Eco.CalculatorA** — реализует `IEcoCalculatorX` (сложение, вычитание), не поддерживает агрегирование
- **Eco.CalculatorB** — реализует `IEcoCalculatorX` (сложение, вычитание), **поддерживает агрегирование**
- **Eco.CalculatorD** — реализует `IEcoCalculatorX` и `IEcoCalculatorY`, включает компонент A или B
- **Eco.CalculatorE** — реализует `IEcoCalculatorY` (умножение, деление), агрегирует компонент B

### 2.2. Интерфейсы

#### IEcoCalculatorX
```c
typedef struct IEcoCalculatorXVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(...);
    uint32_t (ECOCALLMETHOD *AddRef)(...);
    uint32_t (ECOCALLMETHOD *Release)(...);
    int32_t (ECOCALLMETHOD *Addition)(/* in */ int16_t a, /* in */ int16_t b);
    int16_t (ECOCALLMETHOD *Subtraction)(/* in */ int16_t a, /* in */ int16_t b);
} IEcoCalculatorXVTbl;
```

#### IEcoCalculatorY
```c
typedef struct IEcoCalculatorYVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(...);
    uint32_t (ECOCALLMETHOD *AddRef)(...);
    uint32_t (ECOCALLMETHOD *Release)(...);
    int32_t (ECOCALLMETHOD *Multiplication)(/* in */ int16_t a, /* in */ int16_t b);
    int16_t (ECOCALLMETHOD *Division)(/* in */ int16_t a, /* in */ int16_t b);
} IEcoCalculatorYVTbl;
```

---

## 3. Реализация включения и агрегирования

### 3.1. Включение (Containment)

**Включение** — это техника, при которой внешний компонент создает внутренний компонент и делегирует ему вызовы методов, но внутренний компонент не знает о внешнем.

#### Пример: Компонент D включает компонент A или B

В `initCEcoCalculatorD`:

```c
/* Получение интерфейса по работе со сложением и вычитанием у компонента "B" */
result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, 0, 
                                      &IID_IEcoCalculatorX, 
                                      (void**) &pCMe->m_pIX);
/* Если не удалось, используем компонент A */
if (result != 0 || pCMe->m_pIX == 0) {
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorA, 0, 
                                         &IID_IEcoCalculatorX, 
                                         (void**) &pCMe->m_pIX);
}
```

Компонент D хранит указатель на `IEcoCalculatorX` внутреннего компонента и делегирует ему вызовы:

```c
static int32_t ECOCALLMETHOD CEcoCalculatorD_Addition(/* in */ IEcoCalculatorXPtr_t me, 
                                                       /* in */ int16_t a, 
                                                       /* in */ int16_t b) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)me;
    if (me == 0 || pCMe->m_pIX == 0) {
        return -1;
    }
    return pCMe->m_pIX->pVTbl->Addition(pCMe->m_pIX, a, b);
}
```

### 3.2. Агрегирование (Aggregation)

**Агрегирование** — это техника, при которой внутренний компонент делегирует все вызовы `QueryInterface` внешнему компоненту, создавая иллюзию единого объекта.

#### Пример: Компонент B поддерживает агрегирование

В компоненте B все методы `QueryInterface`, `AddRef`, `Release` делегируются внешнему объекту:

```c
static int16_t ECOCALLMETHOD CEcoCalculatorB_QueryInterface(
    /* in */ IEcoCalculatorXPtr_t me, 
    /* in */ const UGUID* riid, 
    /* out */ voidptr_t* ppv) {
    CEcoCalculatorB* pCMe = (CEcoCalculatorB*)me;
    /* Делегируем запрос внешнему объекту */
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}
```

#### Пример: Компонент E агрегирует компонент B

В `initCEcoCalculatorE`:

```c
/* Создание внутреннего компонента "B" с поддержкой агрегирования */
/* ВАЖНО: При агрегировании мы передаем IID IEcoUnknown */
result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, 
                                      pOuterUnknown,  /* Указатель на внешний объект */
                                      &IID_IEcoUnknown, 
                                      (void**) &pCMe->m_pInnerUnknown);
```

### 3.3. Реализация в компоненте Eco.Lab1

Компонент `CEcoLab1` использует оба подхода:

#### Структура данных

```c
typedef struct CEcoLab1 {
    IEcoLab1VTbl* m_pVTblIEcoLab1;
    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;
    IEcoCalculatorYVTbl* m_pVTblIEcoCalculatorY;
    
    /* Указатель на интерфейс IEcoCalculatorX включаемого компонента */
    IEcoCalculatorX* m_pIEcoCalculatorX;
    
    /* Указатель на интерфейс IEcoCalculatorY включаемого компонента */
    IEcoCalculatorY* m_pIEcoCalculatorY;
    
    /* Указатель на IEcoUnknown внутреннего компонента (для агрегирования) */
    IEcoUnknown* m_pInnerUnknown;
    
    /* ... другие поля ... */
} CEcoLab1;
```

#### Инициализация в initCEcoLab1

```c
/* 1. Включение компонента E или D для получения IEcoCalculatorY */
result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorE, 0, 
                                      &IID_IEcoCalculatorY, 
                                      (void**) &pCMe->m_pIEcoCalculatorY);
if (result != 0 || pCMe->m_pIEcoCalculatorY == 0) {
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorD, 0, 
                                          &IID_IEcoCalculatorY, 
                                          (void**) &pCMe->m_pIEcoCalculatorY);
}

/* 2. Агрегирование компонента B */
result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, 
                                      pOuterUnknown,  /* Передаем себя как внешний объект */
                                      &IID_IEcoUnknown, 
                                      (void**) &pCMe->m_pInnerUnknown);
if (result != 0 || pCMe->m_pInnerUnknown == 0) {
    /* Если агрегирование не удалось, включаем компонент A */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorA, 0, 
                                          &IID_IEcoCalculatorX, 
                                          (void**) &pCMe->m_pIEcoCalculatorX);
}
```

#### Реализация QueryInterface для IEcoCalculatorX

```c
static int16_t ECOCALLMETHOD CEcoLab1_IEcoCalculatorX_QueryInterface(
    /* in */ struct IEcoCalculatorX* me, 
    /* in */ const UGUID* riid, 
    /* out */ void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 2);
    
    if (IsEqualUGUID(riid, &IID_IEcoLab1)) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
    else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX) && pCMe->m_pInnerUnknown != 0) {
        /* Если есть агрегированный компонент, делегируем ему */
        return pCMe->m_pInnerUnknown->pVTbl->QueryInterface(
            pCMe->m_pInnerUnknown, riid, ppv);
    } 
    else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        /* Иначе возвращаем свой интерфейс */
        *ppv = &pCMe->m_pVTblIEcoCalculatorX;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
    else if (IsEqualUGUID(riid, &IID_IEcoCalculatorY)) {
        *ppv = &pCMe->m_pVTblIEcoCalculatorY;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*) pCMe);
    } 
    else {
        *ppv = 0;
        return -1;
    }
    return 0;
}
```

#### Делегирование вызовов методов

```c
static int32_t ECOCALLMETHOD CEcoLab1_Addition(
    /* in */ struct IEcoCalculatorX* me, 
    /* in */ int16_t a, 
    /* in */ int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 2);
    
    if (me == 0 || pCMe->m_pIEcoCalculatorX == 0) {
        return -1;
    }
    
    /* Делегируем вызов включенному компоненту */
    return pCMe->m_pIEcoCalculatorX->pVTbl->Addition(
        pCMe->m_pIEcoCalculatorX, a, b);
}
```

---

## 4. Демонстрация работы QueryInterface

### 4.1. Тестирование операций калькулятора

В клиентском коде (`EcoLab1.c`) демонстрируется использование интерфейсов:

```c
/* Получение интерфейсов через QueryInterface */
result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, 
                                          (void**) &pIEcoCalculatorX);
result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, 
                                          (void**) &pIEcoCalculatorY);

/* Использование операций */
printf("%d + %d = %d\n", i32A, i32B, 
       pIEcoCalculatorX->pVTbl->Addition(pIEcoCalculatorX, i32A, i32B));
printf("%d - %d = %d\n", i32A, i32B, 
       pIEcoCalculatorX->pVTbl->Subtraction(pIEcoCalculatorX, i32A, i32B));
printf("%d * %d = %d\n", i32A, i32B, 
       pIEcoCalculatorY->pVTbl->Multiplication(pIEcoCalculatorY, i32A, i32B));
printf("%d / %d = %d\n", i32A, i32B, 
       pIEcoCalculatorY->pVTbl->Division(pIEcoCalculatorY, i32A, i32B));
```

### 4.2. Демонстрация свойств интерфейсов

Важное свойство COM-интерфейсов: через любой указатель на интерфейс можно получить указатель на любой другой интерфейс того же объекта.

#### Пример вывода тестов:

```
A/B components tests:
36 + 2 = 38
36 - 2 = 34

D/E components tests:
36 * 2 = 72
36 / 2 = 18

IEcoLab1 -> pIEcoCalculatorX [OK]
IEcoLab1 -> pIEcoCalculatorY [OK]
IEcoLab1 -> IEcoLab1 [OK]
pIEcoCalculatorX -> pIEcoCalculatorY [OK]
pIEcoCalculatorX -> IEcoLab1 [OK]
pIEcoCalculatorX -> pIEcoCalculatorX [OK]
pIEcoCalculatorY -> pIEcoCalculatorX [OK]
pIEcoCalculatorY -> pIEcoCalculatorY [OK]
pIEcoCalculatorY -> IEcoLab1 [OK]
```

Эти тесты демонстрируют, что:

1. Из `IEcoLab1` можно получить `IEcoCalculatorX` и `IEcoCalculatorY`
2. Из `IEcoCalculatorX` можно получить `IEcoCalculatorY` и `IEcoLab1`
3. Из `IEcoCalculatorY` можно получить `IEcoCalculatorX` и `IEcoLab1`
4. Из любого интерфейса можно получить сам себя

Это подтверждает правильность реализации `QueryInterface` и соблюдение правил COM.

---

## 5. Различия между включением и агрегированием

### 5.1. Включение (Containment)

- Внешний компонент создает внутренний компонент **независимо**
- Внутренний компонент **не знает** о внешнем
- Внешний компонент **делегирует** вызовы методов внутреннему
- При создании передается `pIUnkOuter = 0` или не передается вообще
- Внешний компонент управляет жизненным циклом внутреннего

### 5.2. Агрегирование (Aggregation)

- Внешний компонент создает внутренний компонент, **передавая себя** как внешний объект
- Внутренний компонент **делегирует** все вызовы `QueryInterface` внешнему
- Клиент видит **единый объект**, хотя на самом деле это два компонента
- При создании передается `pIUnkOuter != 0` (указатель на внешний объект)
- Внутренний компонент использует счетчик ссылок внешнего объекта

### 5.3. Когда использовать что?

- **Включение** — когда нужно просто использовать функциональность другого компонента
- **Агрегирование** — когда нужно, чтобы клиент видел интерфейсы внутреннего компонента как свои собственные

---

## 6. Выводы

В ходе выполнения лабораторной работы были реализованы и продемонстрированы:

1. **Включение компонентов** — компонент D включает A или B, компонент E включает B, компонент Lab1 включает E или D
2. **Агрегирование компонентов** — компонент B агрегируется компонентами E и Lab1
3. **Правильная реализация QueryInterface** — обеспечивает возможность получения любого интерфейса из любого другого интерфейса того же объекта
4. **Делегирование вызовов** — внешние компоненты корректно делегируют вызовы методов внутренним компонентам

Реализация полностью соответствует принципам COM и позволяет клиенту работать с компонентом `Eco.Lab1` как с единым объектом, предоставляющим интерфейсы сортировки (`IEcoLab1`) и калькулятора (`IEcoCalculatorX`, `IEcoCalculatorY`).

---

## 7. Примеры кода для отчета

### 7.1. Инициализация компонентов в CEcoLab1

```c
/* Включение компонента E или D для IEcoCalculatorY */
result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorE, 0, 
                                      &IID_IEcoCalculatorY, 
                                      (void**) &pCMe->m_pIEcoCalculatorY);
if (result != 0 || pCMe->m_pIEcoCalculatorY == 0) {
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorD, 0, 
                                          &IID_IEcoCalculatorY, 
                                          (void**) &pCMe->m_pIEcoCalculatorY);
}

/* Агрегирование компонента B */
result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, 
                                      pOuterUnknown, 
                                      &IID_IEcoUnknown, 
                                      (void**) &pCMe->m_pInnerUnknown);
```

### 7.2. QueryInterface с поддержкой агрегирования

```c
else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX) && pCMe->m_pInnerUnknown != 0) {
    /* Делегируем запрос агрегированному компоненту */
    return pCMe->m_pInnerUnknown->pVTbl->QueryInterface(
        pCMe->m_pInnerUnknown, riid, ppv);
}
```

### 7.3. Делегирование вызовов методов

```c
static int32_t ECOCALLMETHOD CEcoLab1_Addition(
    /* in */ struct IEcoCalculatorX* me, 
    /* in */ int16_t a, 
    /* in */ int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*) ((uint64_t) me - sizeof(struct IEcoLab1*) * 2);
    return pCMe->m_pIEcoCalculatorX->pVTbl->Addition(
        pCMe->m_pIEcoCalculatorX, a, b);
}
```

---

**Автор:** [Ваше имя]  
**Дата:** [Дата выполнения]

