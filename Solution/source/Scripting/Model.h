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

#include "..\Util\GTAmath.h"

#include <string>

typedef unsigned long DWORD, Hash;
typedef unsigned __int64 UINT64;
enum VehicleHash : DWORD;

namespace GTAmodel
{
	class ModelDimensions
	{
	public:
		Vector3 Dim1;
		Vector3 Dim2;

		ModelDimensions();
		ModelDimensions(const Vector3& d1, const Vector3& d2);
	};

	class Model // Inconsistent with other class names - fix?
	{
	public:
		Hash hash;

		Model();
		Model(const std::string& name);
		Model(Hash newHash);
		Model(VehicleHash newHash);

		friend bool operator == (const Model& left, const Model& right);
		friend bool operator != (const Model& left, const Model& right);
		bool Equals(const Model& right) const;
		Model& operator = (const Model& right);

		UINT64 MemoryAddress() const;

		std::string VehicleDisplayName(bool properName) const;
		std::string VehicleModelName() const;

		ModelDimensions Dimensions() const;
		void Dimensions(Vector3& dim1C, Vector3& dim2C) const;
		void Dimensions(Vector3_t& dim1, Vector3_t& dim2) const;
		Vector3 Dim1() const;
		Vector3 Dim2() const;

		bool IsBicycle() const noexcept;
		bool IsBike() const noexcept;
		bool IsQuadbike() const noexcept;
		bool IsBoat() const noexcept;
		bool IsCar() const noexcept;
		bool IsHeli() const noexcept;
		bool IsPlane() const noexcept;
		bool IsTrain() const noexcept;
		bool IsFastBoat() const noexcept;
		bool IsCargobob() const;
		bool IsBus() const;
		bool IsPoliceVehicle() const;
		bool HasSiren() const;
		bool IsWheelChangingSupportedVehicle() const;
		bool IsBennySupportedVehicle();

		bool IsVehicle() const noexcept;
		bool IsPed() const;

		void Load() const;
		bool Load(DWORD timeOut) const;
		void LoadAndWait() const;
		bool IsLoaded() const noexcept;
		void Unload() const;

		void LoadCollision() const;
		bool LoadCollision(DWORD timeOut) const;
		bool IsCollisionLoaded() const noexcept;
		void LoadCollisionAndWait() const;


		bool IsValid() const noexcept;
		bool IsInCdImage() const noexcept;
		bool Exists() const;

	};
}

using namespace GTAmodel;
