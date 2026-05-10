/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "TransformGizmo.h"

#include "..\..\Natives\natives2.h"
#include "..\..\Natives\natives.h"
#include "..\..\Scripting\World.h"
#include "..\..\Scripting\GameplayCamera.h"
#include "..\..\Scripting\Game.h"
#include "..\..\Scripting\Model.h"
#include "..\..\Scripting\GTAentity.h"
#include "..\..\Scripting\enums.h"
#include "SpoonerEntity.h"

#include <algorithm>
#include <cmath>

namespace
{
	constexpr float TRANSLATE_SENSITIVITY = 3.0f;
	constexpr float TRANSLATE_SENSITIVITY_MAX = 15.0f;
	constexpr float ROTATE_SENSITIVITY = 180.0f;
	constexpr float AXIS_SCALE_MIN = 0.5f;
	constexpr float AXIS_SCALE_MAX = 10.0f;
	constexpr float ATTACHMENT_DEADZONE = 0.0005f;
	constexpr float AXIS_SCALE_FACTOR = 1.5f;
	constexpr float HANDLE_PICK_RADIUS = 0.035f;
	constexpr float LABEL_RECT_W = 0.025f;
	constexpr float LABEL_RECT_H = 0.022f;
	constexpr float LABEL_Y_OFFSET = 0.008f; // 0.01f = bit too high 
	constexpr float SCREEN_DIR_MIN_LENGTH = 0.001f;

	constexpr const char* TRANS_LABEL_X = "X";
	constexpr const char* TRANS_LABEL_Y = "Y";
	constexpr const char* TRANS_LABEL_Z = "Z";
	constexpr const char* ROT_LABEL_X = "PITCH";
	constexpr const char* ROT_LABEL_Y = "ROLL";
	constexpr const char* ROT_LABEL_Z = "YAW";
}

TransformGizmo::TransformGizmo()
	: mode(GizmoMode::Translate)
{
}

void TransformGizmo::FrameData::ComputeForEntity(GTAentity& entity, const Vector3& worldPos)
{
	distanceToEntity = GameplayCamera::GetPosition().DistanceTo(worldPos);

	Vector3_t forward_t, right_t, up_t, pos_t;
	GET_ENTITY_MATRIX(entity.GetHandle(), &forward_t, &right_t, &up_t, &pos_t);

	localAxes[0] = Vector3(right_t);
	localAxes[1] = -Vector3(forward_t);
	localAxes[2] = Vector3(up_t);

	auto dims = entity.ModelDimensions();
	float halfX = std::abs(dims.Dim2.x - dims.Dim1.x) * 0.5f;
	float halfY = std::abs(dims.Dim2.y - dims.Dim1.y) * 0.5f;
	float halfZ = std::abs(dims.Dim2.z - dims.Dim1.z) * 0.5f;

	axisScales[0] = std::clamp(halfX * AXIS_SCALE_FACTOR, AXIS_SCALE_MIN, AXIS_SCALE_MAX);
	axisScales[1] = std::clamp(halfY * AXIS_SCALE_FACTOR, AXIS_SCALE_MIN, AXIS_SCALE_MAX);
	axisScales[2] = std::clamp(halfZ * AXIS_SCALE_FACTOR, AXIS_SCALE_MIN, AXIS_SCALE_MAX);
}

void TransformGizmo::FrameData::ComputeAxisScreenProjections(const Vector3& worldPos, const Vector2& entityScreenPos, bool entityOnScreen)
{
	for (int i = 0; i < 3; i++)
	{
		Vector3 handleWorld = worldPos + localAxes[i] * axisScales[i];
		handlePositions[i].isOnScreen = World::WorldToScreen(handleWorld, handlePositions[i].screenPos);

		if (entityOnScreen && handlePositions[i].isOnScreen)
		{
			Vector2 dir = handlePositions[i].screenPos - entityScreenPos;
			float len = dir.Length();
			axisScreenProjections[i] = (len > SCREEN_DIR_MIN_LENGTH) ? dir / len : Vector2::Zero();
		}
		else
		{
			axisScreenProjections[i] = Vector2::Zero();
		}
	}
}

Vector3 TransformGizmo::GetLocalAxis(GizmoAxis axis) const
{
	int idx = AxisIndex(axis);
	return (idx >= 0) ? frame.localAxes[idx] : Vector3::Zero();
}

