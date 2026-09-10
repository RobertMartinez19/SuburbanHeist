#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ResidentAttack.generated.h"

/**
 * Fictional chase attack (Section 11) - arcade tension mechanic only, never a realistic
 * weapon simulation. Telegraphs (gives the player a window to break line of sight / escape),
 * then applies the abstract effect via ASHResidentCharacter::Server_PerformChaseAttack,
 * respecting Range/Cooldown/Accuracy. Place inside the Chase branch of BT_Resident, gated by
 * a Decorator checking distance-to-target <= Range if you want a dedicated range check node;
 * this task also self-fails out of range so it degrades gracefully without one.
 */
UCLASS()
class SUBURBANHEIST_API UBTTask_ResidentAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ResidentAttack();

	UPROPERTY(EditAnywhere, Category = "Resident|Attack")
	float Range = 250.f;

	UPROPERTY(EditAnywhere, Category = "Resident|Attack")
	float Cooldown = 2.5f;

	/** 0-1 chance the telegraphed attack actually lands, once in range (Section 11 "accuracy"). */
	UPROPERTY(EditAnywhere, Category = "Resident|Attack", meta = (ClampMin = "0", ClampMax = "1"))
	float Accuracy = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Resident|Attack")
	float TelegraphSeconds = 0.6f;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	struct FMemory
	{
		float ElapsedTelegraph = 0.f;
	};
};
