#include <iostream>
#include <memory>
#include <string>
#include "Station.h"
#include "ModuleFactory.h"

#ifdef _WIN32
#include <windows.h>
#endif

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

static void runTests()
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

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    StandardModuleFactory standardFactory;
    AdvancedModuleFactory advancedFactory;

    // ===== 1 =====
    std::cout << "===== Сценарий 1. Создание станции, добавление модулей через разные фабрики =====" << std::endl;
    Station station("Орбита-1", 10);

    station.addModule(standardFactory.createResidentialModule("Каюта экипажа", 0));
    station.addModule(advancedFactory.createResidentialModule("Медпункт", 0));
    station.addModule(advancedFactory.createResidentialModule("Коворкинг", 1));
    station.addModule(standardFactory.createResidentialModule("Столовая", 1));
    station.addModule(standardFactory.createEnergyModule("Солнечная панель", 0));
    station.addModule(advancedFactory.createEnergyModule("Реактор", 0));
    station.addModule(standardFactory.createEnergyModule("Резервный генератор", 1));
    station.addModule(standardFactory.createResearchModule("Биолаборатория", 0));
    station.addModule(advancedFactory.createResearchModule("Физлаборатория", 0));
    station.addModule(advancedFactory.createResearchModule("Астролаборатория", 1));

    std::cout << "Список модулей через итератор:" << std::endl;
    for (StationIterator it = station.getIterator(); it != station.end(); ++it)
    {
        Module& m = *it;
        std::cout << "  " << m.getId() << ": " << m.getName() << " (" << m.getType() << ")" << std::endl;
    }
    station.showAll();

    // ===== 2 =====
    std::cout << std::endl << "===== Сценарий 2. Копирование станции =====" << std::endl;
    Station copyStation = station;  
    copyStation.getModule(0)->setEfficiency(10);
    copyStation.removeByIndex(1);
    std::cout << "Копия (изменена):" << std::endl;
    copyStation.showAll();
    std::cout << "Оригинал (не изменился):" << std::endl;
    station.showAll();

    Station assigned("Временная", 2);
    assigned = station;  
    std::cout << "После присваивания копированием:" << std::endl;
    assigned.showAll();

    // ===== 3 =====
    std::cout << std::endl << "===== Сценарий 3. Перемещение станции =====" << std::endl;
    Station source = station;                      
    Module* firstBefore = source.getModule(0);     
    Station movedStation = std::move(source);      
    std::cout << "Перемещённая станция:" << std::endl;
    movedStation.showAll();
    std::cout << "Старая станция после перемещения: модулей "
              << source.getCount() << ", максимум " << source.getMax() << std::endl;
    std::cout << "Первый модуль - тот же объект (копирования не было): "
              << (movedStation.getModule(0) == firstBefore ? "да" : "нет") << std::endl;

    Station another("Другая", 3);
    another = std::move(movedStation);             
    std::cout << "После присваивания перемещением:" << std::endl;
    another.showAll();
    std::cout << "movedStation теперь содержит " << movedStation.getCount() << " модулей" << std::endl;

    // ===== 4 =====
    std::cout << std::endl << "===== Сценарий 4. Итерация, активация, смена состояния, перемещение =====" << std::endl;
    std::cout << "-- Активация всех модулей (свой итератор)" << std::endl;
    for (StationIterator it = station.getIterator(); it != station.end(); ++it)
    {
        Module& m = *it;
        m.activate();
        m.showInfo();
    }

    std::cout << "-- Деактивация энергетических модулей (range-for)" << std::endl;
    for (auto& m : station)
    {
        if (m.getCategory() == "Энергетический")
        {
            m.deactivate();
        }
    }
    station.showAll();

    std::cout << "-- Исследовательские модули переводим в ремонт" << std::endl;
    for (auto& m : station)
    {
        if (m.getCategory() == "Исследовательский")
        {
            m.setState(REPAIR);
        }
    }
    station.showAll();

    std::cout << "-- Попытка активировать модуль в ремонте" << std::endl;
    station.getModule(station.getCount() - 2)->activate();

    Module* firstModule = station.getModule(0);
    firstModule->getDurability().decrease(40);
    std::cout << "Прочность после повреждения: " << firstModule->getDurability().getValue() << std::endl;
    firstModule->getDurability().repair(15);
    std::cout << "Прочность после ремонта: " << firstModule->getDurability().getValue() << std::endl;
    firstModule->setState(EMERGENCY);
    firstModule->showInfo();

    std::cout << "-- Перемещение первого модуля в конец станции" << std::endl;
    station.moveModule(0, station.getCount() - 1);
    station.showAll();

    std::cout << "-- Сортировка копии по эффективности" << std::endl;
    Station sortedCopy = station;
    sortedCopy.sortByEfficiency();
    sortedCopy.showAll();

    // ===== 5 =====
    std::cout << std::endl << "===== Сценарий 5. Удаление по индексу и id, ограничение максимума =====" << std::endl;
    std::cout << "Удаляем модуль с индексом 0: " << station.getModule(0)->getName() << std::endl;
    station.removeByIndex(0);
    int idToRemove = station.getModule(1)->getId();
    std::cout << "Удаляем модуль с id=" << idToRemove << ": " << station.findById(idToRemove)->getName() << std::endl;
    station.removeById(idToRemove);
    station.removeById(100);        
    station.removeByIndex(50);      
    station.showAll();

    std::cout << "-- Заполняем станцию до максимума и пробуем добавить лишний модуль" << std::endl;
    while (station.getCount() < (int)station.getMax())
    {
        station.addModule(standardFactory.createResearchModule("Мастерская", 1));
    }
    station.addModule(standardFactory.createResearchModule("Лишний модуль", 1));
    station.showAll();

    runTests();

    std::cout << std::endl << "Конец программы" << std::endl;
    return 0;
}