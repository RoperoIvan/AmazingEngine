#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Geometry.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	//CalculateViewMatrix();

	//X = vec3(1.0f, 0.0f, 0.0f);
	//Y = vec3(0.0f, 1.0f, 0.0f);
	//Z = vec3(0.0f, 0.0f, 1.0f);

	//Position = vec3(0.0f, 0.0f, 5.0f);
	//Reference = vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	c_frustum = &my_camera->frustum;
	CameraControls(dt);
	//CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
//void ModuleCamera3D::Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference)
//{
//	this->Position = Position;
//	this->Reference = Reference;
//
//	Z = normalize(Position - Reference);
//	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
//	Y = cross(Z, X);
//
//	if (!RotateAroundReference)
//	{
//		this->Reference = this->Position;
//		this->Position += Z * 0.05f;
//	}
//
//	CalculateViewMatrix();
//}

// -----------------------------------------------------------------
//void ModuleCamera3D::LookAt(const vec3& Spot)
//{
//	Reference = Spot;
//
//	Z = normalize(Position - Reference);
//	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
//	Y = cross(Z, X);
//
//	CalculateViewMatrix();
//}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3& Movement)
{
	c_frustum->Translate(Movement);
}

void ModuleCamera3D::CameraControls(float dt)
{
	if (!write)
	{
		float3 newPos(0, 0, 0);
		//Movement controls
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed = 8.0f;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += c_frustum->front * speed; Reference += newPos*speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= c_frustum->front * speed; Reference += newPos*speed;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= c_frustum->WorldRight() * speed; Reference += newPos*speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += c_frustum->WorldRight() * speed; Reference += newPos*speed;
		
		// Mouse Zoom controls
		if (App->input->GetMouseZ() > 0)
		{
			newPos -= c_frustum->front * speed * 10;
		}
		if (App->input->GetMouseZ() < 0)
		{
			newPos += c_frustum->front * speed * 10;
		}
		// Mouse Rotation controls
		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			float3 newRot(0, 0, 0);

			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();
			newRot = my_camera->frustum.pos - Reference;
			float Sensitivity = 0.01f;
			if(dx != 0 && dy != 0)
			{
				Quat Delta_x(my_camera->frustum.up, dx * Sensitivity);
				Quat Delta_y(my_camera->frustum.WorldRight(), dy * Sensitivity);
				newRot = Delta_x.Transform(newRot);
				newRot = Delta_y.Transform(newRot);
				my_camera->frustum.pos = newRot - Reference;
				my_camera->Look(Reference);
			}
			c_frustum->Translate(newPos);
		}

		// Mouse motion ----------------

		//if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		//{
		//	int dx = -App->input->GetMouseXMotion();
		//	int dy = -App->input->GetMouseYMotion();

		//	float Sensitivity = 0.25f;

		//	Position -= Reference;

		//	if (dx != 0)
		//	{
		//		float DeltaX = (float)dx * Sensitivity;

		//		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		//		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		//		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		//	}

		//	if (dy != 0)
		//	{
		//		float DeltaY = (float)dy * Sensitivity;

		//		Y = rotate(Y, DeltaY, X);
		//		Z = rotate(Z, DeltaY, X);

		//		if (Y.y < 0.0f)
		//		{
		//			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
		//			Y = cross(Z, X);
		//		}
		//	}

		//	Position = Reference + Z * length(Position);
		//}
	}
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

void ModuleCamera3D::GoAroundGeometry(GameObject* obj)
{
	if (obj == nullptr)
		return;

	std::vector<float3> vertices;
	if (obj->children.empty())
	{
		vertices = LoadAABBVertex(obj, vertices);
	}
	else
	{
		for (std::vector<GameObject*>::iterator iter = obj->children.begin(); iter < obj->children.end(); ++iter)
		{
			vertices = LoadAABBVertex((*iter), vertices);
		}
	}
	//Creates an AABB 
	//math::AABB general(float3(0, 0, 0), float3(0, 0, 0));
	//general.Enclose(&vertices[0], vertices.size());

	//Position.x = general.maxPoint.x*1.5;
	//Position.y = general.maxPoint.y*1.5;
	//Position.z = general.maxPoint.z*1.5;

	//Reference.x = general.CenterPoint().x;
	//Reference.y = general.CenterPoint().y;
	//Reference.z = general.CenterPoint().z;

	//LookAt(Reference);
}

std::vector<float3> ModuleCamera3D::LoadAABBVertex(GameObject * obj, std::vector<float3> vertices)
{
	for (std::vector < Component*>::iterator iter2 = obj->components.begin(); iter2 != obj->components.end(); ++iter2)
	{
		COMPONENT_TYPE type = (*iter2)->type;
		if (type == COMPONENT_TYPE::COMPONENT_MESH)
		{
			//Generate AABBS for each geom in scene
			math::AABB new_aabb(float3(0, 0, 0), float3(0, 0, 0));
			std::vector <float3> vertex_array;

			Geometry* g = dynamic_cast<Geometry*>(*iter2);
			for (int j = 0; j < g->num_vertices * 3; j += 3)
			{
				vertex_array.push_back(float3(g->vertices[j], g->vertices[j + 1], g->vertices[j + 2]));
			}

			new_aabb.Enclose(&vertex_array[0], g->num_vertices);

			//Stores the 8 vertices of the box in a general array
			for (int j = 0; j < 8; j++)
			{
				vertices.push_back(new_aabb.CornerPoint(j));
			}
		}

	}
	return vertices;
}
bool ModuleCamera3D::Save(nlohmann::json& j) const
{
	return true;
}

bool ModuleCamera3D::Load(nlohmann::json& j)
{
	return true;
}