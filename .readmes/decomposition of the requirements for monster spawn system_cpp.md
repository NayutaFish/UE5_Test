# 怪物刷新系统需求拆解（C++ 实现版）

> 项目：Test_GamePlay  
> 当前状态：CameraRig 已实现，角色系统由另一位程序推进。  
> 本阶段目标：用 C++ 跑通 `SpawnPoint → Spawn Enemy → Enemy Death → Room Clear` 主链路。

---

## 1. 本阶段目标

本阶段不是制作完整怪物 AI，而是先完成一个可接入战斗系统的刷怪框架。

核心流程：

```text
RoomManager 开始房间
 ↓
收集场景中的 EnemySpawnPoint
 ↓
按照刷怪点生成 Enemy
 ↓
记录当前存活怪物数量
 ↓
Enemy 死亡时广播 OnEnemyDeath
 ↓
RoomManager 接收死亡事件
 ↓
AliveEnemyCount -= 1
 ↓
AliveEnemyCount <= 0
 ↓
Room Clear
```

---

## 2. 本阶段不做内容

当前不要做：

```text
完整敌人 AI
敌人寻路
近战攻击
远程弹道
敌人血条 UI
复杂波次配置
随机房间
奖励三选一
对象池
DataTable 刷怪配置
Boss
精英怪
```

原因：

```text
玩家角色和伤害系统还在并行开发；
现在最重要的是让关卡流程能独立跑通；
先用 Dummy Enemy 测试刷怪、死亡通知和清怪判断。
```

---

## 3. 系统模块总览

本阶段需要三个核心 C++ 类：

```text
AEnemySpawnPoint
AEnemyBase
ADemoRoomManager
```

对应蓝图子类：

```text
BP_EnemySpawnPoint
BP_EnemyDummy
BP_DemoRoomManager
```

关卡结构：

```text
Level
├── BP_DemoRoomManager
├── BP_EnemySpawnPoint_01
├── BP_EnemySpawnPoint_02
├── BP_EnemySpawnPoint_03
└── CameraRig_Main
```

---

## 4. 类一：AEnemySpawnPoint

### 职责

`AEnemySpawnPoint` 只负责提供怪物生成位置。

它不负责：

```text
刷怪逻辑
敌人数量
波次控制
敌人死亡
房间清理
```

它只提供：

```text
位置
旋转
Transform
编辑器中可视化标记
```

### 功能需求

```text
可放置在关卡中
可被 RoomManager 查找
有 Arrow 显示朝向
有 Billboard 方便在场景中看到
提供 GetSpawnTransform()
```

### 文件位置

```text
Source/Test_GamePlay/Room/EnemySpawnPoint.h
Source/Test_GamePlay/Room/EnemySpawnPoint.cpp
```

### EnemySpawnPoint.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnPoint.generated.h"

class UArrowComponent;
class UBillboardComponent;

UCLASS()
class TEST_GAMEPLAY_API AEnemySpawnPoint : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawnPoint();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawn Point")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawn Point")
    TObjectPtr<UArrowComponent> Arrow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawn Point")
    TObjectPtr<UBillboardComponent> Billboard;

public:
    UFUNCTION(BlueprintCallable, Category="Spawn Point")
    FTransform GetSpawnTransform() const;
};
```

### EnemySpawnPoint.cpp

```cpp
#include "Room/EnemySpawnPoint.h"

#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"

AEnemySpawnPoint::AEnemySpawnPoint()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
    Arrow->SetupAttachment(SceneRoot);

    Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
    Billboard->SetupAttachment(SceneRoot);
}

FTransform AEnemySpawnPoint::GetSpawnTransform() const
{
    return GetActorTransform();
}
```

---

## 5. 类二：AEnemyBase

### 职责

`AEnemyBase` 是敌人基类。

第一版只负责：

```text
血量
受伤
死亡
死亡事件广播
调试死亡
```

不负责：

```text
AI
寻路
攻击
动画
技能
掉落
```

### 核心事件

怪物死亡时必须广播：

```text
OnEnemyDeath
```

RoomManager 通过绑定该事件来监听敌人死亡。

正确方式：

```text
Enemy.Die()
 ↓
