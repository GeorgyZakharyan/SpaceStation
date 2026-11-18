#include <iostream>
#include <memory>
#include <string>
#include "Lab2Demo.h"
#include "CommandCenter.h"
#include "ModuleFactory.h"
#include "SpaceEnvironments.h"
#include "EnvironmentProxy.h"
#include "ExternalObjects.h"
#include "Strategies.h"
#include "ResourcePackage.h"
#include "Damage.h"
#include "Operators.h"
#include "LoggingProxy.h"

static int passed2 = 0;
static int failed2 = 0;

static void check2(bool condition, const std::string& description)
{
    if (condition)
    {
        passed2++;
        std::cout << "  [OK]   " << description << std::endl;
    }
    else
    {
        failed2++;
        std::cout << "  [FAIL] " << description << std::endl;
    }
}

class TestModule : public Module
{
public:
    TestModule(const std::string& name) : Module(name, 100, 5, 1, 50) {}
    std::string getType() const override { return "TestModule"; }
    std::string getCategory() const override { return "Тестовый"; }
    std::unique_ptr<Module> clone() const override { return std::unique_ptr<Module>(new TestModule(*this)); }
};

class RecordingObserver : public IObserver
{
public:
    std::vector<EventType> types;
    std::vector<int> moduleIds;
    std::vector<int> damages;
    std::vector<int> timestamps;

    void onModuleEvent(const ModuleEvent& event) override
    {
        types.push_back(event.getType());
        moduleIds.push_back(event.getModuleId());
        damages.push_back(event.getDamage());
        timestamps.push_back(event.getTimestamp());
    }
};

class CountingEnvironment : public Environment
{
public:
    int calls;

    CountingEnvironment() : calls(0) {}
    void applyEffect(Module&) override { calls++; }
    std::string getName() const override { return "Счётчик"; }
    double getEnergyModifier() const override { return 2.0; }
    double getDamageModifier() const override { return 3.0; }
};

