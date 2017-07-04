#include <sb7.h>
#include <vmath.h>
#include <sb7ktx.h>
#include <shader.h>
#include <object.h>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sb7textoverlay.h>
#include <png.h>
//#include <GL/glu.h>
#include <libgen.h>
#include "data.hpp"

#define MAX_HAIR_MODES 3
#define LEADING_HAIRS 0
#define CIRCULAR_HAIRS 1
#define STRIP_HAIRS 2


#include <stdio.h>

class hair_app : public sb7::application
{
public:
	void init()
	{
		srand(time(NULL));
		static const char title[] = "HairSim: Punk Rock Edition";

		sb7::application::init();

		memcpy(info.title, title, sizeof(title));

		projection_matrix = initProjectionMatrix();
		model_head_matrix = initModelMatrix();
		model_arrow_matrix = initArrowModelMatrix();
		view_head_matrix = initViewMatrix();
		view_arrow_matrix = initViewMatrix();

		status_default = "Last key pressed: ";
		key_pressed = "none";
		frame_count = 0;

		//model_filename = "model/head_model.obj";
		model_filename = "heavy_head/Heavy.obj";
		//texture_filename = "model/head_texture_nomask.png";

		readModelConfig(model_config_filename.c_str());
		readConfig(config_filename.c_str());

		hair_length = physics_constants.hair_length;
		hair_stiffness = 1.0f;
		hair_unit_length = physics_constants.hair_unit_length;

		hair_color = vmath::vec3(255, 0, 255);

		perform_update = true;
		render_hairs = 0;
		check_collision = true;
		render_model = true;

		wind_vertical_angle = 0;
		wind_vertical_angle = 0;
		wind_vector = initWindVector();
	}

	void startup(void)
	{
		model_mdl = loadOBJ(model_filename.c_str());
		arrow_mdl = loadOBJ(arrow_filename.c_str());


		//Compile model shader
		GLuint model_shaders[2];
		model_shaders[0] = sb7::shader::load("render_model.vs.glsl", GL_VERTEX_SHADER);
		model_shaders[1] = sb7::shader::load("render_model.fs.glsl", GL_FRAGMENT_SHADER);
		model_program = sb7::program::link_from_shaders(model_shaders, 2, true);
		uniforms.model_mvpMatrix = glGetUniformLocation(model_program, "mvpMatrix");
		uniforms.model_Texture = glGetUniformLocation(model_program, "modelTexture");

		printCompilerLog(model_shaders[0]);
		printCompilerLog(model_shaders[1]);
		printLinkerLog(model_program, "Model shader");

		GLuint hair_render_shaders[2];
		hair_render_shaders[0] = sb7::shader::load("render_hair.vs.glsl", GL_VERTEX_SHADER);
		hair_render_shaders[1] = sb7::shader::load("render_hair.fs.glsl", GL_FRAGMENT_SHADER);
		hair_render_program = sb7::program::link_from_shaders(hair_render_shaders, 2, true);
		uniforms.hair_mvpMatrix = glGetUniformLocation(hair_render_program, "mvpMatrix");
		uniforms.hair_hairColor = glGetUniformLocation(hair_render_program, "hairColor");
		uniforms.hair_hairPlane = glGetUniformLocation(hair_render_program, "hairPlane");

		printCompilerLog(hair_render_shaders[0]);
		printCompilerLog(hair_render_shaders[1]);
		printLinkerLog(hair_render_program, "Instanced hair (circular) render shader");

		GLuint hair_render_strip_shaders[2];
		hair_render_strip_shaders[0] = sb7::shader::load("render_hair_strip.vs.glsl", GL_VERTEX_SHADER);
		hair_render_strip_shaders[1] = sb7::shader::load("render_hair_strip.fs.glsl", GL_FRAGMENT_SHADER);
		hair_render_strip_program = sb7::program::link_from_shaders(hair_render_strip_shaders, 2, true);
		uniforms.hair_strip_mvpMatrix = glGetUniformLocation(hair_render_strip_program, "mvpMatrix");
		uniforms.hair_strip_hairColor = glGetUniformLocation(hair_render_strip_program, "hairColor");
		uniforms.hair_strip_hairPlane = glGetUniformLocation(hair_render_strip_program, "hairPlane");

		printCompilerLog(hair_render_strip_shaders[0]);
		printCompilerLog(hair_render_strip_shaders[1]);
		printLinkerLog(hair_render_strip_program, "Instanced hair (strip) render shader");

		//std::cout << "<<<<<<<" << glGetUniformLocation(hair_render_program, "hairPlane") << std::endl;

		printCompilerLog(hair_render_shaders[0]);
		printCompilerLog(hair_render_shaders[1]);
		printLinkerLog(hair_render_program, "Instanced hair render shader");

		GLuint hair_noi_render_shaders[2];
		hair_noi_render_shaders[0] = sb7::shader::load("render_hair_noinstance.vs.glsl", GL_VERTEX_SHADER);
		hair_noi_render_shaders[1] = sb7::shader::load("render_hair_noinstance.fs.glsl", GL_FRAGMENT_SHADER);
		hair_noi_render_program = sb7::program::link_from_shaders(hair_noi_render_shaders, 2, true);
		uniforms.hair_noi_mvpMatrix = glGetUniformLocation(hair_noi_render_program, "mvpMatrix");
		

		printCompilerLog(hair_noi_render_shaders[0]);
		printCompilerLog(hair_noi_render_shaders[1]);
		printLinkerLog(hair_noi_render_program, "Hair render shader");
		

		GLuint arrow_render_shaders[2];
		arrow_render_shaders[0] = sb7::shader::load("render_arrow.vs.glsl", GL_VERTEX_SHADER);
		arrow_render_shaders[1] = sb7::shader::load("render_arrow.fs.glsl", GL_FRAGMENT_SHADER);
		arrow_render_program = sb7::program::link_from_shaders(arrow_render_shaders, 2, true);
		uniforms.arrow_render_mvpMatrix = glGetUniformLocation(arrow_render_program, "mvpMatrix");
		

		printCompilerLog(arrow_render_shaders[0]);
		printCompilerLog(arrow_render_shaders[1]);
		printLinkerLog(arrow_render_program, "Arrow render shader");


		GLuint hair_update_shaders[2];
		hair_update_shaders[0] = sb7::shader::load("update_hair.vs.glsl", GL_VERTEX_SHADER);
		hair_update_shaders[1] = sb7::shader::load("update_hair.fs.glsl", GL_FRAGMENT_SHADER);

		hair_update_program = glCreateProgram();

		glAttachShader(hair_update_program, hair_update_shaders[0]);
		glAttachShader(hair_update_program, hair_update_shaders[1]);

		static const char * feedbackVaryings[] = 
		{
			"VS_OUT.position",
			"VS_OUT.velocity"
		};

		glTransformFeedbackVaryings(hair_update_program, 2, feedbackVaryings, GL_SEPARATE_ATTRIBS);
		glLinkProgram(hair_update_program);

		uniforms.hair_update_stringLength = glGetUniformLocation(hair_update_program, "stringLength");
		uniforms.hair_update_mass = glGetUniformLocation(hair_update_program, "mass");
		uniforms.hair_update_force = glGetUniformLocation(hair_update_program, "force");

		printCompilerLog(hair_update_shaders[0]);
		printCompilerLog(hair_update_shaders[1]);
		printLinkerLog(hair_update_program, "Hair updater shader");
		//std::cout << "\033[1;32m[Startup] \033[0mstringLength location: " << glGetUniformLocation(hair_update_program, "stringLength") << std::endl;
		//std::cout << "\033[1;32m[Startup] \033[0m\033[0mmass location: " << glGetUniformLocation(hair_update_program, "mass") << std::endl;
		//std::cout << "\033[1;32m[Startup] \033[0mforce location: " << glGetUniformLocation(hair_update_program, "force") << std::endl;
		std::cout << std::endl;


		///*

		if (scalp_render_program != 0)
		{
			glDeleteProgram(scalp_render_program);
		}

		GLuint scalp_render_shaders[3];

		scalp_render_shaders[0] = sb7::shader::load("gen_scalp_render.vs.glsl", GL_VERTEX_SHADER);
		//scalp_shaders[1] = sb7::shader::load("scalp.tcs.glsl", GL_TESS_CONTROL_SHADER);
		//scalp_shaders[2] = sb7::shader::load("scalp.tes.glsl", GL_TESS_EVALUATION_SHADER);
		scalp_render_shaders[1] = sb7::shader::load("gen_scalp_render.gs.glsl", GL_GEOMETRY_SHADER);
		scalp_render_shaders[2] = sb7::shader::load("gen_scalp_render.fs.glsl", GL_FRAGMENT_SHADER);

		//scalp_program = sb7::program::link_from_shaders(scalp_shaders, 5, true);
		scalp_render_program = glCreateProgram();
		glAttachShader(scalp_render_program, scalp_render_shaders[0]);
		glAttachShader(scalp_render_program, scalp_render_shaders[1]);
		glAttachShader(scalp_render_program, scalp_render_shaders[2]);

		glLinkProgram(scalp_render_program);

		uniforms.scalp_render_Texture = glGetUniformLocation(scalp_render_program, "textureMask");
		uniforms.scalp_render_mvpMatrix = glGetUniformLocation(scalp_render_program, "mvpMatrix");
		
		printCompilerLog(scalp_render_shaders[0]);
		printCompilerLog(scalp_render_shaders[1]);
		printCompilerLog(scalp_render_shaders[2]);
		//printCompilerLog(scalp_shaders[3]);
		//printCompilerLog(scalp_shaders[4]);
		printLinkerLog(scalp_render_program, "Scalp shader (debug render)");
		//std::cout << glGetUniformLocation(scalp_render_program, "mvpMatrix") << std::endl;
	
		//*/


		//Setup overlay
		overlay.init(64, 32, "font.ktx");
		overlay.clear();


		/*
		std::vector<ScalpTFObject> scalp_tf = generateScalp(scalp_masks[0]);
		//scalp = scalp_tf;
		std::vector<ScalpTFFaceObject> scalp_opt = optimizeScalp(scalp_tf);
		std::cout << "\033[1;32m[Startup] \033[0mOptimized scalp: " << scalp.size() << " vertices" << std::endl;
		hair_objects = generateLeadingHairs(scalp_opt);

		hair_vertex_buffer_data = packHairForBuffer(hair_objects);
		*/
		regenHair();
		/*
		for (int i = 0; i < hair_objects.size(); ++i)
		{
			std::cout << "Parent vertex: (" << hair_objects[i].vertices[i][0] << " "
											<< hair_objects[i].vertices[i][1] << " "
											<< hair_objects[i].vertices[i][2] << ")" << std::endl;
			for (int i = 1; i < hair_objects[i].vertices.size(); ++i)
			{
				std::cout << "Hair node: (" << hair_objects[i].vertices[i][0] << " "
											<< hair_objects[i].vertices[i][1] << " "
											<< hair_objects[i].vertices[i][2] << ")" << std::endl;
			}
			std::cout << std::endl;
		}
		*/

		//hair_vertex_buffer_data = packHairForElementsBuffer(hair_objects, hair_indices);
		//std::cout << "Written hair indices: " << hair_indices.size() << std::endl;

		//model_faces = generateModelFaces(model_mdl);
		model_collision_bound = getCollisions(model_mdl);
		for (int i = 0; i < model_collision_bound.bounds.size(); ++i)
		{
			printCollisionBound(model_collision_bound.bounds[i], model_mdl.materials[i].material.name);
		}
		//exit(-1);
	}

	void shutdown(void)
	{
		glDeleteProgram(model_program);
		
		overlay.teardown();
	}


	void render(double currentTime)
	{
		//return;
		
		renderOverlay(currentTime);
		
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		//renderScalp(scalp_masks[0]);
		//bindData(model_texBuffer);
		if (render_model)
		{
			//renderScalp(scalp_masks[0]);
			renderModel();
		}
		

		//updateHair(hair_vertex_buffer_data);
		if (perform_update)
		{
			updateHairCPU(hair_vertex_buffer_data);
		}

		switch(render_hairs)
		{
			case LEADING_HAIRS:
				renderHairNoInstance(hair_vertex_buffer_data);
				break;
			case CIRCULAR_HAIRS:
				renderHairCircular(hair_vertex_buffer_data);
				break;
			case STRIP_HAIRS:
				renderHairStrip(hair_vertex_buffer_data);
				break;
			default:
				break;
		}
		
		overlay.draw();

		renderArrow();

		//clearBuffers();
	}

	void setConfig(const char* conf)
	{
		model_config_filename = conf;
	}

private:	
	void renderOverlay(double currentTime)
	{
		char buffer[1024];
		frame_count++;
		overlay.clear();
		int overlayStart = 0;
		if (currentTime != 0)
		{
			sprintf(buffer, "%3.3f fps", (float)frame_count / (float)currentTime);
			overlay.drawText(buffer, 0, overlayStart++);
		}
		overlayStart++;
		overlay.drawText("WASD: Move camera", 0, overlayStart++);
		overlay.drawText("Arrow keys: Rotate camera", 0, overlayStart++);
		overlay.drawText("RF: Move up/down", 0, overlayStart++);
		overlay.drawText("QE: Z-rotation", 0, overlayStart++);
		overlay.drawText("U: Switch updating on/off", 0, overlayStart++);
		overlay.drawText("I: Reload config", 0, overlayStart++);
		overlay.drawText("O: Change hair rendering", 0, overlayStart++);
		overlay.drawText("C: Turn collisions on/off", 0, overlayStart++);
		overlay.drawText("M: Turn model on/off", 0, overlayStart++);
		overlay.drawText("Z: Reload hair", 0, overlayStart++);
		overlay.drawText("KP1/7: +/- wind speed", 0, overlayStart++);
		overlay.drawText("KP2/8: +/- wind Y angle", 0, overlayStart++);
		overlay.drawText("KP4/6: +/- wind X angle", 0, overlayStart++);
		overlay.drawText("KP5: Turn wind on/off", 0, overlayStart++);

		overlayStart++;
		
		status = status_default + key_pressed;
		overlay.drawText(status.c_str(), 0, overlayStart++);

		std::string info = "Updater is currently: ";
		if (perform_update)
		{
			info += "ENABLED";
		}
		else
		{
			info += "DISABLED";
		}
		overlay.drawText(info.c_str(), 0, overlayStart++);

		info = "Rendering hair: ";
		switch(render_hairs)
		{
			case 0:
				info += "LEADING";
				break;
			case 1:
				info += "CIRCULAR";
				break;
			case 2:
				info += "STRIP";
				break;
			default:
				info += "ERROR";
				break;
		}
		overlay.drawText(info.c_str(), 0, overlayStart++);

		info = "Collisions: ";
		if (check_collision)
		{
			info += "ENABLED";
		}
		else
		{
			info += "DISABLED";
		}
		overlay.drawText(info.c_str(), 0, overlayStart++);

		info = "Model: ";
		if (render_model)
		{
			info += "ENABLED";
		}
		else
		{
			info += "DISABLED";
		}
		overlay.drawText(info.c_str(), 0, overlayStart++);

		info = "Wind: ";
		if (apply_wind)
		{
			info += "ENABLED";
		}
		else
		{
			info += "DISABLED";
		}
		overlay.drawText(info.c_str(), 0, overlayStart++);

		info = "Wind speed: " + std::to_string(wind_speed);
		
		overlay.drawText(info.c_str(), 0, overlayStart++);
	}