OnEnemyDeath.Broadcast(this)
 ↓
Destroy()
```

### 文件位置

```text
Source/Test_GamePlay/Enemy/EnemyBase.h
Source/Test_GamePlay/Enemy/EnemyBase.cpp
```

### EnemyBase.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeathSignature, AEnemyBase*, DeadEnemy);

UCLASS()
class TEST_GAMEPLAY_API AEnemyBase : public AActor
{
    GENERATED_BODY()

public:
    AEnemyBase();

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Stats")
    float CurrentHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|State")
    bool bIsDead = false;

public:
    UPROPERTY(BlueprintAssignable, Category="Enemy|Events")
    FOnEnemyDeathSignature OnEnemyDeath;

public:
    UFUNCTION(BlueprintCallable, Category="Enemy")
    void ApplyDamageToEnemy(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category="Enemy")
    void TestDie();

protected:
    UFUNCTION(BlueprintCallable, Category="Enemy")
    void Die();
};
```

### EnemyBase.cpp

```cpp
#include "Enemy/EnemyBase.h"

#include "Components/StaticMeshComponent.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(SceneRoot);
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
}

void AEnemyBase::ApplyDamageToEnemy(float DamageAmount)
{
    if (bIsDead)
    {
        return;
    }

    CurrentHealth -= DamageAmount;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Enemy %s took %.1f damage. CurrentHealth = %.1f"),
        *GetName(),
        DamageAmount,
        CurrentHealth
    );

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void AEnemyBase::TestDie()
{
    Die();
}

void AEnemyBase::Die()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died."), *GetName());

    OnEnemyDeath.Broadcast(this);

    Destroy();
}
```

---

## 6. 类三：ADemoRoomManager

### 职责

`ADemoRoomManager` 是本阶段核心。

它负责：

```text
收集 SpawnPoint
启动房间
生成敌人
绑定敌人死亡事件
记录存活敌人数量
判断清怪
触发 Room Clear
```

不负责：

```text
敌人 AI
玩家攻击
奖励具体效果
UI 动画
音效表现
```

### 第一版流程

```text
BeginPlay
 ↓
CollectSpawnPoints()
 ↓
Delay StartDelay
 ↓
StartRoom()
 ↓
SpawnWave()
 ↓
Spawn Enemy
 ↓
Bind Enemy.OnEnemyDeath
 ↓
AliveEnemyCount++
 ↓
Enemy Die
 ↓
HandleEnemyDeath()
 ↓
AliveEnemyCount--
 ↓
CheckRoomClear()
```

### 文件位置

```text
Source/Test_GamePlay/Room/DemoRoomManager.h
Source/Test_GamePlay/Room/DemoRoomManager.cpp
```

### DemoRoomManager.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoRoomManager.generated.h"

class AEnemyBase;
class AEnemySpawnPoint;

UCLASS()
class TEST_GAMEPLAY_API ADemoRoomManager : public AActor
{
    GENERATED_BODY()

public:
    ADemoRoomManager();

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room|Spawn")
    TSubclassOf<AEnemyBase> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room|Spawn")
    bool bAutoStart = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room|Spawn")
    float StartDelay = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room|Runtime")
    int32 AliveEnemyCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room|Runtime")
    TArray<TObjectPtr<AEnemyBase>> SpawnedEnemies;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room|Runtime")
    TArray<TObjectPtr<AEnemySpawnPoint>> SpawnPoints;

protected:
    UFUNCTION(BlueprintCallable, Category="Room")
    void CollectSpawnPoints();

    UFUNCTION(BlueprintCallable, Category="Room")
    void StartRoom();

    UFUNCTION(BlueprintCallable, Category="Room")
    void SpawnWave();

    UFUNCTION()
    void HandleEnemyDeath(AEnemyBase* DeadEnemy);

    UFUNCTION(BlueprintCallable, Category="Room")
    void CheckRoomClear();

    UFUNCTION(BlueprintImplementableEvent, Category="Room")
    void OnRoomClear();
};
```

### DemoRoomManager.cpp

```cpp
#include "Room/DemoRoomManager.h"

