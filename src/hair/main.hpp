#include <dirent.h>
#include <stdio.h>


struct ModelConfigValues
{
	std::string model_value;
	std::string mask_value;
	std::string masked_material_value;
	std::string scalp_value;
	std::string use_value;
	std::string conf_value;
};

struct ConfigValues
{
	std::string mass_value;
	std::string hair_unit_length_value;
	std::string hair_length_value;
	std::string air_friction_value;
	std::string timestep_value;
	std::string collision_dist_value;
	std::string wind_v_step_value;
	std::string wind_h_step_value;
	std::string wind_speed_step_value;
	std::string cam_rot_speed_value;
	std::string cam_mv_speed_value;
	std::string arrow_model_fix_value;
};

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
			npath += entry->d_type;
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

//TODO
std::string mdlConfSearchDialogue()
{
	std::string ret;


	std::string p = ".";
	return chooseDirDialogue(p, p);

	

	return ret;
}

//TODO
bool confDialogue(std::string &conf, ConfigValues &vals)
{



	conf = "hair.conf";
	return false;
}

//TODO
bool mdlConfDialogue(std::string &conf, ModelConfigValues &vals)
{
	std::cout << "Would you like to load an existing config or create a new one? (y/n) ";

	std::string answer;

	std::cin >> answer;

	if ((answer == "y") || (answer == "Y"))
	{
		conf = mdlConfSearchDialogue();
	}
	else
	{
		std::cout << "Starting new config creation" << std::endl;
		//modelConfCreatorDialogue();
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