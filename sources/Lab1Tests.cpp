#include <iostream>
#include <memory>
#include <string>
#include "Lab1Demo.h"
#include "Station.h"
#include "ModuleFactory.h"

static int passed = 0;
static int failed = 0;

static void check(bool condition, const std::string& description)
{
    if (condition)
    {
        passed++;
        std::cout << "  [OK]   " << description << std::endl;
    }
    else
    {
        failed++;
        std::cout << "  [FAIL] " << description << std::endl;
    }
}

void runLab1Tests()
{
    std::cout << std::endl << "===== Тестирование =====" << std::endl;
    StandardModuleFactory sf;
    AdvancedModuleFactory af;

    std::cout << "-- Валидация атрибутов" << std::endl;
    Durability d1(-5);
    check(d1.getValue() == 0, "Durability(-5) даёт 0");
    Durability d2(150, 100);
    check(d2.getValue() == 100, "Durability(150, 100) даёт 100");
    Durability d3(50, 0);
    check(d3.getMax() == 1 && d3.getValue() == 1, "Durability(50, 0): максимум 1, значение 1");
    Durability d4(30);
    d4.decrease(50);
    check(d4.getValue() == 0, "decrease не уходит ниже 0");
    d4.repair(500);
    check(d4.getValue() == d4.getMax(), "repair не превышает максимум");
    Capacity c(-1);
    check(c.getValue() == 0, "Capacity(-1) даёт 0");
    EnergyConsumption e(10);
    e.decrease(100);
    check(e.getValue() == 0, "EnergyConsumption::decrease не уходит ниже 0");
    e.increase(7);
    check(e.getValue() == 7, "EnergyConsumption::increase работает");
    std::unique_ptr<Module> m = sf.createResidentialModule("Тест", 0);
    m->setEfficiency(150);
    check(m->getEfficiency() == 100, "эффективность не больше 100");
    m->setEfficiency(-5);
    check(m->getEfficiency() == 0, "эффективность не меньше 0");

    std::cout << "-- Фабрики" << std::endl;
    check(sf.createResidentialModule("a", 0)->getType() == "CrewCabin", "Standard: жилой по умолчанию - CrewCabin");
    check(sf.createResidentialModule("a", 1)->getType() == "GalleyModule", "Standard: жилой, вариант 1 - GalleyModule");
    check(sf.createEnergyModule("a", 0)->getType() == "SolarModule", "Standard: энергетический - SolarModule");
    check(sf.createResearchModule("a", 0)->getType() == "BiologyLab", "Standard: исследовательский - BiologyLab");
    check(af.createResidentialModule("a", 0)->getType() == "MedicalCabin", "Advanced: жилой - MedicalCabin");
    check(af.createEnergyModule("a", 0)->getType() == "ReactorModule", "Advanced: энергетический - ReactorModule");
    check(af.createResearchModule("a", 0)->getType() == "PhysicsLab", "Advanced: исследовательский - PhysicsLab");
    check(sf.createResidentialModule("a")->getCategory() == "Жилой", "категория жилого модуля");
    check(sf.createEnergyModule("a")->getCategory() == "Энергетический", "категория энергетического модуля");
    check(sf.createResearchModule("a")->getCategory() == "Исследовательский", "категория исследовательского модуля");

    std::cout << "-- Правило пяти" << std::endl;
    Station original("Оригинал", 5);
    original.addModule(sf.createEnergyModule("Панель", 0));
    original.addModule(af.createResearchModule("Лаборатория", 0));
    Station copy = original;
    check(copy.getCount() == original.getCount(), "конструктор копирования: то же число модулей");
    check(copy.getModule(0) != original.getModule(0), "глубокое копирование: модули - разные объекты");
    copy.getModule(0)->setEfficiency(10);
    check(original.getModule(0)->getEfficiency() != 10, "изменение копии не влияет на оригинал");
    Station assigned("Временная", 1);
    assigned = original;
    check(assigned.getCount() == 2 && assigned.getMax() == 5, "присваивание копированием");
    Station& selfRef = assigned;
    assigned = selfRef;
    check(assigned.getCount() == 2, "самоприсваивание не ломает станцию");
    Module* firstBefore = original.getModule(0);
    Station moved = std::move(original);
    check(moved.getModule(0) == firstBefore, "конструктор перемещения: те же объекты, без копирования");
    check(original.getCount() == 0, "источник после перемещения пуст");
    Station target("Цель", 1);
    target = std::move(moved);
    check(target.getCount() == 2 && moved.getCount() == 0, "присваивание перемещением");

    std::cout << "-- Итератор" << std::endl;
    int steps = 0;
    for (StationIterator it = target.getIterator(); it != target.end(); ++it)
    {
        steps++;
    }
    check(steps == target.getCount(), "обход через getIterator() посещает все модули");
    int rangeSteps = 0;
    for (auto& mod : target)
    {
        (void)mod;
        rangeSteps++;
    }
    check(rangeSteps == target.getCount(), "range-for посещает все модули");
    const Station& constRef = target;
    int constSteps = 0;
    for (ConstStationIterator it = constRef.begin(); it != constRef.end(); ++it)
    {
        constSteps++;
    }
    check(constSteps == target.getCount(), "константный обход посещает все модули");
    check(target.begin() == target.begin(), "operator== для одинаковых позиций");
    check(!(target.begin() == target.end()), "begin() и end() непустой станции различаются");

    std::cout << "-- Операции станции" << std::endl;
    Station small("Малая", 2);
    check(small.addModule(sf.createEnergyModule("1", 0)), "добавление 1-го модуля");
    check(small.addModule(sf.createEnergyModule("2", 1)), "добавление 2-го модуля");
    check(!small.addModule(sf.createEnergyModule("3", 0)), "превышение максимума отклоняется");
    Module* second = small.getModule(1);
    check(small.findById(second->getId()) == second, "findById находит существующий модуль");
    check(small.findById(-1) == nullptr, "findById возвращает nullptr для несуществующего");
    check(!small.removeByIndex(5), "removeByIndex с неверным индексом отклоняется");
    check(!small.removeById(9999), "removeById с неизвестным id отклоняется");
    check(small.removeById(second->getId()) && small.getCount() == 1, "removeById удаляет модуль");
    check(small.removeByIndex(0) && small.getCount() == 0, "removeByIndex удаляет модуль");
    Station tiny(0);
    check(tiny.getMax() == 1, "максимум меньше 1 заменяется на 1");

    std::cout << "-- Сортировка" << std::endl;
    Station sorted("Сортировка", 5);
    sorted.addModule(af.createResearchModule("Физ", 0));
    sorted.addModule(sf.createResearchModule("Био", 0));
    sorted.addModule(af.createEnergyModule("Реактор", 0));
    sorted.addModule(sf.createResidentialModule("Каюта", 0));
    sorted.sortByEfficiency();
    bool ordered = true;
    bool positionsOk = true;
    for (int i = 0; i < sorted.getCount(); i++)
    {
        if (sorted.getModule(i)->getPosition() != i)
        {
            positionsOk = false;
        }
        if (i > 0 && sorted.getModule(i - 1)->getEfficiency() < sorted.getModule(i)->getEfficiency())
        {
            ordered = false;
        }
    }
    check(ordered, "sortByEfficiency: эффективность не возрастает");
    check(positionsOk, "sortByEfficiency: позиции модулей обновлены");

    std::cout << std::endl << "Итого: пройдено " << passed << ", провалено " << failed << std::endl;
}