#include "Enemy/EnemyBase.h"
#include "Room/EnemySpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ADemoRoomManager::ADemoRoomManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ADemoRoomManager::BeginPlay()
{
    Super::BeginPlay();

    CollectSpawnPoints();

    if (bAutoStart)
    {
        FTimerHandle StartTimerHandle;
        GetWorldTimerManager().SetTimer(
            StartTimerHandle,
            this,
            &ADemoRoomManager::StartRoom,
            StartDelay,
            false
        );
    }
}

void ADemoRoomManager::CollectSpawnPoints()
{
    SpawnPoints.Empty();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        AEnemySpawnPoint::StaticClass(),
        FoundActors
    );

    for (AActor* Actor : FoundActors)
    {
        if (AEnemySpawnPoint* SpawnPoint = Cast<AEnemySpawnPoint>(Actor))
        {
            SpawnPoints.Add(SpawnPoint);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("RoomManager found %d spawn points."), SpawnPoints.Num());
}

void ADemoRoomManager::StartRoom()
{
    UE_LOG(LogTemp, Warning, TEXT("Room started."));
    SpawnWave();
}

void ADemoRoomManager::SpawnWave()
{
    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyClass is not set in DemoRoomManager."));
        return;
    }

    if (SpawnPoints.Num() <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No spawn points found."));
        return;
    }

    AliveEnemyCount = 0;
    SpawnedEnemies.Empty();

    for (AEnemySpawnPoint* SpawnPoint : SpawnPoints)
    {
        if (!IsValid(SpawnPoint))
        {
            continue;
        }

        const FTransform SpawnTransform = SpawnPoint->GetSpawnTransform();

        AEnemyBase* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyBase>(
            EnemyClass,
            SpawnTransform
        );

        if (!IsValid(SpawnedEnemy))
        {
            continue;
        }

        SpawnedEnemy->OnEnemyDeath.AddDynamic(
            this,
            &ADemoRoomManager::HandleEnemyDeath
        );

        SpawnedEnemies.Add(SpawnedEnemy);
        AliveEnemyCount++;

        UE_LOG(LogTemp, Warning, TEXT("Spawned enemy: %s"), *SpawnedEnemy->GetName());
    }

    UE_LOG(LogTemp, Warning, TEXT("Wave spawned. AliveEnemyCount = %d"), AliveEnemyCount);

    if (AliveEnemyCount <= 0)
    {
        CheckRoomClear();
    }
}

void ADemoRoomManager::HandleEnemyDeath(AEnemyBase* DeadEnemy)
{
    if (!IsValid(DeadEnemy))
    {
        return;
    }

    SpawnedEnemies.Remove(DeadEnemy);

    AliveEnemyCount = FMath::Max(0, AliveEnemyCount - 1);

    UE_LOG(LogTemp, Warning, TEXT("Enemy died. AliveEnemyCount = %d"), AliveEnemyCount);

    CheckRoomClear();
}

void ADemoRoomManager::CheckRoomClear()
{
    if (AliveEnemyCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Room Clear!"));
        OnRoomClear();
    }
}
```

---

## 7. 编译后创建蓝图子类

C++ 编译成功后，回到 Unreal Editor。

### 7.1 创建 BP_EnemySpawnPoint

找到 C++ 类：

```text
AEnemySpawnPoint
```

右键：

```text
Create Blueprint Class Based on EnemySpawnPoint
```

命名：

```text
BP_EnemySpawnPoint
```

建议路径：

```text
Content/_Project/Blueprints/Room/
```

---

### 7.2 创建 BP_EnemyDummy

找到 C++ 类：

```text
AEnemyBase
```

创建蓝图子类：

```text
BP_EnemyDummy
```

建议路径：

```text
Content/_Project/Blueprints/Enemy/
```

打开 `BP_EnemyDummy`。

设置 `Mesh`：

```text
Static Mesh = Shape_Cube
```

设置参数：

```text
MaxHealth = 100
```

---

### 7.3 创建 BP_DemoRoomManager

找到 C++ 类：

```text
ADemoRoomManager
```

创建蓝图子类：

```text
BP_DemoRoomManager
```

建议路径：

```text
Content/_Project/Blueprints/Room/
```

打开 `BP_DemoRoomManager`。

设置：

```text
EnemyClass = BP_EnemyDummy
bAutoStart = true
StartDelay = 1.0
```

---

## 8. 放入关卡

在当前 Demo 关卡中放入：

```text
BP_DemoRoomManager
BP_EnemySpawnPoint x 3
```

建议摆放：

```text
SpawnPoint_01：房间左上
SpawnPoint_02：房间右上
SpawnPoint_03：房间中后方
```

不要离玩家出生点太近。

---

## 9. 第一轮测试：只测试刷怪

点击 `Play`。

打开日志：

```text
Window
 ↓
