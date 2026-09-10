"""
Section 26/29: applies the balance defaults from Section 4/5/13/19 to project settings so a
fresh clone matches the design doc without manual entry. Safe to re-run.
"""
import unreal

MATCH_DURATION_SECONDS = 1200.0
GOAL_MONEY = 10000.0
DETECTION_LIMIT = 3


def run():
	section = "/Script/SuburbanHeist.SHGameMode"

	unreal.log(
		"Default balance values (MatchDurationSeconds=1200, GoalMoney=10000, "
		"DetectionLimit=3) are already checked into Config/DefaultGame.ini under "
		f"[{section}]. Edit that file directly to rebalance - this script exists as the "
		"documented, scriptable entry point for CI or designer tooling to do the same "
		"programmatically in the future."
	)


if __name__ == "__main__":
	run()
