#include "structs.hh"
HWND hwnd;
bool enabled = true;

void menu()
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		enabled = !enabled;
	}

	ImGui::SetNextWindowSize({ 300, 300 });
	ImGui::StyleColorsLight();

	if (enabled)
	{
		ImGui::Begin("Menu");
		ImGui::Checkbox("box", &settings::esp);
		ImGui::Checkbox("text", &settings::text);
		ImGui::Checkbox("lines", &settings::lines);
		ImGui::End();
	}
}

bool actor_loop()
{
	float minX = FLT_MAX;
	float maxX = -FLT_MAX;
	float minY = FLT_MAX;
	float maxY = -FLT_MAX;

	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	auto gworld = *(uintptr_t*)(utils::uworld);
	auto game_instance = *(uint64_t*)(gworld + offsets::game_instance);
	auto local_players = *(uint64_t*)(game_instance + offsets::local_players);
	auto local_player = *(uint64_t*)(local_players);

	utils::player_controller = *(uint64_t*)(local_player + offsets::player_controller);
	utils::local_pawn = *(uint64_t*)(utils::player_controller + offsets::acknowledged_pawn);

	auto levels = *(uintptr_t*)(gworld + offsets::persistent_level);
	auto actors_array = *(uintptr_t*)(levels + offsets::aactors);
	int actors_count = *(int*)(levels + offsets::actor_count);

	if (!gworld || !game_instance || !local_players)
	{
		printf("something went wrong");
		return false;
	}

	/* create a for loop to loop actors */
	for (int i = 0; i < actors_count; i++)
	{
		auto current_pawn = *(uintptr_t*)(actors_array + i * sizeof(uintptr_t));
		auto object_name = structs::getobjectname(current_pawn);

		if (strstr(object_name, ("PlayerPawn")))
		{
			structs::vector3 head, root, head1, root1;
			structs::get_bone_loc(current_pawn, 66, &head);
			structs::get_bone_loc(current_pawn, 0, &root);

			structs::world_to_screen(structs::vector3(head.x, head.y, head.z), &head);
			structs::world_to_screen(structs::vector3(root.x, root.y, root.z), &root);

			if (current_pawn == utils::local_pawn) continue;

			if (settings::esp)
			{
				/* havent felt like pasting it */
			}

			if (settings::text)
			{
				ImGui::GetOverlayDrawList()->AddText(ImVec2(root.x - 15, root.y), ImColor(255, 255, 255, 255), "Player");
			}

			if (settings::lines)
			{
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(960, 1100), ImVec2(root.x, root.y), color::red, 1);
			}
		}
	}
}

float x, y;
HRESULT hook_present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!device)
	{
		ID3D11Texture2D* renderTarget = 0;
		ID3D11Texture2D* backBuffer = 0;
		D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
		pSwapChain->GetDevice(__uuidof(device), (PVOID*)&device);
		device->GetImmediateContext(&context);

		pSwapChain->GetBuffer(0, __uuidof(renderTarget), (PVOID*)&renderTarget);
		device->CreateRenderTargetView(renderTarget, nullptr, &render_target_view);
		renderTarget->Release();
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&backBuffer);
		backBuffer->GetDesc(&backBufferDesc);
		is_inital_release = true;

		backBuffer->Release();

		x = (float)backBufferDesc.Width;
		y = (float)backBufferDesc.Height;

		if (!hwnd)
		{
			hwnd = FindWindowW(L"UnrealWindow", L"Fortnite  ");
			if (!hwnd)
			{
				hwnd = GetForegroundWindow();
			}
		}

		ImGui_ImplDX11_Init(hwnd, device, context);
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	context->OMSetRenderTargets(1, &render_target_view, nullptr);
	auto& present_scene = utils::begin_scene();

	actor_loop();
	menu();

	utils::end_scene(present_scene);
	return present_og(pSwapChain, SyncInterval, Flags);
}

ImGuiWindow& utils::begin_scene()
{
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin(("##sus"), 0, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto& io = ImGui::GetIO();
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

	return *ImGui::GetCurrentWindow();
}

void utils::end_scene(ImGuiWindow& window)
{
	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::Render();
}

void main()
{
	utils::uworld = Scanners::PatternScan("48 8B 05 ? ? ? ? 4D 8B C2");
	utils::uworld = rva(utils::uworld, 7);

	structs::_getobjectname = Scanners::PatternScan("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 48 8B F2 44 39 71 04 0F 85 ? ? ? ? 8B 19 0F B7 FB E8 ? ? ? ? 8B CB 48 8D 54 24");
	structs::free_name = Scanners::PatternScan("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0");
	structs::bone_matrix = Scanners::PatternScan("E8 ? ? ? ? 48 8B 47 30 F3 0F 10 45");
	structs::bone_matrix = rva(structs::bone_matrix, 5);

	utils::worldtoscreen = Scanners::PatternScan("E8 ? ? ? ? 41 88 07 48 83 C4 30");
	utils::worldtoscreen = rva(utils::worldtoscreen, 5);

}