Output Log
```

你应该看到类似：

```text
RoomManager found 3 spawn points.
Room started.
Spawned enemy: BP_EnemyDummy_C_0
Spawned enemy: BP_EnemyDummy_C_1
Spawned enemy: BP_EnemyDummy_C_2
Wave spawned. AliveEnemyCount = 3
```

如果这些都出现，说明：

```text
RoomManager 找到了 SpawnPoint
RoomManager 成功生成了 Enemy
AliveEnemyCount 正确增加
```

---

## 10. 第二轮测试：测试敌人死亡与 Room Clear

当前玩家攻击还没接上，所以需要临时测试死亡。

### 临时自动死亡方案

在 `AEnemyBase::BeginPlay()` 中临时添加：

```cpp
FTimerHandle DieTimerHandle;
GetWorldTimerManager().SetTimer(
    DieTimerHandle,
    this,
    &AEnemyBase::TestDie,
    3.0f,
    false
);
```

完整为：

```cpp
void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    FTimerHandle DieTimerHandle;
    GetWorldTimerManager().SetTimer(
        DieTimerHandle,
        this,
        &AEnemyBase::TestDie,
        3.0f,
        false
    );
}
```

测试成功后记得删除这段。

---

### 运行后期望日志

```text
Enemy BP_EnemyDummy_C_0 died.
Enemy died. AliveEnemyCount = 2

Enemy BP_EnemyDummy_C_1 died.
Enemy died. AliveEnemyCount = 1

Enemy BP_EnemyDummy_C_2 died.
Enemy died. AliveEnemyCount = 0

Room Clear!
```

---

## 11. 后续与玩家攻击系统对接

等另一位程序完成玩家攻击后，只需要让攻击命中敌人时调用：

```cpp
ApplyDamageToEnemy(DamageAmount);
```

流程就变成：

```text
Player Attack Hit Enemy
 ↓
Enemy.ApplyDamageToEnemy(Damage)
 ↓
CurrentHealth -= Damage
 ↓
CurrentHealth <= 0
 ↓
Die()
 ↓
OnEnemyDeath.Broadcast(this)
 ↓
RoomManager.HandleEnemyDeath()
 ↓
AliveEnemyCount -= 1
 ↓
Room Clear 判断
```

RoomManager 不需要大改。

---

## 12. 后续扩展方向

第一版跑通后，再逐步加：

```text
多波次 SpawnWave
近战怪 BP_EnemyMelee
远程怪 BP_EnemyRanged
每个 SpawnPoint 指定怪物类型
RoomData / DataAsset 配置
刷怪延迟
刷怪预警特效
敌人死亡掉落
清怪后奖励三选一
房门开关
```

---

## 13. 本阶段验收标准

完成后应满足：

```text
1. C++ 编译通过
2. BP_EnemySpawnPoint 可以放入关卡
3. BP_DemoRoomManager 可以放入关卡
4. BP_DemoRoomManager 可以设置 EnemyClass
5. RoomManager 能找到所有 SpawnPoint
6. RoomManager 能在每个点生成 Enemy
7. AliveEnemyCount 正确增加
8. Enemy 死亡时广播 OnEnemyDeath
9. RoomManager 能收到死亡事件
10. AliveEnemyCount 正确减少
11. 全部敌人死亡后打印 Room Clear
```

---

## 14. 一句话总结

```text
本阶段只做刷怪主链路：
SpawnPoint 提供位置，RoomManager 负责生成和计数，EnemyBase 负责死亡事件，最后由 RoomManager 判断 Room Clear。
```
