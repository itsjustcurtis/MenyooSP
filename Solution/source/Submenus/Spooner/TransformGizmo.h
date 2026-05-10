/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#pragma once

#include "..\..\Util\GTAmath.h"

class GTAentity;
class RGBA;

enum class GizmoMode { Translate, Rotate };
enum class GizmoAxis { None, X, Y, Z };

class TransformGizmo
{
public:
	TransformGizmo();

	void Draw(const Vector3& entityPos);
	void Update();
	void ApplyMovement(GTAentity& entity);

	GizmoAxis GetActiveAxis() const;
	GizmoMode GetMode() const;
	void SetMode(GizmoMode mode);
	bool IsActive() const;

	void ApplyAttachmentMovement(GTAentity& parentEntity, GTAentity& attachedEntity, Vector3& localOffset, Vector3& localRot, int boneIndex = -1);

private:
	struct HandleScreenInfo
	{
		bool isOnScreen;
		Vector2 screenPos;
	};

	struct FrameData
	{
		HandleScreenInfo handlePositions[3];
		Vector2 axisScreenProjections[3];
		float axisScales[3] = {1.0f, 1.0f, 1.0f};
		Vector3 localAxes[3];
		float distanceToEntity = 0.0f;

		void ComputeForEntity(GTAentity& entity, const Vector3& worldPos);
		void ComputeAxisScreenProjections(const Vector3& worldPos, const Vector2& entityScreenPos, bool entityOnScreen);
	};

	struct DragContext
	{
		GizmoAxis activeAxis = GizmoAxis::None;
		GizmoAxis hoveredAxis = GizmoAxis::None;
		Vector2 startCursorPos = Vector2::Zero();
		Vector2 lastCursorPos = Vector2::Zero();
		Vector3 startPos = Vector3::Zero();
		Vector3 startRotation = Vector3::Zero();
		Vector3 accumulatedDelta = Vector3::Zero();
		Vector3 dragStartWorldPos = Vector3::Zero();
		int boneIndex = -1;
		Vector3 initialBoneRelativeOffset = Vector3::Zero();

		void Reset() { *this = DragContext(); }
		bool IsActive() const { return activeAxis != GizmoAxis::None; }
	};

	GizmoMode mode;
	DragContext drag;
	FrameData frame;

	Vector3 GetLocalAxis(GizmoAxis axis) const;
	float GetAxisScale(GizmoAxis axis) const;
	float GetDistanceBasedSensitivity() const;
	int AxisIndex(GizmoAxis axis) const;
	const char* GetAxisLabel(GizmoAxis axis) const;
	Vector2 GetCursorPos() const;
	float NormalizeAngle(float angle);
	void ApplyRotationDelta(Vector3& rotation, GizmoAxis axis, const Vector2& delta);
	Vector3 WorldToBoneRelative(const Vector3& worldOffset, const Vector3& boneRotEuler) const;

	GizmoAxis FindAxisAtCursor(const Vector2& cursorPos) const;
	void DrawAxisLine(const Vector3& origin, const Vector3& direction, float length, const RGBA& colour);
	void DrawHandleLabel(GizmoAxis axis, const RGBA& rectColour, int textAlpha, const char* label);

	Vector3 PrepareAttachmentGizmo(GTAentity& parentEntity, GTAentity& attachedEntity, Vector3& localOffset, int boneIndex = -1);
	void ProcessAttachmentDrag(GTAentity& parentEntity, const Vector3& worldPos, Vector3& localOffset, Vector3& localRot);
};