	vmath::mat4 initProjectionMatrix()
	{
		return vmath::perspective(45.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
	}

	vmath::mat4 initModelMatrix()
	{
		return vmath::mat4().identity();
	}

	vmath::mat4 initArrowModelMatrix()
	{
		vmath::mat4 ret = vmath::mat4().identity();
		vmath::mat4 scaling = vmath::scale(0.3f, 0.3f, 0.3f);
		return ret * scaling;
	}

	vmath::mat4 initViewMatrix()
	{
		return vmath::lookat(vmath::vec3(100.0, 150.0f, 200.0),
							 vmath::vec3(0.0f, 90.0f, 0.0f),
							 vmath::vec3(0.0, 1.0, 0.0));
		/*
		return vmath::lookat(vmath::vec3(500.0, 50.0f, 100.0),
							 vmath::vec3(0.0f, -50.0f, 0.0f),
							 vmath::vec3(0.0, 1.0, 0.0));
		//*/
	}

	void loadData(const char* modelPath)
	{
		//Load model texture
		//int w, h;
		//model_texture = loadPNGTexture(texturePath, &w, &h);

		//Load model
		
		model_mdl = loadOBJ(modelPath);
	}

	void bindData(MaterialFaces mat)
	{
		//Attach texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mat.material.texture);

		//Generate buffers for models
		glGenBuffers(1, &model_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, model_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, mat.vertices.size() * sizeof(vmath::vec3), &mat.vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &model_texBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, model_texBuffer);
		glBufferData(GL_ARRAY_BUFFER, mat.texCoords.size() * sizeof(vmath::vec2), &mat.texCoords[0], GL_STATIC_DRAW);

		//Attach model:
		//1) Vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, model_vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		//2) Texture coords
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, model_texBuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
	}

	void clearBuffers()
	{
		glDeleteBuffers(1, &model_vertexBuffer);
		glDeleteBuffers(1, &model_texBuffer);
		glDeleteBuffers(1, &hair_vertexBuffer);
		glDeleteBuffers(1, &scalp_transformBuffer);
		glDeleteBuffers(1, &hair_vertexBuffer);
		glDeleteBuffers(1, &hair_noi_vertexBuffer);
	}

	void loadMaskTextures(std::vector<std::string> filenames)
	{
		for(int i = 0; i < filenames.size(); ++i)
		{
			int w, h;
			GLint maskTexture = loadPNGTexture(filenames[i].c_str(), &w, &h);
			scalp_masks.push_back(maskTexture);
		}
	}

	void renderModel()
	{
		glUseProgram(model_program);
		glUniformMatrix4fv(uniforms.model_mvpMatrix, 1, GL_FALSE, (projection_matrix * view_head_matrix * model_head_matrix));		
		glUniform1i(uniforms.model_Texture, 0);

		glViewport(0, 0, info.windowWidth, info.windowHeight);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		for(int i = 0; i < model_mdl.materials.size(); ++i)
		{
			bindData(model_mdl.materials[i]);
			glDrawArrays(GL_TRIANGLES, 0, model_mdl.materials[i].vertices.size());
		}
		

		clearBuffers();
	}

	void renderArrow()
	{
		glGenBuffers(1, &arrow_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, arrow_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, arrow_mdl.materials[0].vertices.size() * sizeof(vmath::vec3), &arrow_mdl.materials[0].vertices[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, arrow_vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		
		glUseProgram(arrow_render_program);
		glUniformMatrix4fv(uniforms.arrow_render_mvpMatrix, 1, GL_FALSE, (projection_matrix * view_arrow_matrix * model_arrow_matrix));		

		glViewport(0, 0, info.windowWidth, info.windowHeight);

		glDisable(GL_CULL_FACE);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LEQUAL);

		glDrawArrays(GL_TRIANGLES, 0, arrow_mdl.materials[0].vertices.size());
		

		clearBuffers();
	}

	ModelFace generateFace(vmath::vec3 &v0, vmath::vec3 &v1, vmath::vec3 &v2)
	{
		//	| a11 a12 a13 |	  | x-x0 x1-x0 x2-x0 |
		//	| a21 a22 a23 | = | y-y0 y1-y0 y2-y0 |
		//	| a31 a32 a33 |   | z-z0 z1-z0 z2-z0 |
		// a11*a22*a33 - a11*a23*a32 - a12*a21*a33 + a12*a23*a31 + a13*a21*a32 - a13*a22*a31
		// 
		ModelFace newFace;

		float x0 = v0[0];
		float y0 = v0[1];
		float z0 = v0[2];

		float x1 = v1[0];
		float y1 = v1[1];
		float z1 = v1[2];

		float x2 = v2[0];
		float y2 = v2[1];
		float z2 = v2[2];

		float a11 = -x0;
		float a12 = x1-x0;
		float a13 = x2-x0;

		float a21 = -y0;
		float a22 = y1-y0;
		float a23 = y2-y0;

		float a31 = -z0;
		float a32 = z1-z0;
		float a33 = z2-z0;

		newFace.D = a11*a22*a33 - a11*a23*a32;
		newFace.D += a12*a23*a31 - a12*a21*a33;
		newFace.D += a13*a21*a32 - a13*a22*a31;

		newFace.A = a22*a33 - a23*a32;
		newFace.B = a13*a32 - a12*a33;
		newFace.C = a12*a23 - a13*a22;

		return newFace;
	}

	std::vector<ModelFace> generateModelFaces(Model &mdl)
	{
		std::vector<ModelFace> ret;

		for (int j = 0; j < mdl.materials.size(); ++j)
		{
			for(int i = 0; i < mdl.materials[j].vertices.size(); i+=3)
			{
				vmath::vec3 v0 = mdl.materials[j].vertices[i];
				vmath::vec3 v1 = mdl.materials[j].vertices[i+1];
				vmath::vec3 v2 = mdl.materials[j].vertices[i+2];

				ModelFace newFace = generateFace(v0, v1, v2);

				ret.push_back(newFace);
				/*
				if(i == 0)
				{
					std::cout << newFace.A << "x + " << newFace.B << "y + " << newFace.C << "z + " << newFace.D << " = 0" << std::endl;
				}
				*/
			}
		}

		return ret;
	}

	std::vector<ScalpTFObject> generateScalp(GLint maskTexture)
	{
		//Compile scalp program
		if (scalp_program == 0)
		{
			GLuint scalp_shaders[3];

			scalp_shaders[0] = sb7::shader::load("gen_scalp.vs.glsl", GL_VERTEX_SHADER);
			//scalp_shaders[1] = sb7::shader::load("scalp.tcs.glsl", GL_TESS_CONTROL_SHADER);
			//scalp_shaders[2] = sb7::shader::load("scalp.tes.glsl", GL_TESS_EVALUATION_SHADER);
			scalp_shaders[1] = sb7::shader::load("gen_scalp.gs.glsl", GL_GEOMETRY_SHADER);
			scalp_shaders[2] = sb7::shader::load("gen_scalp.fs.glsl", GL_FRAGMENT_SHADER);

			//scalp_program = sb7::program::link_from_shaders(scalp_shaders, 5, true);
			scalp_program = glCreateProgram();
			glAttachShader(scalp_program, scalp_shaders[0]);
			glAttachShader(scalp_program, scalp_shaders[1]);
			glAttachShader(scalp_program, scalp_shaders[2]);
			
			const GLchar* feedbackVaryings[] =
			{
				"gl_Position",
				"GS_OUT.normal"
			};

			glTransformFeedbackVaryings(scalp_program, 2, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

			glLinkProgram(scalp_program);

			uniforms.scalp_Texture = glGetUniformLocation(scalp_program, "textureMask");
			
			printCompilerLog(scalp_shaders[0]);
			printCompilerLog(scalp_shaders[1]);
			printCompilerLog(scalp_shaders[2]);
			//printCompilerLog(scalp_shaders[3]);
			//printCompilerLog(scalp_shaders[4]);
			printLinkerLog(scalp_program, "Scalp shader");
		}

		glEnable(GL_RASTERIZER_DISCARD);
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, maskTexture);

		glUseProgram(scalp_program);
		glUniform1i(uniforms.scalp_Texture, 0);

		int mdlSize = 0;
		int mdlMatOffset = -1;
		for (int i = 0; i < model_mdl.materials.size(); ++i)
		{
			if (model_mdl.materials[i].material.name == material_mask_name)
			{
				mdlSize = model_mdl.materials[i].vertices.size();
				mdlMatOffset = i;
				break;
			}
		}

		if(mdlMatOffset == -1)
		{
			std::cout << "\033[1;31m[Scalp generator] \033[0mCould not locate the specified mask material!" << std::endl;
			exit(-1);
		}
		
		//Generate buffers for models
		glGenBuffers(1, &model_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, model_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, model_mdl.materials[mdlMatOffset].vertices.size() * sizeof(vmath::vec3), &model_mdl.materials[mdlMatOffset].vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &model_texBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, model_texBuffer);
		glBufferData(GL_ARRAY_BUFFER, model_mdl.materials[mdlMatOffset].texCoords.size() * sizeof(vmath::vec2), &model_mdl.materials[mdlMatOffset].texCoords[0], GL_STATIC_DRAW);

		//Attach model:
		//1) Vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, model_vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		//2) Texture coords
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, model_texBuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);


		//Transform feedback buffer
		glGenBuffers(1, &scalp_transformBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, scalp_transformBuffer);
		glBufferData(GL_ARRAY_BUFFER, mdlSize * (sizeof(vmath::vec4) + sizeof(vmath::vec3)), NULL, GL_STATIC_READ);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, scalp_transformBuffer);
		glBeginTransformFeedback(GL_TRIANGLES);



		//Amount written
		GLuint p1, p1q;
		GLuint primitives_written, primitives_written_query;
		glGenQueries(1, &primitives_written_query);
		glGenQueries(1, &p1q);
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, primitives_written_query);
		glBeginQuery(GL_PRIMITIVES_GENERATED, p1q);

		glDrawArrays(GL_TRIANGLES, 0, model_mdl.materials[mdlMatOffset].vertices.size());

		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		glEndQuery(GL_PRIMITIVES_GENERATED);

		glGetQueryObjectuiv(primitives_written_query, GL_QUERY_RESULT, &primitives_written);
		glGetQueryObjectuiv(p1q, GL_QUERY_RESULT, &p1);

		glEndTransformFeedback();
		glFlush();

		glDisable(GL_RASTERIZER_DISCARD);

		std::cout << "\033[1;32m[Scalp generator] \033[0mScalp contains " << primitives_written << " primitives" << std::endl;
		std::cout << "\033[1;32m[Scalp generator] \033[0mPrimitives generated: " << p1 << std::endl;
		std::cout << std::endl;

