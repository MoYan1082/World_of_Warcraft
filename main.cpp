#include <list>
#include <array>
#include <cmath>
#include <queue>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include <iostream>

// 武士编号
#define DRAGON 0
#define NINJA 1
#define ICEMAN 2
#define LION 3
#define WOLF 4

// 武器编号
#define SWORD 0
#define BOMB 1
#define ARROW 2

// 全局变量
int initialElement, powerOfArrow, loyaltyDecay, endTime;
int gHours, gMinutes, gCitySize;
bool redWin, blueWin;
std::array<int, 5> gCost, gAttack;

namespace {
inline void Log(std::string& out) {  // 全局日志
    std::cout << out << "\n";
}
inline std::string GetTime() {
    std::string res = std::to_string(gHours);
    while (res.length() < 3) res = "0" + res;
    res += ':';
    if (gMinutes >= 10)
        res += std::to_string(gMinutes);
    else
        res += "0" + std::to_string(gMinutes);
    return res;
}
inline std::string GetSoldierName(int id) {
    switch (id) {
        case 0:
            return "dragon";
        case 1:
            return "ninja";
        case 2:
            return "iceman";
        case 3:
            return "lion";
        case 4:
            return "wolf";
        default:
            assert(false);
    }
    return "Undefine";
}
}  // namespace

class Weapon {
   public:
    int type, power, durability;
    virtual void Wear() = 0;
};
class Sword : public Weapon {
   public:
    Sword(int power_ = 0) {
        type = SWORD;
        power = 0.2 * power_;
        durability = power;
    }
    void Wear() override {
        if (durability <= 0) return;
        power = 0.8 * power;
        durability = power;
    }
};
class Arrow : public Weapon {
   public:
    Arrow() {
        power = 0;
        type = ARROW;
        durability = 3;
    }
    void Wear() override {
        if (durability <= 0) return;
        durability--;
    }
};
class Bomb : public Weapon {
   public:
    Bomb() {
        power = 0;
        type = BOMB;
        durability = 1;
    }
    void Wear() {
        if (durability <= 0) return;
        durability--;
    }
};
std::shared_ptr<Weapon> GetWeaponById(int id, int power = 0) {
    switch (id) {
        case SWORD:
            return std::make_shared<Sword>(power);
        case ARROW:
            return std::make_shared<Arrow>();
        case BOMB:
            return std::make_shared<Bomb>();
        default:
            assert(false);
    }
    return nullptr;
}

class Warrior {
   public:
    std::array<std::shared_ptr<Weapon>, 3> weapons;
    int id, elements, sit, power, type;
    int campId;
    bool winOneFight;

