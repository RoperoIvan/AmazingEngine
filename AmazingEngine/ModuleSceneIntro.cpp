#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	// TODO 2: Chain few N spheres together to form a snake
	float size = 0.2f;
	for(int i = 0; i < MAX_SNAKE; ++i)
	{
		s_snake[i].radius = size * (i + 1);
		s_snake[i].color = Color((1.0f/MAX_SNAKE) * i, 0.0f, 0.0f);
		s_snake[i].SetPos(1 + (size * (i + 1)) + (size * i), 10.f, 0.f);
		pb_snake[i] = App->physics->AddBody(s_snake[i]);
		if(i > 0)
			App->physics->AddConstraintP2P(*pb_snake[i], *pb_snake[i - 1], vec3(-((i + 4) * size), 0, 0), vec3((i + 4) * size, 0, 0));
	}

	// TODO 4: Chain few N spheres together to form a a bike's sphere
	// Be sure to put the right axis
	float size2 = 0.2f;
	for(int i = 0; i < MAX_SNAKE; ++i)
	{
		s_snake2[i].radius = size2 * (i + 1);
		s_snake2[i].color = Color(0.0f, 0.0f, (1.0f / MAX_SNAKE) * i);
		s_snake2[i].SetPos(1 + (size2 * (i + 1)) + (size2 * i), 10.f, MAX_SNAKE);
		pb_snake2[i] = App->physics->AddBody(s_snake2[i]);
		if(i > 0)
			App->physics->AddConstraintHinge(*pb_snake2[i], *pb_snake2[i - 1], vec3(-((i * 3) * size2), 0, 0), vec3(0, 0, 0), vec3(0, 1, 0), vec3(0, 1, 0));
	}

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	for(int i = 0; i < MAX_SNAKE; ++i)
	{
		pb_snake[i]->GetTransform(&(s_snake[i].transform));
		s_snake[i].Render();
	}


	for(int i = 0; i < MAX_SNAKE; ++i)
	{
		pb_snake2[i]->GetTransform(&(s_snake2[i].transform));
		s_snake2[i].Render();
	}
	/*
	b1->GetTransform(&s1.transform);
	s1.Render();

	b2->GetTransform(&s2.transform);
	s2.Render();

	b3->GetTransform(&s3.transform);
	s3.Render();

	b4->GetTransform(&s4.transform);
	s4.Render();
	*/
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