float TransformGizmo::GetAxisScale(GizmoAxis axis) const
{
	int idx = AxisIndex(axis);
	return (idx >= 0) ? frame.axisScales[idx] : 1.0f;
}

int TransformGizmo::AxisIndex(GizmoAxis axis) const
{
	int idx = static_cast<int>(axis) - 1;
	return (idx >= 0 && idx < 3) ? idx : -1;
}

const char* TransformGizmo::GetAxisLabel(GizmoAxis axis) const
{
	bool isRot = (mode == GizmoMode::Rotate);
	switch (axis)
	{
	case GizmoAxis::X: return isRot ? ROT_LABEL_X : TRANS_LABEL_X;
	case GizmoAxis::Y: return isRot ? ROT_LABEL_Y : TRANS_LABEL_Y;
	case GizmoAxis::Z: return isRot ? ROT_LABEL_Z : TRANS_LABEL_Z;
	default: return "?";
	}
}

Vector2 TransformGizmo::GetCursorPos() const
{
	return Vector2(
		GET_DISABLED_CONTROL_NORMAL(2, INPUT_CURSOR_X),
		GET_DISABLED_CONTROL_NORMAL(2, INPUT_CURSOR_Y)
	);
}

float TransformGizmo::NormalizeAngle(float angle)
{
	return fmodf(fmodf(angle, 360.0f) + 360.0f, 360.0f);
}

void TransformGizmo::ApplyRotationDelta(Vector3& rotation, GizmoAxis axis, const Vector2& delta)
{
	float rotationDelta = 0.0f;
	switch (axis)
	{
	case GizmoAxis::X: rotationDelta = -delta.y * ROTATE_SENSITIVITY; break;
	case GizmoAxis::Y: rotationDelta = -delta.x * ROTATE_SENSITIVITY; break;
	case GizmoAxis::Z: rotationDelta = delta.x * ROTATE_SENSITIVITY; break;
	default: break;
	}

	switch (axis)
	{
	case GizmoAxis::X: rotation.x = NormalizeAngle(rotation.x + rotationDelta); break;
	case GizmoAxis::Y: rotation.y = NormalizeAngle(rotation.y + rotationDelta); break;
	case GizmoAxis::Z: rotation.z = NormalizeAngle(rotation.z + rotationDelta); break;
	default: break;
	}
}

Vector3 TransformGizmo::WorldToBoneRelative(const Vector3& worldOffset, const Vector3& boneRotEuler) const
{
	float yaw = DegreeToRadian(boneRotEuler.z);
	float pitch = DegreeToRadian(boneRotEuler.y);
	float roll = DegreeToRadian(boneRotEuler.x);

	float cZ = std::cos(yaw), sZ = std::sin(yaw);
	float cX = std::cos(roll), sX = std::sin(roll);
	float cY = std::cos(pitch), sY = std::sin(pitch);

	Vector3 xAxis = Vector3(cZ * cY - sZ * sX * sY, sZ * cY + cZ * sX * sY, -cX * sY);
	Vector3 yAxis = Vector3(-sZ * cX, cZ * cX, sX);
	Vector3 zAxis = Vector3(cZ * sY + sZ * sX * cY, sZ * sY - cZ * sX * cY, cX * cY);

	return Vector3(
		Vector3::Dot(worldOffset, xAxis),
		Vector3::Dot(worldOffset, yAxis),
		Vector3::Dot(worldOffset, zAxis)
	);
}

float TransformGizmo::GetDistanceBasedSensitivity() const
{
	return std::clamp(TRANSLATE_SENSITIVITY + frame.distanceToEntity * 0.1f,
		TRANSLATE_SENSITIVITY, TRANSLATE_SENSITIVITY_MAX);
}