    Warrior(int id_, int type_, int campId_)
        : id(id_), type(type_), sit(-1), campId(campId_), winOneFight(false) {
        elements = gCost[type_];
        power = gAttack[type_];
    }
    void DropWeapon() {
        for (int i = 0; i < 3; ++i)
            if (weapons[i] && weapons[i]->durability <= 0) weapons[i] = nullptr;
    }
    int Power() {
        int allPower = power;
        if (weapons[SWORD]) allPower += weapons[SWORD]->power;
        return allPower;
    }
    virtual bool LionLeave() { return false; }
    virtual int March() { return 1; }
    virtual void Report() {
        std::string out = "";
        out += GetTime() + " " + ((campId == 0) ? "red" : "blue") + " " +
               GetSoldierName(type) + " " + std::to_string(id) + " has ";
        int cnt = 0;
        for (int i = 0; i < 3; ++i)
            if (weapons[i]) cnt++;
        if (cnt == 0) {
            out += "no weapon";
        } else {
            bool flag = false;
            if (weapons[ARROW]) {
                flag = true;
                out +=
                    "arrow(" + std::to_string(weapons[ARROW]->durability) + ")";
            }
            if (weapons[BOMB]) {
                if (flag)
                    out += ",";
                else
                    flag = true;
                out += "bomb";
            }
            if (weapons[SWORD]) {
                if (flag)
                    out += ",";
                else
                    flag = true;
                out += "sword(" + std::to_string(weapons[SWORD]->power) + ")";
            }
        }
        Log(out);
    }
    virtual void FightBack(Warrior* enemy) {
        if (elements <= 0) return;
        int allPower = power / 2;
        if (weapons[SWORD]) {
            allPower += weapons[SWORD]->power;
            weapons[SWORD]->Wear();
        }
        enemy->elements -= allPower;
        std::string out = "";
        out += GetTime() + " " + ((campId == 0) ? "red" : "blue") + " " +
               GetSoldierName(type) + " " + std::to_string(id) +
               " fought back against " + ((campId == 1) ? "red" : "blue") +
               " " + GetSoldierName(enemy->type) + " " +
               std::to_string(enemy->id) + " in city " + std::to_string(sit);
        Log(out);
    }
    void Attack(std::shared_ptr<Warrior> enemy) {
        if (elements <= 0 || enemy->elements <= 0) return;
        std::string out = "";
        out += GetTime() + " " + ((campId == 0) ? "red" : "blue") + " " +
               GetSoldierName(type) + " " + std::to_string(id) + " attacked " +
               ((campId == 1) ? "red" : "blue") + " " +
               GetSoldierName(enemy->type) + " " + std::to_string(enemy->id) +
               " in city " + std::to_string(sit) + " with " +
               std::to_string(elements) + " elements and force " +
               std::to_string(power);
        Log(out);

        int enemyElementsBefore = enemy->elements;
        int ownElementBefore = elements;
        enemy->elements -= this->Power();
        if (weapons[SWORD]) weapons[SWORD]->Wear();
        if (enemy->elements <= 0) {  // 获胜
            std::string out = "";
            out += GetTime() + " " + ((enemy->campId == 0) ? "red" : "blue") +
                   " " + GetSoldierName(enemy->type) + " " +
                   std::to_string(enemy->id) + " was killed in city " +
                   std::to_string(sit);
            Log(out);
            if (enemy->type == LION) elements += enemyElementsBefore;
        } else {
            enemy->FightBack(this);
            if (elements <= 0) {  // 失败
                if (type == LION) enemy->elements += ownElementBefore;
                std::string out = "";
                out += GetTime() + " " +
                       ((this->campId == 0) ? "red" : "blue") + " " +
                       GetSoldierName(this->type) + " " +
                       std::to_string(this->id) + " was killed in city " +
                       std::to_string(sit);
                Log(out);
            }
        }
    }
    virtual void AttackAfter(std::shared_ptr<Warrior> enemy) { /*战斗后的行为*/
    }
};
class Dragon : public Warrior {
   public:
    double moral;
    Dragon(int id_, int campId_, double moral_)
        : Warrior(id_, DRAGON, campId_) {
        moral = moral_;
        weapons[id % 3] = GetWeaponById(id % 3, power);
        /*
         * 题目中说士气需要“四舍五入到小数点后两位”，
         * 但是我在std的输出中发现，std可能并没有按照四舍五入来进行，
         * 所以我就换成了下面这种写法，并且通过了代码
         */
        std::string out = "";
        out += GetTime() + " " + ((campId_ == 0) ? "red" : "blue") +
               " dragon " + std::to_string(id) + " born\n";
        out += "Its morale is ";
        std::cout << out;
        printf("%.2f\n", moral);
    }
    void Yell() {
        std::string out = "";
        out += GetTime() + " " + ((campId == 0) ? "red" : "blue") + " dragon " +
               std::to_string(id) + " yelled in city " + std::to_string(sit);
        Log(out);
    }
    void AttackAfter(std::shared_ptr<Warrior> enemy) override {
        if (elements <= 0) return;
        if (enemy->elements <= 0)
            moral += 0.2;
        else
            moral -= 0.2;
        if (moral > 0.8) Yell();
    }
};
class Ninja : public Warrior {
   public:
    Ninja(int id_, int campId_) : Warrior(id_, NINJA, campId_) {
        weapons[id % 3] = GetWeaponById(id % 3, power);
        weapons[(id + 1) % 3] = GetWeaponById((id + 1) % 3, power);
        std::string out = "";
        out += GetTime() + " " + ((campId_ == 0) ? "red" : "blue") + " ninja " +
               std::to_string(id) + " born";
        Log(out);
    }
    void FightBack(Warrior* enemy) override {}
};
class Iceman : public Warrior {
   public:
    bool twoStep;
    Iceman(int id_, int campId_) : Warrior(id_, ICEMAN, campId_) {
        weapons[id % 3] = GetWeaponById(id % 3, power);
        std::string out = "";
        out += GetTime() + " " + ((campId_ == 0) ? "red" : "blue") +
               " iceman " + std::to_string(id) + " born";
        Log(out);
        twoStep = false;
    }
    int March() override {
        if (twoStep) {
            elements = std::max(1, elements - 9);
            power += 20;
            twoStep = false;
        } else {
            twoStep = true;
        }
        return 1;
    }
};
class Lion : public Warrior {
   public:
    int loyalty;
    Lion(int id_, int campId_, int restElements) : Warrior(id_, LION, campId_) {
        loyalty = restElements;
        std::string out = "";
        out += GetTime() + " " + ((campId_ == 0) ? "red" : "blue") + " lion " +
               std::to_string(id) + " born\n";
        out += "Its loyalty is " + std::to_string(loyalty);
        Log(out);
    }
    bool LionLeave() override {
        if (loyalty > 0) return false;
        std::string out = "";
        out += GetTime() + " " + ((campId == 0) ? "red" : "blue") + " lion " +
               std::to_string(id) + " ran away";
        Log(out);
        return true;
    }
    void AttackAfter(std::shared_ptr<Warrior> enemy) override {
        if (elements <= 0) return;
        if (enemy->elements > 0) loyalty -= loyaltyDecay;
    }
    void FightBack(Warrior* enemy) override {
        Warrior::FightBack(enemy);
        if (elements <= 0) return;
        if (enemy->elements > 0) loyalty -= loyaltyDecay;
    }
};
class Wolf : public Warrior {
   public:
    Wolf(int id_, int campId_) : Warrior(id_, WOLF, campId_) {
        std::string out = "";
        out += GetTime() + " " + ((campId_ == 0) ? "red" : "blue") + " wolf " +
               std::to_string(id) + " born";
        Log(out);
    }
    void AttackAfter(std::shared_ptr<Warrior> enemy) override {
        if (elements <= 0) return;
        if (enemy->elements <= 0) {
            for (int i = 0; i < 3; i++) {
                if (weapons[i] == nullptr || weapons[i]->durability <= 0)
                    weapons[i] = enemy->weapons[i];
            }
        }
    }
    void FightBack(Warrior* enemy) override {
        Warrior::FightBack(enemy);
        if (elements <= 0) return;
        if (enemy->elements <= 0) {
            for (int i = 0; i < 3; i++) {
                if (weapons[i] == nullptr || weapons[i]->durability <= 0)
                    weapons[i] = enemy->weapons[i];
            }
        }
    }
};

