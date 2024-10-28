#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstring>
#include <ctime>
#include <streambuf>
#include "wwivnet.h"

void print_help() {
    std::cerr << "Usage \"ghostup [command] [args]\"\n\n";
    std::cerr << "        subping    /path/to/network.dir\n";
    std::cerr << "        subcomp    /path/to/network.dir\n";
    std::cerr << "        subsend    /path/to/network.dir\n";
    std::cerr << "        sendfile   [file] /path/to/network.dir\n";
    std::cerr << "                    file: bbslist, connect, fbackhdr,\n";
    std::cerr << "                          wwivnews, categ, networks,\n";
    std::cerr << "                          binkp\n";
    std::cerr << "        sendother   /path/to/file.net /path/to/network.dir\n";
}

std::vector<uint16_t> get_all_nodes(const char *path_to_network_dir) {
    std::ifstream in(std::string(path_to_network_dir) + "/bbslist.net");
    std::vector<uint16_t> nodes;
    if (in.is_open()) {
        std::string str;
        while (getline(in, str)) {
            if (!str.empty()) {
                if (str.at(0) == '@') {
                    try {
                        uint16_t node = std::stoi(str.substr(1, -1));
                        nodes.push_back(node);
                    } catch (std::invalid_argument const &) {

                    } catch (std::out_of_range const &) {

                    } 
                }
            }
        }
        in.close();
    }
    return nodes;
}

FILE *get_px_net(const char *path_to_network_dir) {
    std::filesystem::path p(path_to_network_dir);

    int i = 0;
    p.append("p0.net");

    while(std::filesystem::exists(p)) {
        p.replace_filename(std::string("p") + std::to_string(++i) + ".net");
    }
    FILE *fptr = fopen(p.u8string().c_str(), "wb");
    return fptr;
}

struct category_s {
    int no;
    std::string name;
    std::vector<std::string> subs;
};