void runLab2Tests()
{
    std::cout << std::endl << "===== Тестирование ЛР 2 =====" << std::endl;
    LoggingProxy::setEnabled(false);        

    StandardModuleFactory sf;
    AdvancedModuleFactory af;

    std::cout << "-- Наблюдатель" << std::endl;
    {
        CommandCenter center("Тест", 5, 1000, 200, 50);
        center.createEnergyModule(sf, "Панель", 0);
        Module* m = center.getModule(0);

        m->takeDamage(10);
        check2(center.getEventCount() == 1, "центр получил уведомление о повреждении");
        check2(center.getSafetyLevel() == 49, "повреждение снижает уровень безопасности на 1");

        m->activate();
        check2(center.getEventCount() == 2, "центр получил уведомление об активации");
        m->activate();
        check2(center.getEventCount() == 2, "повторная активация не рассылает уведомление");

        m->consumeEnergy(30);
        check2(center.getEnergy() == 970, "расход энергии модулем уменьшает энергию центра");

        m->unsubscribe(&center);
        m->takeDamage(5);
        check2(center.getEventCount() == 3, "после отписки уведомления не приходят");
        m->subscribe(&center);

        std::unique_ptr<Module> clone = m->clone();
        int events = center.getEventCount();
        clone->takeDamage(5);
        check2(center.getEventCount() == events, "у клона модуля нет подписчиков оригинала");

        m->takeDamage(1000);
        check2(m->isDestroyed(), "модуль с нулевой прочностью разрушен");
        check2(center.getModuleCount() == 1, "разрушенный модуль удаляется не сразу, а после операции");
        center.removeDestroyedModules();
        check2(center.getModuleCount() == 0, "центр удалил разрушенный модуль из списка");
        check2(center.getSafetyLevel() == 43, "повреждение (-1) и разрушение (-5): безопасность 49 -> 48 -> 43");
    }
    {
        RecordingObserver recorder;
        std::unique_ptr<Module> m = sf.createEnergyModule("Панель", 0);
        m->subscribe(&recorder);
        m->subscribe(&recorder);        

        m->activate();
        m->takeDamage(30);
        m->consumeEnergy(7);
        m->takeDamage(1000);            

        check2(recorder.types.size() == 5, "модуль разослал ровно 5 событий (повторная подписка не дублирует)");
        check2(recorder.types.size() == 5 && recorder.types[0] == EVENT_ACTIVATED && recorder.types[1] == EVENT_DAMAGED
               && recorder.types[2] == EVENT_ENERGY_CONSUMED && recorder.types[3] == EVENT_DAMAGED
               && recorder.types[4] == EVENT_DESTROYED, "типы событий идут в правильном порядке");
        check2(recorder.damages.size() == 5 && recorder.damages[1] == 30 && recorder.damages[2] == 7,
               "в событиях верные числа: урон 30, расход энергии 7");
        check2(recorder.damages.size() == 5 && recorder.damages[3] == 70 && recorder.damages[4] == 70,
               "урон ограничен остатком прочности (70), в обоих событиях");
        bool sameId = true;
        for (size_t i = 0; i < recorder.moduleIds.size(); i++)
        {
            if (recorder.moduleIds[i] != m->getId())
            {
                sameId = false;
            }
        }
        check2(sameId && !recorder.moduleIds.empty(), "во всех событиях правильный id модуля");
        bool growing = true;
        for (size_t i = 1; i < recorder.timestamps.size(); i++)
        {
            if (recorder.timestamps[i] <= recorder.timestamps[i - 1])
            {
                growing = false;
            }
        }
        check2(growing, "номера (timestamp) событий строго растут");

        m->takeDamage(10);
        m->activate();
        check2(recorder.types.size() == 5, "разрушенный модуль не шлёт новых событий");

        RecordingObserver second;
        std::unique_ptr<Module> n = sf.createEnergyModule("Панель 2", 0);
        n->subscribe(&recorder);
        n->subscribe(&second);
        n->takeDamage(5);
        n->unsubscribe(&recorder);
        n->takeDamage(5);
        check2(second.types.size() == 2 && recorder.types.size() == 6,
               "два наблюдателя: второй получил 2 события, первый - только до отписки");
    }

    std::cout << "-- Прокси" << std::endl;
    {
        EnvironmentProxy earthProxy(std::make_shared<EarthOrbit>());
        std::unique_ptr<Module> crew = sf.createResidentialModule("Каюта", 0);
        earthProxy.applyEffect(*crew);
        check2(crew->getDurability().getValue() == 100, "EarthOrbit через прокси не наносит урон");
        check2(earthProxy.getName() == "Орбита Земли", "прокси возвращает название настоящего окружения");

        EnvironmentProxy deepProxy(std::make_shared<DeepSpace>());
        check2(deepProxy.getEnergyModifier() == 1.5, "DeepSpace: модификатор энергии 1.5 через прокси");
        deepProxy.applyEffect(*crew);
        check2(crew->getDurability().getValue() == 94, "DeepSpace бьёт жилой модуль на 6");
        deepProxy.applyEffect(*crew);
        check2(crew->getDurability().getValue() == 94, "повторный вызов в том же ходу берётся из кеша");
        check2(deepProxy.getCacheSize() == 1, "в кеше один результат");
        deepProxy.clearCache();
        deepProxy.applyEffect(*crew);
        check2(crew->getDurability().getValue() == 88, "после очистки кеша эффект применяется снова");

        std::shared_ptr<CountingEnvironment> counter(new CountingEnvironment());
        EnvironmentProxy countProxy(counter);
        check2(countProxy.getEnergyModifier() == 2.0 && countProxy.getDamageModifier() == 3.0
               && countProxy.getName() == "Счётчик", "прокси пробрасывает модификаторы и название настоящего окружения");
        check2(countProxy.getRealEnvironment() == counter, "прокси хранит именно переданное окружение");

        std::unique_ptr<Module> broken = sf.createEnergyModule("Сломанная", 0);
        broken->takeDamage(1000);
        countProxy.applyEffect(*broken);
        check2(counter->calls == 0 && countProxy.getCacheSize() == 0, "разрушенный модуль: прокси не вызвал окружение");

        std::unique_ptr<Module> repaired = sf.createEnergyModule("В ремонте", 0);
        repaired->setState(REPAIR);
        countProxy.applyEffect(*repaired);
        check2(counter->calls == 0 && countProxy.getCacheSize() == 0, "модуль в ремонте: прокси не вызвал окружение");

        std::unique_ptr<Module> blockedLab = sf.createResearchModule("Закрытая лаба", 0);
        blockedLab->setBlocked(true);
        countProxy.applyEffect(*blockedLab);
        check2(counter->calls == 0, "заблокированная лаборатория: прокси не вызвал окружение");

        std::unique_ptr<Module> okLab = sf.createResearchModule("Рабочая лаба", 0);
        int envLogs = LoggingProxy::getCount(LogSource::ENV);
        int centerLogs = LoggingProxy::getCount(LogSource::CENTER);
        countProxy.applyEffect(*okLab);
        check2(counter->calls == 1, "обычная лаборатория: прокси передал модуль окружению");
        check2(LoggingProxy::getCount(LogSource::ENV) == envLogs + 1 && LoggingProxy::getCount(LogSource::CENTER) == centerLogs,
               "прокси пишет в лог ровно одно сообщение с источником ENV");
        countProxy.applyEffect(*okLab);
        check2(counter->calls == 1, "повторный вызов: окружение не вызывалось (кеш)");
        countProxy.clearCache();
        countProxy.applyEffect(*okLab);
        check2(counter->calls == 2, "после очистки кеша окружение вызвано снова");

        EnvironmentProxy emptyProxy(nullptr);
        emptyProxy.applyEffect(*okLab);
        check2(emptyProxy.getName() == "нет окружения" && emptyProxy.getEnergyModifier() == 1.0,
               "прокси без окружения не падает");

        EnvironmentProxy asteroidProxy(std::make_shared<AsteroidField>());
        std::unique_ptr<Module> lab = sf.createResearchModule("Лаборатория", 0);
        lab->activate();
        asteroidProxy.applyEffect(*lab);
        check2(lab->isBlocked() && !lab->isActive(), "астероидное поле блокирует исследовательский модуль");
        int durabilityBlocked = lab->getDurability().getValue();
        asteroidProxy.clearCache();
        asteroidProxy.applyEffect(*lab);
        check2(lab->getDurability().getValue() == durabilityBlocked, "прокси не трогает заблокированный исследовательский модуль");
        lab->activate();
        check2(!lab->isActive(), "заблокированный модуль нельзя активировать");

        std::unique_ptr<Module> solar = sf.createEnergyModule("Панель", 0);
        asteroidProxy.applyEffect(*solar);
        check2(solar->getDurability().getValue() < 100 && !solar->isBlocked(),
               "астероидное поле снижает прочность, но не блокирует энергетический модуль");
    }
    {
        CommandCenter center("Тест", 5, 1000, 200, 50);
        center.createResearchModule(sf, "Лаборатория", 0);
        center.setEnvironment(std::make_shared<AsteroidField>());
        check2(center.getModule(0)->isBlocked(), "центр установил окружение, лаборатория заблокирована");
        center.setEnvironment(std::make_shared<EarthOrbit>());
        check2(!center.getModule(0)->isBlocked(), "смена окружения снимает блокировку");
        check2(center.getEnvironmentName() == "Орбита Земли", "центр знает название окружения");
    }

    std::cout << "-- Стратегия" << std::endl;
    {
        std::unique_ptr<Module> reactor = af.createEnergyModule("Реактор", 0);
        std::unique_ptr<Module> solar = sf.createEnergyModule("Панель", 0);
        std::unique_ptr<Module> crew = sf.createResidentialModule("Каюта", 0);
        std::unique_ptr<Module> lab = sf.createResearchModule("Лаборатория", 0);
        TestModule other("Новый тип");

        check2(chooseStrategy(*reactor, 10)->getName() == "ReactorModuleStrategy", "для реактора выбрана ReactorModuleStrategy");
        check2(chooseStrategy(*solar, 10)->getName() == "EnergyModuleStrategy", "для панели выбрана EnergyModuleStrategy");
        check2(chooseStrategy(*crew, 10)->getName() == "CrewModuleStrategy", "для каюты выбрана CrewModuleStrategy");
        check2(chooseStrategy(*lab, 10)->getName() == "ResearchModuleStrategy", "для лаборатории выбрана ResearchModuleStrategy");
        check2(chooseStrategy(other, 10)->getName() == "DefaultModuleStrategy", "для нового типа выбрана DefaultModuleStrategy");

        chooseStrategy(*reactor, 40)->apply(*reactor);
        chooseStrategy(*solar, 40)->apply(*solar);
        chooseStrategy(*crew, 40)->apply(*crew);
        chooseStrategy(*lab, 40)->apply(*lab);
        check2(reactor->getDurability().getValue() == 90, "реактор получил 10 урона");
        check2(solar->getDurability().getValue() == 80 && solar->getEfficiency() == 49, "панель: 20 урона и сбой электроники (70% -> 49%)");
        check2(crew->getDurability().getValue() == 60 && crew->getEfficiency() == 72, "каюта: 40 урона");
        check2(lab->getDurability().getValue() == 80 && lab->getEfficiency() == 60, "лаборатория: 20 урона, эффективность 60%");
    }

    std::cout << "-- Перегрузка операторов" << std::endl;
    {
        CommandCenter center("Тест", 5, 100, 50, 50);
        center + ResourcePackage(20, 10, 5);
        check2(center.getEnergy() == 120 && center.getOxygen() == 60 && center.getSafetyLevel() == 55,
               "operator+ добавляет энергию, кислород и безопасность");
        center + ResourcePackage(1, 1, 0) + ResourcePackage(2, 2, 0);
        check2(center.getEnergy() == 123, "operator+ можно писать цепочкой");
        center + ResourcePackage(0, 0, 100);
        check2(center.getSafetyLevel() == 100, "безопасность не превышает 100");

        std::unique_ptr<Module> m = sf.createEnergyModule("Панель", 0);
        Module& result = *m - Damage(30, "тест");
        check2(m->getDurability().getValue() == 70, "operator- снижает прочность");
        check2(&result == m.get(), "operator- возвращает тот же модуль");
        *m - Damage(500, "тест");
        check2(m->getDurability().getValue() == 0, "прочность не уходит ниже нуля");

        std::unique_ptr<Module> m2 = sf.createEnergyModule("Панель 2", 0);
        *m2 * 0.5;
        check2(m2->getEfficiency() == 35, "operator* уменьшает эффективность (70 -> 35)");
        *m2 * 10;
        check2(m2->getEfficiency() == 100, "operator* не даёт превысить 100%");
    }

    std::cout << "-- Внешние объекты" << std::endl;
    {
        CommandCenter center("Тест", 5, 1000, 200, 50);
        center.createEnergyModule(af, "Реактор", 0);
        center.createResidentialModule(sf, "Каюта", 0);
        int reactorId = center.getModule(0)->getId();
        int crewId = center.getModule(1)->getId();

        check2(center.calculateDamage(10) == 7, "урон 10 при безопасности 50 снижается до 7");
        CommandCenter safer("Тест", 5, 1000, 200, 51);
        check2(center.calculateDamage(100) == 75 && safer.calculateDamage(100) == 74,
               "безопасность 50 и 51 дают разный урон (75 и 74), без потери половинки");
        center.setEnvironment(std::make_shared<DeepSpace>());
        check2(center.calculateDamage(10) == 11, "в глубоком космосе урон 10 растёт до 11");
        center.setEnvironment(std::make_shared<EarthOrbit>());

        SolarStorm storm(30);
        center.receiveObject(storm, crewId);
        check2(center.getModule(1)->getDurability().getValue() < 100, "солнечная буря повредила модуль");
        check2(center.getRadiation() == 3, "солнечная буря повысила радиацию");

        MeteorShower meteors(30);
        int before = center.getModule(0)->getDurability().getValue();
        center.receiveObject(meteors, reactorId);
        check2(center.getModule(0)->getDurability().getValue() < before, "метеоритный поток снизил прочность");

        CargoContainer container(50);
        int energyBefore = center.getEnergy();
        center.receiveObject(container, reactorId);
        check2(center.getEnergy() == energyBefore + 100, "контейнер добавил энергию");
        check2(center.getOxygen() == 250, "контейнер добавил кислород");

        SpaceAnomaly anomaly;
        int count = center.getModuleCount();
        anomaly.interactWithEffect(center, *center.findById(crewId), 0);
        check2(center.getModuleCount() == count, "телепортация не теряет модули");
        int energyNow = center.getEnergy();
        anomaly.interactWithEffect(center, *center.findById(crewId), 1);
        check2(center.getEnergy() == energyNow * 2, "аномалия удвоила энергию станции");
        anomaly.interactWithEffect(center, *center.findById(crewId), 2);
        check2(center.findById(crewId)->getEfficiency() == 0 && !center.findById(crewId)->isActive(),
               "аномалия отключила модуль");
        check2(!center.receiveObject(storm, 99999), "объект не попадает в несуществующий модуль");

        std::cout << "  [..]   метеориты разрушают модуль до конца:" << std::endl;
        while (center.findById(reactorId) != nullptr)
        {
            center.receiveObject(meteors, reactorId);
        }
        check2(center.findById(reactorId) == nullptr && center.getModuleCount() == 1,
               "разрушенный метеоритами модуль удалён из центра");
    }

    std::cout << "-- Случайная аномалия и логирование" << std::endl;
    {
        int teleports = 0, doublings = 0, shutdowns = 0;
        bool allValid = true;
        for (int i = 0; i < 60; i++)
        {
            CommandCenter center("Тест", 5, 1000, 200, 50);
            center.createResearchModule(af, "Лаборатория", 0);
            center.createResidentialModule(sf, "Каюта", 0);
            center.getModule(0)->activate();
            int labId = center.getModule(0)->getId();
            SpaceAnomaly anomaly;
            center.receiveObject(anomaly, labId);

            Module* lab = center.findById(labId);
            if (center.getEnergy() == 2000)
            {
                doublings++;
            }
            else if (!lab->isActive() && lab->getEfficiency() == 0)
            {
                shutdowns++;
            }
            else if (lab->getEfficiency() == 77 && lab->isActive())     // 85 * 0.9 = 76.5 -> 77
            {
                teleports++;
            }
            else
            {
                allValid = false;
            }
            if (center.getModuleCount() != 2)
            {
                allValid = false;
            }
        }
        check2(allValid, "случайная аномалия всегда даёт один из трёх эффектов и не теряет модули");
        check2(teleports > 0 && doublings > 0 && shutdowns > 0, "за 60 попыток встретились все три эффекта");
    }
    {
        CommandCenter center("Тест", 5, 1000, 200, 50);
        center.createEnergyModule(sf, "Панель", 0);
        Module* m = center.getModule(0);

        int module0 = LoggingProxy::getCount(LogSource::MODULE);
        int center0 = LoggingProxy::getCount(LogSource::CENTER);
        int env0 = LoggingProxy::getCount(LogSource::ENV);
        int object0 = LoggingProxy::getCount(LogSource::OBJECT);

        m->activate();
        check2(LoggingProxy::getCount(LogSource::MODULE) == module0 + 1, "активация: одно сообщение от MODULE");
        check2(LoggingProxy::getCount(LogSource::CENTER) == center0 + 1, "активация: одно сообщение от CENTER (уведомление)");

        module0 = LoggingProxy::getCount(LogSource::MODULE);
        center0 = LoggingProxy::getCount(LogSource::CENTER);
        m->takeDamage(5);
        check2(LoggingProxy::getCount(LogSource::MODULE) == module0 + 1, "повреждение: сообщение от MODULE");
        check2(LoggingProxy::getCount(LogSource::CENTER) == center0 + 1, "повреждение: сообщение от CENTER");

        module0 = LoggingProxy::getCount(LogSource::MODULE);
        center0 = LoggingProxy::getCount(LogSource::CENTER);
        m->consumeEnergy(5);
        check2(LoggingProxy::getCount(LogSource::CENTER) == center0 + 1 && LoggingProxy::getCount(LogSource::MODULE) == module0,
               "расход энергии: сообщение от CENTER");

        m->setBlocked(true);
        check2(LoggingProxy::getCount(LogSource::MODULE) == module0 + 1, "блокировка: сообщение от MODULE");
        module0 = LoggingProxy::getCount(LogSource::MODULE);
        m->activate();
        check2(LoggingProxy::getCount(LogSource::MODULE) == module0 + 1, "отказ активации: сообщение от MODULE");
        m->setBlocked(false);

        center0 = LoggingProxy::getCount(LogSource::CENTER);
        center + ResourcePackage(1, 1, 1);
        check2(LoggingProxy::getCount(LogSource::CENTER) == center0 + 1, "получение ресурсов: сообщение от CENTER");

        env0 = LoggingProxy::getCount(LogSource::ENV);
        center.setEnvironment(std::make_shared<DeepSpace>());
        check2(LoggingProxy::getCount(LogSource::ENV) > env0, "окружение и прокси пишут в лог от ENV");

        object0 = LoggingProxy::getCount(LogSource::OBJECT);
        MeteorShower meteors(30);
        center.receiveObject(meteors, m->getId());
        check2(LoggingProxy::getCount(LogSource::OBJECT) == object0 + 1, "внешний объект пишет в лог от OBJECT");

        center0 = LoggingProxy::getCount(LogSource::CENTER);
        center.removeById(m->getId());
        check2(LoggingProxy::getCount(LogSource::CENTER) == center0 + 1, "удаление модуля: сообщение от CENTER");

        check2(LoggingProxy::getCount() == LoggingProxy::getCount(LogSource::ENV) + LoggingProxy::getCount(LogSource::OBJECT)
               + LoggingProxy::getCount(LogSource::MODULE) + LoggingProxy::getCount(LogSource::CENTER),
               "общий счётчик равен сумме по четырём источникам");
    }

    std::cout << "-- Командный центр" << std::endl;
    {
        CommandCenter center("Тест", 3, 1000, 200, 50);
        check2(center.createEnergyModule(sf, "Панель 1", 0), "создание энергетического модуля через фабрику");
        check2(center.createEnergyModule(sf, "Панель 2", 0), "создание второго модуля");
        check2(center.addModule(std::unique_ptr<Module>(new TestModule("Новый"))), "центр принимает модуль нового типа");
        check2(!center.createResidentialModule(sf, "Лишний", 0), "превышение максимума отклоняется");
        check2(center.getModuleCount() == 3, "в центре 3 модуля");
        check2(!center.addModule(nullptr), "пустой указатель отклоняется");

        Module* panel1 = center.getModule(0);
        Module* panel2 = center.getModule(1);
        check2(panel1->getTypeInfo() == panel2->getTypeInfo() && panel1->getTypeInfo() != nullptr,
               "Легковес: два SolarModule ссылаются на один объект с информацией о типе");
        check2(panel1->getTypeInfo()->getMaxHealth() == 100 && panel1->getTypeInfo()->getBaseEnergyConsumption() == 5
               && panel1->getTypeInfo()->getBaseEfficiency() == 70 && panel1->getTypeInfo()->getCost() == 150,
               "в легковесе верные характеристики SolarModule");
        check2(center.getModule(2)->getTypeInfo() != panel1->getTypeInfo(), "у другого типа - своё описание");

        check2(center.findById(panel2->getId()) == panel2, "findById находит модуль");
        check2(center.findById(-1) == nullptr, "findById не находит несуществующий");
        check2(center.removeById(panel2->getId()) && center.getModuleCount() == 2, "removeById удаляет модуль");
        check2(!center.removeById(-1), "removeById с неверным id отклоняется");
    }
    {
        CommandCenter center("Тест", 5, 1000, 200, 50);
        center.createEnergyModule(af, "Реактор", 0);
        center.createResidentialModule(sf, "Каюта", 0);
        center.getModule(0)->activate();
        center.getModule(1)->activate();
        center.tick();
        check2(center.getEnergy() == 975, "ход на орбите: 15 + 10 = 25 энергии");
        check2(center.getOxygen() == 196, "ход: каюта потратила 4 кислорода");

        center.setEnvironment(std::make_shared<DeepSpace>());
        int energyBefore = center.getEnergy();
        center.tick();
        check2(energyBefore - center.getEnergy() == 22 + 15, "в глубоком космосе расход энергии x1.5");

        CommandCenter poor("Бедный", 5, 10, 200, 50);
        poor.createResearchModule(af, "Лаборатория", 0);
        poor.getModule(0)->activate();
        poor.tick();
        check2(poor.getEnergy() == 0 && !poor.getModule(0)->isActive(), "без энергии модули отключаются");

        CommandCenter airless("Без воздуха", 5, 1000, 3, 50);
        airless.createResidentialModule(sf, "Каюта", 0);
        airless.getModule(0)->activate();
        airless.tick();
        check2(airless.getOxygen() == 0, "кислород не уходит ниже нуля");
    }
    {
        CommandCenter center("Тест", 5, 1000, 200, 50);
        center.createEnergyModule(af, "Реактор", 0);
        center.createResearchModule(af, "Физлаборатория", 0);
        int reactorId = center.getModule(0)->getId();
        int labId = center.getModule(1)->getId();

        check2(!center.transferEnergy(reactorId, labId, 20), "неактивный модуль не может передавать энергию");
        center.getModule(0)->activate();
        check2(center.transferEnergy(reactorId, labId, 20), "активный реактор взял нагрузку лаборатории");
        check2(center.getModule(1)->getEnergy().getValue() == 30 && center.getModule(0)->getEnergy().getValue() == 35,
               "нагрузка перешла: лаборатория 50 -> 30, реактор 15 -> 35");
        check2(center.getEnergy() == 1000, "передача нагрузки не списывает энергию станции сразу");

        center.getModule(1)->activate();
        center.tick();
        check2(center.getEnergy() == 1000 - (35 + 30), "за ход списана суммарная нагрузка 35 + 30, ровно один раз");
        check2(!center.transferEnergy(reactorId, reactorId, 5), "модуль не передаёт энергию сам себе");
        check2(!center.transferEnergy(reactorId, -1, 5), "передача несуществующему модулю отклоняется");
    }

    std::cout << "-- Копирование и перемещение центра" << std::endl;
    {
        CommandCenter original("Оригинал", 5, 500, 100, 40);
        original.createEnergyModule(sf, "Панель", 0);
        original.createResidentialModule(sf, "Каюта", 0);
        original.setEnvironment(std::make_shared<DeepSpace>());

        CommandCenter copy = original;
        check2(copy.getModuleCount() == 2 && copy.getEnergy() == 500, "копия содержит те же модули и характеристики");
        check2(copy.getModule(0) != original.getModule(0), "модули копии - другие объекты");
        check2(copy.getEnvironmentName() == "Глубокий космос", "копия сохранила окружение");

        int originalEvents = original.getEventCount();
        copy.getModule(0)->takeDamage(5);
        check2(copy.getEventCount() == originalEvents + 1 && original.getEventCount() == originalEvents,
               "модули копии уведомляют только копию");
        check2(original.getModule(0)->getDurability().getValue() > copy.getModule(0)->getDurability().getValue(),
               "изменение копии не влияет на оригинал");

        CommandCenter assigned("Другой", 1, 1, 1, 1);
        assigned = original;
        check2(assigned.getModuleCount() == 2 && assigned.getMaxModules() == 5, "присваивание копированием");
        CommandCenter& selfRef = assigned;
        assigned = selfRef;
        check2(assigned.getModuleCount() == 2, "самоприсваивание не ломает центр");

        Module* first = original.getModule(0);
        CommandCenter moved = std::move(original);
        check2(moved.getModule(0) == first && original.getModuleCount() == 0, "перемещение: те же модули, источник пуст");
        int movedEvents = moved.getEventCount();
        moved.getModule(0)->takeDamage(5);
        check2(moved.getEventCount() == movedEvents + 1, "после перемещения модули уведомляют новый центр");

        CommandCenter target("Цель", 1, 1, 1, 1);
        target = std::move(moved);
        int targetEvents = target.getEventCount();
        target.getModule(0)->takeDamage(5);
        check2(target.getModuleCount() == 2 && moved.getModuleCount() == 0, "присваивание перемещением");
        check2(target.getEventCount() == targetEvents + 1, "после присваивания перемещением уведомления идут в цель");
    }

    std::cout << "-- Логирование" << std::endl;
    int logsBefore = LoggingProxy::getCount();
    LoggingProxy::log(LogSource::ENV, "проверка");
    check2(LoggingProxy::getCount() == logsBefore + 1, "LoggingProxy считает сообщения даже в тихом режиме");
    check2(LoggingProxy::getSourceName(LogSource::CENTER) == "CENTER", "имя источника CENTER");

    LoggingProxy::setEnabled(true);
    std::cout << std::endl << "Итого ЛР 2: пройдено " << passed2 << ", провалено " << failed2 << std::endl;
}