class Camp {
   public:
    int pos, elements, campId, warriorSize;
    std::array<int, 5> soldierOrd;
    Camp() {
        elements = initialElement;
        pos = 0;
        warriorSize = 0;
    }
    int Get() { return soldierOrd[pos]; }
    void Next() { pos = (pos + 1) % 5; }
    bool HaveNext() { return elements >= gCost[soldierOrd[pos]]; }
    std::shared_ptr<Warrior> Born(int warriorId_) {
        switch (warriorId_) {
            case DRAGON:
                return std::make_shared<Dragon>(
                    ++warriorSize, campId,
                    1.0 * (elements - gCost[warriorId_]) / gCost[warriorId_]);
            case NINJA:
                return std::make_shared<Ninja>(++warriorSize, campId);
            case ICEMAN:
                return std::make_shared<Iceman>(++warriorSize, campId);
            case LION:
                return std::make_shared<Lion>(++warriorSize, campId,
                                              elements - gCost[warriorId_]);
            case WOLF:
                return std::make_shared<Wolf>(++warriorSize, campId);
            default:
                assert(false);
        }
        assert(false);
        return nullptr;
    }
};
class RedCamp : public Camp {
   public:
    RedCamp() {
        campId = 0;
        soldierOrd = {ICEMAN, LION, WOLF, NINJA, DRAGON};
    }
};
class BlueCamp : public Camp {
   public:
    BlueCamp() {
        campId = 1;
        soldierOrd = {LION, DRAGON, NINJA, ICEMAN, WOLF};
    }
};