void subcomp(const char *path_to_network_dir) {
    std::vector<struct category_s> categories;

    std::ifstream categ(std::string(path_to_network_dir) + "/categ.net");
    if (categ.is_open()) {
        std::string str;
        while (getline(categ, str)) {
            struct category_s c;
            try {
                c.no = std::stoi(str);
                c.name = str;
                categories.push_back(c);
            } catch (std::invalid_argument const &) {

            } catch (std::out_of_range const &) {

            } 
        }
        categ.close();
    }

    if (categories.size() == 0) {
        struct category_s c;
        c.no = 0;
        c.name = "0.   Uncategorized";
        categories.push_back(c);
    }
    std::ifstream subs(std::string(path_to_network_dir) + "/subs.inf");
    if (subs.is_open()) {
        std::string str;
        while (getline(subs, str)) {
            std::string subline = str.substr(0, str.find("~"));
            try {
                int cat = std::stoi(str.substr(str.find("~") + 1));
                bool found = false;
                for (size_t i = 0; i < categories.size(); i++) {
                    if (categories.at(i).no == cat) {
                        categories.at(i).subs.push_back(subline);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    categories.at(0).subs.push_back(subline);
                }

            } catch (std::invalid_argument const &) {

            } catch (std::out_of_range const &) {

            } 
        } 
        subs.close();

        std::ofstream out(std::string(path_to_network_dir) + "/subs.lst.new");
        if (out.is_open()) {
            for (size_t i = 0; i < categories.size(); i++) {
                for (int k = 0; k < 78; k++) {
                    out << "\xC4";
                }
                out << "\r\n";
                out << categories.at(i).name << "\r\n";
                out << "\r\n";
                out << " SubType Host Flags Description" << "\r\n";
                out << " ====== ----- ===== ----------------------------------------------------------" << "\r\n";
                for (size_t j = 0; j < categories.at(i).subs.size(); j++) {
                    out << categories.at(i).subs.at(j) << "\r\n";
                }
                out << "\r\n";
            }
            out.close();
            std::cout << "Please check " << (std::string(path_to_network_dir) + "/subs.lst.new") << " before sending out\n";

            return;
        }
    }
    std::cout << "Failed\n";
    return;
}

void subsend(const char *path_to_network_dir) {
    std::vector<uint16_t> nodes = get_all_nodes(path_to_network_dir);

    std::ifstream t(std::string(path_to_network_dir) + "/subs.lst.new");
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    if (!nodes.empty()) {
        FILE *fptr = get_px_net(path_to_network_dir);
        struct net_header_rec msg;
        msg.fromsys = 1;
        msg.fromuser = 1;
        msg.daten = time(NULL);
        msg.list_len = nodes.size();
        msg.main_type = 9;
        msg.minor_type = 0;
        msg.tosys = 0;
        msg.touser = 0;
        msg.method = 0;
        msg.length = str.length();

        fwrite(&msg, sizeof(struct net_header_rec), 1, fptr);
        for (size_t i = 0; i < nodes.size(); i++) {
            fwrite(&nodes.at(i), sizeof(uint16_t), 1, fptr);
        }
        fwrite(str.c_str(), str.length(), 1, fptr);
        fclose(fptr);

        std::filesystem::remove(std::string(path_to_network_dir) + "/subs.lst.new");
        std::cout << "Now run network1\n";        
    }
}

void subping(const char *path_to_network_dir) {
    std::filesystem::path p(path_to_network_dir);
    p.append("subs.inf");

    if (std::filesystem::exists(p)) {
        std::filesystem::path p2(path_to_network_dir);
        p2.append("subs.inf.old");
        std::filesystem::rename(p, p2);
    }

    std::vector<uint16_t> nodes = get_all_nodes(path_to_network_dir);

    if (!nodes.empty()) {
        FILE *fptr = get_px_net(path_to_network_dir);
        struct net_header_rec msg;
        msg.fromsys = 1;
        msg.fromuser = 1;
        msg.daten = time(NULL);
        msg.list_len = nodes.size();
        msg.main_type = 20;
        msg.minor_type = 0;
        msg.tosys = 0;
        msg.touser = 0;
        msg.method = 0;
        msg.length = 0;

        fwrite(&msg, sizeof(struct net_header_rec), 1, fptr);
        for (size_t i = 0; i < nodes.size(); i++) {
            fwrite(&nodes.at(i), sizeof(uint16_t), 1, fptr);
        }
        fclose(fptr);
        std::cout << "Now run network1\n";
    } else {
        std::cerr << "Failed to read nodes from " << path_to_network_dir << "/bbslist.net\n";
    }
}

void sendfile(const char *path_to_network_dir, int minor_type, std::string file) {
    std::vector<uint16_t> nodes = get_all_nodes(path_to_network_dir);

    std::ifstream t(std::string(path_to_network_dir) + "/" + file);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    if (!nodes.empty()) {
        FILE *fptr = get_px_net(path_to_network_dir);
        struct net_header_rec msg;
        msg.fromsys = 1;
        msg.fromuser = 1;
        msg.daten = time(NULL);
        msg.list_len = nodes.size();
        msg.main_type = 1;
        msg.minor_type = minor_type;
        msg.tosys = 0;
        msg.touser = 0;
        msg.method = 0;
        msg.length = str.length();

        fwrite(&msg, sizeof(struct net_header_rec), 1, fptr);
        for (size_t i = 0; i < nodes.size(); i++) {
            fwrite(&nodes.at(i), sizeof(uint16_t), 1, fptr);
        }
        fwrite(str.c_str(), str.length(), 1, fptr);
        fclose(fptr);

        std::cout << "Now run network1\n";        
    }
}

void sendother(const char *path_to_network_dir, std::string file) {
    
    std::filesystem::path filep(file);

    if (filep.extension() != ".net" && filep.extension() != ".zip") {
        std::cerr << "File extension must be .net or .zip\n";
        return;
    }

    if (filep.stem().u8string().length() > 8) {
        std::cerr << "Filename must be 8.3 format.\n";
        return;
    }

    uint16_t flags;

    if (filep.extension() == ".zip") {
        flags = 0x2;
    } else {
        flags = 0;
    }



    std::vector<uint16_t> nodes = get_all_nodes(path_to_network_dir);

    std::ifstream t(file);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    uint32_t msgsize = str.length() + filep.stem().u8string().length() + 2;

    if (!nodes.empty()) {
        FILE *fptr = get_px_net(path_to_network_dir);
        struct net_header_rec msg;
        msg.fromsys = 1;
        msg.fromuser = 1;
        msg.daten = time(NULL);
        msg.list_len = nodes.size();
        msg.main_type = 1;
        msg.minor_type = 9;
        msg.tosys = 0;
        msg.touser = 0;
        msg.method = 0;
        msg.length = msgsize;

        fwrite(&flags, 2, 1, fptr);
        fwrite(filep.stem().u8string().c_str(), filep.stem().u8string().length(), 1, fptr);
        fwrite(&msg, sizeof(struct net_header_rec), 1, fptr);
        for (size_t i = 0; i < nodes.size(); i++) {
            fwrite(&nodes.at(i), sizeof(uint16_t), 1, fptr);
        }
        fwrite(str.c_str(), str.length(), 1, fptr);
        fclose(fptr);

        std::cout << "Now run network1\n";        
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        print_help();
        exit(-1);
    } else if (argc == 2) {
        if (strcmp(argv[1], "help") == 0) {
            print_help();
            exit(0);
        } else {
            print_help();
            exit(-1);
        }
    } else if (argc == 3) {
        if (strcmp(argv[1], "subping") == 0) {
            subping(argv[2]);
            exit(0);
        } else if (strcmp(argv[1], "subcomp") == 0) {
            subcomp(argv[2]);
            exit(0);
        } else if (strcmp(argv[1], "subsend") == 0) {
            subsend(argv[2]);
            exit(0);
        } else {
            print_help();
            exit(-1);
        }
    } else if (argc == 4) {
        if (strcmp(argv[1], "sendfile") == 0) {
            if (strcmp(argv[2], "bbslist") == 0) {
                sendfile(argv[3], 1, "bbslist.net");
            } else if (strcmp(argv[2], "connect") == 0) {
                sendfile(argv[3], 2, "connect.net");
            } else if (strcmp(argv[2], "fbackhdr") == 0) {
                sendfile(argv[3], 5, "fbackhdr.net");
            } else if (strcmp(argv[2], "wwivnews") == 0) {
                sendfile(argv[3], 6, "wwivnews.net");
            } else if (strcmp(argv[2], "categ") == 0) {
                sendfile(argv[3], 7, "categ.net");
            } else if (strcmp(argv[2], "networks") == 0) {
                sendfile(argv[3], 8, "networks.lst");
            } else if (strcmp(argv[2], "binkp") == 0) {
                sendfile(argv[3], 10, "binkp.net");
            }
        } else if (strcmp(argv[1], "sendother") == 0) {
            sendother(argv[3], argv[2]);
        } else {
            print_help();
            exit(-1);
        }
    } else {
        print_help();
        exit(-1);
    }
}