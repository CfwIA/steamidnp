#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include <cstring>

#include <unistd.h>

const static std::string STEAM_INSTALL_PATH = "/.steam/steam/steamapps/appmanifest_";
const static std::string EXTENTION = "acf";

void display_help() {
	std::cout << "usage:" << std::endl
		<< "\tsteamidnp : list the games installed from the steam directory" << std::endl
		<< "\tsteamidnp [id] : print the name of the game if installed" << std::endl;
}

std::string get_linux_username() {
	static std::string usrn;
	if (usrn.empty()) {
		char tmpusrn[256];
		memset(tmpusrn, '\0', sizeof(tmpusrn));
		getlogin_r(tmpusrn, sizeof(tmpusrn));
		usrn = tmpusrn;
	}
	return usrn;
}

std::string get_file_path(const std::string& id) {
	auto mid = id;
	if (mid.back() == '/')
		mid.back() = '.';
	else
		mid += '.';
	return "/home/" + get_linux_username() + STEAM_INSTALL_PATH + mid + EXTENTION;
}

std::string read_game_name(const std::string& path) {
	std::string filepath = path;
	std::ifstream in(filepath);
	if (in.is_open()) {
		std::string line;
		while (!std::getline(in, line).eof()) {
			if (line.contains("\"name\"")) {
				size_t third_quote = line.find('"', line.find('"', line.find('"')+1)+1);
				std::string name = line.substr(third_quote+1, line.find('"', third_quote+1)-third_quote-1);
				return name;
			}
		}
	} else {
		std::cerr << "err: file \"" << path << "\" doesn't exist, game is either not installed or not existent, failbit " << in.rdstate() << std::endl;
	}
	return "";
}

int mainpp(const std::vector<std::string>& args) {
	if (args.size() > 1) {
		display_help();
		return 1;
	}

	std::string currentusr = get_linux_username();
	if (args.size() == 0) {
		auto p = std::filesystem::path("/home/" + currentusr + "/.steam/steam/steamapps/");
		for(const auto& entries : std::filesystem::directory_iterator(p)) {
			auto strp = entries.path().string();
			if (strp.contains("appmanifest_") && entries.path().extension().string().contains("acf")) {
				std::string game_name = read_game_name(strp);
				size_t last_underscore_pos = strp.find_last_of('_');
				std::string id = strp.substr(last_underscore_pos+1, strp.find_last_of('.')-last_underscore_pos-1);
				std::cout << id << "\t\t" << game_name << std::endl;
			}
		}
		return 0;
	}

	if (args.at(0) == "-h" || args.at(0) == "--help") {
		display_help();
		return 0;
	}
	std::string filepath = get_file_path(args.at(0));
	std::cout << read_game_name(filepath) << std::endl;
	return 0;
}

int main(int argc, char** argv) {
	if (argc < 1) {
		display_help();
		return 1;
	}
	std::vector<std::string> args;
	for(int i = 1; i < argc; ++i) {
		args.push_back(argv[i]);
	}
	return mainpp(args);
}

