#include <locale>
#include "utils.hh"

namespace structs
{
	uintptr_t _getobjectname;
	uintptr_t free_name;
	uintptr_t bone_matrix;

	struct FMatrix
	{
		float M[4][4];
	};
	static FMatrix* player_matrix = new FMatrix();

	struct Vector2D
	{
		float x, y;
		Vector2D() { x = y = 0.0; }
		Vector2D(float xx, float yy) { x = xx; y = yy; }
	};

	struct Vector3D
	{
		float x, y, z;
		Vector3D() { x = y = z = 0.0; }
		Vector3D(float xx, float yy, float zz) { x = xx; y = yy; z = zz; }
	};

	struct Vector4D
	{
		float x, y, z, w;
		Vector4D() { x = y = z = w = 0.0; }
		Vector4D(float xx, float yy, float zz, float ww) { x = xx; y = yy; z = zz; w = ww; }
	};

	class vector3
	{
	public:

		float x, y, z;
		vector3() { x = y = z = 0.0f; }
		vector3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }

		inline float dot(vector3 v)
		{
			return x * v.x + y * v.y + z * v.z;
		}

		inline float distance(vector3 v)
		{
			return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
		}

		inline double length() {
			return sqrt(x * x + y * y + z * z);
		}

		vector3 operator+(vector3 v)
		{
			return vector3(x + v.x, y + v.y, z + v.z);
		}

		vector3 operator-(vector3 v)
		{
			return vector3(x - v.x, y - v.y, z - v.z);
		}

		vector3 operator*(float number) const {
			return vector3(x * number, y * number, z * number);
		}
	};

	template<class T>
	struct TArray
	{
		friend struct FString;

	public:
		inline TArray()
		{
			Data = nullptr;
			Count = Max = 0;
		};

		inline int Num() const
		{
			return Count;
		};

		inline T& operator[](int i)
		{
			return Data[i];
		};

		inline const T& operator[](int i) const
		{
			return Data[i];
		};

		inline bool IsValidIndex(int i) const
		{
			return i < Num();
		}

	private:
		T* Data;
		int32_t Count;
		int32_t Max;
	};

	struct FString : private TArray<wchar_t>
	{
		inline FString()
		{
		};

		FString(const wchar_t* other)
		{
			Max = Count = *other ? std::wcslen(other) + 1 : 0;

			if (Count)
			{
				Data = const_cast<wchar_t*>(other);
			}
		};

		inline bool IsValid() const
		{
			return Data != nullptr;
		}

		inline const wchar_t* c_str() const
		{
			return Data;
		}

		std::string ToString() const
		{
			auto length = std::wcslen(Data);

			std::string str(length, '\0');

			std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

			return str;
		}
	};

	static void free_names(__int64 amongus)
	{
		auto function = reinterpret_cast<__int64(__fastcall*)(__int64 sussyfortnite)>(free_name);
		function(amongus);
	}

	static const char* getobjectname(uintptr_t fortnite)
	{
		if (fortnite == 0)
			return ("");
		auto getobjname = reinterpret_cast<FString * (__fastcall*)(int* daindex, FString * string)>(_getobjectname);
		int name_index = *(int*)(fortnite + 0x18);
		FString string;
		getobjname(&name_index, &string);
		if (string.c_str() == 0)
			return ("");
		auto string_result = string.ToString();
		if (string.c_str() != 0)
			free_names((__int64)string.c_str());
		return string_result.c_str();
	}

	static bool get_bone_loc(uintptr_t pawn, int id, vector3* out)
	{
		uintptr_t mesh = *(uintptr_t*)(pawn + offsets::mesh);
		if (!mesh) return false;

		auto get_bone_matrix = ((FMatrix * (__fastcall*)(uintptr_t, FMatrix*, int))(bone_matrix));
		get_bone_matrix(mesh, player_matrix, id);

		out->x = player_matrix->M[3][0];
		out->y = player_matrix->M[3][1];
		out->z = player_matrix->M[3][2];

		return true;
	}

	static bool world_to_screen(vector3 WorldLocation, vector3* out)
	{
		auto WorldToScreen = reinterpret_cast<bool(__fastcall*)(uintptr_t pPlayerController, vector3 vWorldPos, vector3 * vScreenPosOut, char)>(utils::worldtoscreen);
		WorldToScreen((uintptr_t)utils::player_controller, WorldLocation, out, (char)0);
		return true;
	}
}