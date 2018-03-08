#define PLATFORM_WIN
#include "ChokoLait.h"
#include "Water.h"
CHOKOLAIT_INIT_VARS;

Background* bg = new Background(IO::path + "/refl.hdr");
Font* font = new Font(IO::path + "/arimo.ttf");
Water* water;

double told = 0;
uint fpsc = 0;
uint fps;
ulong simtime;

float camz = 2.0f;

Material* mat;
Shader* shad;
Mesh* mesh;

void updateFunc() {
	if (Input::KeyHold(Key_W)) camz -= Time::delta;
	else if (Input::KeyHold(Key_S)) camz += Time::delta;
	camz = Clamp<float>(camz, 0.5f, 10);
}

void rendFunc() {
	glTranslatef(-Water::me->wall / 2, -Water::me->wall / 2, -Water::me->wall / 2);
	Engine::DrawMeshInstanced(mesh, 0, mat, 256 * 3);
}

void paintfunc() {
	if (++fpsc == 100) {
		fps = (uint)roundf(100.0f / (Time::time - told));
		told = Time::time;
		fpsc = 0;
	}
	UI::Label(10, 10, 12, "fps: " + std::to_string(fps), font, white());
	UI::Label(10, 25, 12, "t: " + std::to_string(++simtime / 1000) + " ps", font, white());
	//UI::Label(10, 40, 12, "T: " + std::to_string((int)roundf(Water::me->res_tmp)) + " K", font, white());
	//UI::Label(10, 55, 12, "P: " + std::to_string(Water::me->res_prs) + " atm", font, white());
}

int main(int argc, char **argv)
{
	ChokoLait::Init(500, 500);

	auto& set = Scene::active->settings;
	set.sky = bg;
	set.skyStrength = 2;

	shad = new Shader(IO::GetText(IO::path + "/instV.txt"), IO::GetText(IO::path + "/instF.txt"));
	mat = new Material(shad);
	mesh = Procedurals::UVSphere(20, 10);

	font->Align(ALIGN_TOPLEFT);
	water = new Water(IO::path + "/water.compute", 4, 1000.0f / 1.66f / 17.99f, 200);
	mat->SetBuffer(5, water->psb);

	while (ChokoLait::alive()) {
		ChokoLait::Update(updateFunc);
		ChokoLait::mainCamera->object->transform.localPosition(Vec3(0, 0, -camz));
		//if (Input::KeyHold(Key_D)) tr.Rotate(0, Time::delta * 60, 0);
		//if (Input::KeyHold(Key_A)) tr.Rotate(0, -Time::delta * 60, 0);
		water->Update();
		
		ChokoLait::Paint(rendFunc, paintfunc);
	}
}