void TransformGizmo::Update()
{
	SET_MOUSE_CURSOR_VISIBLE(true);
	SET_MOUSE_CURSOR_THIS_FRAME();

	Vector2 cursorPos = GetCursorPos();

	GTAentity selectedEntity = sub::Spooner::selectedEntity.handle;
	if (!selectedEntity.Exists())
		return;

	Vector3 entityPos = selectedEntity.GetPosition();
	Vector2 entityScreen;
	bool entityOnScreen = World::WorldToScreen(entityPos, entityScreen);

	frame.ComputeForEntity(selectedEntity, entityPos);
	frame.ComputeAxisScreenProjections(entityPos, entityScreen, entityOnScreen);

	drag.hoveredAxis = FindAxisAtCursor(cursorPos);

	if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_ACCEPT))
	{
		if (drag.activeAxis == GizmoAxis::None && drag.hoveredAxis != GizmoAxis::None)
		{
			drag.activeAxis = drag.hoveredAxis;
			drag.startCursorPos = cursorPos;
			drag.startPos = entityPos;
			drag.startRotation = selectedEntity.Rotation_get();
		}
	}
	else
	{
		drag.activeAxis = GizmoAxis::None;
	}
}

void TransformGizmo::ApplyMovement(GTAentity& entity)
{
	if (drag.activeAxis == GizmoAxis::None || !entity.Exists())
		return;

	Vector2 cursorPos = GetCursorPos();
	Vector2 delta = cursorPos - drag.startCursorPos;

	Vector3 axisDir = GetLocalAxis(drag.activeAxis);

	if (mode == GizmoMode::Translate)
	{
		int idx = AxisIndex(drag.activeAxis);
		Vector2 screenDir = frame.axisScreenProjections[idx];
		float movement = delta.x * screenDir.x + delta.y * screenDir.y;

		movement *= GetDistanceBasedSensitivity() * GetAxisScale(drag.activeAxis);
		Vector3 newPos = drag.startPos + axisDir * movement;
		SET_ENTITY_COORDS_NO_OFFSET(entity.GetHandle(), newPos.x, newPos.y, newPos.z, false, false, false);
	}
	else if (mode == GizmoMode::Rotate)
	{
		Vector3 newRot = drag.startRotation;
		ApplyRotationDelta(newRot, drag.activeAxis, delta);
		entity.SetRotation(newRot);
	}
}

GizmoAxis TransformGizmo::GetActiveAxis() const
{
	return drag.activeAxis;
}

GizmoMode TransformGizmo::GetMode() const
{
	return mode;
}

void TransformGizmo::SetMode(GizmoMode newMode)
{
	if (mode != newMode)
	{
		mode = newMode;
		drag.activeAxis = GizmoAxis::None;
	}
}

bool TransformGizmo::IsActive() const
{
	return drag.IsActive();
}

Vector3 TransformGizmo::PrepareAttachmentGizmo(GTAentity& parentEntity, GTAentity& attachedEntity, Vector3& localOffset, int boneIndex)
{
	SET_MOUSE_CURSOR_VISIBLE(true);
	SET_MOUSE_CURSOR_THIS_FRAME();

	Vector3 worldPos;
	if (boneIndex >= 0)
	{
		worldPos = attachedEntity.GetPosition();
	}
	else
	{
		worldPos = parentEntity.GetOffsetInWorldCoords(localOffset);
	}

	Vector2 entityScreen;
	bool entityOnScreen = World::WorldToScreen(worldPos, entityScreen);

	frame.ComputeForEntity(attachedEntity, worldPos);
	frame.ComputeAxisScreenProjections(worldPos, entityScreen, entityOnScreen);

	drag.hoveredAxis = FindAxisAtCursor(GetCursorPos());

	Draw(worldPos);

	if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_ACCEPT))
	{
		if (drag.activeAxis == GizmoAxis::None && drag.hoveredAxis != GizmoAxis::None)
		{
			drag.activeAxis = drag.hoveredAxis;
			drag.lastCursorPos = GetCursorPos();
			drag.dragStartWorldPos = worldPos;
			drag.accumulatedDelta = Vector3::Zero();

			if (boneIndex >= 0)
			{
				drag.boneIndex = boneIndex;
				drag.initialBoneRelativeOffset = localOffset;
			}
		}
	}
	else
	{
		drag.activeAxis = GizmoAxis::None;
		drag.boneIndex = -1;
	}

	return worldPos;
}

