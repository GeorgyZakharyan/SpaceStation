#include <iostream>
#include "Lab1Demo.h"
#include "Station.h"
#include "ModuleFactory.h"

void runLab1Demo()
{
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
}
