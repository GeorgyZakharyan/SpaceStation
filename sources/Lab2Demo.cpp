#include <iostream>
#include <memory>
#include <cstdlib>
#include <vector>
#include "Lab2Demo.h"
#include "CommandCenter.h"
#include "ModuleFactory.h"
#include "SpaceEnvironments.h"
#include "ExternalObjects.h"
#include "ResourcePackage.h"
#include "Damage.h"
#include "Operators.h"

static int idByName(const CommandCenter& center, const std::string& name)
{
    for (int i = 0; i < center.getModuleCount(); i++)
    {
        if (center.getModule(i)->getName() == name)
        {
            return center.getModule(i)->getId();
        }
    }
    return -1;
}

void runLab2Demo()
{
    std::cout << std::endl << "################ ЛАБОРАТОРНАЯ РАБОТА 2 ################" << std::endl;
    std::srand(7);    

    StandardModuleFactory standardFactory;
    AdvancedModuleFactory advancedFactory;

    // =====  1 =====
    std::cout << std::endl << "===== Сценарий 1. Командный центр, модули, окружение EarthOrbit =====" << std::endl;
    CommandCenter center("Альфа", 8, 1000, 200, 50);

    center.createResidentialModule(standardFactory, "Каюта экипажа", 0);
    center.createResidentialModule(advancedFactory, "Медпункт", 0);
    center.createEnergyModule(standardFactory, "Солнечная панель", 0);
    center.createEnergyModule(advancedFactory, "Реактор", 0);
    center.createResearchModule(standardFactory, "Биолаборатория", 0);
    center.createResearchModule(advancedFactory, "Физлаборатория", 0);

    std::cout << "-- Активируем все модули (центр получает уведомления)" << std::endl;
    for (int i = 0; i < center.getModuleCount(); i++)
    {
        center.getModule(i)->activate();
    }

    std::cout << "-- Устанавливаем окружение" << std::endl;
    center.setEnvironment(std::make_shared<EarthOrbit>());
    center.showStatistics();

    // =====  2 =====
    std::cout << std::endl << "===== Сценарий 2. Смена окружения на DeepSpace, эффекты через EnvironmentProxy =====" << std::endl;
    center.setEnvironment(std::make_shared<DeepSpace>());
    std::cout << "-- Ход 1" << std::endl;
    center.tick();
    std::cout << "-- Повторный вызов в том же ходу: прокси берёт результат из кеша" << std::endl;
    center.applyEnvironment();
    center.showStatistics();

    // =====  3 =====
    std::cout << std::endl << "===== Сценарий 3. Солнечная буря (operator-, стратегии по типам модулей) =====" << std::endl;
    SolarStorm storm(30);
    std::vector<int> ids;
    for (int i = 0; i < center.getModuleCount(); i++)
    {
        ids.push_back(center.getModule(i)->getId());
    }
    for (size_t i = 0; i < ids.size(); i++)
    {
        center.receiveObject(storm, ids[i]);
    }
    center.showStatistics();

    std::cout << "-- Метеоритный поток добивает солнечную панель (уведомления о разрушении)" << std::endl;
    MeteorShower meteors(25);
    int solarId = idByName(center, "Солнечная панель");
    while (center.findById(solarId) != nullptr)
    {
        center.receiveObject(meteors, solarId);
    }
    center.showStatistics();

    // ==== 4 =====
    std::cout << std::endl << "===== Сценарий 4. Грузовой контейнер (operator+) =====" << std::endl;
    CargoContainer container(50);
    center.receiveObject(container, idByName(center, "Реактор"));
    std::cout << "-- Цепочка: центр + пакет + пакет" << std::endl;
    center + ResourcePackage(10, 5, 0) + ResourcePackage(20, 0, 1);
    center.showStatistics();

    // ===== 5 =====
    std::cout << std::endl << "===== Сценарий 5. Космическая аномалия (operator*) =====" << std::endl;
    SpaceAnomaly anomaly;
    int labId = idByName(center, "Физлаборатория");
    std::cout << "-- Случайный эффект (эффективность лаборатории до: "
              << center.findById(labId)->getEfficiency() << "%)" << std::endl;
    center.receiveObject(anomaly, labId);
    std::cout << "-- Эффективность лаборатории после: " << center.findById(labId)->getEfficiency() << "%" << std::endl;

    std::cout << "-- Все три эффекта по очереди: 0 - телепортация, 1 - удвоение энергии, 2 - отключение" << std::endl;
    for (int effect = 0; effect < 3; effect++)
    {
        int efficiencyBefore = center.findById(labId)->getEfficiency();
        int energyBefore = center.getEnergy();
        anomaly.interactWithEffect(center, *center.findById(labId), effect);
        std::cout << "   эффект " << effect << ": эффективность модуля " << efficiencyBefore << "% -> "
                  << center.findById(labId)->getEfficiency() << "%, энергия станции " << energyBefore
                  << " -> " << center.getEnergy() << std::endl;
    }
    center.showStatistics();

    // ===== 6 =====
    std::cout << std::endl << "===== Сценарий 6. Астероидное поле: прокси блокирует исследовательские модули =====" << std::endl;
    center.setEnvironment(std::make_shared<AsteroidField>());
    std::cout << "-- Пробуем включить заблокированную лабораторию" << std::endl;
    center.findById(idByName(center, "Биолаборатория"))->activate();
    std::cout << "-- Следующий ход: заблокированные лаборатории прокси уже не трогает" << std::endl;
    center.tick();
    std::cout << "-- Возвращаемся на орбиту Земли: блокировка снимается" << std::endl;
    center.setEnvironment(std::make_shared<EarthOrbit>());
    center.findById(idByName(center, "Биолаборатория"))->activate();
    center.findById(idByName(center, "Физлаборатория"))->activate();

    std::cout << "-- Взаимодействие модулей: реактор берёт на себя 20 кВт нагрузки физлаборатории" << std::endl;
    center.transferEnergy(idByName(center, "Реактор"), idByName(center, "Физлаборатория"), 20);
    center.showStatistics();

    // ===== 7 =====
    std::cout << std::endl << "===== Сценарий 7. Копирование и перемещение командного центра =====" << std::endl;
    CommandCenter copy = center;
    std::cout << "-- Бьём модуль копии: уведомление получает только копия" << std::endl;
    int before = center.getEventCount();
    copy.receiveObject(storm, copy.getModule(0)->getId());
    std::cout << "Уведомлений у оригинала: было " << before << ", стало " << center.getEventCount() << std::endl;
    std::cout << "Уведомлений у копии: " << copy.getEventCount() << std::endl;

    CommandCenter moved = std::move(copy);
    std::cout << "-- После перемещения подписчиком модулей стал новый центр" << std::endl;
    int eventsBefore = moved.getEventCount();
    moved.receiveObject(storm, moved.getModule(0)->getId());
    std::cout << "Новый центр получил уведомлений: " << moved.getEventCount() - eventsBefore
              << ", модулей в старом центре: " << copy.getModuleCount() << std::endl;
}
