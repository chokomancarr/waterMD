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

void DrawMeshInstanced(Mesh* mesh, uint matId, Material* mat, uint count) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glVertexPointer(3, GL_FLOAT, 0, &(mesh->vertices[0]));
	GLfloat matrix[16], matrix2[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	glGetFloatv(GL_PROJECTION_MATRIX, matrix2);
	Mat4x4 m1(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6], matrix[7], matrix[8], matrix[9], matrix[10], matrix[11], matrix[12], matrix[13], matrix[14], matrix[15]);
	Mat4x4 m2(matrix2[0], matrix2[1], matrix2[2], matrix2[3], matrix2[4], matrix2[5], matrix2[6], matrix2[7], matrix2[8], matrix2[9], matrix2[10], matrix2[11], matrix2[12], matrix2[13], matrix2[14], matrix2[15]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, &(mesh->vertices[0]));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 0, &(mesh->uv0[0]));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, &(mesh->normals[0]));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, 0, &(mesh->tangents[0]));
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
	glVertexAttribDivisor(3, 0);
	

	glUseProgram(shad->pointer);
	GLint mvp = glGetUniformLocation(shad->pointer, "_MVP");
	glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(m2 * m1));

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, Water::me->psb->pointer);
	
	glDrawElementsInstanced(GL_TRIANGLES, mesh->triangles.size(), GL_UNSIGNED_INT, &(mesh->triangles[0]), count);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	glUseProgram(0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_CULL_FACE);
}

void updateFunc() {
	if (Input::KeyHold(Key_W)) camz -= Time::delta;
	else if (Input::KeyHold(Key_S)) camz += Time::delta;
	camz = Clamp<float>(camz, 0.5f, 10);
}

void rendFunc() {
	glTranslatef(-Water::me->wall / 2, -Water::me->wall / 2, -Water::me->wall / 2);
	DrawMeshInstanced(mesh, 0, mat, 256 * 3);
}

void paintfunc() {
	if (++fpsc == 100) {
		fps = (uint)roundf(100.0f / (Time::time - told));
		told = Time::time;
		fpsc = 0;
	}
	UI::Label(10, 10, 12, "fps: " + std::to_string(fps) + "  u: " + std::to_string(Water::me->res_pot), font, white());
	UI::Label(10, 25, 12, "t: " + std::to_string(++simtime / 1000) + " ps", font, white());
	UI::Label(10, 40, 12, "T: " + std::to_string((int)roundf(Water::me->res_tmp)) + " K", font, white());
}

int main(int argc, char **argv)
{
	ChokoLait::Init(500, 500);

	auto& set = Scene::active->settings;
	set.sky = bg;

	shad = new Shader(IO::GetText(IO::path + "/instV.txt"), IO::GetText(IO::path + "/instF.txt"));
	mat = new Material(shad);
	mesh = Procedurals::UVSphere(20, 10);

	font->Align(ALIGN_TOPLEFT);
	water = new Water(IO::path + "/water.compute", 4, 1000.0f / 1.66f / 17.99f, 200);

	while (ChokoLait::alive()) {
		ChokoLait::Update(updateFunc);
		ChokoLait::mainCamera->object->transform.localPosition(Vec3(0, 0, -camz));
		water->Update();
		
		ChokoLait::Paint(rendFunc, paintfunc);
	}
}