		//Extract data written
		ScalpTFObject feedback_data[primitives_written];
		//feedback_data.reserve(primitives_written);
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, (sizeof(vmath::vec4) + sizeof(vmath::vec3))*primitives_written, feedback_data);
		
		std::vector<ScalpTFObject> feedback_data_vector;
		for(int i = 0; i < primitives_written; ++i)
		{
			feedback_data_vector.push_back(feedback_data[i]);
		}

		/*
		for(int i = 0; i < 3; ++i)
		{
			std::cout << "gl_Position:\t(" 	<< feedback_data[i].gl_Position[0] << " " 
											<< feedback_data[i].gl_Position[1] << " " 
											<< feedback_data[i].gl_Position[2] << ")" << std::endl;
			std::cout << "normal:\t\t(" 		<< feedback_data[i].normal[0] << " "
											<< feedback_data[i].normal[1] << " "
											<< feedback_data[i].normal[2] << ")" << std::endl;
		}
		*/

		clearBuffers();
		return feedback_data_vector;
	}

	//NEEDS FIXING
	void renderScalp(GLint texture)
	{
		/*
		glUseProgram(model_program);
		glUniformMatrix4fv(uniforms.model_mvpMatrix, 1, GL_FALSE, (projection_matrix * view_head_matrix * model_head_matrix));		
		glUniform1i(uniforms.model_Texture, 0);
		//*/
		///*
		glUseProgram(scalp_render_program);
		glUniformMatrix4fv(uniforms.scalp_render_mvpMatrix, 1, GL_FALSE, (projection_matrix * view_head_matrix * model_head_matrix));
		glUniform1i(uniforms.scalp_render_Texture, 0);
		//*/

		//Amount written
		GLuint p1, p1q;
		glGenQueries(1, &p1q);
		glBeginQuery(GL_PRIMITIVES_GENERATED, p1q);

		int mdlSize = 0;
		int mdlMatOffset = -1;
		for (int i = 0; i < model_mdl.materials.size(); ++i)
		{
			if (model_mdl.materials[i].material.name == material_mask_name)
			{
				mdlSize = model_mdl.materials[i].vertices.size();
				mdlMatOffset = i;
				break;
			}
		}

		if(mdlMatOffset == -1)
		{
			std::cout << "\033[1;31m[Scalp generator] \033[0mCould not locate the specified mask material!" << std::endl;
			exit(-1);
		}

		MaterialFaces customMat = model_mdl.materials[mdlMatOffset];
		customMat.material.texture = texture;
		bindData(customMat);
		
		glDrawArrays(GL_TRIANGLES, 0, customMat.vertices.size());

		//glDrawArrays(GL_TRIANGLES, 0, model_mdl.vertices.size());

		glEndQuery(GL_PRIMITIVES_GENERATED);

		glGetQueryObjectuiv(p1q, GL_QUERY_RESULT, &p1);
		glFlush();

		static GLuint p2 = 0;
		if (p2 != p1)
		{
			std::cout << "\033[1;32m[Scalp renderer] \033[0mPrimitives generated: " << p1 << std::endl;
			p2 = p1;
		}

		clearBuffers();

	}

	std::vector<ScalpTFFaceObject> collectFaces(std::vector<ScalpTFObject> &source)
	{
		std::vector<ScalpTFFaceObject> ret;

		if (source.size() % 3 != 0)
		{
			std::cout << "\033[1;31m[Faces Collector] \033[0mInvalid array size!" << std::endl;
				exit(-1);
		}

		for(int i = 0; i < source.size(); i += 3)
		{
			vmath::vec3 v1 = vmath::vec3(source[i].gl_Position[0], 
										source[i].gl_Position[1], 
										source[i].gl_Position[2]);
			vmath::vec3 v2 = vmath::vec3(source[i+1].gl_Position[0], 
										source[i+1].gl_Position[1], 
										source[i+1].gl_Position[2]);
			vmath::vec3 v3 = vmath::vec3(source[i+2].gl_Position[0], 
										source[i+2].gl_Position[1], 
										source[i+2].gl_Position[2]);

			ScalpTFFaceObject newFaceObj;
			newFaceObj.face_plane = generateFace(v1, v2, v3);

			newFaceObj.normal = source[i].normal;
			newFaceObj.center = vmath::vec4((v1[0] + v2[0] + v3[0])/3,
											(v1[1] + v2[1] + v3[1])/3,
											(v1[2] + v2[2] + v3[2])/3,
											1.0f);
			/*
			std::cout << source[0].normal[0] << " "
						<< source[0].normal[1] << " "
						<< source[0].normal[2] << std::endl;
						*/

			//newFaceObj.gl_Position = source[i].gl_Position;

			ret.push_back(newFaceObj);
		}

		return ret;
	}

	std::vector<ScalpTFFaceObject> optimizeScalp(std::vector<ScalpTFObject> &source)
	{
		/*
		std::vector<ScalpTFCollectorObject> normalAggregator;

		int initSize = source.size();
		while(source.size() > 0 && (source.size() <= initSize))
		{
			//std::cout << source.size() << std::endl;
			normalAggregator.push_back(collectRepeatingVertexData(source, 0));
		}
		*/

		std::vector<ScalpTFFaceObject> faceAggregator = collectFaces(source);
		
		return faceAggregator;

		/*
		for (int i = 0; i < normalAggregator.size(); ++i)
		{
			std::cout << "gl_Position:\t(" 	<< normalAggregator[i].gl_Position[0] << " " 
											<< normalAggregator[i].gl_Position[1] << " " 
											<< normalAggregator[i].gl_Position[2] << ")" << std::endl;

			std::cout << "normal amount: " << normalAggregator[i].normals.size() << std::endl;
			for(int j = 0; j < normalAggregator[i].normals.size(); ++j)
			{
				std::cout << "normal:\t\t(" << normalAggregator[i].normals[j][0] << " "
											<< normalAggregator[i].normals[j][1] << " "
											<< normalAggregator[i].normals[j][2] << ")" << std::endl;
			}
			std::cout << std::endl;
		}
		*/

		//std::vector<ScalpTFObject> ret;

		/*
		for(int i = 0; i < normalAggregator.size(); ++i)
		{
			vmath::vec3 finalNormal = vmath::vec3(0, 0, 0);
			for(int j = 0; j < normalAggregator[i].normals.size(); ++j)
			{
				finalNormal += normalAggregator[i].normals[j];
			}
			//std::cout << normalAggregator[i].normals.size() << std::endl;

			ScalpTFObject newObj;
			newObj.gl_Position = normalAggregator[i].gl_Position;
			newObj.normal = vmath::normalize(finalNormal);
			ret.push_back(newObj);
		}
		//*/

		//return ret;
	}

	ScalpTFCollectorObject collectRepeatingVertexData(std::vector<ScalpTFObject> &source, int index)
	{
		ScalpTFCollectorObject ret;

		if (source.size() == 0)
		{
			std::cout << "\033[1;31m[Scalp Repeating Vertices Collector] \033[0mEmpty array passed!" << std::endl;
			return ret;
		}

		std::vector<int> toDel;

		ret.gl_Position = source[index].gl_Position;
		ret.normals.push_back(source[index].normal);
		toDel.push_back(index);
		//source.erase(source.begin() + index);

		//std::vector<ScalpTFObject>::iterator it = source.begin();
		for(int i = 0; i < source.size(); ++i)
		{
			if (i == index)
				continue;

			if ((ret.gl_Position[0] == source[i].gl_Position[0]) &&
				(ret.gl_Position[1] == source[i].gl_Position[1]) &&
				(ret.gl_Position[2] == source[i].gl_Position[2]))
			{
				ret.normals.push_back(source[i].normal);
				toDel.push_back(i);
				//source.erase(it);
			}
		}

		for(int i = toDel.size() - 1; i > -1; --i)
		{
			//std::cout << "del " << toDel[i] << std::endl;
			source.erase(source.begin() + toDel[i]);
		}
		//std::cout << std::endl;


		return ret;
	}

	std::vector<HairObject> generateLeadingHairs(std::vector<ScalpTFFaceObject> &source)
	{
		std::vector<HairObject> ret;
		
		for (int i = 0; i < source.size(); ++i)
		{
			ret.push_back(generateHairString(source[i]));
		}
		

		return ret;
	}

	int getHairArrayOffset() const
	{
		return (int) (hair_length/hair_unit_length);
	}

	HairObject generateHairString(ScalpTFFaceObject &source)
	{
		HairObject ret;
		ret.color = hair_color;

		ret.normal = source.normal;
		ret.vertices.push_back(source.center);
		//ret.vertices.push_back(source.gl_Position);
		ret.plane = source.face_plane;
		int hairUnits = getHairArrayOffset();
		for (int i = 1; i <= hairUnits; ++i)
		{
			vmath::vec4 newVertex = ret.vertices[i-1] + vmath::vec4(source.normal, 0) * hair_unit_length;
			ret.vertices.push_back(newVertex);
			ret.velocities.push_back(vmath::vec4(0.0, 0.0, 0.0, 0.0));
			ret.forces.push_back(vmath::vec4(0.0, 0.0, 0.0, 0.0));
		}

		//std::cout << "Generated " << ret.vertices.size() << std::endl;


		return ret;
	}

	HairBufferObject packHairForBuffer(std::vector<HairObject> &source)
	{
		HairBufferObject ret;
		ret.offset = getHairArrayOffset();

		for (int i = 0; i < source.size(); ++i)
		{
			for (int j = 0; j < getHairArrayOffset(); ++j)
			{
				ret.vertices.push_back(source[i].vertices[j]);
				ret.forces.push_back(source[i].forces[j]);
				ret.velocities.push_back(source[i].velocities[j]);
				ret.normals.push_back(source[i].normal);
				ret.planes.push_back(source[i].plane);
			}
			ret.colors.push_back(source[i].color);
			//counter++;
		}
		//std::cout << "\033[1;32m[Hair buffer packer] \033[0mPacked size: " << ret.vertices.size() << std::endl;

		return ret;
	}

	std::vector<HairObject> unpackHairFromBuffer(HairBufferObject &source)
	{
		std::vector<HairObject> ret;

		for (int i = 0; i < source.vertices.size(); i += source.offset)
		{
			HairObject newHair;
			newHair.normal = source.normals[i];
			newHair.plane = source.planes[i];
			for(int j = i; j < i + source.offset; ++j)
			{
				newHair.vertices.push_back(source.vertices[j]);
				newHair.forces.push_back(source.forces[j]);
				newHair.velocities.push_back(source.velocities[j]);
			}
			ret.push_back(newHair);
		}

		//std::cout << "\033[1;32m[Hair buffer unpacker] \033[0mUnpacked hairs: " << ret.size() << std::endl;
		//std::cout << "\033[1;32m[Hair buffer unpacker] \033[0mUnpacked hair has " << ret[0].vertices.size() << " nodes" << std::endl;

		return ret;
	}

	std::vector<vmath::vec4> packHairForElementsBuffer(std::vector<HairObject> &source, std::vector<GLuint> &indices)
	{
		std::vector<vmath::vec4> ret;

		int counter = 0;
		for (int i = 0; i < source.size(); ++i)
		{
			for (int j = 0; j < getHairArrayOffset(); ++j)
			{
				ret.push_back(source[i].vertices[j]);
				indices.push_back(counter);
				if (j != getHairArrayOffset() - 1)
				{
					indices.push_back(counter);
				}
				counter++;
			}
			//counter++;
		}

		return ret;
	}

	void renderHairCircular(HairBufferObject &toRender)
	{

		glGenBuffers(1, &hair_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, hair_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, toRender.vertices.size() * sizeof(vmath::vec4), &toRender.vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, hair_vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			4,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		/*
		glGenBuffers(1, &hair_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, hair_normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, hair_vertex_buffer_data.normals.size() * sizeof(vmath::vec3), &hair_vertex_buffer_data.normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, hair_normalBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		*/

		/*
		glGenBuffers(1, &hair_indices_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hair_indices_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, hair_indices.size() * sizeof(GLuint), &hair_indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, hair_indices_buffer);
		*/
		glUseProgram(hair_render_program);
		glUniformMatrix4fv(uniforms.hair_mvpMatrix, 1, GL_FALSE, (projection_matrix * view_head_matrix * model_head_matrix));		

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//glEnable(GL_PRIMITIVE_RESTART);
		//glPrimitiveRestartIndex(9);

		GLuint p1q, p1;
		glGenQueries(1, &p1q);
		glBeginQuery(GL_PRIMITIVES_GENERATED, p1q);

		/////////////
		//glDrawElements(GL_LINES, hair_indices.size(), GL_UNSIGNED_INT, NULL);
		//glDrawArrays(GL_LINE_STRIP, 0, hair_vertex_buffer_data.size());
		int i_counter = 0;
		static bool once = false;
		for(int i = 0; i < toRender.vertices.size(); i+= toRender.offset)
		{
			//std::cout << i << std::endl;
			/*
			if (!once)
			{
				std::cout << hair_vertex_buffer_data.normals[i_counter][0] << " " 
							<< hair_vertex_buffer_data.normals[i_counter][1] << " "
							<< hair_vertex_buffer_data.normals[i_counter][2] << " "
							<< std::endl;
				
			}
			*/
			
			/*
			GLfloat normal[3];
			normal[0] = toRender.normals[i_counter][0];
			normal[1] = toRender.normals[i_counter][1];
			normal[2] = toRender.normals[i_counter][2];
			//glUniform3f(uniforms.hair_hairColor, 1, toRender.colors[i_counter]);
			glUniform3fv(uniforms.hair_hairNormal, 1, toRender.normals[i_counter]);
			*/
			GLfloat plane[4];
			plane[0] = toRender.planes[i].A;
			plane[1] = toRender.planes[i].B;
			plane[2] = toRender.planes[i].C;
			plane[3] = toRender.planes[i].D;
			/*
			std::cout << plane[0] << " "
						<< plane[1] << " "
						<< plane[2] << " "
						<< plane[3] << std::endl;
						*/

			GLfloat color[3];
			color[0] = 0;
			color[1] = 255;
			color[2] = 0;

			
			//glUniform3f(uniforms.hair_hairColor, 1, toRender.colors[i_counter]);
			glUniform3fv(uniforms.hair_hairColor, 1, color);
			glUniform4fv(uniforms.hair_hairPlane, 1, plane);

			glDrawArraysInstanced(GL_LINE_STRIP, i, toRender.offset, 360 * 2);
			i_counter++;
		}
		once = true;
		/////////////

		glEndQuery(GL_PRIMITIVES_GENERATED);

		glGetQueryObjectuiv(p1q, GL_QUERY_RESULT, &p1);
		//std::cout << "\033[1;32m[Hair renderer] \033[0mPrimitives generated in hair: " << p1 << std::endl;
		static int p2 = -1;
		if (p2 != p1)
		{
			std::cout << "\033[1;32m[Hair renderer (circ)] \033[0mPrimitives generated in hair: " << p1 << std::endl;
			//printErrors(791, "renderHair");
			p2 = p1;
		}
		printErrors(791, "renderHairCircular");

		clearBuffers();
	}

	void renderHairStrip(HairBufferObject &toRender)
	{

		glGenBuffers(1, &hair_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, hair_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, toRender.vertices.size() * sizeof(vmath::vec4), &toRender.vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, hair_vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			4,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		/*
		glGenBuffers(1, &hair_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, hair_normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, hair_vertex_buffer_data.normals.size() * sizeof(vmath::vec3), &hair_vertex_buffer_data.normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, hair_normalBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		*/

		/*
		glGenBuffers(1, &hair_indices_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hair_indices_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, hair_indices.size() * sizeof(GLuint), &hair_indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, hair_indices_buffer);
		*/
		glUseProgram(hair_render_strip_program);
		glUniformMatrix4fv(uniforms.hair_strip_mvpMatrix, 1, GL_FALSE, (projection_matrix * view_head_matrix * model_head_matrix));		

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//glEnable(GL_PRIMITIVE_RESTART);
		//glPrimitiveRestartIndex(9);

		GLuint p1q, p1;
		glGenQueries(1, &p1q);
		glBeginQuery(GL_PRIMITIVES_GENERATED, p1q);

		/////////////
		//glDrawElements(GL_LINES, hair_indices.size(), GL_UNSIGNED_INT, NULL);
		//glDrawArrays(GL_LINE_STRIP, 0, hair_vertex_buffer_data.size());
		int i_counter = 0;
		static bool once = false;
		for(int i = 0; i < toRender.vertices.size(); i+= toRender.offset)
		{
			//std::cout << i << std::endl;
			/*
			if (!once)
			{
				std::cout << hair_vertex_buffer_data.normals[i_counter][0] << " " 
							<< hair_vertex_buffer_data.normals[i_counter][1] << " "
							<< hair_vertex_buffer_data.normals[i_counter][2] << " "
							<< std::endl;
				
			}
			*/
			
			/*
			GLfloat normal[3];
			normal[0] = toRender.normals[i_counter][0];
			normal[1] = toRender.normals[i_counter][1];
			normal[2] = toRender.normals[i_counter][2];
			//glUniform3f(uniforms.hair_hairColor, 1, toRender.colors[i_counter]);
			glUniform3fv(uniforms.hair_hairNormal, 1, toRender.normals[i_counter]);
			*/
			GLfloat plane[4];
			plane[0] = toRender.planes[i].A;
			plane[1] = toRender.planes[i].B;
			plane[2] = toRender.planes[i].C;
			plane[3] = toRender.planes[i].D;
			/*
			std::cout << plane[0] << " "
						<< plane[1] << " "
						<< plane[2] << " "
						<< plane[3] << std::endl;
						*/

			GLfloat color[3];
			color[0] = 0;
			color[1] = 255;
			color[2] = 0;

			
			//glUniform3f(uniforms.hair_hairColor, 1, toRender.colors[i_counter]);
			glUniform3fv(uniforms.hair_strip_hairColor, 1, color);
			glUniform4fv(uniforms.hair_strip_hairPlane, 1, plane);

			glDrawArraysInstanced(GL_LINE_STRIP, i, toRender.offset, 120);
			i_counter++;
		}
		once = true;
		/////////////

		glEndQuery(GL_PRIMITIVES_GENERATED);

		glGetQueryObjectuiv(p1q, GL_QUERY_RESULT, &p1);
		//std::cout << "\033[1;32m[Hair renderer] \033[0mPrimitives generated in hair: " << p1 << std::endl;
		static int p2 = -1;
		if (p2 != p1)
		{
			std::cout << "\033[1;32m[Hair renderer (strip)] \033[0mPrimitives generated in hair: " << p1 << std::endl;
			//printErrors(791, "renderHair");
			p2 = p1;
		}
		printErrors(791, "renderHairStrip");

		clearBuffers();
	}

	void renderHairNoInstance(HairBufferObject toRender, bool debug = true, bool print_once = true)
	{
		static bool printed = false;
		if (debug && !printed)
		{
			std::cout << "\033[1;32m[Hair renderer (not intanced)] \033[0mVertices: " << toRender.vertices.size() << std::endl;
			std::cout << "\033[1;32m[Hair renderer (not intanced)] \033[0mNormals: " << toRender.normals.size() << std::endl;
			std::cout << "\033[1;32m[Hair renderer (not intanced)] \033[0mOffset: " << toRender.offset << std::endl;
		}
		
		
		glGenBuffers(1, &hair_noi_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, hair_noi_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, toRender.vertices.size() * sizeof(vmath::vec4), &toRender.vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, hair_noi_vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			4,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		/*
		glGenBuffers(1, &hair_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, hair_normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, hair_vertex_buffer_data.normals.size() * sizeof(vmath::vec3), &hair_vertex_buffer_data.normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, hair_normalBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		*/

		/*
		glGenBuffers(1, &hair_indices_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hair_indices_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, hair_indices.size() * sizeof(GLuint), &hair_indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, hair_indices_buffer);
		*/
		glUseProgram(hair_noi_render_program);
		glUniformMatrix4fv(uniforms.hair_noi_mvpMatrix, 1, GL_FALSE, (projection_matrix * view_head_matrix * model_head_matrix));		

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//glEnable(GL_PRIMITIVE_RESTART);
		//glPrimitiveRestartIndex(9);

		GLuint p1q, p1;
		glGenQueries(1, &p1q);
		glBeginQuery(GL_PRIMITIVES_GENERATED, p1q);

		/////////////
		for(int i = 0; i < toRender.vertices.size(); i+= toRender.offset)
		{
			glDrawArrays(GL_LINE_STRIP, i, toRender.offset);
		}
		/////////////

		glEndQuery(GL_PRIMITIVES_GENERATED);

		glGetQueryObjectuiv(p1q, GL_QUERY_RESULT, &p1);
		static int p2 = -1;
		if (p2 != p1)
		{
			std::cout << "\033[1;32m[Hair renderer (not intanced)] \033[0mPrimitives generated in hair: " << p1 << std::endl;
			printErrors(874, "renderHairNoInstance");
			p2 = p1;
		}

		if (print_once)
		{
			printed = true;
		}

		clearBuffers();

	}

	HairUpdateBufferObject packHairForUpdate(HairBufferObject &toRepack)
	{
		HairUpdateBufferObject ret;

		for (int i = 1; i < toRepack.vertices.size(); i += toRepack.offset)
		{
			for(int j = i; j < i + toRepack.offset; ++j)
			{
				ret.prevPosition.push_back(toRepack.vertices[j-1]);
				ret.currentPosition.push_back(toRepack.vertices[j]);
				ret.velocity.push_back(vmath::vec3(0.0, 0.0, 0.0));
			}
		}
		ret.mass = 0.001;
		ret.force = vmath::vec3(0.0, 0.0, 0.0);
		ret.length = hair_unit_length;

		return ret;
	}

	//TODO- MAYBE NOT NEEDED
	HairUpdateBufferObject updateHairFromTFData(std::vector<HairTFObject> &newData)
	{
		HairUpdateBufferObject ret;

		return ret;
	}

	//TODO- MAYBE NOT NEEDED
	HairBufferObject unpackHairFromUpdateBuffer(HairUpdateBufferObject &toUnpack)
	{
		HairBufferObject ret;

		return ret;
	}

	void initHairForUpdate(std::vector<HairObject> &toInit)
	{
		for (int i = 0; i < toInit.size(); ++i)
		{
			bool needCreate = false;
			if (toInit[i].forces.size() == 0)
			{
				needCreate = true;
			}
			for(int j = 0; j < toInit[i].forces.size(); ++j)
			{
				if (needCreate)
				{
					toInit[i].forces.push_back(vmath::vec4(0.0, 0.0, 0.0, 0.0));
					toInit[i].velocities.push_back(vmath::vec4(0.0, 0.0, 0.0, 0.0));
				}
				else
				{
					toInit[i].forces[j] = vmath::vec4(0.0, 0.0, 0.0, 0.0);
					toInit[i].velocities[j] = vmath::vec4(0.0, 0.0, 0.0, 0.0);
				}
			}
		}
	}

	void resetHairForces(HairObject &toReset)
	{
		for(int j = 0; j < toReset.forces.size(); ++j)
		{
			toReset.forces[j] = vmath::vec4(0.0, 0.0, 0.0, 0.0);
		}
	}

	float distanceFromFaceToVertex(ModelFace &face, vmath::vec4 &vert)
	{
		float ret = fabs(face.A * vert[0] + face.B * vert[1] + face.C * vert[2] + face.D);
		float divisor = sqrt(face.A * face.A + face.B * face.B + face.C * face.C);
		return ret/divisor;
	}

	ModelFace getClosestFace(vmath::vec4 &vert)
	{
		int closest = 0;
		float closestDistance = distanceFromFaceToVertex(model_faces[closest], vert);
		for (int i = 0; i < model_faces.size(); ++i)
		{
			float newDistance = distanceFromFaceToVertex(model_faces[i], vert);
			if (newDistance < closestDistance)
			{
				closest = i;
				closestDistance = newDistance;
			}
		}

		return model_faces[closest];
	}

	bool vectorContains(std::vector<vmath::vec3> &v, vmath::vec3 &val)
	{
		for (int i = 0; i < v.size(); ++i)
		{
			if (v[i] == val)
				return true;
		}
		return false;
	}

	std::vector<vmath::vec3> getUniqueVertices(MaterialFaces &mdl)
	{
		std::vector<vmath::vec3> ret;

		for (int i = 0; i < mdl.vertices.size(); ++i)
		{
			if (!vectorContains(ret, mdl.vertices[i]))
			{
				ret.push_back(mdl.vertices[i]);
			}
		}

		return ret;
	}

	BoundingBox getBoundingBox(std::vector<vmath::vec3> &vert)
	{
		BoundingBox ret;

		ret.xmin = vert[0][0];
		ret.ymin = vert[0][1];
		ret.zmin = vert[0][2];
		ret.xmax = vert[0][0];
		ret.ymax = vert[0][1];
		ret.zmax = vert[0][2];

		for (int i = 0; i < vert.size(); ++i)
		{
			if (vert[i][0] < ret.xmin)
			{
				ret.xmin = vert[i][0];
			}
			if (vert[i][1] < ret.ymin)
			{
				ret.ymin = vert[i][1];
			}
			if (vert[i][2] < ret.zmin)
			{
				ret.zmin = vert[i][2];
			}
			if (vert[i][0] > ret.xmax)
			{
				ret.xmax = vert[i][0];
			}
			if (vert[i][1] > ret.ymax)
			{
				ret.ymax = vert[i][1];
			}
			if (vert[i][2] > ret.zmax)
			{
				ret.zmax = vert[i][2];
			}
		}

		/*
		std::cout << "xmin: " << ret.xmin << std::endl;
		std::cout << "ymin: " << ret.ymin << std::endl;
		std::cout << "zmin: " << ret.zmin << std::endl;
		std::cout << "xmax: " << ret.xmax << std::endl;
		std::cout << "ymax: " << ret.ymax << std::endl;
		std::cout << "zmax: " << ret.zmax << std::endl;
		*/

		return ret;
	}

	float getMax(float a1, float a2)
	{
		if (a1 > a2)
			return a1;
		return a2;
	}

	BoundingSphere getBoundingSphere(BoundingBox &box)
	{
		BoundingSphere ret;

		float xdist = box.xmax - box.xmin;
		float ydist = box.ymax - box.ymin;
		float zdist = box.zmax - box.zmin;

		float xcenter = box.xmax - (xdist)/2;
		float ycenter = box.ymax - (ydist)/2;
		float zcenter = box.zmax - (zdist)/2;

		ret.center = vmath::vec3(xcenter, ycenter, zcenter);

		ret.rad = getMax(xdist/2, getMax(ydist/2, zdist/2));

		/*
		printVector(ret.center);
		std::cout << ret.rad << std::endl;
		*/

		return ret;
	}

	CollisionBound getCollisionBound(MaterialFaces &mdl, vmath::vec3 &divisor)
	{
		int divIndex = -1;
		for (int i = 0; i < 3; ++i)
		{
			if (divisor[i] != 0.0)
			{
				divIndex = i;
				break;
			}
		}

		if (divIndex == -1)
		{
			std::cout << "\033[1;31m[Collision calculator] \033[0mInvalid divisor passed!" << std::endl;
			exit(-1);
		}

		std::vector<vmath::vec3> uniqueVertices = getUniqueVertices(mdl);
		std::cout << "\033[1;32m[Collision calculator] \033[0mGot " << uniqueVertices.size() << " unique vertices" << std::endl;
		std::vector<vmath::vec3> part1;
		std::vector<vmath::vec3> part2;

		for (int i = 0; i < uniqueVertices.size(); ++i)
		{
			//std::cout << i << std::endl;
			if (uniqueVertices[i][divIndex] > divisor[divIndex])
			{
				part1.push_back(uniqueVertices[i]);
			}
			else
			{
				part2.push_back(uniqueVertices[i]);
			}
		}
		std::cout << "\033[1;32m[Collision calculator] \033[0mModel divided: " << part1.size() << " and " << part2.size() << std::endl;

		CollisionBound ret;

		BoundingBox box1 = getBoundingBox(part1);
		BoundingBox box2 = getBoundingBox(part2);
		ret.part1 = getBoundingSphere(box1);
		ret.part2 = getBoundingSphere(box2);

		std::cout << "\033[1;32m[Collision calculator] \033[0mBound finished" << std::endl;

		return ret;
	}

	void printCollisionBound(CollisionBound &col, std::string &name)
	{
		std::cout << "\033[1;32m[Collision info] \033[0mCollision info for \"" << name << "\"" << std::endl;
		std::cout << "\033[1;32m[Collision info] \033[0mCenter 1: ";
		printVector(col.part1.center);
		std::cout << "\033[1;32m[Collision info] \033[0mRadius 1: " << col.part1.rad << std::endl;
		std::cout << "\033[1;32m[Collision info] \033[0mCenter 2: ";
		printVector(col.part2.center);
		std::cout << "\033[1;32m[Collision info] \033[0mRadius 2: " << col.part2.rad << std::endl;
		std::cout << std::endl;
	}

	void printVector(vmath::vec3 &v)
	{
		std::cout << "("
					<< v[0] << " "
					<< v[1] << " "
					<< v[2] << ")" << std::endl;
	}

	vmath::vec3 getDivisor(MaterialFaces &mdl)
	{
		//find distances first
		int xminIndex = 0;
		int yminIndex = 0;
		int zminIndex = 0;
		int xmaxIndex = 0;
		int ymaxIndex = 0;
		int zmaxIndex = 0;

		float xmin = mdl.vertices[xminIndex][0];
		float ymin = mdl.vertices[yminIndex][1];
		float zmin = mdl.vertices[zminIndex][2];
		float xmax = mdl.vertices[xmaxIndex][0];
		float ymax = mdl.vertices[ymaxIndex][1];
		float zmax = mdl.vertices[zmaxIndex][2];

		for (int i = 0; i < mdl.vertices.size(); ++i)
		{
			if (mdl.vertices[i][0] < xmin)
			{
				xminIndex = i;
				xmin = mdl.vertices[i][0];
			}
			if (mdl.vertices[i][1] < ymin)
			{
				yminIndex = i;
				ymin = mdl.vertices[i][1];
			}
			if (mdl.vertices[i][2] < zmin)
			{
				zminIndex = i;
				zmin = mdl.vertices[i][2];
			}
			if (mdl.vertices[i][0] > xmax)
			{
				xmaxIndex = i;
				xmax = mdl.vertices[i][0];
			}
			if (mdl.vertices[i][1] > ymax)
			{
				ymaxIndex = i;
				ymax = mdl.vertices[i][1];
			}
			if (mdl.vertices[i][2] > zmax)
			{
				zmaxIndex = i;
				zmax = mdl.vertices[i][2];
			}
		}

		float xdist = xmax - xmin;
		float ydist = ymax - ymin;
		float zdist = zmax - zmin;
		/*
		std::cout << "x max: " << xmax << " min: " << xmin << std::endl;
		std::cout << "y max: " << ymax << " min: " << ymin << std::endl;
		std::cout << "z max: " << zmax << " min: " << zmin << std::endl;
		*/

		vmath::vec3 ret = vmath::vec3(0.0, 0.0, 0.0);
		//find the axis on which to divide the model
		if (xdist > ydist)
		{
			if (xdist > zdist)
			{
				//x is the divisor
				//std::cout << mdl.vertices[xminIndex][0] << " " << xdist/2 << std::endl;
				ret[0] = mdl.vertices[xminIndex][0] + xdist/2;
				if (ret[0] == 0)
				{
					ret[0] += 0.01;
				}
				//printVector(ret);
				return ret;
			}
			else
			{
				//z is the divisor
				ret[2] = mdl.vertices[zminIndex][2] + zdist/2;
				if (ret[2] == 0)
				{
					ret[2] += 0.01;
				}
				//printVector(ret);
				return ret;
			}
		}
		else
		{
			if (ydist > zdist)
			{
				//y is the divisor
				ret[1] = mdl.vertices[yminIndex][1] + ydist/2;
				if (ret[1] == 0)
				{
					ret[1] += 0.01;
				}
				//printVector(ret);
				return ret;
			}
			else
			{
				//z is the divisor
				ret[2] = mdl.vertices[zminIndex][2] + zdist/2;
				if (ret[2] == 0)
				{
					ret[2] += 0.01;
				}
				//printVector(ret);
				return ret;
			}
		}

		std::cout << "\033[1;31m[Divisor calculator] \033[0mUnknown error occured..." << std::endl;
		exit(-1);

	}

	ModelCollision getCollisions(Model &mdl)
	{
		ModelCollision ret;

		for (int i = 0; i < mdl.materials.size(); ++i)
		{
			vmath::vec3 divisor = getDivisor(mdl.materials[i]);
			ret.bounds.push_back(getCollisionBound(mdl.materials[i], divisor));
		}

		return ret;
	}

	float getDistance(vmath::vec3 &v, CollisionBound &col)
	{
		vmath::vec3 v1 = col.part2.center - col.part1.center;
		vmath::vec3 v2 = v - col.part1.center;

		double l1 = vmath::dot(v2, v1);
		double l2 = vmath::dot(v1, v1);
		double offset = l1/l2;

		vmath::vec3 projection = col.part1.center + v * offset;

		return 0;
	}

	void applyCollisionForces(HairObject &hair)
	{
		bool d_print = false;
		bool enforceCollisions = true;
		for(int i = 1; i < hair.vertices.size(); ++i)
		{
			for (int j = 0; j < model_collision_bound.bounds.size(); ++j)
			{
				vmath::vec3 v = vmath::vec3(hair.vertices[i][0], hair.vertices[i][1], hair.vertices[i][2]);

				vmath::vec3 v1 = model_collision_bound.bounds[j].part2.center - model_collision_bound.bounds[j].part1.center;
				vmath::vec3 v2 = v - model_collision_bound.bounds[j].part1.center;

				double l1 = vmath::dot(v2, v1);
				double l2 = vmath::dot(v1, v1);
				double offset = l1/l2;

				vmath::vec3 projection = model_collision_bound.bounds[j].part1.center + v1 * offset;

				if (d_print)
				{
					std::cout << "c 1: ";
					printVector(model_collision_bound.bounds[j].part1.center);
					std::cout << "prj: ";
					printVector(projection);
					std::cout << "c 2: ";
					printVector(model_collision_bound.bounds[j].part2.center);
					std::cout << "vct: ";
					printVector(v);
				}

				vmath::vec3 pp1 = projection - model_collision_bound.bounds[j].part1.center;
				vmath::vec3 pp2 = projection - model_collision_bound.bounds[j].part2.center;
				double checker = vmath::dot(pp1, pp2);
				float test = 1.0;
				float filler = 0.0;
				if (checker <= 0)
				{
					///*
					//Projection is on the segment
					double length_bound = vmath::length(v1);
					double length_partial = vmath::length(model_collision_bound.bounds[j].part2.center - projection);
					double rad_coeff = length_partial/length_bound;
					double new_rad = model_collision_bound.bounds[j].part2.rad + 
										(model_collision_bound.bounds[j].part1.rad - model_collision_bound.bounds[j].part1.rad) * rad_coeff;

					vmath::vec3 dist_vector = projection - v;

					double dist = vmath::length(dist_vector);

					if (d_print)
					{
						std::cout << "INSIDE THE SEGMENT" << std::endl;
						std::cout << "p2 -> p1  length: " << length_bound << std::endl;
						std::cout << "p2 -> prj length: " << length_partial << std::endl;
						std::cout << "v -> prj length: " << dist << std::endl;

						std::cout << "coeff: " << rad_coeff << std::endl;
						std::cout << "new rad: " << new_rad << std::endl;
						std::cout << "p1 rad: " << model_collision_bound.bounds[j].part1.rad << std::endl;
						std::cout << "p2 rad: " << model_collision_bound.bounds[j].part2.rad << std::endl;
					}

					if (dist < new_rad + physics_constants.collision_distance)
					{
						if(d_print)
						{
							std::cout << "COLLISION" << std::endl;
							std::cout << "dist: " << dist << std::endl;
							std::cout << "rad: " << new_rad << std::endl;
						}
						
						//there's a collision
						float diff = (dist - (new_rad + physics_constants.collision_distance))/dist;
						vmath::vec3 constraint = dist_vector * diff;
						vmath::vec4 constraint_v4 = vmath::vec4(constraint[0], constraint[1], constraint[2], filler);
						if (enforceCollisions)
						{
							hair.vertices[i] += constraint_v4;
						}
						else
						{
							hair.forces[i] += constraint_v4;
						}
					}
					//*/
				}
				else
				{
					///*
					//projection is out of the segment
					vmath::vec3 v3 = v - model_collision_bound.bounds[j].part2.center;
					double length_part1 = vmath::length(v2);
					double length_part2 = vmath::length(v3);
					if (length_part1 < length_part2)
					{
						if (d_print)
						{
							std::cout << "OUTSIDE P1" << std::endl;
						}
						
						//we're closer to part1
						if (length_part1 < model_collision_bound.bounds[j].part1.rad + physics_constants.collision_distance)
						{
							
							if(d_print)
							{
								std::cout << "COLLISION" << std::endl;
								std::cout << "dist: " << length_part1 << std::endl;
								std::cout << "rad: " << model_collision_bound.bounds[j].part1.rad << std::endl;
							}
							
							//There's a collision
							float diff = (length_part1 - (model_collision_bound.bounds[j].part1.rad + physics_constants.collision_distance))/length_part1;
							vmath::vec3 constraint = v2 * diff * (-1);
							vmath::vec4 constraint_v4 = vmath::vec4(constraint[0], constraint[1], constraint[2], filler);
							if (enforceCollisions)
							{
								hair.vertices[i] += constraint_v4;
							}
							else
							{
								hair.forces[i] += constraint_v4;
							}
						}
					}
					else
					{
						if (d_print)
						{
							std::cout << "OUTSIDE P2" << std::endl;
						}
						//we're closer to part2
						if (length_part2 < model_collision_bound.bounds[j].part2.rad + physics_constants.collision_distance)
						{
							if (d_print)
							{
								std::cout << "COLLISION" << std::endl;
								std::cout << "dist: " << length_part2 << std::endl;
								std::cout << "rad: " << model_collision_bound.bounds[j].part2.rad << std::endl;
							}
							
							//There's a collision
							float diff = (length_part2 - (model_collision_bound.bounds[j].part2.rad + physics_constants.collision_distance))/length_part1;
							vmath::vec3 constraint = v3 * diff * (-1);
							vmath::vec4 constraint_v4 = vmath::vec4(constraint[0], constraint[1], constraint[2], filler);
							if (enforceCollisions)
							{
								hair.vertices[i] += constraint_v4;
							}
							else
							{
								hair.forces[i] += constraint_v4;
							}
						}
					}
					//*/
					
				}

				if (d_print) 
				{
					std::cout << std::endl;
				}
			}
		}
		/*
		for(int i = 0; i < hair.vertices.size(); ++i)
		{
			ModelFace closestFace = getClosestFace(hair.vertices[i]);
			float dist = distanceFromFaceToVertex(closestFace, hair.vertices[i]);
			if (dist < physics_constants.collision_distance)
			{
				//std::cout << "Collision distance:\t" << dist << std::endl;
				vmath::vec4 normal = vmath::vec4(closestFace.A, closestFace.B, closestFace.C, 0.0f);
				float diff = (dist - physics_constants.collision_distance)/dist;

				hair.vertices[i] -= normal*diff;
				dist = distanceFromFaceToVertex(closestFace, hair.vertices[i]);
				//std::cout << "New distance:\t\t" << dist << std::endl << std::endl;
			}
		}
		*/
	}

	void applyWind(HairObject &hair)
	{
		float modvalx = rand() % 3;
		float modvaly = rand() % 4;
		float modvalz = rand() % 2;
		float modx = sin(modvalx);
		float mody = cos(modvaly);
		float modz = sin(modvalz);
		vmath::vec4 wind_mod = wind_vector;
		wind_mod[0] += modx;
		wind_mod[1] += mody;
		wind_mod[2] += modz;
		for(int i = 1; i < hair.forces.size(); ++i)
		{
			hair.forces[i] += wind_mod * wind_speed;
		}
	}

	void updateHairCPU(HairBufferObject &toUpdate)
	{
		std::vector<HairObject> hairs;

		hairs = unpackHairFromBuffer(toUpdate);

		static bool firstRun = true;

		if (firstRun)
		{
			for (int i = 0; i < toUpdate.vertices.size(); ++i)
			{
				initHairForUpdate(hairs);
			}
			firstRun = false;
		}

		

		float mass = physics_constants.mass;
		//float spring_constant = physics_constants.spring_constant;
		float hair_unit_length = physics_constants.hair_unit_length;
		//float friction_constant = physics_constants.friction_constant;
		vmath::vec4 gravity = physics_constants.gravity;
		float air_friction = physics_constants.air_friction;
		float dt = physics_constants.dt;

		for(int i = 0; i < hairs.size(); ++i)
		{
			vmath::vec4 backupPosition = hairs[i].vertices[0];

			/*
			for (int j = 0; j < hairs[i].vertices.size() - 1; ++j)
			{
				vmath::vec4 springVector = hairs[i].vertices[j] - hairs[i].vertices[j+1];

				float r = vmath::length(springVector);

				//std::cout << r << std::endl;
				vmath::vec4 force;

				if (r != 0)	
				{
					//force += vmath::normalize(springVector) * (r - hair_unit_length);
					force += (springVector / r) * fabs(r - hair_unit_length) * (-spring_constant);
				}

				force += -(hairs[i].velocities[j]  - hairs[i].velocities[j+1]) * friction_constant;
				

				hairs[i].forces[j] += force;
				hairs[i].forces[j+1] += -force;
			}
			*/

			resetHairForces(hairs[i]);

			if (apply_wind)
			{
				applyWind(hairs[i]);
			}
			
			for (int j = 0; j < hairs[i].vertices.size(); ++j)
			{
				hairs[i].forces[j] += gravity * mass;

				hairs[i].forces[j] += -hairs[i].velocities[j] * air_friction;

				hairs[i].velocities[j] += (hairs[i].forces[j] / mass) * dt;

				hairs[i].vertices[j] += hairs[i].velocities[j] * dt;
			}

			if (check_collision)
			{
				applyCollisionForces(hairs[i]);
			}

			hairs[i].vertices[0] = backupPosition;
			hairs[i].velocities[0] = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);

			for (int j = 0; j < hairs[i].vertices.size() - 1; ++j)
			{
				
				vmath::vec4 hairVector = hairs[i].vertices[j] - hairs[i].vertices[j+1];
				float vecLength = vmath::length(hairVector);
				float diff = 0;
				if (fabs(vecLength - hair_unit_length) > 0.000001)
				{
					diff = (vecLength - hair_unit_length)/vecLength;
				}
				/*
				vmath::vec4 constraint = vmath::vec4(hairVector[0] * 0.5 * diff,
													hairVector[1] * 0.5 * diff,
													hairVector[2] * 0.5 * diff,
													hairVector[3] * 0.5 * diff);
													*/
				vmath::vec4 constraint = hairVector * 0.5 * diff;
				//if (j == 0)
				{
					hairs[i].vertices[j+1] += constraint * 2;
				}
				/*
				else
				{
					hairs[i].vertices[j] -= constraint;
					hairs[i].vertices[j+1] += constraint;
				}
				*/
				/*
				//if (i == 0 && j == (hairs[i].vertices.size() - 2))
				if (i == 0 && j == 0)
				{
					vmath::vec4 newHairVector = hairs[i].vertices[j] - hairs[i].vertices[j+1];
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0m" << j << " && " << j+1 << " vertices" << std::endl;
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0mOld vec length =\t" << vecLength << std::endl;
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0mNew vec length =\t" << vmath::length(newHairVector) << std::endl;
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0mUnit length =\t" << hair_unit_length << std::endl;
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0mDiff length =\t" << diff << std::endl;
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0mConstraint vector:" << std::endl;
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0m(" 
								<< constraint[0] << " "
								<< constraint[1] << " "
								<< constraint[2] << " "
								<< constraint[3]
								<< ")" << std::endl;
					
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0m" << j << " vertex:" << std::endl;
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0m(" 
								<< hairs[i].vertices[j][0] << " "
								<< hairs[i].vertices[j][1] << " "
								<< hairs[i].vertices[j][2] << " "
								<< hairs[i].vertices[j][3]
								<< ")" << std::endl;
					
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0m" << j+1 <<" vertex:" << std::endl;
					std::cout << "\033[1;32m[Hair updater (CPU)] \033[0m(" 
								<< hairs[i].vertices[j+1][0] << " "
								<< hairs[i].vertices[j+1][1] << " "
								<< hairs[i].vertices[j+1][2] << " "
								<< hairs[i].vertices[j+1][3]
								<< ")" << std::endl << std::endl;
				}
				//*/
			}

			hairs[i].vertices[0] = backupPosition;
			hairs[i].velocities[0] = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		toUpdate = packHairForBuffer(hairs);
		
		/*
		int test = 1;
		std::cout << "\033[1;32m[Hair updater (CPU)] \033[0mUPDATE" << std::endl;
		std::cout << "\033[1;32m[Hair updater (CPU)] \033[0mvert[" << test << "] coord: (" <<  
			toUpdate.vertices[test][0] << " " <<
			toUpdate.vertices[test][1] << " " <<
			toUpdate.vertices[test][2] << ")" << std::endl;
		std::cout << "\033[1;32m[Hair updater (CPU)] \033[0mvert[" << test << "] velocity: (" <<  
			toUpdate.velocities[test][0] << " " <<
			toUpdate.velocities[test][1] << " " <<
			toUpdate.velocities[test][2] << ")" << std::endl;
		std::cout << "\033[1;32m[Hair updater (CPU)] \033[0mvert[" << test << "] force: (" <<  
			toUpdate.forces[test][0] << " " <<
			toUpdate.forces[test][1] << " " <<
			toUpdate.forces[test][2] << ")" << std::endl;
		std::cout << "\033[1;32m[Hair updater (CPU)] /////////////////////////////" << std::endl;
		*/

	}

	//NOT WORKING
	void updateHair(HairBufferObject &toUpdate)
	{
		static bool firstRun = true;
		if (firstRun)
		{
			hair_update_buffer = packHairForUpdate(toUpdate);
			firstRun = false;
		}
		

		glGenBuffers(1, &hair_update_current);
		glBindBuffer(GL_ARRAY_BUFFER, hair_update_current);
		glBufferData(GL_ARRAY_BUFFER, hair_update_buffer.currentPosition.size() * sizeof(vmath::vec4), &hair_update_buffer.currentPosition[0], GL_STATIC_DRAW);

		glGenBuffers(1, &hair_update_previous);
		glBindBuffer(GL_ARRAY_BUFFER, hair_update_previous);
		glBufferData(GL_ARRAY_BUFFER, hair_update_buffer.prevPosition.size() * sizeof(vmath::vec4), &hair_update_buffer.prevPosition[0], GL_STATIC_DRAW);

		glGenBuffers(1, &hair_update_velocity);
		glBindBuffer(GL_ARRAY_BUFFER, hair_update_current);
		glBufferData(GL_ARRAY_BUFFER, hair_update_buffer.velocity.size() * sizeof(vmath::vec3), &hair_update_buffer.velocity[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, hair_update_current);
		glVertexAttribPointer(
			0,                  // attribute
			4,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, hair_update_previous);
		glVertexAttribPointer(
			1,                                // attribute
			4,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, hair_update_velocity);
		glVertexAttribPointer(
			1,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);


		glEnable(GL_RASTERIZER_DISCARD);

		glUseProgram(hair_update_program);

		//Transform feedback buffer
		glGenBuffers(1, &hair_update_transformBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, hair_update_transformBuffer);
		glBufferData(GL_ARRAY_BUFFER, hair_update_buffer.currentPosition.size() * (sizeof(vmath::vec4) + sizeof(vmath::vec3)), NULL, GL_STATIC_READ);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, hair_update_transformBuffer);
		glBeginTransformFeedback(GL_POINTS);



		//Amount written
		GLuint p1, p1q;
		GLuint primitives_written, primitives_written_query;
		glGenQueries(1, &primitives_written_query);
		glGenQueries(1, &p1q);
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, primitives_written_query);
		glBeginQuery(GL_PRIMITIVES_GENERATED, p1q);

		glDrawArrays(GL_POINTS, 0, hair_update_buffer.currentPosition.size());

		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		glEndQuery(GL_PRIMITIVES_GENERATED);

		glGetQueryObjectuiv(primitives_written_query, GL_QUERY_RESULT, &primitives_written);
		glGetQueryObjectuiv(p1q, GL_QUERY_RESULT, &p1);

		glEndTransformFeedback();
		glFlush();

		glDisable(GL_RASTERIZER_DISCARD);

		std::cout << "\033[1;32m[Hair updater] \033[0mWritten in transform feedback: " << primitives_written << " primitives" << std::endl;
		std::cout << "\033[1;32m[Hair updated] \033[0mPrimitives generated: " << p1 << std::endl;
		std::cout << std::endl;

		if (p1 == 0)
		{
			std::cout << "\033[1;31m[Hair updated] \033[0mExitting due to no update, checking for errors..." << std::endl;
			printErrors(1019, "updateHair");
			exit(-1);
		}

		//Extract data written
		HairTFObject feedback_data[primitives_written];
		//feedback_data.reserve(primitives_written);
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, (sizeof(vmath::vec3) + sizeof(vmath::vec4))*primitives_written, feedback_data);
		
		std::vector<HairTFObject> feedback_data_vector;
		for(int i = 0; i < primitives_written; ++i)
		{
			feedback_data_vector.push_back(feedback_data[i]);
		}

		static int runIteration = 1;


	}

	GLuint loadPNGTexture(const char* fileName, int* width, int* height)
	{

		std::cout << "\033[1;32m[Load PNG] \033[0mPNG filename: [" << fileName << "]" << std::endl;

		png_byte 	header[8];
		size_t 		headerReadBytes;
		png_structp pngVersionString;
		png_infop 	pngStartInfo;
		png_infop 	pngEndInfo;
		int 		bitDepth;
		int 		colorType;
		png_uint_32 tmpWidth;
		png_uint_32 tmpHeigth;
		int 		rowBytes;
		png_byte* 	imageData;
		png_bytep* 	rowPtr;

		FILE* fl = fopen(fileName, "rb");

		if (fl == NULL)
		{
			std::cout << fileName << std::endl;
			std::cout << "\033[1;31m[Load PNG] \033[0mERROR: Texture file open failed" << std::endl;
			exit(-1);
		}

		headerReadBytes = fread(header, 1, 8, fl);
		if (headerReadBytes != 8)
		{
			std::cout << "\033[1;31m[Load PNG] \033[0mERROR: Failed reading header bytes" << std::endl;
			fclose(fl);
			exit(-1);
		}
		if (png_sig_cmp(header, 0, 8))
		{
			std::cout << "\033[1;31m[Load PNG] \033[0mERROR: Not a valid PNG file" << std::endl;
			fclose(fl);
			exit(-1);
		}

		pngVersionString = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (pngVersionString == 0)
		{
			std::cout << "\033[1;31m[Load PNG] \033[0mERROR: Can't create PNG version struct" << std::endl;
			fclose(fl);
			exit(-1);
		}

		pngStartInfo = png_create_info_struct(pngVersionString);
		if (pngStartInfo == 0)
		{
			std::cout << "\033[1;31m[Load PNG] \033[0mERROR: Can't create PNG info header struct" << std::endl;
			png_destroy_read_struct(&pngVersionString, (png_infopp)NULL, (png_infopp)NULL);
			fclose(fl);
			exit(-1);
		}

		pngEndInfo = png_create_info_struct(pngVersionString);
		if (pngEndInfo == 0)
		{
			std::cout << "\033[1;31m[Load PNG] \033[0mERROR: Can't create PNG ending info struct" << std::endl;
			png_destroy_read_struct(&pngVersionString, &pngStartInfo, (png_infopp) NULL);
			fclose(fl);
			exit(-1);
		}

		if (setjmp(png_jmpbuf(pngVersionString))) {
			std::cout << "\033[1;31m[Load PNG] \033[0mERROR: LibPNG error callback: Image is corrupt" << std::endl;
			png_destroy_read_struct(&pngVersionString, &pngStartInfo, &pngEndInfo);
			fclose(fl);
			exit(-1);
		}

		png_init_io(pngVersionString, fl);
		png_set_sig_bytes(pngVersionString, 8);
		png_read_info(pngVersionString, pngStartInfo);
		png_get_IHDR(pngVersionString, 
						pngStartInfo, 
						&tmpWidth, 
						&tmpHeigth, 
						&bitDepth, 
						&colorType,
						NULL, 
						NULL, 
						NULL);

		if (width)
		{
			*width = tmpWidth; 
		}

		if (height)
		{ 
			*height = tmpHeigth; 
		}

		png_read_update_info(pngVersionString, pngStartInfo);
		rowBytes = png_get_rowbytes(pngVersionString, pngStartInfo);

		//Fix for openGL texture format (4-byte aligned)
		rowBytes += 3 - ((rowBytes-1) % 4);
		
		imageData = (png_byte*) malloc(rowBytes*tmpHeigth*sizeof(png_byte)+15);
		if (imageData == NULL)
		{
			std::cout << "\033[1;31m[Load PNG] \033[0mERROR: Memory allocation for image failed" << std::endl;
			png_destroy_read_struct(&pngVersionString, &pngStartInfo, &pngEndInfo);
			fclose(fl);
			exit(-1);
		}

		rowPtr = (png_bytep*) malloc(tmpHeigth*sizeof(png_bytep));
		if (rowPtr == NULL)
		{
			std::cout << "\033[1;31m[Load PNG] \033[0mERROR: Memory allocation for rowdata failed" << std::endl;
			png_destroy_read_struct(&pngVersionString, &pngStartInfo, &pngEndInfo);
			free(imageData);
			fclose(fl);
			exit(-1);
		}

		for (int i = 0; i < tmpHeigth; i++)
		{
			rowPtr[tmpHeigth - 1 - i] = imageData + i * rowBytes;
		}

		png_read_image(pngVersionString, rowPtr);

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpWidth, tmpHeigth, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		png_destroy_read_struct(&pngVersionString, &pngStartInfo, &pngEndInfo);
		free(imageData);
		free(rowPtr);
		fclose(fl);

		return texture;
	}

	std::vector<Material> loadMaterialLib(const char* path)
	{
		std::cout << "\033[1;32m[Load MTL] \033[0mMTL filename: [" << path << "]" << std::endl;
		std::vector<Material> ret;

		FILE* file = fopen(path, "r");
		if (file == NULL)
		{
			std::cout << "\033[1;31m[Load MTL] \033[0mMTL file load failed!" << std::endl;

			exit(-1);
		}

		int lineCount = 1;
		Material currentMaterial;
		while(1)
		{
			char lineHeader[128];
			int success = fscanf(file, "%s", lineHeader);
			if (success == EOF)
				break; 
			
			if (strcmp(lineHeader, "newmtl") == 0)
			{
				char mtlName[128];
				size_t scanned = fscanf(file, "%s", mtlName);
				currentMaterial.name = mtlName;
			}
			else if (strcmp(lineHeader, "map_kd") == 0)
			{
				int w;
				int h;
				char mtlFileName[128];
				size_t scanned = fscanf(file, "%s", mtlFileName);
				currentMaterial.filename = mtlFileName;

				char copyPath[1024];
				strcpy(copyPath, path);
				char* dirName = dirname(copyPath);
				std::string concated = dirName;
				concated += "/";
				concated += mtlFileName;
				//std::vector<Material> mats = loadMaterialLib(concated.c_str());

				currentMaterial.texture = loadPNGTexture(concated.c_str(), &w, &h);
				ret.push_back(currentMaterial);
			}
			else
			{
				char junk[1024];
				//just to fix a warning
				char* more_junk = fgets(junk, 1024, file);
			}
			lineCount++;
		}

		fclose(file);
		

		return ret;
	}

	Model loadOBJ(const char* path)
	{
		std::vector<unsigned int> currentFaceVertexCoords;
		std::vector<unsigned int> currentFaceTexCoords;
		std::vector<unsigned int> currentFaceNormalCoords;
		
		std::vector<vmath::vec3> tempVertexCoords; 
		std::vector<vmath::vec2> tempTexCoords;
		std::vector<vmath::vec3> tempNormalCoords;
		
		std::cout << "\033[1;32m[Load OBJ] \033[0mOBJ filename: [" << path << "]" << std::endl;

		Model retvalue;

		FILE* file = fopen(path, "r");
		if (file == NULL)
		{
			std::cout << "\033[1;31m[Load OBJ] \033[0mOBJ file load failed" << std::endl;
			exit(-1);
		}

		int lineCount = 1;
		MaterialFaces currentMaterial;
		char materialLibName[128];
		bool usesMaterials = false;
		while(1)
		{
			char lineHeader[128];
			int success = fscanf(file, "%s", lineHeader);
			if (success == EOF)
				break; 
			
			if (strcmp(lineHeader, "mtllib") == 0)
			{
				size_t scanned = fscanf(file, "%s", materialLibName);
				usesMaterials = true;
			}
			else if (strcmp(lineHeader, "v") == 0)
			{
				vmath::vec3 vertex;
				size_t scanned = fscanf(file, "%f %f %f", &vertex[0], &vertex[1], &vertex[2]);

				if (scanned != 3)
				{
					std::cout << "\033[1;31m[Load OBJ] \033[0m" << path << "[" << lineCount << "]: Error reading vertex from OBJ file" << std::endl;
					fclose(file);
					exit(-1);
				}

				tempVertexCoords.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				vmath::vec2 texCoord;
				size_t scanned = fscanf(file, "%f %f", &texCoord[0], &texCoord[1]);

				if (scanned != 2)
				{
					std::cout << "\033[1;31m[Load OBJ] \033[0m" << path << "[" << lineCount << "]: Error reading texture coordinate from OBJ file" << std::endl;
					fclose(file);
					exit(-1);
				}

				//uv[1] = -uv[1];
				tempTexCoords.push_back(texCoord);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				vmath::vec3 normal;
				size_t scanned = fscanf(file, "%f %f %f", &normal[0], &normal[1], &normal[2] );

				if (scanned != 3)
				{
					std::cout << "\033[1;31m[Load OBJ] \033[0m" << path << "[" << lineCount << "]: Error reading vertex normal from OBJ file" << std::endl;
					fclose(file);
					exit(-1);
				}

				tempNormalCoords.push_back(normal);
			}
			else if (strcmp(lineHeader, "usemtl") == 0)
			{
				//Every other vector is the same size
				//It's 0 if this is the first material that we use
				if (currentFaceVertexCoords.size() != 0)
				{
					for(int i=0; i < currentFaceVertexCoords.size(); i++)
					{
						int vertexIndex = currentFaceVertexCoords[i];
						int texIndex = currentFaceTexCoords[i];
						int normalIndex = currentFaceNormalCoords[i];
						
						vmath::vec3 vertex = tempVertexCoords[vertexIndex-1];
						vmath::vec2 tex = tempTexCoords[texIndex-1];
						vmath::vec3 normal = tempNormalCoords[normalIndex-1];
						
						currentMaterial.vertices.push_back(vertex);
						currentMaterial.texCoords.push_back(tex);
						currentMaterial.normals.push_back(normal);
					}

					retvalue.materials.push_back(currentMaterial);

					currentFaceVertexCoords.clear();
					currentFaceTexCoords.clear();
					currentFaceNormalCoords.clear();
					currentMaterial.vertices.clear();
					currentMaterial.normals.clear();
					currentMaterial.texCoords.clear();
					currentMaterial.material.name = "";
					currentMaterial.material.filename = "";
				}

				char materialName[128];
				size_t scanned = fscanf(file, "%s", materialName);

				currentMaterial.material.name = materialName;
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				size_t scanned = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
				
				if (scanned != 9)
				{
					std::cout << "\033[1;31m[Load OBJ] \033[0m" << path << "[" << lineCount << "]: Error reading face data from OBJ file" << std::endl;
					fclose(file);
					exit(-1);
				}

				currentFaceVertexCoords.push_back(vertexIndex[0]);
				currentFaceVertexCoords.push_back(vertexIndex[1]);
				currentFaceVertexCoords.push_back(vertexIndex[2]);
				currentFaceTexCoords.push_back(uvIndex[0]);
				currentFaceTexCoords.push_back(uvIndex[1]);
				currentFaceTexCoords.push_back(uvIndex[2]);
				currentFaceNormalCoords.push_back(normalIndex[0]);
				currentFaceNormalCoords.push_back(normalIndex[1]);
				currentFaceNormalCoords.push_back(normalIndex[2]);
			}
			else
			{
				char junk[1024];
				//just to fix a warning
				char* more_junk = fgets(junk, 1024, file);
			}
			lineCount++;
		}

		if (currentFaceVertexCoords.size() != 0)
		{
			for(int i=0; i < currentFaceVertexCoords.size(); i++)
			{
				int vertexIndex = currentFaceVertexCoords[i];
				int texIndex = currentFaceTexCoords[i];
				int normalIndex = currentFaceNormalCoords[i];
				
				vmath::vec3 vertex = tempVertexCoords[vertexIndex-1];
				vmath::vec2 tex = tempTexCoords[texIndex-1];
				vmath::vec3 normal = tempNormalCoords[normalIndex-1];
				
				currentMaterial.vertices.push_back(vertex);
				currentMaterial.texCoords.push_back(tex);
				currentMaterial.normals.push_back(normal);
			}

			retvalue.materials.push_back(currentMaterial);
		}

		if(usesMaterials)
		{
			char copyPath[1024];
			strcpy(copyPath, path);
			char* dirName = dirname(copyPath);
			std::string concated = dirName;
			concated += "/";
			concated += materialLibName;
			std::vector<Material> mats = loadMaterialLib(concated.c_str());

			for (int i = 0; i < retvalue.materials.size(); ++i)
			{
				bool foundMat = false;
				for(int j = 0; j < mats.size(); ++j)
				{
					if (retvalue.materials[i].material.name == mats[j].name)
					{
						retvalue.materials[i].material = mats[j];
						foundMat = true;
						break;
					}
				}
				if (!foundMat)
				{
					std::cout << "\033[1;31m[Load OBJ] \033[0mCould not find the material: " << retvalue.materials[i].material.name << std::endl;
					exit(-1);
				}
			}
		}


		/*
		for(int i=0; i < vertexCoords.size(); i++)
		{
			int vertexIndex = vertexCoords[i];
			int texIndex = texCoords[i];
			int normalIndex = normalCoords[i];
			
			vmath::vec3 vertex = tempVertexCoords[vertexIndex-1];
			vmath::vec2 tex = tempTexCoords[texIndex-1];
			vmath::vec3 normal = tempNormalCoords[normalIndex-1];
			
			retvalue.vertices.push_back(vertex);
			retvalue.texCoords.push_back(tex);
			retvalue.normals.push_back(normal);
		}
		*/
		fclose(file);
		
		return retvalue;
	}

	std::vector<ScalpTFObject> loadScalp(Model &source)
	{
		std::vector<ScalpTFObject> ret;

		if (source.materials[0].vertices.size() % 3 != 0)
		{
			std::cout << "\033[1;31m[Load scalp] \033[0mInvalid scalp size!" << std::endl;
			exit(-1);
		}

		for (int i = 0; i < source.materials[0].vertices.size(); i += 3)
		{
			vmath::vec3 v1 = source.materials[0].vertices[i];
			vmath::vec3 v2 = source.materials[0].vertices[i+1];
			vmath::vec3 v3 = source.materials[0].vertices[i+2];

			/*
			vmath::vec3 faceNormal = vmath::normalize(source.materials[0].normals[i] + 
														source.materials[0].normals[i+1] + 
														source.materials[0].normals[i+2]);
			*/

			vmath::vec3 s1 = v2 - v1;
			vmath::vec3 s2 = v3 - v1;
			vmath::vec3 faceNormal = vmath::normalize(vmath::cross(s1, s2));
	
			ScalpTFObject o1;
			ScalpTFObject o2;
			ScalpTFObject o3;

			o1.gl_Position = vmath::vec4(v1[0], v1[1], v1[2], 1.0);
			o1.normal = faceNormal;

			o2.gl_Position = vmath::vec4(v2[0], v2[1], v2[2], 1.0);
			o2.normal = faceNormal;

			o3.gl_Position = vmath::vec4(v3[0], v3[1], v3[2], 1.0);
			o3.normal = faceNormal;

			ret.push_back(o1);
			ret.push_back(o2);
			ret.push_back(o3);

			/*
			std::cout << faceNormal[0] << " "
						<< faceNormal[1] << " "
						<< faceNormal[2] << std::endl;
			*/
		}

		return ret;
	}

	void regenHair()
	{
		//std::vector<std::string> masks;
		//masks.push_back(std::string("model/head_model.png"));
		//masks.push_back(std::string("model/head_texture_beard_mask.png"));

		//loadMaskTextures(masks);

		if (use_masks)
		{
			std::vector<std::string> masks;
			masks.push_back(texture_mask_filename.c_str());

			loadMaskTextures(masks);

			std::vector<ScalpTFObject> scalp_tf = generateScalp(scalp_masks[0]);
			std::vector<ScalpTFFaceObject> scalp_opt = optimizeScalp(scalp_tf);
			std::cout << "\033[1;32m[Regenerate hair] \033[0mOptimized scalp (mask): " << scalp_opt.size() << " vertices" << std::endl;
			hair_objects = generateLeadingHairs(scalp_opt);

			hair_vertex_buffer_data = packHairForBuffer(hair_objects);
		}
		else
		{
			scalp_mdl = loadOBJ(scalp_filename.c_str());

			std::vector<ScalpTFObject> scalp_tf = loadScalp(scalp_mdl);
			std::vector<ScalpTFFaceObject> scalp_opt = optimizeScalp(scalp_tf);
			std::cout << "\033[1;32m[Regenerate hair] \033[0mOptimized scalp (model): " << scalp_opt.size() << " vertices" << std::endl;
			hair_objects = generateLeadingHairs(scalp_opt);

			hair_vertex_buffer_data = packHairForBuffer(hair_objects);
		}

	}

	void transformViewMatrix(Transformation t, float units)
	{
		vmath::mat4 translation_matrix;
		vmath::mat4 rotation_matrix;

		bool translate = false;
		bool rotate = false;

		switch(t)
		{
				case MOVE_UP:
					translate = true;
					translation_matrix = vmath::translate(0.0f, 0-units, 0.0f);
					break;
				case MOVE_DOWN:
					translate = true;
					translation_matrix = vmath::translate(0.0f, units, 0.0f);
					break;
				case MOVE_LEFT:
					translate = true;
					translation_matrix = vmath::translate(units, 0.0f, 0.0f);
					break;
				case MOVE_RIGHT:
					translate = true;
					translation_matrix = vmath::translate(0-units, 0.0f, 0.0f);
					break;
				case MOVE_FORWARD:
					translate = true;
					translation_matrix = vmath::translate(0.0f, 0.0f, units);
					break;
				case MOVE_BACKWARD:
					translate = true;
					translation_matrix = vmath::translate(0.0f, 0.0f, 0-units);
					break;
				case ROTATE_UP:
					rotate = true;
					rotation_matrix = vmath::rotate(0-units, vmath::vec3(1.0f, 0.0f, 0.0f));
					wind_camera_v_fix += units;
					break;
				case ROTATE_DOWN:
					rotate = true;
					rotation_matrix = vmath::rotate(units, vmath::vec3(1.0f, 0.0f, 0.0f));
					wind_camera_v_fix += 0-units;
					break;
				case ROTATE_LEFT:
					rotate = true;
					rotation_matrix = vmath::rotate(0-units, vmath::vec3(0.0f, 1.0f, 0.0f));
					wind_camera_h_fix += 0-units;
					break;
				case ROTATE_RIGHT:
					rotate = true;
					rotation_matrix = vmath::rotate(units, vmath::vec3(0.0f, 1.0f, 0.0f));
					wind_camera_h_fix += units;
					break;
				case ROTATE_Z_LEFT:
					rotate = true;
					rotation_matrix = vmath::rotate(0-units, vmath::vec3(0.0f, 0.0f, 1.0f));
					break;
				case ROTATE_Z_RIGHT:
					rotate = true;
					rotation_matrix = vmath::rotate(units, vmath::vec3(0.0f, 0.0f, 1.0f));
					break;
				default:
					std::cout << "\033[1;31m[View Matrix modifyer] \033[0m" << "Invalid modifyer command" << std::endl;
					exit(-1);
					break;
		}

		if (translate)
		{
			view_head_matrix = translation_matrix * view_head_matrix;
		}

		if (rotate)
		{
			view_head_matrix = rotation_matrix * view_head_matrix;
		}

		applyWindAngle();
	}

	vmath::vec4 initWindVector()
	{
		return vmath::vec4(0.0, 0.0, -1.0, 0.0);
	}

	//FINISH ME
	void applyWindAngle()
	{
		float arrow_fix = physics_constants.arrow_model_fix;
		vmath::mat4 transformMatrix = vmath::rotate(wind_horizontal_angle*arrow_fix + wind_camera_h_fix, vmath::vec3(0.0, 1.0, 0.0))
										* vmath::rotate(wind_vertical_angle*arrow_fix + wind_camera_v_fix, vmath::vec3(1.0, 0.0, 0.0));
		model_arrow_matrix = initArrowModelMatrix() * transformMatrix;
		

		vmath::mat4 transformVectorMatrix = vmath::rotate(0-wind_horizontal_angle, vmath::vec3(0.0, 1.0, 0.0))
										* vmath::rotate(0-wind_vertical_angle, vmath::vec3(1.0, 0.0, 0.0));
		wind_vector = initWindVector() * transformVectorMatrix; 

		/*
		vmath::mat4 modelAlignment = vmath::lookat(vmath::normalize(vmath::vec3(wind_vector[0], wind_vector[1], wind_vector[2])), 
													vmath::vec3(0.0, 0.0, 0.0),
													vmath::vec3(0.0, 1.0, 0.0));
		model_arrow_matrix = initArrowModelMatrix() * modelAlignment.transpose();
		*/
	}

	void modifyWind(Transformation t, float units)
	{
		switch(t)
		{
			case ROTATE_UP:
				wind_vertical_angle += units;
				break;
			case ROTATE_DOWN:
				wind_vertical_angle -= units;
				break;
			case ROTATE_LEFT:
				wind_horizontal_angle += units;
				break;
			case ROTATE_RIGHT:
				wind_horizontal_angle -= units;
				break;
			default:
				std::cout << "\033[1;31m[Wind modifyer] \033[0m" << "Invalid modifier direction" << std::endl;
				exit(-1);
				break;
		}

		applyWindAngle();
	}

	void changeWindSpeed(float units)
	{
		wind_speed += units;
	}

	void printModel()
	{
		for (int i = 0; i < model_mdl.materials.size(); ++i)
		{
			std::cout << "Material name: " << model_mdl.materials[i].material.name << std::endl;
		}
	}

	void printCompilerLog(GLuint shader)
	{
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength == 0)
		{
			return;
		}

		std::cout << "\033[1;31m[Shader log] \033[0mCompiler log for shader " << shader << std::endl;

		GLchar* compilerLog = (GLchar*) malloc(logLength*sizeof(GLchar));

		glGetShaderInfoLog(shader, logLength, NULL, compilerLog);

		std::cout << "\033[1;31m[Shader log error] \033[0m" << compilerLog << std::endl << std::endl;

		std::cout << "\033[1;31m[Shader log error] \033[0m" << "Exitting due to shader compilation errors" << std::endl;
		exit(-1);

	}

	void printLinkerLog(GLuint program, const char* programName)
	{
		std::cout << "\033[1;32m[Linker log] \033[0mGeneral info for program #" << program << ": \"" << programName << "\"" << std::endl;
		GLint attachedShaders;
		glGetProgramiv(program, GL_ATTACHED_SHADERS, &attachedShaders);
		std::cout << "\033[1;32m[Linker log] \033[0mAttached shaders: " << attachedShaders << std::endl;

		GLint i;
		GLint count;

		GLint size;
		GLenum type;

		const GLsizei bufSize = 16; 
		GLchar name[bufSize];
		GLsizei length;

		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
		std::cout << "\033[1;32m[Linker log] \033[0mActive Attributes: " << count << std::endl;
		if (count > 10)
			count = 10;
		for (int i = 0; i < count; i++)
		{
			glGetActiveAttrib(program, (GLuint)i, bufSize, &length, &size, &type, name);

			std::cout << "\033[1;32m[Linker log] \033[0mAttribute #" << i << " is of type <" << type << ">, name: \"" << name << "\"" << std::endl;
		}

		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
		std::cout << "\033[1;32m[Linker log] \033[0mActive Uniforms: " << count << std::endl;

		for (i = 0; i < count; i++)
		{
			glGetActiveUniform(program, (GLuint)i, bufSize, &length, &size, &type, name);

			std::cout << "\033[1;32m[Linker log] \033[0mUniform #" << i << " is of type <" << type << ">, name: \"" << name << "\"" << std::endl;
		}

		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength == 0)
		{
			std::cout << std::endl;
			return;
		}

		std::cout << "\033[1;31m[Linker log] \033[0mLinker log for program " << program << std::endl;
		GLchar* linkerLog = (GLchar*) malloc(logLength*sizeof(GLchar));

		glGetProgramInfoLog(program, logLength, NULL, linkerLog);

		std::cout << "\033[1;31m[Linker log error] \033[0m" << linkerLog << std::endl << std::endl;
		std::cout << "\033[1;31m[Linker log error] \033[0m" << "Exitting due to linking errors" << std::endl;
		exit(-1);
	}

	void printErrors(int line = -1, const char* message = "unknown")
	{
		bool once = false;
		bool foundErrors = false;
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) 
		{
			foundErrors = true;
			if (!once && line != -1)
			{
				std::cout << "\033[1;31m[Error printer] \033[0mLine " << line << ": called from " << message << std::endl;
				once = true; 
			}
			std::cout << "\033[1;31m[Error printer] \033[0mError: " << err << std::endl;
		}

		if (foundErrors)
		{
			std::cout << "\033[1;31m[Error printer] \033[0mExitting due to errors" << std::endl;
		}
	}


	void onKey(int key, int action)
	{
		float rotation_value = 4.5f;
		float translation_value = 7.5f;
		if (action)
		{
			switch (key)
			{
				case 'W':
					key_pressed = "W";
					transformViewMatrix(MOVE_FORWARD, translation_value);
					break;
				case 'A':
					key_pressed = "A";
					transformViewMatrix(MOVE_LEFT, translation_value);
					break;
				case 'S':
					key_pressed = "S";
					transformViewMatrix(MOVE_BACKWARD, translation_value);
					break;
				case 'D':
					key_pressed = "D";
					transformViewMatrix(MOVE_RIGHT, translation_value);
					break;

				case 'R':
					key_pressed = "R";
					transformViewMatrix(MOVE_UP, translation_value);
					break;
				case 'F':
					key_pressed = "F";
					transformViewMatrix(MOVE_DOWN, translation_value);
					break;

				case 'Q':
					key_pressed = "Q";
					transformViewMatrix(ROTATE_Z_LEFT, rotation_value);
					break;
				case 'E':
					key_pressed = "E";
					transformViewMatrix(ROTATE_Z_RIGHT, rotation_value);
					break;
				case 'Z':
					key_pressed = 'Z';
					regenHair();
					break;

				case GLFW_KEY_UP: 
					key_pressed = "UP";
					transformViewMatrix(ROTATE_UP, rotation_value);
					break;
				case GLFW_KEY_DOWN:
					key_pressed = "DOWN";
					transformViewMatrix(ROTATE_DOWN, rotation_value);
					break;
				case GLFW_KEY_LEFT: 
					key_pressed = "LEFT";
					transformViewMatrix(ROTATE_LEFT, rotation_value);
					break;
				case GLFW_KEY_RIGHT:
					key_pressed = "RIGHT";
					transformViewMatrix(ROTATE_RIGHT, rotation_value);
					break;

				case 'U':
					key_pressed = "U";
					perform_update = !perform_update;
					break;
				case 'I':
					key_pressed = "I";
					readConfig(config_filename.c_str());
					break;
				case 'O':
					key_pressed = "O";
					render_hairs++;
					if (render_hairs >= MAX_HAIR_MODES)
						render_hairs = 0;
					break;
				case 'C':
					key_pressed = "C";
					check_collision = !check_collision;
					break;
				case 'M':
					key_pressed = "M";
					render_model = !render_model;
					break;

				case GLFW_KEY_KP_2: 
					key_pressed = "KP_DOWN";
					modifyWind(ROTATE_DOWN, physics_constants.wind_vertical_step);
					break;
				case GLFW_KEY_KP_8:
					key_pressed = "KP_UP";
					modifyWind(ROTATE_UP, physics_constants.wind_vertical_step);
					break;
				case GLFW_KEY_KP_4: 
					key_pressed = "KP_LEFT";
					modifyWind(ROTATE_LEFT, physics_constants.wind_horizontal_step);
					break;
				case GLFW_KEY_KP_6:
					key_pressed = "KP_RIGHT";
					modifyWind(ROTATE_RIGHT, physics_constants.wind_horizontal_step);
					break;
				case GLFW_KEY_KP_7:
					key_pressed = "KP_7";
					changeWindSpeed(physics_constants.wind_speed_step);
					break;
				case GLFW_KEY_KP_1:
					key_pressed = "KP_1";
					changeWindSpeed(-physics_constants.wind_speed_step);
					break;
				case GLFW_KEY_KP_5:
					key_pressed = "KP_5";
					apply_wind = !apply_wind;
					break;

			}
		}
	}

	void readConfig(const char* flname)
	{
		std::cout << "\033[1;32m[Config loader] \033[0mLoading config..." << std::endl;
		std::ifstream infile(flname);

		if (!infile.good())
		{
			std::cout << "\033[1;31m[Config loader] \033[0mError: can't open file!" << std::endl;
			infile.close();
			exit(-1);
		}
		bool b_mass = false;
		bool b_spring_tension = true;
		bool b_hair_unit_length = false;
		bool b_hair_length = false;
		bool b_friction = true;
		bool b_air_friction = false;
		bool b_timestep = false;
		bool b_collision_dist = false;
		bool b_wind_v_step = false;
		bool b_wind_h_step = false;
		bool b_wind_speed_step = false;
		bool b_cam_rot_speed = false;
		bool b_cam_mv_speed = false;
		bool b_arrow_model_fix = false;

		std::string flcheck;
		infile >> flcheck;
		if (flcheck != "HAIRCONFIG")
		{
			std::cout << "\033[1;31m[Config loader] \033[0mError: Missing config header!" << std::endl;
			infile.close();
			exit(-1);
		}

		while(infile.good())
		{
			std::string header;
			float value;
			infile >> header;
			infile >> value;
			if (header == "mass")
			{
				physics_constants.mass = value;
				b_mass = true;
				continue;
			}
			if (header == "spring_tension")
			{
				physics_constants.spring_constant = value;
				b_spring_tension = true;
				continue;
			}
			if (header == "hair_unit_length")
			{
				physics_constants.hair_unit_length = value;
				b_hair_unit_length = true;
				continue;
			}
			if (header == "hair_length")
			{
				physics_constants.hair_length = value;
				b_hair_length = true;
				continue;
			}
			if (header == "friction")
			{
				physics_constants.friction_constant = value;
				b_friction = true;
				continue;
			}
			if (header == "air_friction")
			{
				physics_constants.air_friction = value;
				b_air_friction = true;
				continue;
			}
			if (header == "timestep")
			{
				physics_constants.dt = value;
				b_timestep = true;
				continue;
			}
			if (header == "collision_dist")
			{
				physics_constants.collision_distance = value;
				b_collision_dist = true;
				continue;
			}
			if (header == "wind_v_step")
			{
				physics_constants.wind_vertical_step = value;
				b_wind_v_step = true;
				continue;
			}
			if (header == "wind_h_step")
			{
				physics_constants.wind_horizontal_step = value;
				b_wind_h_step = true;
				continue;
			}
			if (header == "wind_speed_step")
			{
				physics_constants.wind_speed_step = value;
				b_wind_speed_step = true;
				continue;
			}
			if (header == "cam_rot_speed")
			{
				physics_constants.camera_rotate_speed = value;
				b_cam_rot_speed = true;
				continue;
			}
			if (header == "cam_mv_speed")
			{
				physics_constants.camera_move_speed = value;
				b_cam_mv_speed = true;
				continue;
			}
			if (header == "arrow_model_fix")
			{
				physics_constants.arrow_model_fix = value;
				b_arrow_model_fix = true;
				continue;
			}
			std::cout << "\033[1;31m[Config loader] \033[0mError: unknown header \"" << header << "\"" << std::endl;
			infile.close();
			exit(-1);
		}

		bool noerror = b_mass &&
					b_spring_tension &&
					b_hair_unit_length &&
					b_hair_length &&
					b_friction &&
					b_air_friction &&
					b_timestep &&
					b_collision_dist &&
					b_wind_v_step &&
					b_wind_h_step &&
					b_wind_speed_step &&
					b_cam_rot_speed &&
					b_cam_mv_speed &&
					b_arrow_model_fix;

		infile.close();

		if(!noerror)
		{
			if (!b_mass)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mMass not specified!" << std::endl;
			}
			if (!b_spring_tension)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mSpring tension not specified!" << std::endl;
			}
			if (!b_hair_unit_length)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mHair unit length not specified!" << std::endl;
			}
			if (!b_hair_length)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mHair length not specified!" << std::endl;
			}
			if (!b_friction)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mFriction not specified!" << std::endl;
			}
			if (!b_air_friction)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mAir friction not specified!" << std::endl;
			}
			if (!b_timestep)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mTimestep not specified!" << std::endl;
			}
			if (!b_collision_dist)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mCollision distance not specified!" << std::endl;
			}
			if (!b_wind_v_step)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mWind vertical angle step not specified!" << std::endl;
			}
			if (!b_wind_h_step)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mWind horizontal angle step not specified!" << std::endl;
			}
			if (!b_wind_speed_step)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mWind speed step not specified!" << std::endl;
			}
			if (!b_cam_rot_speed)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mCamera rotation speed not specified!" << std::endl;
			}
			if (!b_cam_mv_speed)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mCamera movement speed not specified!" << std::endl;
			}
			if (!b_arrow_model_fix)
			{
				std::cout << "\033[1;31m[Config loader] \033[0mArrow model fix not specified!" << std::endl;
			}
			exit(-1);
		}

		std::cout << "\033[1;32m[Config loader] \033[0mLoad successful!" << std::endl;
		
	}

	void readModelConfig(const char* flname)
	{
		bool b_mask_specified = false;
		bool b_scalp_specified = false;

		std::cout << "\033[1;32m[Model config loader] \033[0mLoading config: " << flname << std::endl;
		std::ifstream infile(flname);

		if (!infile.good())
		{
			std::cout << "\033[1;31m[Model config loader] \033[0mError: can't open file!" << std::endl;
			infile.close();
			exit(-1);
		}

		std::string flcheck;
		infile >> flcheck;
		if (flcheck != "MDLCONFIG")
		{
			std::cout << "\033[1;31m[Model config loader] \033[0mError: Missing config header!" << std::endl;
			infile.close();
			exit(-1);
		}

		bool b_model_filename = false;
		bool b_mask_filename = false;
		bool b_mask_material = false;
		bool b_scalp_filename = false;
		bool b_conf_filename = false;

		while(infile.good())
		{
			std::string header;
			std::string value;
			infile >> header;
			infile >> value;
			if (header == "model")
			{
				model_filename = value;
				b_model_filename = true;
				continue;
			}
			if (header == "mask")
			{
				texture_mask_filename = value;
				b_mask_filename = true;
				continue;
			}
			if (header == "masked_material")
			{
				material_mask_name = value;
				b_mask_material = true;
				continue;
			}
			if (header == "scalp")
			{
				scalp_filename = value;
				b_scalp_filename = true;
				continue;
			}
			if (header == "use")
			{
				if (value == "scalp")
				{
					use_masks = false;
					continue;
				}
				if (value == "mask")
				{
					use_masks = true;
					continue;
				}
				std::cout << "\033[1;31m[Model config loader] \033[0mUnknown use parameter:" << value << std::endl;
				infile.close();
				exit(-1);
			}
			if (header == "conf")
			{
				config_filename = value;
				b_conf_filename = true;
				continue;
			}

		}
		infile.close();

		if (!b_model_filename)
		{
			std::cout << "\033[1;31m[Model config loader] \033[0mNo filename for model specified!" << std::endl;
			exit(-1);
		}
		if (!b_conf_filename)
		{
			std::cout << "\033[1;31m[Model config loader] \033[0mNo filename for hair config specified!" << std::endl;
			exit(-1);
		}

		if (use_masks)
		{
			if (!b_mask_material)
			{
				std::cout << "\033[1;31m[Model config loader] \033[0mNo material name for mask specified!" << std::endl;
				exit(-1);
			}
			if (!b_mask_filename)
			{
				std::cout << "\033[1;31m[Model config loader] \033[0mNo filename for mask specified!" << std::endl;
				exit(-1);
			}
		}
		else
		{
			if (!b_scalp_filename)
			{
				std::cout << "\033[1;31m[Model config loader] \033[0mNo filename for scalp specified!" << std::endl;
				exit(-1);
			}
		}


		std::cout << "\033[1;32m[Config loader] \033[0mLoad successful!" << std::endl;
	}

	std::string 			model_filename = "model/head_model.obj";
	std::string 			scalp_filename = "";
	std::string 			texture_filename = "model/head_texture_nomask.png";
	std::string 			material_mask_name = "HeavyHead";
	std::string 			texture_mask_filename = "heavy_head/heavy_head_mask.png";
	std::string 			config_filename = "hair.conf";
	std::string 			model_config_filename = "heavy.conf";
	std::string 			arrow_filename = "arrow/arrow.obj";

	std::string 			status_default;
	std::string 			key_pressed;
	std::string 			status;
	unsigned long long int 	frame_count;
	sb7::text_overlay		overlay;

	bool 					perform_update;
	uint						render_hairs;
	bool 					check_collision;
	bool 					render_model;
	bool 					apply_wind;

	bool 					use_masks;

	float 					wind_vertical_angle;
	float 					wind_horizontal_angle;
	float 					wind_speed;
	float 					wind_camera_v_fix;
	float 					wind_camera_h_fix;
	vmath::vec4 			wind_vector;
	float 					arrow_model_fix;

	GLuint 					model_buffer;
	GLuint 					model_vao;
	GLuint 					model_program;

	GLuint 					scalp_program;
	GLuint 					scalp_transformBuffer;
	std::vector<GLint> 		scalp_masks;

	GLuint 					scalp_render_program;

	GLuint 					model_vertexBuffer;
	GLuint 					model_texBuffer;
	GLuint 					model_texture;

	GLuint 					arrow_render_program;
	GLuint 					arrow_vertexBuffer;

	GLuint 					hair_render_program;
	GLuint 					hair_vertexBuffer;

	GLuint 					hair_render_strip_program;

	GLuint 					hair_update_program;
	GLuint 					hair_update_transformBuffer;
	GLuint 					hair_update_current;
	GLuint 					hair_update_previous;
	GLuint 					hair_update_velocity;


	GLuint 					hair_noi_render_program;
	GLuint 					hair_noi_update_program;
	GLuint 					hair_noi_vertexBuffer;

	Model 					model_mdl;
	std::vector<ModelFace> 	model_faces;
	ModelCollision 			model_collision_bound;

	Model 					arrow_mdl;

	Model 					scalp_mdl;

	vmath::mat4 			projection_matrix;
	vmath::mat4 			model_head_matrix;
	vmath::mat4 			model_arrow_matrix;
	vmath::mat4 			view_head_matrix;
	vmath::mat4 			view_arrow_matrix;

	float 					hair_length;
	float 					hair_stiffness;
	float 					hair_unit_length;
	vmath::vec3 			hair_color;

	std::vector<ScalpTFObject> 	scalp;
	std::vector<HairObject>					hair_objects;
	HairBufferObject 						hair_vertex_buffer_data;
	HairUpdateBufferObject 					hair_update_buffer;
	std::vector<GLuint> 					hair_indices;
	GLuint 									hair_indices_buffer;

	GLfloat 								hair_patch_radius;

	struct 
	{
		float mass;
		float spring_constant;
		float hair_unit_length;
		float hair_length;
		float friction_constant;
		vmath::vec4 gravity = vmath::vec4(0.0f, -9.81f, 0.0f, 0.0f);
		float air_friction;
		float dt;

		float collision_distance;

		float wind_vertical_step;
		float wind_horizontal_step;
		float wind_speed_step;

		float camera_move_speed;
		float camera_rotate_speed;

		float arrow_model_fix;

	} physics_constants;

	struct
	{
		GLint 				model_mvpMatrix;
		GLint 				model_Texture;

		GLint				scalp_Texture;

		GLint 				scalp_render_Texture;
		GLint 				scalp_render_mvpMatrix;

		GLint 				hair_mvpMatrix;
		GLint 				hair_hairColor;
		GLint 				hair_hairPlane;

		GLint 				hair_noi_vertexBuffer;
		GLint 				hair_noi_mvpMatrix;

		GLint 				hair_update_stringLength;
		GLint 				hair_update_mass;
		GLint 				hair_update_force;

		GLint 				arrow_render_mvpMatrix;

		GLint 				hair_strip_mvpMatrix;
		GLint 				hair_strip_hairColor;
		GLint 				hair_strip_hairPlane;
	} uniforms;
};