class City {
   public:
    int flag, elements, wonLastTime;
    std::shared_ptr<Warrior> rWarrior, bWarrior;
    City() {
        flag = -1;
        elements = 0;
        wonLastTime = -1;
        rWarrior = nullptr;
        bWarrior = nullptr;
    }
};
std::vector<City> gCitys;
std::vector<bool> cityWithBomb;
int GetPrior(int ind) {
    int prior = (ind % 2 == 0);  // 编号为奇数的无旗城市 -> redCamp优先
    if (gCitys[ind].flag != -1) prior = gCitys[ind].flag;
    return prior;
}

namespace event {
void WarriorBorn(std::shared_ptr<Camp> camp) {  // 武士降生
    if (camp->HaveNext()) {
        auto tmpWarrior = camp->Born(camp->Get());
        if (camp->campId == 0) {
            tmpWarrior->sit = 0;
            gCitys[0].rWarrior = tmpWarrior;
        } else {
            tmpWarrior->sit = gCitySize + 1;
            gCitys[gCitySize + 1].bWarrior = tmpWarrior;
        }
        camp->elements -= gCost[camp->Get()];
        camp->Next();
    }
}
void LionLeave() {  // lion逃跑
    for (int i = 0; i <= gCitySize + 1; ++i) {
        if (gCitys[i].rWarrior && gCitys[i].rWarrior->type == LION) {
            if (gCitys[i].rWarrior->LionLeave()) {
                gCitys[i].rWarrior = nullptr;
            }
        }
        if (gCitys[i].bWarrior && gCitys[i].bWarrior->type == LION) {
            if (gCitys[i].bWarrior->LionLeave()) {
                gCitys[i].bWarrior = nullptr;
            }
        }
    }
}
void WarriorMarch() {  // 武士前进到某一城市
    std::vector<std::shared_ptr<Warrior>> tmpCitys(gCitySize + 2);
    bool rTob = false;  // red武士到达blueCamp
    bool bTow = false;  // blue武士到达redCamp
    for (int i = 0; i <= gCitySize; ++i) {
        if (gCitys[i].rWarrior) {
            int steps = gCitys[i].rWarrior->March();
            if (i + steps >= gCitySize + 1) {  // red武士到达blueCamp
                if (gCitys[gCitySize + 1]
                        .rWarrior)  // 到达敌方camp两个武士，获得胜利
                    redWin = true;
                else
                    rTob = true;
                tmpCitys[gCitySize + 1] = gCitys[i].rWarrior;
            } else {
                tmpCitys[i + steps] = gCitys[i].rWarrior;
            }
        }
    }
    if (tmpCitys[gCitySize + 1] == nullptr)
        tmpCitys[gCitySize + 1] = gCitys[gCitySize + 1].rWarrior;
    for (int i = 0; i <= gCitySize + 1; ++i) {
        gCitys[i].rWarrior = tmpCitys[i];
        if (gCitys[i].rWarrior) gCitys[i].rWarrior->sit = i;
    }

    for (int i = 0; i <= gCitySize + 1; i++) tmpCitys[i] = nullptr;
    for (int i = 1; i <= gCitySize + 1; ++i) {
        if (gCitys[i].bWarrior) {
            int steps = gCitys[i].bWarrior->March();
            if (i - steps <= 0) {  // blue武士到达redCamp
                if (gCitys[0].bWarrior)  // 到达敌方camp两个武士，获得胜利
                    blueWin = true;
                else
                    bTow = true;
                tmpCitys[0] = gCitys[i].bWarrior;
            } else {
                tmpCitys[i - steps] = gCitys[i].bWarrior;
            }
        }
    }
    if (tmpCitys[0] == nullptr) tmpCitys[0] = gCitys[0].bWarrior;
    for (int i = 0; i <= gCitySize + 1; ++i) {
        gCitys[i].bWarrior = tmpCitys[i];
        if (gCitys[i].bWarrior) gCitys[i].bWarrior->sit = i;
    }
    if (bTow || blueWin == 1) {  // blueWarrior进入 RedCamp
        std::string out = "";
        out += GetTime() + " blue " + GetSoldierName(gCitys[0].bWarrior->type) +
               " " + std::to_string(gCitys[0].bWarrior->id) +
               " reached red headquarter with " +
               std::to_string(gCitys[0].bWarrior->elements) +
               " elements and force " +
               std::to_string(gCitys[0].bWarrior->power);
        Log(out);
        if (blueWin == 1) {
            std::string out = "";
            out += GetTime() + " red headquarter was taken";
            Log(out);
        }
    }
    for (int i = 1; i <= gCitySize; ++i) {
        if (gCitys[i].rWarrior) {
            std::string out = "";
            out += GetTime() + " red " +
                   GetSoldierName(gCitys[i].rWarrior->type) + " " +
                   std::to_string(gCitys[i].rWarrior->id) +
                   " marched to city " + std::to_string(i) + " with " +
                   std::to_string(gCitys[i].rWarrior->elements) +
                   " elements and force " +
                   std::to_string(gCitys[i].rWarrior->power);
            Log(out);
        }
        if (gCitys[i].bWarrior) {
            std::string out = "";
            out += GetTime() + " blue " +
                   GetSoldierName(gCitys[i].bWarrior->type) + " " +
                   std::to_string(gCitys[i].bWarrior->id) +
                   " marched to city " + std::to_string(i) + " with " +
                   std::to_string(gCitys[i].bWarrior->elements) +
                   " elements and force " +
                   std::to_string(gCitys[i].bWarrior->power);
            Log(out);
        }
    }
    if (rTob || redWin) {  // redWarrior进入 BlueCamp
        std::string out = "";
        out += GetTime() + " red " +
               GetSoldierName(gCitys[gCitySize + 1].rWarrior->type) + " " +
               std::to_string(gCitys[gCitySize + 1].rWarrior->id) +
               " reached blue headquarter with " +
               std::to_string(gCitys[gCitySize + 1].rWarrior->elements) +
               " elements and force " +
               std::to_string(gCitys[gCitySize + 1].rWarrior->power);
        Log(out);
        if (redWin) {
            std::string out = "";
            out += GetTime() + " blue headquarter was taken";
            Log(out);
        }
    }
}
void FetchElement(std::shared_ptr<Camp> redCamp,
                  std::shared_ptr<Camp> blueCamp) {  // 武士取走城市生命元
    for (int i = 1; i <= gCitySize; ++i) {
        if (gCitys[i].elements <= 0) continue;
        if (gCitys[i].rWarrior &&
            gCitys[i].bWarrior == nullptr) {  // redCamp获取生命元
            redCamp->elements += gCitys[i].elements;
            std::string out = "";
            out += GetTime() + " red " +
                   GetSoldierName(gCitys[i].rWarrior->type) + " " +
                   std::to_string(gCitys[i].rWarrior->id) + " earned " +
                   std::to_string(gCitys[i].elements) +
                   " elements for his headquarter";
            Log(out);
            gCitys[i].elements = 0;
        }
        if (gCitys[i].bWarrior &&
            gCitys[i].rWarrior == nullptr) {  // blueCamp获取生命元
            blueCamp->elements += gCitys[i].elements;
            std::string out = "";
            out += GetTime() + " blue " +
                   GetSoldierName(gCitys[i].bWarrior->type) + " " +
                   std::to_string(gCitys[i].bWarrior->id) + " earned " +
                   std::to_string(gCitys[i].elements) +
                   " elements for his headquarter";
            Log(out);
            gCitys[i].elements = 0;
        }
    }
}
void WarriorArchery() {  // 武士放箭
    auto fight = [&](auto agent, auto enemy) {
        enemy->elements -= powerOfArrow;
        agent->weapons[ARROW]->Wear();

        std::string out = "";
        out += GetTime() + " " + ((agent->campId == 0) ? "red" : "blue") + " " +
               GetSoldierName(agent->type) + " " + std::to_string(agent->id) +
               " shot";
        if (enemy->elements <= 0) {  // 敌方被射死
            out += std::string(" and killed ") +
                   ((enemy->campId == 0) ? "red" : "blue") + " " +
                   GetSoldierName(enemy->type) + " " +
                   std::to_string(enemy->id);
        }
        Log(out);
    };
    for (int i = 1; i <= gCitySize; ++i) {
        if (gCitys[i].rWarrior && gCitys[i].rWarrior->weapons[ARROW] &&
            (i + 1) <= gCitySize && gCitys[i + 1].bWarrior)
            fight(gCitys[i].rWarrior, gCitys[i + 1].bWarrior);
        if (gCitys[i].bWarrior && gCitys[i].bWarrior->weapons[ARROW] &&
            (i - 1) >= 1 && gCitys[i - 1].rWarrior)
            fight(gCitys[i].bWarrior, gCitys[i - 1].rWarrior);
    }
}
void WarriorBomb() {  // 武士爆破
    auto fight = [&](auto agent, auto enemy, bool prior) {
        bool use = false;  // 判断是否需要使用Bomb
        if (prior) {
            int powerOfEnemyBack = enemy->power / 2;
            if (enemy->type == NINJA) powerOfEnemyBack = 0;
            if (enemy->weapons[SWORD])
                powerOfEnemyBack += enemy->weapons[SWORD]->power;

            if (agent->Power() < enemy->elements &&
                powerOfEnemyBack >= agent->elements)
                use = true;
        } else {
            if (enemy->Power() >= agent->elements) use = true;
        }
        if (use) {
            agent->weapons[BOMB]->Wear();
            agent->elements = 0;  // 同归于尽
            enemy->elements = 0;
            std::string out = "";
            out += GetTime() + " " + ((agent->campId == 0) ? "red" : "blue") +
                   " " + GetSoldierName(agent->type) + " " +
                   std::to_string(agent->id) + " used a bomb and killed " +
                   ((enemy->campId == 0) ? "red" : "blue") + " " +
                   GetSoldierName(enemy->type) + " " +
                   std::to_string(enemy->id);
            Log(out);
        }
        return use;
    };
    for (int i = 1; i <= gCitySize; i++) {
        if (gCitys[i].rWarrior && gCitys[i].bWarrior &&
            gCitys[i].rWarrior->elements >= 0 &&
            gCitys[i].bWarrior->elements >= 0 &&
            gCitys[i].rWarrior->weapons[BOMB]) {
            // 标记使用炸弹的城市
            bool tmpUse =
                fight(gCitys[i].rWarrior, gCitys[i].bWarrior, GetPrior(i) == 0);
            if (tmpUse) cityWithBomb[i] = true;
        }

        if (gCitys[i].rWarrior && gCitys[i].bWarrior &&
            gCitys[i].rWarrior->elements >= 0 &&
            gCitys[i].bWarrior->elements >= 0 &&
            gCitys[i].bWarrior->weapons[BOMB]) {
            // 标记使用炸弹的城市
            bool tmpUse =
                fight(gCitys[i].bWarrior, gCitys[i].rWarrior, GetPrior(i) == 1);
            if (tmpUse) cityWithBomb[i] = true;
        }
    }
}
void RaiseFlag(int ind, int flag) {  // 旗帜升起
    gCitys[ind].flag = flag;
    std::string out = "";
    out += GetTime() + " " + ((flag == 0) ? "red" : "blue") +
           " flag raised in city " + std::to_string(ind);
    Log(out);
}
void WarriorAttack(std::shared_ptr<Camp> redCamp,
                   std::shared_ptr<Camp> blueCamp) {  // 武士主动进攻
    int redUp = 0, blueUp = 0;
    for (int i = 1; i <= gCitySize; i++) {
        if (cityWithBomb[i]) continue;
        if (gCitys[i].rWarrior && gCitys[i].bWarrior) {
            auto rw = gCitys[i].rWarrior;
            auto bw = gCitys[i].bWarrior;
            if (rw->elements <= 0 && bw->elements <= 0) {  // 没有发生战斗
                continue;
            }
            int prior = GetPrior(i);
            if (prior == 0) {
                if (rw->elements > 0) {
                    rw->Attack(bw);
                    rw->AttackAfter(bw);
                } else {
                    if (bw->type != DRAGON) bw->AttackAfter(rw);
                }
            } else if (prior == 1) {
                if (bw->elements > 0) {
                    bw->Attack(rw);
                    bw->AttackAfter(rw);
                } else {
                    if (rw->type != DRAGON) rw->AttackAfter(bw);
                }
            } else {
                assert(false);
            }

            if (bw->elements > 0 && rw->elements > 0) {  // 平局
                gCitys[i].wonLastTime = -1;
                continue;
            }
            if (bw->elements <= 0) {  // redWarrior 获胜
                rw->winOneFight = true;
                if (gCitys[i].elements > 0) {  // 武士获取生命元
                    std::string out = "";
                    out += GetTime() + " red " +
                           GetSoldierName(gCitys[i].rWarrior->type) + " " +
                           std::to_string(gCitys[i].rWarrior->id) + " earned " +
                           std::to_string(gCitys[i].elements) +
                           " elements for his headquarter";
                    Log(out);
                    redUp += gCitys[i].elements;
                    gCitys[i].elements = 0;
                }
                if (gCitys[i].wonLastTime == 0 && gCitys[i].flag != 0)
                    RaiseFlag(i, 0);  // 升旗
                gCitys[i].wonLastTime = 0;
            } else if (rw->elements <= 0) {  // blue Warrior 获胜
                bw->winOneFight = true;
                if (gCitys[i].elements > 0) {  // 武士获取生命元
                    std::string out = "";
                    out += GetTime() + " blue " +
                           GetSoldierName(gCitys[i].bWarrior->type) + " " +
                           std::to_string(gCitys[i].bWarrior->id) + " earned " +
                           std::to_string(gCitys[i].elements) +
                           " elements for his headquarter";
                    Log(out);
                    blueUp += gCitys[i].elements;
                    gCitys[i].elements = 0;
                }
                if (gCitys[i].wonLastTime == 1 && gCitys[i].flag != 1)
                    RaiseFlag(i, 1);  // 升旗
                gCitys[i].wonLastTime = 1;
            } else {
                assert(false);
            }
        }
    }

    // 奖励获胜warrior生命元
    for (int i = gCitySize; i >= 1; --i) {
        if (gCitys[i].rWarrior && gCitys[i].rWarrior->winOneFight &&
            redCamp->elements >= 8) {
            assert(gCitys[i].rWarrior->elements > 0);
            gCitys[i].rWarrior->elements += 8;
            redCamp->elements -= 8;
        }
    }
    for (int i = 1; i <= gCitySize; ++i) {
        if (gCitys[i].bWarrior && gCitys[i].bWarrior->winOneFight &&
            blueCamp->elements >= 8) {
            assert(gCitys[i].bWarrior->elements > 0);
            gCitys[i].bWarrior->elements += 8;
            blueCamp->elements -= 8;
        }
    }
    redCamp->elements += redUp;
    blueCamp->elements += blueUp;

    // 丢弃破损武器
    for (int i = 1; i <= gCitySize; ++i) {
        if (gCitys[i].rWarrior) gCitys[i].rWarrior->DropWeapon();
        if (gCitys[i].bWarrior) gCitys[i].bWarrior->DropWeapon();
    }
}
void CampReport(std::shared_ptr<Camp> redCamp, std::shared_ptr<Camp> blueCamp) {
    std::string out = "";
    out += GetTime() + " " + std::to_string(redCamp->elements) +
           " elements in red headquarter";
    Log(out);
    out = "";
    out += GetTime() + " " + std::to_string(blueCamp->elements) +
           " elements in blue headquarter";
    Log(out);
}
}  // namespace event

