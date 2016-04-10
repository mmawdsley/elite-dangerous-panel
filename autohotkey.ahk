SendMode Input

MyLightsButton = l
MyGearButton = g
MyHardPointsButton = w

ToggleLightsOnRelease:
	if GetKeyState("2Joy32")
		return
	Send, %MyLightsButton%
	SetTimer, ToggleLightsOnRelease, off
	return

ToggleGearOnRelease:
	if GetKeyState("2Joy29")
		return
	Send, %MyGearButton%
	SetTimer, ToggleGearOnRelease, off
	return

ToggleHardPointsOnRelease:
	if GetKeyState("2Joy27")
		return
	Send, %MyHardPointsButton%
	SetTimer, ToggleHardPointsOnRelease, off
	return

2Joy32::
	Send, %MyLightsButton%
	SetTimer, ToggleLightsOnRelease, 10
	return

2Joy29::
	Send, %MyGearButton%
	SetTimer, ToggleGearOnRelease, 10
	return

2Joy27::
	Send, %MyHardPointsButton%
	SetTimer, ToggleHardPointsOnRelease, 10
	return