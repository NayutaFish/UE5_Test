# 基础肉鸽技能奖励系统（C++ + UMG）

## 当前完成范围

已跑通最小闭环：

```text
Room Clear
  → RoguelikeRewardManager 生成两个合法奖励
  → WBP_RoguelikeReward 显示选项并切换至 UIOnly 输入
  → 玩家点击奖励
  → 技能获取或升级立即生效
  → UI 关闭并恢复 GameOnly 输入
```

本实现不包含 DataAsset、GAS、技能树、存档、商店、权重池、图标和正式特效。

## 文件职责

```text
Source/Test_GamePlay/
├── Skill/
│   ├── SkillTypes.h
│   │   └── 技能 ID、升级类型、奖励类型和传递给 UI 的结构体
│   └── HikariSkillComponent.h/.cpp
│       └── 三技能槽、技能释放、冷却、参数计算、技能获取与升级
├── Roguelike/
│   ├── RoguelikeRewardManager.h/.cpp
│   │   └── Room Clear 奖励生成、UI 创建、输入模式切换、奖励应用
│   └── RoguelikeRewardWidget.h/.cpp
│       └── WBP_RoguelikeReward 的 C++ 父类和按钮选择入口
├── HikariPlayerCharacter.h/.cpp
│   └── 技能槽 1/2/3 的角色转发接口
└── Room/DemoRoomManager.h/.cpp
    └── 清房后通知 RewardManager
```

## 技能槽与默认键位

```text
Slot 1：TripleProjectile，默认拥有，等级 1
Slot 2：空
Slot 3：空

U：释放 Slot 1（TripleProjectile）
I：释放 Slot 2（初始为空；获得 CircularSlash 后可用）
```

角色向外提供：

```cpp
TryCastSkillSlot1();
TryCastSkillSlot2();
TryCastSkillSlot3();
```

旧的 `TryCastSkill1()`、`TryCastSkill2()` 保留为兼容入口，分别转发到 Slot 1、Slot 2。

## 技能数值与上限

| 技能 | 默认效果 | Mechanic 升级 | Cooldown 升级 |
|---|---|---|---|
| TripleProjectile | 3 发，4.0 秒冷却 | 每级 +2 发，最多 7 发（2 级） | 每级 -0.5 秒，最低 2.0 秒（4 级） |
| CircularSlash | 半径 260，3.0 秒冷却 | 每级 +60，最高 440（3 级） | 每级 -0.4 秒，最低 1.6 秒（4 级） |

`UHikariSkillComponent` 在每次释放时都会读取当前升级值，不使用写死的射弹数、半径或冷却。

## 奖励池和过滤规则

候选奖励：

```text
获得 CircularSlash
TripleProjectile：射弹数量 +2
TripleProjectile：冷却 -0.5 秒
CircularSlash：范围 +60
CircularSlash：冷却 -0.4 秒
```

生成时会自动过滤：

- 已拥有 CircularSlash：不再出现“获得 CircularSlash”。
- 未拥有 CircularSlash：不出现其升级奖励。
- 没有空槽：不出现获得技能奖励。
- 升级达到上限：不再出现该升级。
- 从剩余合法项中随机取最多两个；不足两个时 UI 应只显示已有项。

## 必需的蓝图和关卡配置

### 1. 创建奖励 Widget

创建 `WBP_RoguelikeReward`，父类必须是：

```text
RoguelikeRewardWidget
```

不是默认的 `UserWidget`。若父类错误，使用：

```text
File → Reparent Blueprint → RoguelikeRewardWidget
```

建议 Designer 层级：

```text
Canvas Panel
└─ Vertical Box
   ├─ Text_Title_0
   ├─ Text_Description_0
   ├─ Button_Option_0
   ├─ Text_Title_1
   ├─ Text_Description_1
   └─ Button_Option_1
```

实际项目可将每个按钮内部再放一个 `Vertical Box`，包含该选项自己的标题和描述。用于 Event Graph 的 Text 和 Button 控件应保留为变量。

### 2. 配置 WBP Event Graph

在 `My Blueprint → Overrides` 添加：

```text
On Reward Options Set
```

此事件接收 `In Options` 数组。读取索引 0 和 1 的元素，使用 `Break RoguelikeRewardOption`，再把 `Title`、`Description` 写入各自的 Text 控件。

按钮事件：

```text
Button_Option_0.OnClicked → Select Option(0)
Button_Option_1.OnClicked → Select Option(1)
```

`Select Option` 是继承自 `RoguelikeRewardWidget` 的函数；它会转交给 C++ 的 RewardManager，不需要在 WBP 中自行保存 Manager 引用。

### 3. 创建并配置奖励管理器

创建 `BP_RoguelikeRewardManager`，父类：

```text
RoguelikeRewardManager
```

在 `Class Defaults` 设置：

```text
Reward → UI → Reward Widget Class = WBP_RoguelikeReward
```

将 `BP_RoguelikeRewardManager` 放入需要测试的关卡。

### 4. 连接 Room Manager

选中关卡中 `BP_DemoRoomManager`，在 Details 指定：

```text
Room → Reward → Reward Manager = 关卡内的 BP_RoguelikeRewardManager 实例
```

没有此引用时，Room Clear 本身仍会正常，但不会创建奖励 UI。

## 输入模式行为

奖励 UI 创建后：

```text
FInputModeUIOnly
鼠标显示
```

选择奖励后：

```text
FInputModeGameOnly
鼠标隐藏
```

## 日志与排查

输出日志可按分类筛选：

```text
LogSkill
LogRoguelike
```

常见日志与处理：

| 日志 | 原因与处理 |
|---|---|
| `RewardWidgetClass is not configured` | 在 `BP_RoguelikeRewardManager` 的 Class Defaults 指定 `WBP_RoguelikeReward`。 |
| `Could not find a Hikari player with HikariSkillComponent` | 当前 GameMode/Pawn 未使用 `BP_Hikari`，或角色没有 SkillComponent。 |
| `Room clear reward shown with 2 option(s)` | 奖励 UI 已成功创建。 |
| 清房后 I 无效果 | Slot 2 初始为空；需要先选择“获得 CircularSlash”。 |
| WBP 找不到 `On Reward Options Set` 或 `Select Option` | 检查 WBP 父类是否为 `RoguelikeRewardWidget`，然后 Compile。 |

## 测试清单

1. 开局按 U，确认默认 3 发射弹。
2. 清理全部敌人，确认出现两项奖励和鼠标。
3. 选择“获得 CircularSlash”，确认 UI 关闭、游戏恢复。
4. 按 I，确认释放圆形斩击。
5. 选择射弹机制升级后，再按 U，确认 3 发 → 5 发 → 7 发。
6. 验证冷却升级与范围升级达到上限后不会再生成。
