#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "ImGui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION	// include之前必须定义
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
struct ImageBuf
{
	typedef unsigned int u32;

	int             Width, Height;
	u32*            Data;

	ImageBuf() { Width = Height = 0; Data = NULL; }
	~ImageBuf() { Clear(); }
	void Clear() { if (Data) free(Data); Data = NULL; }

	void CreateEmpty(int w, int h)
	{
		Clear();
		Width = w;
		Height = h;
		Data = (u32*)malloc(Width * Height * 4);
		memset(Data, 0, Width * Height * 4);
	}
	void CreateFromCaptureGL(int x, int y, int w, int h)
	{
		Clear();
		Width = w;
		Height = h;
		Data = (u32*)malloc(Width * Height * 4);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, Data);
		RemoveAlpha();
	}

	void SaveFile(const char* filename)
	{
		stbi_write_png(filename, Width, Height, 4, Data, Width * 4);
	}

	void RemoveAlpha()
	{
		u32* p = Data;
		int n = Width * Height;
		while (n-- > 0)
		{
			*p |= 0xFF000000;
			p++;
		}
	}

	void BlitTo(ImageBuf* dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h)
	{
		ImageBuf* src = this;
		IM_ASSERT(dst != src);
		IM_ASSERT(dst != NULL);
		IM_ASSERT(src_x >= 0 && src_y >= 0);
		IM_ASSERT(src_x + w <= src->Width);
		IM_ASSERT(src_y + h <= src->Height);
		IM_ASSERT(dst_x >= 0 && dst_y >= 0);
		IM_ASSERT(dst_x + w <= dst->Width);
		IM_ASSERT(dst_y + h <= dst->Height);
		for (int y = 0; y < h; y++)
			memcpy(dst->Data + dst_x + (dst_y + y) * dst->Width, src->Data + src_x + (src_y + y) * src->Width, w * 4);
	}

	void FlipVertical()
	{
		int comp = 4;
		int stride = Width * comp;
		unsigned char* line_tmp = new unsigned char[stride];
		unsigned char* line_a = (unsigned char*)Data;
		unsigned char* line_b = (unsigned char*)Data + (stride * (Height - 1));
		while (line_a < line_b)
		{
			memcpy(line_tmp, line_a, stride);
			memcpy(line_a, line_b, stride);
			memcpy(line_b, line_tmp, stride);
			line_a += stride;
			line_b -= stride;
		}
		delete[] line_tmp;
	}
	u32* GetPtr(int x, int y)
	{
		return &Data[x + y * Width];
	}
	u32 GetPixel(int x, int y) const
	{
		return Data[x + y * Width];
	}
};

struct ScreenshotMaker
{
	ImGuiStorage    Storage;
	float           BorderSize;
	bool            LargePictureMode;
	ImRect          VisibleRect;
	bool            CaptureActive;
	int             CaptureFrame;
	ImRect          CaptureRect;
	int             CaptureLargeOffY;
	ImGuiWindow*    CaptureWindow;
	ImageBuf        Output;

	ScreenshotMaker()
	{
		BorderSize = 16.0f;
		LargePictureMode = false;
		CaptureActive = false;
		CaptureFrame = 0;
		CaptureLargeOffY = 0;
		CaptureWindow = NULL;
	}

	void CaptureStart(ImGuiWindow* window)
	{
		CaptureActive = true;
		CaptureFrame = 0;
		Output.Clear();
		CaptureWindow = window;
		if (!CaptureWindow)
			LargePictureMode = false;

		if (LargePictureMode)
		{
			ImGui::SetWindowSize(CaptureWindow->Name, ImVec2(CaptureWindow->SizeFull.x, CaptureWindow->SizeContents.y));
			Output.CreateEmpty((int)(CaptureWindow->Size.x + BorderSize * 2), (int)(CaptureWindow->Size.y + BorderSize * 2));
		}
		else
		{
			// Capture immediately
			ImGuiIO& io = ImGui::GetIO();
			Output.CreateFromCaptureGL((int)CaptureRect.Min.x, (int)io.DisplaySize.y - (int)CaptureRect.Max.y, (int)CaptureRect.GetWidth(), (int)CaptureRect.GetHeight());
			Output.FlipVertical();
			CaptureEnd();
			return;
		}
	}