#include <dirent.h>
#include <stdio.h>




std::string configPacker(ConfigValues &src)
{
	std::string ret;

	if (src.mass_value != "")
	{
		ret += "mass ";
		ret += src.mass_value;
		ret += "\n";
	}
	if (src.hair_unit_length_value != "")
	{
		ret += "hair_unit_length ";
		ret += src.hair_unit_length_value;
		ret += "\n";
	}
	if (src.hair_length_value != "")
	{
		ret += "hair_length ";
		ret += src.hair_length_value;
		ret += "\n";
	}
	if (src.air_friction_value != "")
	{
		ret += "air_friction ";
		ret += src.air_friction_value;
		ret += "\n";
	}
	if (src.timestep_value != "")
	{
		ret += "timestep ";
		ret += src.timestep_value;
		ret += "\n";
	}
	if (src.collision_dist_value != "")
	{
		ret += "collision_dist ";
		ret += src.collision_dist_value;
		ret += "\n";
	}
	if (src.wind_v_step_value != "")
	{
		ret += "wind_v_step ";
		ret += src.wind_v_step_value;
		ret += "\n";
	}
	if (src.wind_h_step_value != "")
	{
		ret += "wind_h_step ";
		ret += src.wind_h_step_value;
		ret += "\n";
	}
	if (src.wind_speed_step_value != "")
	{
		ret += "wind_speed_step ";
		ret += src.wind_speed_step_value;
		ret += "\n";
	}
	if (src.cam_rot_speed_value != "")
	{
		ret += "cam_rot_speed ";
		ret += src.cam_rot_speed_value;
		ret += "\n";
	}
	if (src.cam_mv_speed_value != "")
	{
		ret += "cam_mv_speed ";
		ret += src.cam_mv_speed_value;
		ret += "\n";
	}
	if (src.arrow_model_fix_value != "")
	{
		ret += "arrow_model_fix ";
		ret += src.arrow_model_fix_value;
		ret += "\n";
	}

	return ret;
}

