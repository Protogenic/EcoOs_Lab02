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

## 4. Реализация (ключевые моменты)
Регистрация компонентов:
```c
#ifdef ECO_LIB
    pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, ...);
    pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorA, ...);
    pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorB, ...);
    pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorC, ...);
    pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorD, ...);
    pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoCalculatorE, ...);
#endi
```
Инициализация зависимостей:
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

## 5. Результаты

<img width="556" height="566" alt="image" src="https://github.com/user-attachments/assets/df425de5-732b-433c-9de4-72b60bb22ba2" />

<img width="574" height="432" alt="image" src="https://github.com/user-attachments/assets/0e35981d-26ba-43b1-9d37-e1251bc04a8c" />

<img width="332" height="135" alt="image" src="https://github.com/user-attachments/assets/b4899e40-7a56-46dc-9246-07707876e5e0" />






