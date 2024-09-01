#include "ObjectListUI.h"

void DoDockSpace()
{
    // READ THIS !!!
    // TL;DR; this demo is more complicated than what most users you would normally use.
    // If we remove all options we are showcasing, this demo would become:
    //     void ShowExampleAppDockSpace()
    //     {
    //         ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    //     }
    // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
    // In this specific demo, we are not using DockSpaceOverViewport() because:
    // - (1) we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
    // - (2) we allow the host window to have padding (when opt_padding == true)
    // - (3) we expose many flags and need a way to have them visible.
    // - (4) we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport()
    //      in your code, but we don't here because we allow the window to be floating)

    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
            if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
            if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
            ImGui::Separator();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}


void Table::RenderUI(ObjectsList& objList)
{
    DoDockSpace();
    ImGui::Begin("Objects");
    size_t objectsAmount = objList.getObjectsAmount();
    if (ImGui::BeginTable("Objects", 5, true))
    {
        if (objList.getCurrentSort() == NOSORT)
        {
            for (size_t row = 0; row < objectsAmount; row++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(objList.getObject(row).m_Name.c_str(), row, 0);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(objList.getObject(row).m_CoordX.c_str(), row, 1);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text(objList.getObject(row).m_CoordY.c_str(), row, 2);
                ImGui::TableSetColumnIndex(3);
                ImGui::Text(objList.getObject(row).m_Type.c_str(), row, 3);
                ImGui::TableSetColumnIndex(4);
                ImGui::Text(std::to_string(objList.getObject(row).m_Time).c_str(), row, 4);
            }
        }
        else
        {
            static int row = 0;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            std::string sortLabel = "Group and sorted by ";
            switch (objList.getCurrentSort())
            {
            case BY_DIST:
                sortLabel += "distance";
                break;
            case BY_NAME:
                sortLabel += "name";
                break;
            case BY_TYPE:
                sortLabel += "type";
                break;
            case BY_TIME:
                sortLabel += "creation date";
                break;
            default:
                break;
            }
            ImGui::Text(sortLabel.c_str());
            for (const auto& so : objList.getGroupedObjects())
            {
                if (so.second.size() == 0)
                    continue;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(so.first.c_str(), row++, 0);
                for (const auto& obj : so.second)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text(obj.m_Name.c_str(), row++, 0);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text(obj.m_CoordX.c_str(), row++, 1);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text(obj.m_CoordY.c_str(), row++, 2);
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text(obj.m_Type.c_str(), row++, 3);
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text(std::to_string(obj.m_Time).c_str(), row, 4);
                }
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("", row++, 0);
            }            
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void Menu::RenderUI(ObjectsList& objList)
{
    static char fileName[128] = "";

    ImGui::Begin("Menu");
    ImGui::InputTextWithHint(" ", "Enter File Name", fileName, IM_ARRAYSIZE(fileName));
    if (ImGui::Button("Load File"))
    {

        if (!objList.readFile(fileName))
        {
            ImGui::OpenPopup("Not Opened!");
        }

    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Not Opened!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("File hasn't opened!\nWrong file name.");
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }   

    if (ImGui::Button("Save File"))
    {
        objList.saveFile(fileName);
    }

    const char* objectCategory[] = { "By Distance", "By Name", "By Type", "By Creation Time" };
    static int selectedCategory = 0;
    ImGui::Combo("Sort Method", &selectedCategory, objectCategory, IM_ARRAYSIZE(objectCategory));
    
    if (ImGui::Button("Group Objects"))
    {
        objList.groupAndSort(static_cast<SORTSTATE>(selectedCategory + 1));
    }

    if (ImGui::Button("Add Object"))
        ImGui::OpenPopup("Adding Object");
    if (ImGui::BeginPopupModal("Adding Object", NULL, ImGuiWindowFlags_MenuBar))
    {
        AddObjectMenu::RenderUI(objList);
        ImGui::EndPopup();
    }

    ImGui::End();
}

void AddObjectMenu::RenderUI(ObjectsList& objList)
{
    static char objectName[128] = "";
    ImGui::InputText("Object Name", objectName, IM_ARRAYSIZE(objectName));

    static float objectCoords[2] = { 0.f, 0.f };
    ImGui::InputFloat2("Coordinates (X/Y)", objectCoords);

    const char* objectCategory[] = { "...", "Building", "Car", "Person" };
    static int selectedCategory = 0;
    ImGui::Combo("Category", &selectedCategory, objectCategory, IM_ARRAYSIZE(objectCategory));


    static int objectDate[3] = { 0, 0, 0 };
    ImGui::InputInt3("Creation date (DD/MM/YYYY)", objectDate);
    
    static int objectTime[3] = { 0, 0, 0 };
    ImGui::InputInt3("Creation time (HH/MM/SS)", objectTime);

    if (ImGui::Button("Add"))
    {
        uint32_t newTimeInUNIX = objectTime[2] + objectTime[1] * 60 + objectTime[0] * 3600;
        uint32_t newDateInUNIX = objectDate[2] * 86400 + objectDate[1] * 86400 * 30 + objectDate[0] * 31536000;
        uint32_t newUNIX = newTimeInUNIX + newDateInUNIX;
        std::string currCat(objectCategory[selectedCategory]);

        std::stringstream ss;
        ss << objectName << ' ';
        ss << objectCoords[0] << ' ' << objectCoords[1] << ' ';
        ss << currCat + ' ';
        ss << std::to_string(newUNIX);

        objList.addObject(objList.getObjectFromString(ss));
        ImGui::CloseCurrentPopup();
    } 
    if (ImGui::Button("Close"))
    {
        ImGui::CloseCurrentPopup();
    }
}