std::string modelConfigPacker(ModelConfigValues &src)
{
	std::string ret;

	if(src.model_value != "")
	{
		ret += "model ";
		ret += src.model_value;
		ret += "\n";
	}
	if(src.mask_value != "")
	{
		ret += "mask ";
		ret += src.mask_value;
		ret += "\n";
	}
	if(src.masked_material_value != "")
	{
		ret += "masked_material ";
		ret += src.masked_material_value;
		ret += "\n";
	}
	if(src.scalp_value != "")
	{
		ret += "scalp ";
		ret += src.scalp_value;
		ret += "\n";
	}
	if(src.use_value != "")
	{
		ret += "use ";
		ret += src.use_value;
		ret += "\n";
	}
	if(src.conf_value != "")
	{
		ret += "conf ";
		ret += src.conf_value;
		ret += "\n";
	}

	return ret;
}

void writeToFile(const char* flname, std::string &text)
{
	std::ofstream ofl(flname, std::ofstream::out | std::ofstream::trunc);
	ofl << text;
	ofl.close();
}

//TODO
ConfigValues confCreatorDialogue()
{
	ConfigValues ret;

	return ret;
}

//TODO
ModelConfigValues modelConfCreatorDialogue()
{
	ModelConfigValues ret;

	//YOU ARE HERE NOW

	return ret;
}


