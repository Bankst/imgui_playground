#include <chrono>
#include <cstdint>
#include <iostream>
#include <ntcore_cpp.h>
#include <opencv2/opencv.hpp>
#include <imgui.h>
#include <imgui_ProggyDotted.h>
#include <imgui_impl_glfw.h>
#include "GL/glcorearb.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>
#include "GL/gl3w.h"
#include <GLFW/glfw3.h>
#include <ratio>
#include <list>

#include "CvUtils.hpp"
#include "colormod.h"

using highres_clock = std::chrono::steady_clock;

Color::Modifier default_color(Color::FG_DEFAULT);
Color::Modifier info_color(Color::FG_CYAN);
Color::Modifier err_color(Color::FG_RED);

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool show_demo_window = false;

GLFWwindow* window;
const ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

cv::Mat cvImage;
GLuint cvimgTextureId = -1;

void guiLoop();
void fillFrame();
void render();
void endFrame();

void log_info(const char* msg) {
	std::cout << info_color << "[INFO] " << default_color << msg << std::endl;
}

void log_err(const char* msg) {
	std::cout << err_color << "[ERROR] " << default_color << msg << std::endl;
}

int main() {
	log_info("Starting ImGui Thingy++");

	glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
			log_err("GLFW init FAIL");
			return 1;
		}
	log_info("GLFW init OK");

	#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
	#endif

	window = glfwCreateWindow(1280, 720, "ImGui Thingy++", NULL, NULL);
	if (window == NULL) {
		log_err("GLFW window create FAIL");
		return 1;
	}
	log_info("GLFW window create OK");

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	gl3wInit();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	
	log_info("ImGui init OK");

	cvImage = cv::imread("../resources/coolBoye.png");

	while (!glfwWindowShouldClose(window)) {
		guiLoop();
	}

	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void guiLoop() {
	// Poll GLFW events
	glfwPollEvents();

	// Start frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// fill frame
	fillFrame();

	// render
	render();

	endFrame();	
}

void fillFrame() {
	if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

	ImGuiWindowFlags window_flags = 0;

	ImGui::Begin( "imgui image", NULL, window_flags);

	auto imageConvertBeginTime = highres_clock::now();
	if (cvimgTextureId == (GLuint)-1) glGenTextures( 1, &cvimgTextureId );
	glBindTexture( GL_TEXTURE_2D, cvimgTextureId );
	glPixelStorei(GL_UNPACK_ALIGNMENT, (cvImage.step & 3) ? 1 : 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, cvImage.cols, cvImage.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, cvImage.ptr() );
	auto imageConvertElapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(highres_clock::now() - imageConvertBeginTime);
	ImGui::Image( reinterpret_cast<void*>( static_cast<intptr_t>( cvimgTextureId ) ), ImVec2( cvImage.cols, cvImage.rows ) );


	ImGui::Text("Image Type: %s", CvUtils::type2str(cvImage.type()).c_str());
	ImGui::End();

	float imageConvertElapsedMillis = imageConvertElapsedTime.count() / 1000000.0f;
	ImGui::Begin("Debug stats");
	if(ImGui::TreeNode("Image Conversion")) {
		ImGui::Text("OpenCV->OpenGL: %.2fms", imageConvertElapsedMillis);
		ImGui::TreePop();
	}
	ImGui::End();
}

// Render frame
void render() {
	ImGui::Render();

	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
}

void endFrame() {

}
