# Интеграция калькулятора через включение и агрегацию (IEcoCalculatorX/Y)

## 1. Постановка задачи

Цель: расширить компонент `CEcoLab1` поддержкой операций калькулятора (сложение/вычитание/умножение/деление) с использованием приемов включения (inclusion) и агрегации (aggregation) компонентов A–E по методологии InsideACOM.

Требования:
- Предоставить клиенту интерфейсы `IEcoCalculatorX` (Addition, Subtraction) и `IEcoCalculatorY` (Multiplication, Division) через `QueryInterface` у `CEcoLab1`.
- Показать разные варианты компоновки: включение и агрегация.
- Обеспечить работу через интерфейсную шину (InterfaceBus) без жестких бинарных связей.

## 2. Архитектура и интерфейсы

Интерфейсы:
- `IEcoCalculatorX` (IID `{93221116-2248-4742-AE06-82819447843D}`): Addition, Subtraction
- `IEcoCalculatorY` (IID `{BD6414C2-9096-423E-A90C-04D77AFD1CAD}`): Multiplication, Division

Компоненты (CLSID):
- X-провайдеры: `CID_EcoCalculatorA`, `CID_EcoCalculatorB`
- Y-провайдеры: `CID_EcoCalculatorE`, `CID_EcoCalculatorD`

Файлы проекта:
- `SharedFiles/IEcoCalculatorX.h`, `SharedFiles/IEcoCalculatorY.h`
- `SharedFiles/IdEcoCalculatorA.h`, `IdEcoCalculatorB.h`, `IdEcoCalculatorD.h`, `IdEcoCalculatorE.h`
- `HeaderFiles/CEcoLab1.h`, `SourceFiles/CEcoLab1.c`

## 3. Схемы компоновки

- X: включение A → при недоступности A — агрегация B
- Y: включение E → при недоступности E — включение D

`CEcoLab1` выступает «рамкой»: клиент получает X/Y через `QueryInterface` у `CEcoLab1`, а реализация делегируется внутренним компонентам.

## 4. Реализация (ключевые моменты)

Инициализация зависимостей в `initCEcoLab1` (упрощенная схема):
```c
// X: A (inclusion) → B (aggregation fallback)
IEcoCalculatorX* pIX = 0;
result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorA, 0, &IID_IEcoCalculatorX, (void**)&pIX);
if (result == 0 && pIX != 0) {
    pCMe->m_pIX = pIX;
} else {
    IEcoCalculatorX* pIXB = 0;
    int16_t ra = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, (IEcoUnknown*)me, &IID_IEcoCalculatorX, (void**)&pIXB);
    if (ra == 0 && pIXB != 0) {
        pCMe->m_pIX = pIXB;
    }
}

// Y: E (inclusion) → D (inclusion fallback)
IEcoCalculatorY* pIY = 0;
int16_t r2 = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorE, 0, &IID_IEcoCalculatorY, (void**)&pIY);
if (r2 == 0 && pIY != 0) {
    pCMe->m_pIY = pIY;
} else {
    IEcoCalculatorY* pIYD = 0;
    int16_t r3 = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorD, 0, &IID_IEcoCalculatorY, (void**)&pIYD);
    if (r3 == 0 && pIYD != 0) {
        pCMe->m_pIY = pIYD;
    }
}
```

Выдача интерфейсов клиенту через `CEcoLab1::QueryInterface`:
```c
if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) { /* return m_pIX */ }
if (IsEqualUGUID(riid, &IID_IEcoCalculatorY)) { /* return m_pIY */ }
```

Очистка в `deleteCEcoLab1`:
```c
if (pCMe->m_pIX) { pCMe->m_pIX->pVTbl->Release(pCMe->m_pIX); pCMe->m_pIX = 0; }
if (pCMe->m_pIY) { pCMe->m_pIY->pVTbl->Release(pCMe->m_pIY); pCMe->m_pIY = 0; }
```

## 5. Тестирование (UnitTest)

После получения `IEcoLab1` клиент запрашивает X/Y и вызывает операции:
```c
IEcoCalculatorX* pIX = 0; IEcoCalculatorY* pIY = 0;
int16_t rqx = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pIX);
int16_t rqy = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**)&pIY);
if (rqx == 0 && pIX) { printf("X Addition(3,5) = %d\n", pIX->pVTbl->Addition(pIX, 3, 5)); pIX->pVTbl->Release(pIX);} 
if (rqy == 0 && pIY) { printf("Y Multiplication(4,7) = %d\n", pIY->pVTbl->Multiplication(pIY, 4, 7)); pIY->pVTbl->Release(pIY);} 
```

Ожидаемое поведение:
- При наличии DLL A/E — используются они; иначе X переключается на B (aggregation), Y — на D (inclusion).
- Клиент всегда работает через `IEcoLab1` и `QueryInterface`.

## 6. Зависимости и развертывание

- Требуются для InterfaceBus бинарные компоненты: A, B, D, E (см. Lesson02–07).
- DLL располагать рядом с исполняемым UnitTest или настроить путь поиска (как в демо из уроков).

## 7. Вывод

Реализована интеграция калькулятора в `CEcoLab1` с поддержкой двух техник — включение и агрегация. Клиент получает единообразный доступ к `IEcoCalculatorX`/`IEcoCalculatorY` через `QueryInterface` от `CEcoLab1`, при этом конкретные внутренние реализации подставляются динамически через InterfaceBus с учетом доступности компонент.