bool checkConfFileDialogue(std::string &conf)
{
	std::ifstream infile(conf);

	if (!infile.good())
	{
		std::cout << "\033[1;31m[Check if file is a config] \033[0mError: can't open file \"" << conf << "\"" << std::endl;
		infile.close();
		exit(-1);
	}

	std::string flcheck;
	infile >> flcheck;
	return (flcheck == "HAIRCONFIG");
}

bool checkModelConfFileDialogue(std::string &conf)
{
	std::ifstream infile(conf);

	if (!infile.good())
	{
		std::cout << "\033[1;31m[Check if file is a mdlconfig] \033[0mError: can't open file \"" << conf << "\"" << std::endl;
		infile.close();
		exit(-1);
	}

	std::string flcheck;
	infile >> flcheck;
	return (flcheck == "MDLCONFIG");
}

std::string getDirPath(const char* path)
{
	char copyPath[1024];
	strcpy(copyPath, path);
	char* dirName = dirname(copyPath);
	std::string concated = copyPath;
	concated += "/";
	return concated;
}

std::string stringVectorToPath(std::vector<std::string>& paths)
{
	std::string ret;

	for (int i = 0; i < paths.size(); ++i)
	{
		ret += paths[i];
		ret += "/";
	}

	return ret;
}

//TODO
std::vector<std::string> pathToStringVector(std::string path)
{
	std::vector<std::string> ret;


	return ret;
}