	void CaptureUpdate()
	{
		if (!LargePictureMode)
			return;

		int TIME_STEP = 15;
		IM_ASSERT(TIME_STEP > 5);

		int chunk_no = CaptureFrame / TIME_STEP;
		int chunk_w = (int)CaptureRect.GetWidth();
		int chunk_h = (int)CaptureRect.GetHeight();

		if ((CaptureFrame % TIME_STEP) == 2)
		{
			// Move window
			ImVec2 p = ImVec2(BorderSize+CaptureRect.Min.x, BorderSize+CaptureRect.Min.y);
			p.y -= chunk_no * CaptureRect.GetHeight();
			ImGui::SetWindowPos(CaptureWindow->Name, p);
		}

		if ((CaptureFrame % TIME_STEP) == 4)
		{
			// Capture, stitch into output
			ImGuiIO& io = ImGui::GetIO();
			ImageBuf img;
			img.CreateFromCaptureGL((int)CaptureRect.Min.x, (int)io.DisplaySize.y - (int)CaptureRect.Max.y, chunk_w, chunk_h);
			img.FlipVertical();
			int chunk_y = chunk_no * chunk_h;
			img.BlitTo(&Output, 0, 0, 0, chunk_y, chunk_w, ImMin(chunk_h, Output.Height - chunk_y));
			char buf[128];
			sprintf_s(buf, "out%03d.png", chunk_no);
			//img.SaveFile(buf);
			if (CaptureWindow->Pos.y + CaptureWindow->SizeFull.y < CaptureRect.Max.y)
				CaptureEnd();
		}

		CaptureFrame++;
	}

	void CaptureEnd()
	{
		if (Output.Data)
			Output.SaveFile("out.png");
		CaptureActive = false;
	}

	void Draw(bool* p_open = NULL)
	{
		if (!ImGui::Begin("Screenshot Maker", p_open))
		{
			ImGui::End();
			return;
		}

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = ImGui::GetIO();

		bool capture = ImGui::Button("ALT+C: Capture");
		if (CaptureActive)
		{
			ImGui::SameLine();
			ImGui::Text("Capturing...");
		}
		ImGui::SliderFloat("Border", &BorderSize, 0.0f, 32.0f, "%.0f");
		ImGui::Checkbox("Large Window Capture Mode (1 window)", &LargePictureMode);
		ImGui::Separator();

		ImVector<ImGuiWindow*> windows_selected;
		VisibleRect = ImRect(ImVec2(0, 0), io.DisplaySize);
		ImRect capture_rect;

		ImGui::Text("Windows:");
		for (int i = 0; i < g.Windows.size(); i++)
		{
			ImGuiWindow* window = g.Windows[i];
			if (!window->Active && !window->WasActive)
				continue;
			if (window->Flags & ImGuiWindowFlags_Popup)
			{
				CaptureRect.Add(window->Rect());
				continue;
			}
			if (window->Flags & ImGuiWindowFlags_ChildWindow)
				continue;

			ImGui::PushID(window);
			bool* p_selected = (bool*)Storage.GetIntRef(window->RootWindow->ID, 0);
			ImGui::Checkbox("##checkbox", p_selected);
			ImGui::SameLine();
			ImGui::TextUnformatted(window->Name);
			if (*p_selected)
			{
				capture_rect.Add(window->Rect());
				windows_selected.push_back(window);
			}
			ImGui::SameLine(160);
			ImGui::PushItemWidth(120);
			ImGui::DragFloat2("Pos", &window->Pos.x, 0.05f, 0.0f, 0.0f, "%.0f");
			ImGui::SameLine();
			ImGui::DragFloat2("Size", &window->SizeFull.x, 0.05f, 0.0f, 0.0f, "%.0f");
			ImGui::PopItemWidth();
			ImGui::PopID();
		}

		capture_rect.Expand(BorderSize);
		capture_rect.ClipWithFull(VisibleRect);

		if (!CaptureActive)
			CaptureRect = capture_rect;

		char buf[128];
		sprintf_s(buf, "%.0f x %.0f", CaptureRect.GetWidth(), capture_rect.GetHeight());
		//g.drawq.AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(1, 1), IM_COL32_WHITE, buf);
		//g.OverlayDrawList.AddRect(CaptureRect.Min - ImVec2(1, 1), CaptureRect.Max + ImVec2(1, 1), IM_COL32_WHITE);

		capture |= io.KeyAlt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C));
		if (capture)
			CaptureStart(windows_selected.empty() ? NULL : windows_selected[0]);

		if (CaptureActive)
			CaptureUpdate();

		ImGui::End();
	}
};