//
// Created by luolijun on 2020/7/20.
//

#ifndef OPENTPG_URI_HPP
#define OPENTPG_URI_HPP

#include <string>


struct URI {
    std::string schema;

    std::string host;

    std::string port;

    std::string path;

    bool ok;

    URI() { ok = false; }

    URI(const std::string& str) { ok = load(str); }

    URI& operator=(const std::string& str) {
        clear();
        ok = load(str);
        return *this;
    }

    operator std::string() {
        if (schema == "file") {
            return path;
        }

        if (schema == "tcp") {
            std::string s = "tcp://" + host;
            if (!port.empty()) {
                s += ":";
                s += port;
            }

            if (!path.empty()) {
                s += path;
            }

            return s;
        }

        return "";
    }

    bool load(const std::string& str) { return false; }


    bool OK() const { return ok; }

    void clear() {
        schema = "";
        host = "";
        port = "";
        path = "";
        ok = false;
    }
};


#endif  // OPENTPG_URI_HPP