void TransformGizmo::ProcessAttachmentDrag(GTAentity& parentEntity, const Vector3& worldPos, Vector3& localOffset, Vector3& localRot)
{
	if (!drag.IsActive())
		return;

	Vector2 cursorPos = GetCursorPos();
	Vector2 delta = cursorPos - drag.lastCursorPos;
	drag.lastCursorPos = cursorPos;

	if (delta.Length() <= ATTACHMENT_DEADZONE)
		return;

	if (mode == GizmoMode::Translate)
	{
		int idx = AxisIndex(drag.activeAxis);
		Vector2 screenDir = frame.axisScreenProjections[idx];
		float movement = delta.x * screenDir.x + delta.y * screenDir.y;
		movement *= GetDistanceBasedSensitivity() * GetAxisScale(GizmoAxis::None);

		drag.accumulatedDelta = drag.accumulatedDelta + GetLocalAxis(drag.activeAxis) * movement;
		Vector3 newWorldPos = drag.dragStartWorldPos + drag.accumulatedDelta;

		if (drag.boneIndex >= 0)
		{
			Vector3 currentBoneRot = GET_ENTITY_BONE_ROTATION(parentEntity.GetHandle(), drag.boneIndex);
			localOffset = drag.initialBoneRelativeOffset + WorldToBoneRelative(drag.accumulatedDelta, currentBoneRot);
		}
		else
		{
			localOffset = parentEntity.GetOffsetGivenWorldCoords(newWorldPos);
		}
	}
	else if (mode == GizmoMode::Rotate)
	{
		ApplyRotationDelta(localRot, drag.activeAxis, delta);
	}
}

void TransformGizmo::ApplyAttachmentMovement(GTAentity& parentEntity, GTAentity& attachedEntity, Vector3& localOffset, Vector3& localRot, int boneIndex)
{
	Vector3 worldPos = PrepareAttachmentGizmo(parentEntity, attachedEntity, localOffset, boneIndex);
	ProcessAttachmentDrag(parentEntity, worldPos, localOffset, localRot);
}

GizmoAxis TransformGizmo::FindAxisAtCursor(const Vector2& cursorPos) const
{
	GizmoAxis closest = GizmoAxis::None;
	float closestDist = HANDLE_PICK_RADIUS;

	for (int i = 0; i < 3; i++)
	{
		if (frame.handlePositions[i].isOnScreen)
		{
			float dist = cursorPos.DistanceTo(frame.handlePositions[i].screenPos);
			if (dist < closestDist)
			{
				closestDist = dist;
				closest = static_cast<GizmoAxis>(i + 1);
			}
		}
	}

	return closest;
}

void TransformGizmo::DrawAxisLine(const Vector3& origin, const Vector3& direction, float length, const RGBA& colour)
{
	Vector3 end = origin + direction * length;
	World::DrawLine(origin, end, colour);
}

void TransformGizmo::DrawHandleLabel(GizmoAxis axis, const RGBA& rectColour, int textAlpha, const char* label)
{
	int idx = static_cast<int>(axis) - 1;
	const auto& info = frame.handlePositions[idx];
	if (!info.isOnScreen)
		return;

	DRAW_RECT(info.screenPos.x, info.screenPos.y, LABEL_RECT_W, LABEL_RECT_H, rectColour.R, rectColour.G, rectColour.B, rectColour.A, false);

	Game::Print::SetupDraw(GTAfont::Arial, { 0.225f, 0.225f }, true, false, true, { 255, 255, 255, textAlpha });
	Game::Print::drawstring(label, info.screenPos.x, info.screenPos.y - LABEL_Y_OFFSET);
}

void TransformGizmo::Draw(const Vector3& entityPos)
{
	struct AxisColors { RGBA colour; GizmoAxis axis; };
	AxisColors axes[] = {
		{ RGBA(220, 40, 40, 0), GizmoAxis::X },
		{ RGBA(40, 200, 40, 0), GizmoAxis::Y },
		{ RGBA(40, 80, 220, 0), GizmoAxis::Z }
	};

	for (const auto& ac : axes)
	{
		int alpha, textAlpha;

		if (ac.axis == drag.activeAxis)
		{
			alpha = 255; textAlpha = 255;
		}
		else if (ac.axis == drag.hoveredAxis)
		{
			alpha = 220; textAlpha = 255;
		}
		else
		{
			alpha = 180; textAlpha = 200;
		}

		RGBA axisColour(ac.colour.R, ac.colour.G, ac.colour.B, alpha);

		Vector3 dir = GetLocalAxis(ac.axis);
		DrawAxisLine(entityPos, dir, frame.axisScales[static_cast<int>(ac.axis) - 1], axisColour);
		DrawHandleLabel(ac.axis, axisColour, textAlpha, GetAxisLabel(ac.axis));
	}
}
