# 初版技能系统实现说明（C++）

## 结论与范围

《初版技能系统需求拆解（C++ 实现版）》与现有工程架构兼容，已按最小可用范围实现：

- `AHikariPlayerCharacter` 只公开技能调用入口并持有技能组件。
- `UHikariSkillComponent` 负责释放限制、冷却、圆形伤害区与三向射弹生成。
- `ASkillCircleDamageArea` 负责圆形重叠判定、单次命中去重和敌人伤害。
- 三向射弹继续复用 `AAttackAreaBase`，不会建立第二套射弹伤害链路。
- 未修改输入资产、Mapping Context、UI、动画、Niagara、GAS 或技能 DataAsset。

当前角色状态机没有 `Sprinting` 枚举，疾跑仅通过移动速度实现。为满足技能释放限制，角色新增只读疾跑标记；技能只允许在角色存活、`CanStartAction()` 为真且没有疾跑时释放，不改变现有动作状态。

## 文件与职责

```text
Source/Test_GamePlay/
├── HikariPlayerCharacter.h/.cpp
└── Skill/
    ├── HikariSkillComponent.h/.cpp
    └── SkillCircleDamageArea.h/.cpp
```

输入侧只需调用：

```cpp
AHikariPlayerCharacter::TryCastSkill1();
AHikariPlayerCharacter::TryCastSkill2();
```

建议绑定仍为：

```text
IA_Skill_1 -> TryCastSkill1
IA_Skill_2 -> TryCastSkill2
```

## 默认参数

`SkillComponent` 在 C++ 中已有可运行默认类，因此未配置蓝图子类时也能执行逻辑。

### Circular Slash

```text
Cooldown = 3.0 s
Damage = 120
Radius = 260
LifeTime = 0.25 s
AreaClass = ASkillCircleDamageArea（C++ 默认）
```

圆形区域使用 `DamageArea` 对象通道，只响应 `EnemyHitbox`。生成时会处理已经位于范围内的敌人，并用命中集合保证一次释放对同一敌人只伤害一次。伤害调用现有 `AEnemyBase::ApplyDamageToEnemy()`，因此敌人死亡广播和 Room Clear 链路保持不变。开发构建默认绘制与生命周期一致的青色 Debug Sphere，可通过 `bDrawDebugArea` 关闭。

### Triple Projectile

```text
Cooldown = 4.0 s
Damage = 80
Speed = 900
LifeTime = 2.5 s
SpreadAngle = 15 degrees
ForwardOffset = 120
SideOffset = 35
ProjectileClass = AAttackAreaBase（C++ 默认，可覆盖为 BP_AttackArea）
```

三个射弹使用角色水平 Forward 方向，角度为 `-SpreadAngle / 0 / +SpreadAngle`，生成点同时带左右偏移。射弹通过延迟生成在 `BeginPlay` 前完成 `Initialize()`，参数固定为只伤敌人、检测障碍物、非近战。

## 蓝图配置

本次已创建并保存：

```text
Content/A_StudyContent/BluePrint/Skill/BP_SkillCircleDamageArea
```

`BP_Hikari.SkillComponent` 已配置：

```text
CircularSlashAreaClass = BP_SkillCircleDamageArea
ProjectileAttackAreaClass = BP_AttackArea
```

C++ 默认类仍作为配置丢失时的功能回退。需要正式表现时，可在 `BP_SkillCircleDamageArea` 中添加圆盘 Mesh/材质；蓝图只负责表现与资产引用，不应重复实现伤害或冷却。

## 本次验证结果

2026-07-10 已完成 `Test_GamePlayEditor Win64 Development` 全量构建，UHT、三个本次编译单元和模块 DLL 链接均成功，结果为 `Succeeded`。随后通过 UE 无界面编辑器创建技能蓝图、写入 `BP_Hikari` 类引用并完成属性校验；最终增量构建再次为 `Succeeded`。首次全量构建仅报告本机旧版 NETFXSDK 目录不存在的环境警告，与技能代码无关。

## 验证清单

- C++ Development Editor 编译通过。
- `BP_Hikari` 可见 `SkillComponent` 和全部可编辑参数。
- Skill 1 在角色中心生成，范围内敌人只受伤一次，生命周期结束后销毁。
- Skill 2 生成左、中、右三个射弹，命中敌人或墙壁后销毁，超时自动销毁。
- 冷却中、死亡、攻击中、疾跑中不能释放技能。
- 普攻、敌人死亡广播和 Room Clear 行为不变。
- 输入资产由负责输入的成员单独绑定，本次实现不修改输入资产。

## 日志

使用 `LogSkill` 分类，主要输出：

```text
Skill1 Cast Circular Slash
Skill1 On Cooldown
Circular Slash Hit Enemy: <Name>
Skill2 Cast Triple Projectile
Skill2 On Cooldown
Triple Projectile Spawned: Left / Center / Right
```
