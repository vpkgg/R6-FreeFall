#pragma once
#include "Defines.hpp"
#include "Vector.hpp"
#include <xmmintrin.h>
#include <d3d9.h>
#include "globals.hpp"


namespace Game
{
	uint64_t GamerProfile()
	{
		//E8 ? ? ? ? 80 BE ? ? ? ? ? 0F 84 ? ? ? ? 4D 39 FC
		return (((driver::Read<uint64_t>)((((driver::Read<uint64_t>)(((driver::Read<uint64_t>(Base + 0x626FDF8) - 65) ^ 0xCDECFA1D53E14678ui64) + 0x1B) - 55i64) ^ 0x257AA015A9CCDE03i64) - 0x2CCBCDD60B845E55i64) + 40i64) - 47i64) ^ 0x14;
	}
	uint64_t GameManager()
	{
		// E8 ? ? ? ? 48 83 C7 ? 48 39 FE 74 ? 48 8B 1F
		//E8 ? ? ? ? 4C 89 64 24 ? 48 C7 44 24 ? ? ? ? ? 4C 89 64 24 ?
		return ((driver::Read<uint64_t>(Base + 0x62436D0) ^ 0x223768D2418BDAD5i64) - 10) ^ 0xB7EA2884DF893376ui64;
	}
	uintptr_t GetCamera()
	{
		//E8 ? ? ? ? 83 7C 24 ? ? 44 8B 6C 24 ?
		//E8 ? ? ? ? B9 ? ? ? ? 23 8E ? ? ? ? 8B 86 ? ? ? ?
		uintptr_t cameraManager = driver::Read<uintptr_t>(Base + 0x6134C80);
		cameraManager = driver::Read<uintptr_t>(cameraManager + 0x40);
		cameraManager = driver::Read<uintptr_t>(cameraManager + 0x0);
		return cameraManager;
	}
	static std::pair<uint64_t, uint32_t> EntityManager(const uint64_t game_manager)
	{
		const auto decryption = [&game_manager](const uint32_t offset = 0) -> uint64_t
		{
			const auto temp = driver::Read <uint64_t>(game_manager + offset + 0x50);
			return (temp ^ 0x172C2910827EB42E) + 0x2954C82F81C817B9;
		};

		return { decryption(), static_cast<uint32_t>(decryption(8) & 0x3FFFFFFF) };
	}
	static uint64_t Skeleton(const uint64_t pawn)
	{
		return __ROL8__((driver::Read<uintptr_t>(pawn + 0xA60) ^ 0x7Di64) + 0x552793BE79114B13i64, 4);
	}
	uint64_t Pawn(uint64_t address)
	{
		return __ROL8__(driver::Read<uint64_t>(address + 0x38) - 86, 19) - 0x1BF4CD74E0E65D4;
	}
	uint64_t component(uint64_t pawn)
	{
		return (driver::Read<uint64_t>(pawn + 0x18));
	}
	float Health(uintptr_t pawn)
	{
		uintptr_t entity_actor = component(pawn);
		uintptr_t entity_info = driver::Read<uintptr_t>(entity_actor + 0xD8);
		uintptr_t main_component = driver::Read<uintptr_t>(entity_info + 0x8);
		return driver::Read<int>(main_component + 0x1BC);
	}
	vec3 GetPos(uint64_t entPawn)
	{
		entPawn = driver::Read<uint64_t>(entPawn + 0x18);
		return driver::Read<vec3>(entPawn + 0x50);
	}
	void TransformsCalculation(__int64 pBones, __m128* ResultPosition, __m128* BoneInfo)
	{
		__m128 v5; // xmm2
		__m128 v6; // xmm3
		__m128 v7; // xmm0
		__m128 v8; // xmm4
		__m128 v9; // xmm1

		__m128 v10 = { 0.500f, 0.500f, 0.500f, 0.500f };
		__m128 v11 = { 2.000f, 2.000f, 2.000f, 0.000f };

		__m128 v12 = driver::Read<__m128>(pBones);
		__m128 v13 = driver::Read<__m128>(pBones + 0x10);

		v5 = v13;

		v6 = _mm_mul_ps(*(__m128*)BoneInfo, v5);
		v6.m128_f32[0] = v6.m128_f32[0]
			+ (float)(_mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0x55)) + _mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0xAA)));
		v7 = _mm_shuffle_ps(v13, v5, 0xFF);
		v8 = _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(v5, v5, 0xD2), *(__m128*)BoneInfo),
			_mm_mul_ps(_mm_shuffle_ps(*(__m128*)BoneInfo, *(__m128*)BoneInfo, 0xD2), v5));
		v9 = _mm_shuffle_ps(v12, v12, 0x93);
		v9.m128_f32[0] = 0.0;
		*(__m128*)ResultPosition = _mm_add_ps(
			_mm_shuffle_ps(v9, v9, 0x39),
			_mm_mul_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(_mm_shuffle_ps(v8, v8, 0xD2), v7),
						_mm_mul_ps(_mm_shuffle_ps(v6, v6, 0), v5)),
					_mm_mul_ps(
						_mm_sub_ps(_mm_mul_ps(v7, v7), (__m128)v10),
						*(__m128*)BoneInfo)),
				(__m128)v11));
	}
	vec3 GetBoneLocationByIndex(uintptr_t player, int index)
	{
		__m128 arrResultPosition;
		uintptr_t pBonesChain1 = Game::Skeleton(player);
		uintptr_t pBonesChain2 = driver::Read<uintptr_t>(pBonesChain1);
		uintptr_t pBones = driver::Read<uintptr_t>(pBonesChain2 + 0x238);
		uintptr_t pBonesData = driver::Read<uintptr_t>(pBones + 0x58);
		__m128 pBoneInfo = driver::Read<__m128>((0x20 * index) + pBonesData);
		TransformsCalculation(pBones, &arrResultPosition, &pBoneInfo);
		return { arrResultPosition.m128_f32[0], arrResultPosition.m128_f32[1], arrResultPosition.m128_f32[2] };
	}
	vec3 DecryptCamera(__m128i address) // updated
	{
		__m128i vector = _mm_load_si128(&address);
		vector.m128i_i64[0] -= 4;
		vector.m128i_i64[1] -= 4;
		vector.m128i_i64[0] = (__ROL8__(vector.m128i_i64[0], 0x9) ^ 0x117CF778AE210F90);
		vector.m128i_i64[1] = (__ROL8__(vector.m128i_i64[1], 0x9) ^ 0x117CF778AE210F90);
		return *reinterpret_cast<vec3*>(&vector);
	}
	static bool WorldToScreen(vec3 world, vec2& output)
	{
		vec3 temp = world - DecryptCamera(driver::Read<__m128i>(globals.cam + 0x1B0)); // translation
		float x, y, z = { };
		x = temp.dot(DecryptCamera(driver::Read<__m128i>(globals.cam + 0x180))); //right
		y = temp.dot(DecryptCamera(driver::Read<__m128i>(globals.cam + 0x190))); // up
		z = temp.dot(DecryptCamera(driver::Read<__m128i>(globals.cam + 0x1A0)) * -1.f); // forward

		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		output.x = (width / 2.f) * (1.f + x / - driver::Read<float>(globals.cam + 0x360) / z); // fovx
		output.y = (height / 2.f) * (1.f - y / - driver::Read<float>(globals.cam + 0x364) / z); // fovy

		if (output.x < 0.0f || output.x > width)
		return false;
		return z >= 1.0f;
	}
	uint64_t ReplicationInfo(uint64_t controller)
	{
		return _rotl64(driver::Read<uint64_t>(controller + 0x98) - 0x34, 0x2F) - 0x6F;
	}
	std::string name(uintptr_t entity)
	{
		uintptr_t replicationInfo = Game::ReplicationInfo(entity);
		return driver::ReadUnicode(driver::Read<uintptr_t>(replicationInfo + 0x328));
	}
	static uint32_t get_team_id(const uint64_t replicationinfo)
	{
		uintptr_t v18 = _rotl64(driver::Read<uintptr_t>(replicationinfo + 0x88) - 96i64, 19);
		uint32_t v15 = _rotl((driver::Read<uint32_t>(v18 + 0xA0) ^ 0xF16A0058) - 96, 30);
		return v15;
	}
	uintptr_t  localplayer()
	{
		// maybe work? have not tested
		uintptr_t  unencrypted = driver::Read<uintptr_t>(Base + 0x626FDF8);
		return (((driver::Read<uintptr_t >)((((driver::Read<uintptr_t >)(((driver::Read<uintptr_t >(unencrypted - 65) ^ 0xCDECFA1D53E14678ui64) + 0x1B) - 55i64) ^ 0x257AA015A9CCDE03i64) - 0x2CCBCDD60B845E55i64) + 40i64) - 47i64) ^ 0x14) - 0x69BCCADF0EFC7339i64;
	}
	uint64_t RoundManager()
	{
		return __ROL8__(driver::Read<uint64_t>(Base + 0x712CC88) - 0x75FC718305CFEEBDi64, 2);
	}
	uint64_t GameState()
	{
		return __ROL4__((driver::Read<uint64_t>(globals.roundManager + 181) ^ 0x13) - 13, 24);
	}
	const char* OperatorNames[28][6] =
	{
		// Credits Dude on UC for opnames. Please note only up till thunderbird
		{"AI","SMOKE","MUTE","SLEDGE","THATCHER"}, //0
		{"RECRUIT","CASTLE","ASH","PULSE","THERMITE"}, //1
		{"","DOC","ROOK","TWITCH","MONTAGNE"}, //2
		{"","GLAZ","FUZE","KAPKAN","TACHANKA"}, //3
		{"","BLITZ","IQ","JAGER","BANDIT"}, //4
		{"","BUCK","FROST"}, //5
		{"","BLACKBEARD","VALKYRIE"}, //6
		{"","CAPITAO","CAVEIRA"}, //7
		{"","HIBANA","ECHO"}, //8
		{"","JACKAL","MIRA"}, //9
		{"","YING","LESION"}, //10
		{"","ELA","ZOFIA"}, //11
		{"","DOKKAEBI","VIGIL"}, //12
		{"","","LION","FINKA"}, //13
		{"","MAESTRO","ALIBI"}, //14
		{"","MAVERICK","CLASH"}, //15
		{"","NOMAD","KAID", ""}, //16
		{"","MOZZIE","GRIDLOCK"}, //17
		{"","NOKK"}, //18
		{"","WARDEN"}, //19
		{"","GOYO"}, //20
		{"","AMARU"}, //21
		{"","KALI","WAMAI", "ACE", "ARUNI", "OSA"}, //22
		{"","ORYX", "FLORES"}, //23
		{"","IANA"}, //24
		{"","MELUSI"}, //25
		{"ZERO"}, //26
		{"THUNDERBIRD"}, //26
	};
	auto OpName(uintptr_t player) -> const char*
	{
		uintptr_t replicationInfo = Game::ReplicationInfo(player);
		auto ctu = driver::Read<uint8_t>(replicationInfo + 0x1C0);
		auto op = driver::Read<uint8_t>(replicationInfo + 0x1C1);
		return OperatorNames[ctu][op];
	}
};



