
#ifndef bash_wrapper_h
#define bash_wrapper_h


#include <vector>
#include <string>

namespace bash{

	bool cmd (const std::string& cmd,std::vector<std::string>& out ) {
		FILE*           fp;
		const int       SIZEBUF = 1234;
		char            buf [SIZEBUF];
		out = std::vector<std::string> ();
		if ((fp = popen(cmd.c_str (), "r")) == NULL) {
			return false;
		}
		std::string  cur_string = "";
		while (fgets(buf, sizeof (buf), fp)) {
			cur_string += buf;
		}
		out.push_back (cur_string.substr (0, cur_string.size () - 1));
		pclose(fp);
		return true;
	}

	bool realpath(std::string const &relative_path, std::string &abs_path){

		std::vector<std::string> _realpath;
		auto status=cmd("realpath "+relative_path, _realpath);
		assert(status==true);
		assert(_realpath.size()>0);
		abs_path=_realpath[0];
		return status;

	}

}

#endif