//TODO
std::string confSearchDialogue()
{
	std::string ret;



	return ret;
}

std::string chooseDirDialogue(std::string& path, std::string& what_check)
{
	DIR *dir = opendir(path.c_str());

	struct dirent* entry = readdir(dir);

	std::vector<std::string> vals;
	std::vector<bool> isconf;
	int cnt = 0;
	while (entry != NULL)
	{
		if (entry->d_type == DT_DIR)
		{
			std::cout << "(" << cnt << ") " << "[DIR] " << entry->d_name << std::endl;
			isconf.push_back(false);
			vals.push_back(entry->d_name);
			cnt++;
		}
		if (entry->d_type == DT_REG)
		{
			bool b_conf = false;
			std::string npath = path;
			npath += "/";
			npath += entry->d_name;
			if (what_check == "mdl")
			{
				b_conf = checkModelConfFileDialogue(npath);
			}
			else
			{
				b_conf = checkConfFileDialogue(npath);
			}

			if (b_conf)
			{
				std::cout << "(" << cnt << ") " << entry->d_name << std::endl;
				vals.push_back(entry->d_name);
				isconf.push_back(true);
				cnt++;
			}
		}
		
		entry = readdir(dir);
	}
	closedir(dir);

	std::cout << "Go to: ";
	int wherego = -2;
	while((scanf("%d", &wherego) != 1) &&
			(wherego > -1) &&
			(wherego < cnt))
	{
		std::cout << "please input a valid integer (from 0 to " << cnt-1 << ": ";
	}

	std::string newpath = path;
	newpath += "/" + vals[wherego];

	if (isconf[wherego])
	{
		return newpath;
	}
	else
	{
		return chooseDirDialogue(newpath, what_check);
	}
}