void GameStart() {
    using namespace event;
    std::shared_ptr<RedCamp> redCamp = std::make_shared<RedCamp>();
    std::shared_ptr<BlueCamp> blueCamp = std::make_shared<BlueCamp>();
    for (gHours = 0; gHours <= (endTime / 60 + 1); ++gHours) {
        gMinutes = 0;  // 00min 武士降生
        if (gHours * 60 + gMinutes > endTime) return;
        WarriorBorn(redCamp);
        WarriorBorn(blueCamp);

        gMinutes = 5;  // 05min lion逃跑
        if (gHours * 60 + gMinutes > endTime) return;
        LionLeave();

        gMinutes = 10;  // 10min 武士前进到某一城市
        if (gHours * 60 + gMinutes > endTime) return;
        WarriorMarch();
        if (blueWin || redWin) return;

        gMinutes = 20;  // 20min 城市产生生命元
        if (gHours * 60 + gMinutes > endTime) return;
        for (int i = 0; i <= gCitySize + 1; i++) gCitys[i].elements += 10;

        gMinutes = 30;  // 30min 武士取走城市中的生命元
        if (gHours * 60 + gMinutes > endTime) return;
        FetchElement(redCamp, blueCamp);

        gMinutes = 35;  // 35min 武士放箭
        if (gHours * 60 + gMinutes > endTime) return;
        WarriorArchery();

        gMinutes = 38;  // 38min 武士爆破
        if (gHours * 60 + gMinutes > endTime) return;
        WarriorBomb();

        gMinutes = 40;  // 40min 武士主动进攻
        if (gHours * 60 + gMinutes > endTime) return;
        WarriorAttack(redCamp, blueCamp);

        gMinutes = 50;  // 50min 司令部报告生命元数量
        if (gHours * 60 + gMinutes > endTime) return;
        CampReport(redCamp, blueCamp);

        gMinutes = 55;  // 55min 武士报告武器情况
        if (gHours * 60 + gMinutes > endTime) return;
        for (int i = 0; i <= gCitySize + 1; ++i)
            if (gCitys[i].rWarrior && gCitys[i].rWarrior->elements > 0)
                gCitys[i].rWarrior->Report();
        for (int i = 0; i <= gCitySize + 1; ++i)
            if (gCitys[i].bWarrior && gCitys[i].bWarrior->elements > 0)
                gCitys[i].bWarrior->Report();

        // 清理尸体和warrior的胜利状态
        for (int i = 1; i <= gCitySize; i++) {
            if (gCitys[i].rWarrior && gCitys[i].rWarrior->elements <= 0)
                gCitys[i].rWarrior = nullptr;
            if (gCitys[i].bWarrior && gCitys[i].bWarrior->elements <= 0)
                gCitys[i].bWarrior = nullptr;
            if (gCitys[i].rWarrior) gCitys[i].rWarrior->winOneFight = false;
            if (gCitys[i].bWarrior) gCitys[i].bWarrior->winOneFight = false;
        }
        for (int i = 0; i <= gCitySize + 1; ++i) cityWithBomb[i] = false;
    }
}
void GameInit(int testCase) {
    std::cin >> initialElement >> gCitySize >> powerOfArrow >> loyaltyDecay >>
        endTime;
    for (int i = 0; i < 5; i++) std::cin >> gCost[i];    // 花费=生命值
    for (int i = 0; i < 5; i++) std::cin >> gAttack[i];  // 攻击力
    cityWithBomb.clear();
    cityWithBomb.resize(gCitySize + 2);
    for (int i = 0; i <= gCitySize + 1; ++i) cityWithBomb[i] = false;
    gCitys.clear();
    gCitys.resize(gCitySize + 2);
    std::string out = "Case " + std::to_string(testCase) + ":";
    Log(out);
    redWin = false;
    blueWin = false;
    gHours = 0;
    gMinutes = 0;
}

int main() {
    int numOfTestCase;
    std::cin >> numOfTestCase;
    for (int testCase = 1; testCase <= numOfTestCase; ++testCase) {
        GameInit(testCase);
        GameStart();
    }
    return 0;
}