std::string mdlConfSearchDialogue()
{
	std::string p = ".";
	std::string type = "mdl";
	return chooseDirDialogue(p, type);
}

//TODO
bool confDialogue(std::string &conf, ConfigValues &vals)
{



	conf = "hair.conf";
	return false;
}

bool mdlConfDialogue(std::string &conf, ModelConfigValues &vals)
{
	std::cout << "Would you like to load an existing config or create a new one? (y/n) ";

	std::string answer;

	std::cin >> answer;

	if ((answer == "y") || (answer == "Y"))
	{
		conf = mdlConfSearchDialogue();
		return false;
	}
	else
	{
		std::cout << "Starting new config creation" << std::endl;
		vals = modelConfCreatorDialogue();
		return true;
	}

	//conf = "heavy.conf";
	conf = "warrior.conf";
	return false;
}

void dialogue(std::string &conf)
{
	bool newModelConf = false;

	ModelConfigValues mdlConfV;

	newModelConf = mdlConfDialogue(conf, mdlConfV);

	if (newModelConf)
	{
		conf = "tmp_model.conf";
		std::string mdlConfValues = modelConfigPacker(mdlConfV);
		writeToFile(conf.c_str(), mdlConfValues);
	}

}

int main(int argc, const char ** argv)
{
	std::string config = "";

	if (argc == 1)
	{
		dialogue(config);
	}

	if (argc > 1)
	{
		config = argv[1];
	}

	hair_app *app = new hair_app();
	app->setConfig(config.c_str());
	app->run(app);
	delete app; 
	return 0;
}